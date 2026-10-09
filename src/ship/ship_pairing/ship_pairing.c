/*
 * Copyright 2025 NIBE AB
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/**
 * @file
 * @brief shippairing evaluator implementation
 */

#include "src/ship/ship_pairing/ship_pairing.h"

#include <string.h>

#include "src/common/debug.h"
#include "src/common/eebus_timer/eebus_timer.h"
#include "src/common/string_util.h"
#include "src/ship/ship_pairing/ship_pairing_digest.h"
#include "src/ship/ship_pairing/ship_pairing_internal.h"
#include "src/ship/tls_certificate/tls_certificate.h"

/** Set SHIP_PAIRING_DEBUG 1 to enable debug prints */
#ifndef SHIP_PAIRING_DEBUG
#define SHIP_PAIRING_DEBUG 0
#endif

/** shippairing debug printf(), enabled with SHIP_PAIRING_DEBUG = 1 */
#if SHIP_PAIRING_DEBUG
#define SHIP_PAIRING_DEBUG_PRINTF(fmt, ...) DebugPrintf(fmt, ##__VA_ARGS__)
#else
#define SHIP_PAIRING_DEBUG_PRINTF(fmt, ...)
#endif  // SHIP_PAIRING_DEBUG

static void Destruct(ShipPairingObject* self);
static ShipPairingResult Evaluate(ShipPairingObject* self, const ShipPairingEntry* entry);
static bool IsAddCuActivated(const ShipPairingObject* self);
static void ActivateAddCu(ShipPairingObject* self);
static void DeactivateAddCu(ShipPairingObject* self);
static void NotifyMessageExchange(ShipPairingObject* self);
static bool HasTrustedPeer(const ShipPairingObject* self);
static void GetRingBuffer(const ShipPairingObject* self, ShipPairingRingBuffer* ring_buffer);
static void SetRingBuffer(ShipPairingObject* self, const ShipPairingRingBuffer* ring_buffer);
static void SetSecret(ShipPairingObject* self, const uint8_t* secret, size_t secret_size);
static void SetSupportedCurves(ShipPairingObject* self, uint32_t curves);
static void SetEnabledCallback(ShipPairingObject* self, OnShipPairingEnabledCallback cb, void* ctx);
static bool IsEnabled(const ShipPairingObject* self);
static void ShipPairingReportEnabled(ShipPairing* self);

static const ShipPairingInterface interface = {
    .destruct                = Destruct,
    .evaluate                = Evaluate,
    .is_add_cu_activated     = IsAddCuActivated,
    .activate_add_cu         = ActivateAddCu,
    .deactivate_add_cu       = DeactivateAddCu,
    .notify_message_exchange = NotifyMessageExchange,
    .has_trusted_peer        = HasTrustedPeer,
    .get_ring_buffer         = GetRingBuffer,
    .set_ring_buffer         = SetRingBuffer,
    .set_secret              = SetSecret,
    .set_supported_curves    = SetSupportedCurves,
    .set_enabled_callback    = SetEnabledCallback,
    .is_enabled              = IsEnabled,
};

static void ShipPairingTimeoutCallback(void* ctx);
static bool ShipPairingIsCurveSupported(const ShipPairing* self, const char* curve);
static bool ShipPairingIsKnownDigest(const ShipPairing* self, const ShipPairingEntry* entry);
static void ShipPairingAddDigest(ShipPairing* self, const ShipPairingEntry* entry);
static void ShipPairingRestartReactivationTimer(ShipPairing* self);
static void ShipPairingCopyTo(char* dst, size_t dst_size, const char* src);

/**
 * @brief Index of the entry a new pair is stored at, counting from zero
 *
 * Section 11.2 numbers the entries from one and lets "next" be zero only before
 * anything has ever been stored.
 */
static size_t ShipPairingNextIndex(const ShipPairing* self) {
  return (self->ring_buffer.next == 0) ? 0 : (size_t)(self->ring_buffer.next - 1);
}

/**
 * @brief Index of the entry of the currently trusted node, section 11.4
 */
static size_t ShipPairingCurrentIndex(const ShipPairing* self) {
  const size_t next = ShipPairingNextIndex(self);
  return (next == 0) ? (SHIP_PAIRING_RING_BUFFER_SIZE - 1) : (next - 1);
}

