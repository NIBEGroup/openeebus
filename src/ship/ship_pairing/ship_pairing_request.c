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
 * @brief Creating and announcing a shippairing request implementation
 */

#include "src/ship/ship_pairing/ship_pairing_request.h"

#include <string.h>

#include "src/common/api/eebus_timer_interface.h"
#include "src/common/debug.h"
#include "src/common/eebus_timer/eebus_timer.h"
#include "src/common/string_util.h"
#include "src/ship/ship_pairing/ship_pairing_digest.h"
#include "src/ship/ship_pairing/ship_pairing_request_internal.h"
#include "src/ship/tls_certificate/tls_certificate.h"

/** Set SHIP_PAIRING_REQUEST_DEBUG 1 to enable debug prints */
#ifndef SHIP_PAIRING_REQUEST_DEBUG
#define SHIP_PAIRING_REQUEST_DEBUG 0
#endif

/** shippairing request debug printf(), enabled with SHIP_PAIRING_REQUEST_DEBUG = 1 */
#if SHIP_PAIRING_REQUEST_DEBUG
#define SHIP_PAIRING_REQUEST_DEBUG_PRINTF(fmt, ...) DebugPrintf(fmt, ##__VA_ARGS__)
#else
#define SHIP_PAIRING_REQUEST_DEBUG_PRINTF(fmt, ...)
#endif  // SHIP_PAIRING_REQUEST_DEBUG

static void ShipPairingRequestTimeoutCallback(void* ctx);

/**
 * @brief Fills the entry's TXT record keys from the configuration
 * @return true if every key could be set
 */
static bool ShipPairingRequestBuildEntry(ShipPairingEntry* entry, const ShipPairingRequestConfig* config) {
  const uint8_t* const cert = (const uint8_t*)TLS_CERTIFICATE_GET_CERTIFICATE(config->tls_certificate);
  const size_t cert_size    = TLS_CERTIFICATE_GET_CERTIFICATE_SIZE(config->tls_certificate);

  const char* const trust_curve = TlsCertificateGetCurveName(cert, cert_size);
  if (trust_curve == NULL) {
    SHIP_PAIRING_REQUEST_DEBUG_PRINTF("%s(), own certificate uses a curve SHIP does not permit\n", __func__);
    return false;
  }

  // Section 6.2: a new random nonce per request, so that re-running the process
  // with the same node never reuses one.
  uint8_t nonce[SHIP_PAIRING_NONCE_SIZE];
  if (TlsCertificateRandomBytes(nonce, sizeof(nonce)) != kEebusErrorOk) {
    SHIP_PAIRING_REQUEST_DEBUG_PRINTF("%s(), no random source for the nonce\n", __func__);
    return false;
  }

  entry->txtvers     = StringCopy(SHIP_PAIRING_TXTVERS);
  entry->par_type    = StringCopy(SHIP_PAIRING_PAR_TYPE_FP_SHA256);
  entry->for_id      = StringCopy(config->for_id);
  entry->for_par     = StringCopy(config->for_par);
  entry->trust_id    = StringCopy(config->trust_id);
  entry->trust_par   = TlsCertificateCalcFingerprintSha256(cert, cert_size);
  entry->trust_curve = StringCopy(trust_curve);
  entry->type        = StringCopy(SHIP_PAIRING_TYPE_ADD_CU);
  entry->trust_nonce = StringWithHexUpper(nonce, sizeof(nonce));
  entry->alg         = StringCopy(SHIP_PAIRING_ALG_HMAC_SHA256);

  memset(nonce, 0, sizeof(nonce));

  // The record is built here, so "txtvers" is the first key by construction.
  entry->txtvers_is_first = true;

  entry->digest = ShipPairingCalcDigest(entry, config->secret, config->secret_size);

  return ShipPairingEntryIsValid(entry);
}

ShipPairingRequest* ShipPairingRequestCreate(const ShipPairingRequestConfig* config) {
  if ((config == NULL) || (config->tls_certificate == NULL) || (config->secret == NULL) || (config->secret_size == 0)) {
    return NULL;
  }

  if (StringIsEmpty(config->instance_name) || StringIsEmpty(config->for_id) || StringIsEmpty(config->for_par)
      || StringIsEmpty(config->trust_id)) {
    return NULL;
  }

  ShipPairingRequest* const self = (ShipPairingRequest*)EEBUS_MALLOC(sizeof(ShipPairingRequest));
  if (self == NULL) {
    return NULL;
  }

  self->announcing    = false;
  self->settled_timer = EebusTimerCreate(ShipPairingRequestTimeoutCallback, self);
  self->entry         = ShipPairingEntryCreate(config->instance_name, SHIP_PAIRING_DOMAIN, 0);

  if ((self->settled_timer == NULL) || (self->entry == NULL) || !ShipPairingRequestBuildEntry(self->entry, config)) {
    ShipPairingRequestDelete(self);
    return NULL;
  }

  // Section 4.2: the request is announced from the moment it exists, and keeps
  // being announced until a connection with the addressed node has held for
  // fifteen minutes.
  self->announcing = true;

  return self;
}

void ShipPairingRequestDelete(ShipPairingRequest* request) {
  if (request == NULL) {
    return;
  }

  if (request->settled_timer != NULL) {
    EEBUS_TIMER_STOP(request->settled_timer);
    EebusTimerDelete(request->settled_timer);
  }

  ShipPairingEntryDelete(request->entry);

  EEBUS_FREE(request);
}

void ShipPairingRequestTimeoutCallback(void* ctx) {
  ShipPairingRequest* const self = (ShipPairingRequest*)ctx;

  // Section 4.2: fifteen uninterrupted minutes of SHIP connection mean the
  // pairing has taken. The request has done its job and is deleted, and is not
  // announced again after a restart either.
  SHIP_PAIRING_REQUEST_DEBUG_PRINTF(
      "%s(), connection held for %d minutes, no longer announcing\n",
      __func__,
      SHIP_PAIRING_REQUEST_SETTLED_MINUTES
  );
  self->announcing = false;
}

const ShipPairingEntry* ShipPairingRequestGetEntry(const ShipPairingRequest* request) {
  return (request == NULL) ? NULL : request->entry;
}

bool ShipPairingRequestIsAnnouncing(const ShipPairingRequest* request) {
  return (request == NULL) ? false : request->announcing;
}

void ShipPairingRequestNotifyConnected(ShipPairingRequest* request) {
  if ((request == NULL) || !request->announcing || (request->settled_timer == NULL)) {
    return;
  }

  EEBUS_TIMER_STOP(request->settled_timer);
  EEBUS_TIMER_START(request->settled_timer, SECONDS(SHIP_PAIRING_REQUEST_SETTLED_MINUTES * 60), false);
}

void ShipPairingRequestNotifyDisconnected(ShipPairingRequest* request) {
  if ((request == NULL) || (request->settled_timer == NULL)) {
    return;
  }

  // The fifteen minutes measure one uninterrupted connection, so an
  // interruption discards what had been counted rather than pausing it.
  EEBUS_TIMER_STOP(request->settled_timer);
}

void ShipPairingRequestStop(ShipPairingRequest* request) {
  if (request == NULL) {
    return;
  }

  request->announcing = false;

  if (request->settled_timer != NULL) {
    EEBUS_TIMER_STOP(request->settled_timer);
  }
}
