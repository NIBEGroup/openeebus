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
#include "src/common/eebus_mutex/eebus_mutex.h"
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
static ShipPairingResult ShipPairingEvaluateLocked(ShipPairing* ship_pairing, const ShipPairingEntry* entry);
static bool ShipPairingHasTrustedPeerLocked(const ShipPairing* self);

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

  self->obj.interface_    = &interface;
  self->reporting_enabled = false;
  self->own_ship_id       = StringCopy(own_ship_id);
  self->own_fingerprint   = fingerprint;
  self->secret_size       = 0;

  self->mutex                     = EebusMutexCreate();
  self->timer_mutex               = EebusMutexCreate();
  self->activation_revision       = 0;
  self->timer_restart_in_progress = false;
  memset(self->secret, 0, sizeof(self->secret));

  self->supported_curves
      = kShipPairingCurveSecp256r1 | kShipPairingCurveBrainpoolP256r1 | kShipPairingCurveBrainpoolP384r1;

  // Nothing has been paired yet, so there is nothing to protect from a further
  // request (section 4.3).
  self->add_cu_activated = true;

  memset(&self->ring_buffer, 0, sizeof(self->ring_buffer));

  self->reactivation_timer            = EebusTimerCreate(ShipPairingTimeoutCallback, self);
  self->message_exchange_available_cb = NULL;
  self->message_exchange_ctx          = NULL;

  self->on_enabled_cb = NULL;
  self->enabled_ctx   = NULL;
  // Nothing can be accepted until a secret is provided, so this is where it
  // starts and any later change is worth reporting.
  self->reported_enabled = false;

  if ((self->own_ship_id == NULL) || (self->reactivation_timer == NULL) || (self->mutex == NULL)
      || (self->timer_mutex == NULL)) {
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

  EebusMutexDelete(ship_pairing->timer_mutex);
  EebusMutexDelete(ship_pairing->mutex);
}

void ShipPairingTimeoutCallback(void* ctx) {
  ShipPairing* const self = (ShipPairing*)ctx;

  // Section 4.3.1, item a.i: an idle but functional connection does not meet
  // the condition for automatic reactivation. Its eventual close will start
  // a fresh interval through NotifyMessageExchange().
  EEBUS_MUTEX_LOCK(self->mutex);
  if (self->timer_restart_in_progress) {
    EEBUS_MUTEX_UNLOCK(self->mutex);
    return;
  }

  const uint64_t revision                              = self->activation_revision;
  const ShipPairingMessageExchangeAvailableCallback cb = self->message_exchange_available_cb;
  void* const callback_ctx                             = self->message_exchange_ctx;
  EEBUS_MUTEX_UNLOCK(self->mutex);

  const bool message_exchange_available = (cb != NULL) && cb(callback_ctx);

  EEBUS_MUTEX_LOCK(self->mutex);
  if ((revision != self->activation_revision) || self->timer_restart_in_progress) {
    EEBUS_MUTEX_UNLOCK(self->mutex);
    return;
  }

  self->add_cu_activated = !message_exchange_available;
  ++self->activation_revision;
  EEBUS_MUTEX_UNLOCK(self->mutex);

  if (!message_exchange_available) {
    SHIP_PAIRING_DEBUG_PRINTF(
        "%s(), no message exchange for %d minutes, processing addCu-requests again\n",
        __func__,
        SHIP_PAIRING_REACTIVATION_MINUTES
    );
  }

  ShipPairingReportEnabled(self);
}

void ShipPairingRestartReactivationTimer(ShipPairing* self) {
  // The caller holds timer_mutex from before its state update. Stop can wait
  // for the timeout callback, so the state mutex must be unlocked here.
  EEBUS_TIMER_STOP(self->reactivation_timer);

  EEBUS_MUTEX_LOCK(self->mutex);
  const bool has_trusted_peer = ShipPairingHasTrustedPeerLocked(self);
  EEBUS_MUTEX_UNLOCK(self->mutex);

  if (has_trusted_peer) {
    EEBUS_TIMER_START(self->reactivation_timer, SECONDS(SHIP_PAIRING_REACTIVATION_MINUTES * 60), false);
  }

  EEBUS_MUTEX_LOCK(self->mutex);
  self->timer_restart_in_progress = false;
  EEBUS_MUTEX_UNLOCK(self->mutex);
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

  EEBUS_MUTEX_LOCK(ship_pairing->timer_mutex);
  EEBUS_MUTEX_LOCK(ship_pairing->mutex);
  const ShipPairingResult result = ShipPairingEvaluateLocked(ship_pairing, entry);
  EEBUS_MUTEX_UNLOCK(ship_pairing->mutex);

  if (result == kShipPairingResultAccepted) {
    ShipPairingRestartReactivationTimer(ship_pairing);
  }

  EEBUS_MUTEX_UNLOCK(ship_pairing->timer_mutex);

  if (result == kShipPairingResultAccepted) {
    ShipPairingReportEnabled(ship_pairing);
  }

  return result;
}