ShipPairingObject* ShipPairingCreate(const char* own_ship_id, const TlsCertificateObject* tls_certificate) {
  if (StringIsEmpty(own_ship_id) || (tls_certificate == NULL)) {
    return NULL;
  }

  const uint8_t* const cert = (const uint8_t*)TLS_CERTIFICATE_GET_CERTIFICATE(tls_certificate);
  const size_t cert_size    = TLS_CERTIFICATE_GET_CERTIFICATE_SIZE(tls_certificate);

  const char* const fingerprint = TlsCertificateCalcFingerprintSha256(cert, cert_size);
  if (fingerprint == NULL) {
    SHIP_PAIRING_DEBUG_PRINTF("%s(), failed to calculate own certificate fingerprint\n", __func__);
    return NULL;
  }

  ShipPairing* const self = (ShipPairing*)EEBUS_MALLOC(sizeof(ShipPairing));
  if (self == NULL) {
    StringDelete((char*)fingerprint);
    return NULL;
  }

  self->obj.interface_  = &interface;
  self->own_ship_id     = StringCopy(own_ship_id);
  self->own_fingerprint = fingerprint;
  self->secret_size     = 0;
  memset(self->secret, 0, sizeof(self->secret));

  self->supported_curves
      = kShipPairingCurveSecp256r1 | kShipPairingCurveBrainpoolP256r1 | kShipPairingCurveBrainpoolP384r1;

  // Nothing has been paired yet, so there is nothing to protect from a further
  // request (section 4.3).
  self->add_cu_activated = true;

  memset(&self->ring_buffer, 0, sizeof(self->ring_buffer));

  self->reactivation_timer = EebusTimerCreate(ShipPairingTimeoutCallback, self);

  self->on_enabled_cb = NULL;
  self->enabled_ctx   = NULL;
  // Nothing can be accepted until a secret is provided, so this is where it
  // starts and any later change is worth reporting.
  self->reported_enabled = false;

  if ((self->own_ship_id == NULL) || (self->reactivation_timer == NULL)) {
    Destruct(SHIP_PAIRING_OBJECT(self));
    EEBUS_FREE(self);
    return NULL;
  }

  return SHIP_PAIRING_OBJECT(self);
}

void Destruct(ShipPairingObject* self) {
  ShipPairing* const ship_pairing = SHIP_PAIRING(self);

  if (ship_pairing->reactivation_timer != NULL) {
    EEBUS_TIMER_STOP(ship_pairing->reactivation_timer);
    EebusTimerDelete(ship_pairing->reactivation_timer);
    ship_pairing->reactivation_timer = NULL;
  }

  StringDelete((char*)ship_pairing->own_ship_id);
  ship_pairing->own_ship_id = NULL;
  StringDelete((char*)ship_pairing->own_fingerprint);
  ship_pairing->own_fingerprint = NULL;

  // The secret outlives neither the object nor a core dump of it.
  memset(ship_pairing->secret, 0, sizeof(ship_pairing->secret));
  ship_pairing->secret_size = 0;
}

void ShipPairingTimeoutCallback(void* ctx) {
  ShipPairing* const self = (ShipPairing*)ctx;

  // Section 4.3.1, item a: fifteen minutes without SHIP Message Exchange with
  // the trusted node, this node running throughout. The pairing may be over, so
  // a replacement is allowed to announce itself.
  SHIP_PAIRING_DEBUG_PRINTF(
      "%s(), no message exchange for %d minutes, processing addCu-requests again\n",
      __func__,
      SHIP_PAIRING_REACTIVATION_MINUTES
  );
  self->add_cu_activated = true;
  ShipPairingReportEnabled(self);
}

void ShipPairingRestartReactivationTimer(ShipPairing* self) {
  if (self->reactivation_timer == NULL) {
    return;
  }

  EEBUS_TIMER_STOP(self->reactivation_timer);

  // Only meaningful while there is a pairing that could be discontinued.
  if (HasTrustedPeer(SHIP_PAIRING_OBJECT(self))) {
    EEBUS_TIMER_START(self->reactivation_timer, SECONDS(SHIP_PAIRING_REACTIVATION_MINUTES * 60), false);
  }
}

bool ShipPairingIsCurveSupported(const ShipPairing* self, const char* curve) {
  if (StringIsEmpty(curve)) {
    return false;
  }

  if (strcmp(curve, TLS_CERTIFICATE_CURVE_SECP256R1) == 0) {
    return (self->supported_curves & kShipPairingCurveSecp256r1) != 0;
  }

  if (strcmp(curve, TLS_CERTIFICATE_CURVE_BRAINPOOLP256R1) == 0) {
    return (self->supported_curves & kShipPairingCurveBrainpoolP256r1) != 0;
  }

  if (strcmp(curve, TLS_CERTIFICATE_CURVE_BRAINPOOLP384R1) == 0) {
    return (self->supported_curves & kShipPairingCurveBrainpoolP384r1) != 0;
  }

  return false;
}