static ShipPairingResult ShipPairingEvaluateLocked(ShipPairing* ship_pairing, const ShipPairingEntry* entry) {
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
  ++ship_pairing->activation_revision;
  ship_pairing->timer_restart_in_progress = true;

  return kShipPairingResultAccepted;
}

void SetEnabledCallback(ShipPairingObject* self, OnShipPairingEnabledCallback cb, void* ctx) {
  ShipPairing* const ship_pairing = SHIP_PAIRING(self);

  EEBUS_MUTEX_LOCK(ship_pairing->mutex);
  ship_pairing->on_enabled_cb = cb;
  ship_pairing->enabled_ctx   = ctx;
  EEBUS_MUTEX_UNLOCK(ship_pairing->mutex);
}

bool IsEnabled(const ShipPairingObject* self) {
  const ShipPairing* const ship_pairing = SHIP_PAIRING(self);

  // Without a secret nothing can be authenticated, and with addCu-requests no
  // longer being processed nothing would be accepted even if it were.
  EEBUS_MUTEX_LOCK(ship_pairing->mutex);
  const bool enabled = (ship_pairing->secret_size > 0) && ship_pairing->add_cu_activated;
  EEBUS_MUTEX_UNLOCK(ship_pairing->mutex);

  return enabled;
}

/**
 * @brief Reports a change in whether a request could be accepted
 *
 * Called wherever the secret or the processing of addCu-requests changes, and
 * reports only transitions, so that a caller can act on it by starting and
 * stopping work rather than by counting.
 */
void ShipPairingReportEnabled(ShipPairing* self) {
  EEBUS_MUTEX_LOCK(self->mutex);
  if (self->reporting_enabled) {
    EEBUS_MUTEX_UNLOCK(self->mutex);
    return;
  }

  self->reporting_enabled = true;
  for (;;) {
    const bool enabled = (self->secret_size > 0) && self->add_cu_activated;
    if (enabled == self->reported_enabled) {
      break;
    }

    self->reported_enabled                = enabled;
    const OnShipPairingEnabledCallback cb = self->on_enabled_cb;
    void* const ctx                       = self->enabled_ctx;
    EEBUS_MUTEX_UNLOCK(self->mutex);
    if (cb != NULL) {
      cb(enabled, ctx);
    }

    EEBUS_MUTEX_LOCK(self->mutex);
  }

  self->reporting_enabled = false;
  EEBUS_MUTEX_UNLOCK(self->mutex);
}

bool IsAddCuActivated(const ShipPairingObject* self) {
  const ShipPairing* const ship_pairing = SHIP_PAIRING(self);
  EEBUS_MUTEX_LOCK(ship_pairing->mutex);
  const bool activated = ship_pairing->add_cu_activated;
  EEBUS_MUTEX_UNLOCK(ship_pairing->mutex);
  return activated;
}

void ActivateAddCu(ShipPairingObject* self) {
  // The ring buffer is deliberately left alone. It is not a trust store
  // (section 10.4) but a record of what has already been seen, and forgetting
  // it here would let a request that was accepted once be replayed.
  ShipPairing* const ship_pairing = SHIP_PAIRING(self);
  EEBUS_MUTEX_LOCK(ship_pairing->mutex);
  ship_pairing->add_cu_activated = true;
  ++ship_pairing->activation_revision;
  EEBUS_MUTEX_UNLOCK(ship_pairing->mutex);
  ShipPairingReportEnabled(ship_pairing);
}

void DeactivateAddCu(ShipPairingObject* self) {
  ShipPairing* const ship_pairing = SHIP_PAIRING(self);
  EEBUS_MUTEX_LOCK(ship_pairing->mutex);
  ship_pairing->add_cu_activated = false;
  ++ship_pairing->activation_revision;
  EEBUS_MUTEX_UNLOCK(ship_pairing->mutex);
  ShipPairingReportEnabled(ship_pairing);
}

void NotifyMessageExchange(ShipPairingObject* self) {
  ShipPairing* const ship_pairing = SHIP_PAIRING(self);

  EEBUS_MUTEX_LOCK(ship_pairing->timer_mutex);
  EEBUS_MUTEX_LOCK(ship_pairing->mutex);
  // Section 4.3.1, item b.ii: the trusted node is reachable after all, so the
  // pairing stands and no replacement is accepted.
  if (ShipPairingHasTrustedPeerLocked(ship_pairing)) {
    ship_pairing->add_cu_activated = false;
  }

  ++ship_pairing->activation_revision;
  ship_pairing->timer_restart_in_progress = true;
  EEBUS_MUTEX_UNLOCK(ship_pairing->mutex);
  ShipPairingRestartReactivationTimer(ship_pairing);
  EEBUS_MUTEX_UNLOCK(ship_pairing->timer_mutex);
  ShipPairingReportEnabled(ship_pairing);
}

void ShipPairingSetMessageExchangeAvailableCallback(
    ShipPairingObject* self,
    ShipPairingMessageExchangeAvailableCallback cb,
    void* ctx
) {
  ShipPairing* const ship_pairing = SHIP_PAIRING(self);
  EEBUS_MUTEX_LOCK(ship_pairing->mutex);
  ship_pairing->message_exchange_available_cb = cb;
  ship_pairing->message_exchange_ctx          = ctx;
  ++ship_pairing->activation_revision;
  EEBUS_MUTEX_UNLOCK(ship_pairing->mutex);
}