bool ShipPairingIsKnownDigest(const ShipPairing* self, const ShipPairingEntry* entry) {
  for (size_t i = 0; i < SHIP_PAIRING_RING_BUFFER_SIZE; ++i) {
    const ShipPairingRingEntry* const stored = &self->ring_buffer.entries[i];
    if (stored->digest[0] == '\0') {
      continue;
    }

    if ((strcmp(stored->alg, entry->alg) == 0) && (strcmp(stored->digest, entry->digest) == 0)) {
      return true;
    }
  }

  return false;
}

/**
 * @brief Copies a NUL terminated string into a fixed buffer, or empties it
 *
 * The values stored here have been validated against section 5.4 and cannot be
 * too long for their buffer; emptying it on overflow keeps that assumption from
 * turning into an unterminated string if it is ever wrong.
 */
void ShipPairingCopyTo(char* dst, size_t dst_size, const char* src) {
  const size_t size = (src == NULL) ? 0 : (strlen(src) + 1);

  if ((size == 0) || (size > dst_size)) {
    dst[0] = '\0';
    return;
  }

  memcpy(dst, src, size);
}

void ShipPairingAddDigest(ShipPairing* self, const ShipPairingEntry* entry) {
  // Section 11.3: store at "next", then advance it, wrapping back to the first
  // entry after the last.
  ShipPairingRingEntry* const stored = &self->ring_buffer.entries[ShipPairingNextIndex(self)];

  memset(stored, 0, sizeof(*stored));
  ShipPairingCopyTo(stored->alg, sizeof(stored->alg), entry->alg);
  ShipPairingCopyTo(stored->digest, sizeof(stored->digest), entry->digest);

  const uint8_t next     = (self->ring_buffer.next == 0) ? 1 : self->ring_buffer.next;
  self->ring_buffer.next = (next >= SHIP_PAIRING_RING_BUFFER_SIZE) ? 1 : (uint8_t)(next + 1);
}

ShipPairingResult Evaluate(ShipPairingObject* self, const ShipPairingEntry* entry) {
  ShipPairing* const ship_pairing = SHIP_PAIRING(self);

  if (entry == NULL) {
    return kShipPairingResultError;
  }

  // Step 1, analyse relevance. Everything that can be decided without the
  // secret is decided first, so that an announcement which is not ours does not
  // reach the digest calculation at all.
  if (!ShipPairingEntryIsValid(entry)) {
    return kShipPairingResultInvalidRecord;
  }

  if (strcmp(entry->for_id, ship_pairing->own_ship_id) != 0) {
    return kShipPairingResultNotForThisNode;
  }

  // "forPar", together with "parType", has to identify a certificate of this
  // node. ShipPairingEntryIsValid() has already established that "parType" is
  // the SHA-256 fingerprint this node also uses.
  if (strcmp(entry->for_par, ship_pairing->own_fingerprint) != 0) {
    return kShipPairingResultNotForThisNode;
  }

  if (!ShipPairingIsCurveSupported(ship_pairing, entry->trust_curve)) {
    return kShipPairingResultUnsupported;
  }

  if ((strcmp(entry->type, SHIP_PAIRING_TYPE_ADD_CU) == 0) && !ship_pairing->add_cu_activated) {
    return kShipPairingResultNotProcessing;
  }

  if (ship_pairing->secret_size == 0) {
    SHIP_PAIRING_DEBUG_PRINTF("%s(), no secret set, cannot authenticate a request\n", __func__);
    return kShipPairingResultError;
  }

  // Step 2, only process new requests. A digest already in the ring buffer is a
  // replay. It is deliberately not added again here.
  if (ShipPairingIsKnownDigest(ship_pairing, entry)) {
    return kShipPairingResultAlreadySeen;
  }

  // Step 3, verify the digest.
  if (!ShipPairingVerifyDigest(entry, ship_pairing->secret, ship_pairing->secret_size)) {
    return kShipPairingResultDigestMismatch;
  }

  // Step 4, store the digest. Only now, so that a request which failed to
  // authenticate cannot fill the buffer or suppress a later genuine one.
  ShipPairingAddDigest(ship_pairing, entry);

  // Section 4.2, step 3: having accepted one, stop processing further ones.
  ship_pairing->add_cu_activated = false;
  ShipPairingRestartReactivationTimer(ship_pairing);
  ShipPairingReportEnabled(ship_pairing);

  return kShipPairingResultAccepted;
}

void SetEnabledCallback(ShipPairingObject* self, OnShipPairingEnabledCallback cb, void* ctx) {
  ShipPairing* const ship_pairing = SHIP_PAIRING(self);

  ship_pairing->on_enabled_cb = cb;
  ship_pairing->enabled_ctx   = ctx;
}