bool HasTrustedPeer(const ShipPairingObject* self) {
  const ShipPairing* const ship_pairing = SHIP_PAIRING(self);

  EEBUS_MUTEX_LOCK(ship_pairing->mutex);
  const bool has_trusted_peer = ShipPairingHasTrustedPeerLocked(ship_pairing);
  EEBUS_MUTEX_UNLOCK(ship_pairing->mutex);
  return has_trusted_peer;
}

static bool ShipPairingHasTrustedPeerLocked(const ShipPairing* ship_pairing) {
  if (ship_pairing->ring_buffer.next == 0) {
    return false;
  }

  return ship_pairing->ring_buffer.entries[ShipPairingCurrentIndex(ship_pairing)].digest[0] != '\0';
}

void GetRingBuffer(const ShipPairingObject* self, ShipPairingRingBuffer* ring_buffer) {
  if (ring_buffer == NULL) {
    return;
  }

  const ShipPairing* const ship_pairing = SHIP_PAIRING(self);
  EEBUS_MUTEX_LOCK(ship_pairing->mutex);
  *ring_buffer = ship_pairing->ring_buffer;
  EEBUS_MUTEX_UNLOCK(ship_pairing->mutex);
}

static bool ShipPairingRingEntryIsValid(const ShipPairingRingEntry* entry) {
  if (entry->digest[0] == '\0') {
    return entry->alg[0] == '\0';
  }

  // The bounded comparison includes the expected algorithm's NUL terminator.
  if (memcmp(entry->alg, SHIP_PAIRING_ALG_HMAC_SHA256, sizeof(SHIP_PAIRING_ALG_HMAC_SHA256)) != 0) {
    return false;
  }

  const size_t digits = SHIP_PAIRING_DIGEST_SIZE * 2;
  if (entry->digest[digits] != '\0') {
    return false;
  }

  for (size_t i = 0; i < digits; ++i) {
    const char c = entry->digest[i];
    if (!(((c >= '0') && (c <= '9')) || ((c >= 'A') && (c <= 'F')))) {
      return false;
    }
  }

  return true;
}

void SetRingBuffer(ShipPairingObject* self, const ShipPairingRingBuffer* ring_buffer) {
  ShipPairing* const ship_pairing = SHIP_PAIRING(self);

  if (ring_buffer == NULL) {
    return;
  }

  bool valid = ring_buffer->next <= SHIP_PAIRING_RING_BUFFER_SIZE;
  for (size_t i = 0; i < SHIP_PAIRING_RING_BUFFER_SIZE; ++i) {
    if (!valid) {
      break;
    }

    valid = ShipPairingRingEntryIsValid(&ring_buffer->entries[i]);
  }

  EEBUS_MUTEX_LOCK(ship_pairing->timer_mutex);
  EEBUS_MUTEX_LOCK(ship_pairing->mutex);
  if (valid) {
    ship_pairing->ring_buffer = *ring_buffer;
  } else {
    SHIP_PAIRING_DEBUG_PRINTF("%s(), discarding an invalid persisted ring buffer\n", __func__);
    memset(&ship_pairing->ring_buffer, 0, sizeof(ship_pairing->ring_buffer));
  }

  // A restored pairing is a pairing: it is protected until the fifteen minutes
  // of section 4.3.1 elapse, counted from this node starting up.
  ship_pairing->add_cu_activated = !ShipPairingHasTrustedPeerLocked(ship_pairing);
  ++ship_pairing->activation_revision;
  ship_pairing->timer_restart_in_progress = true;
  EEBUS_MUTEX_UNLOCK(ship_pairing->mutex);
  ShipPairingRestartReactivationTimer(ship_pairing);
  EEBUS_MUTEX_UNLOCK(ship_pairing->timer_mutex);
  ShipPairingReportEnabled(ship_pairing);
}

void SetSecret(ShipPairingObject* self, const uint8_t* secret, size_t secret_size) {
  ShipPairing* const ship_pairing = SHIP_PAIRING(self);

  EEBUS_MUTEX_LOCK(ship_pairing->mutex);
  memset(ship_pairing->secret, 0, sizeof(ship_pairing->secret));
  ship_pairing->secret_size = 0;

  if ((secret != NULL) && (secret_size == SHIP_PAIRING_SECRET_SIZE)) {
    memcpy(ship_pairing->secret, secret, secret_size);
    ship_pairing->secret_size = secret_size;
  }

  EEBUS_MUTEX_UNLOCK(ship_pairing->mutex);
  ShipPairingReportEnabled(ship_pairing);
}

void SetSupportedCurves(ShipPairingObject* self, uint32_t curves) {
  ShipPairing* const ship_pairing = SHIP_PAIRING(self);
  EEBUS_MUTEX_LOCK(ship_pairing->mutex);
  ship_pairing->supported_curves = curves;
  EEBUS_MUTEX_UNLOCK(ship_pairing->mutex);
}