bool IsEnabled(const ShipPairingObject* self) {
  const ShipPairing* const ship_pairing = SHIP_PAIRING(self);

  // Without a secret nothing can be authenticated, and with addCu-requests no
  // longer being processed nothing would be accepted even if it were.
  return (ship_pairing->secret_size > 0) && ship_pairing->add_cu_activated;
}

/**
 * @brief Reports a change in whether a request could be accepted
 *
 * Called wherever the secret or the processing of addCu-requests changes, and
 * reports only transitions, so that a caller can act on it by starting and
 * stopping work rather than by counting.
 */
void ShipPairingReportEnabled(ShipPairing* self) {
  const bool enabled = IsEnabled(SHIP_PAIRING_OBJECT(self));
  if (enabled == self->reported_enabled) {
    return;
  }

  self->reported_enabled = enabled;

  if (self->on_enabled_cb != NULL) {
    self->on_enabled_cb(enabled, self->enabled_ctx);
  }
}

bool IsAddCuActivated(const ShipPairingObject* self) {
  return SHIP_PAIRING(self)->add_cu_activated;
}

void ActivateAddCu(ShipPairingObject* self) {
  // The ring buffer is deliberately left alone. It is not a trust store
  // (section 10.4) but a record of what has already been seen, and forgetting
  // it here would let a request that was accepted once be replayed.
  SHIP_PAIRING(self)->add_cu_activated = true;
  ShipPairingReportEnabled(SHIP_PAIRING(self));
}

void DeactivateAddCu(ShipPairingObject* self) {
  SHIP_PAIRING(self)->add_cu_activated = false;
  ShipPairingReportEnabled(SHIP_PAIRING(self));
}

void NotifyMessageExchange(ShipPairingObject* self) {
  ShipPairing* const ship_pairing = SHIP_PAIRING(self);

  // Section 4.3.1, item b.ii: the trusted node is reachable after all, so the
  // pairing stands and no replacement is accepted.
  if (HasTrustedPeer(self)) {
    ship_pairing->add_cu_activated = false;
  }

  ShipPairingRestartReactivationTimer(ship_pairing);
  ShipPairingReportEnabled(ship_pairing);
}

bool HasTrustedPeer(const ShipPairingObject* self) {
  const ShipPairing* const ship_pairing = SHIP_PAIRING(self);

  if (ship_pairing->ring_buffer.next == 0) {
    return false;
  }

  return ship_pairing->ring_buffer.entries[ShipPairingCurrentIndex(ship_pairing)].digest[0] != '\0';
}

void GetRingBuffer(const ShipPairingObject* self, ShipPairingRingBuffer* ring_buffer) {
  if (ring_buffer == NULL) {
    return;
  }

  *ring_buffer = SHIP_PAIRING(self)->ring_buffer;
}

void SetRingBuffer(ShipPairingObject* self, const ShipPairingRingBuffer* ring_buffer) {
  ShipPairing* const ship_pairing = SHIP_PAIRING(self);

  if (ring_buffer == NULL) {
    return;
  }

  ship_pairing->ring_buffer = *ring_buffer;

  // A stored index outside the buffer would be read back as an entry that is
  // not there, so a damaged one is treated as an empty buffer rather than
  // trusted.
  if (ship_pairing->ring_buffer.next > SHIP_PAIRING_RING_BUFFER_SIZE) {
    SHIP_PAIRING_DEBUG_PRINTF("%s(), discarding a ring buffer with an out of range index\n", __func__);
    memset(&ship_pairing->ring_buffer, 0, sizeof(ship_pairing->ring_buffer));
  }

  // A restored pairing is a pairing: it is protected until the fifteen minutes
  // of section 4.3.1 elapse, counted from this node starting up.
  ship_pairing->add_cu_activated = !HasTrustedPeer(self);
  ShipPairingRestartReactivationTimer(ship_pairing);
  ShipPairingReportEnabled(ship_pairing);
}

void SetSecret(ShipPairingObject* self, const uint8_t* secret, size_t secret_size) {
  ShipPairing* const ship_pairing = SHIP_PAIRING(self);

  memset(ship_pairing->secret, 0, sizeof(ship_pairing->secret));
  ship_pairing->secret_size = 0;

  if ((secret == NULL) || (secret_size == 0) || (secret_size > sizeof(ship_pairing->secret))) {
    ShipPairingReportEnabled(ship_pairing);
    return;
  }

  memcpy(ship_pairing->secret, secret, secret_size);
  ship_pairing->secret_size = secret_size;

  ShipPairingReportEnabled(ship_pairing);
}

void SetSupportedCurves(ShipPairingObject* self, uint32_t curves) {
  SHIP_PAIRING(self)->supported_curves = curves;
}
