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
 * @brief shippairing digest calculation implementation
 */

#include "src/ship/ship_pairing/ship_pairing_digest.h"

#include <string.h>

#include "src/common/string_util.h"
#include "src/ship/tls_certificate/tls_certificate.h"

char* ShipPairingBuildMessage(const ShipPairingEntry* entry) {
  if (entry == NULL) {
    return NULL;
  }

  // Every key the message is built from has to be present. "digest" is not one
  // of them: it carries the result and cannot be an input to it.
  if (StringIsEmpty(entry->txtvers) || StringIsEmpty(entry->par_type) || StringIsEmpty(entry->for_id)
      || StringIsEmpty(entry->for_par) || StringIsEmpty(entry->trust_id) || StringIsEmpty(entry->trust_par)
      || StringIsEmpty(entry->trust_curve) || StringIsEmpty(entry->type) || StringIsEmpty(entry->trust_nonce)
      || StringIsEmpty(entry->alg)) {
    return NULL;
  }

  return (char*)StringFmtSprintf(
      "txtvers=%s;"
      "parType=%s;"
      "forId=%s;"
      "forPar=%s;"
      "trustId=%s;"
      "trustPar=%s;"
      "trustCurve=%s;"
      "type=%s;"
      "trustNonce=%s;"
      "alg=%s;",
      entry->txtvers,
      entry->par_type,
      entry->for_id,
      entry->for_par,
      entry->trust_id,
      entry->trust_par,
      entry->trust_curve,
      entry->type,
      entry->trust_nonce,
      entry->alg
  );
}

char* ShipPairingCalcDigest(const ShipPairingEntry* entry, const uint8_t* secret, size_t secret_size) {
  if ((entry == NULL) || (secret == NULL) || (secret_size == 0)) {
    return NULL;
  }

  // This version of the specification defines one algorithm. A request naming
  // another cannot be authenticated and is not guessed at.
  if (StringIsEmpty(entry->alg) || (strcmp(entry->alg, SHIP_PAIRING_ALG_HMAC_SHA256) != 0)) {
    return NULL;
  }

  uint8_t nonce[SHIP_PAIRING_NONCE_SIZE];
  if (!StringHexToBytes(entry->trust_nonce, nonce, sizeof(nonce))) {
    return NULL;
  }

  char* const message = ShipPairingBuildMessage(entry);
  if (message == NULL) {
    return NULL;
  }

  // K is the secret followed by the nonce, as octets (section 7.3).
  uint8_t key[SHIP_PAIRING_SECRET_SIZE + SHIP_PAIRING_NONCE_SIZE];
  if (secret_size > SHIP_PAIRING_SECRET_SIZE) {
    StringDelete(message);
    return NULL;
  }

  memcpy(key, secret, secret_size);
  memcpy(key + secret_size, nonce, sizeof(nonce));

  uint8_t digest[TLS_CERTIFICATE_SHA256_SIZE];
  const EebusError error
      = TlsCertificateHmacSha256(key, secret_size + sizeof(nonce), (const uint8_t*)message, strlen(message), digest);

  StringDelete(message);
  memset(key, 0, sizeof(key));

  if (error != kEebusErrorOk) {
    return NULL;
  }

  return StringWithHexUpper(digest, sizeof(digest));
}

bool ShipPairingVerifyDigest(const ShipPairingEntry* entry, const uint8_t* secret, size_t secret_size) {
  if ((entry == NULL) || StringIsEmpty(entry->digest)) {
    return false;
  }

  char* const expected = ShipPairingCalcDigest(entry, secret, secret_size);
  if (expected == NULL) {
    return false;
  }

  const size_t expected_size = strlen(expected);

  bool matches = (strlen(entry->digest) == expected_size);
  if (matches) {
    uint8_t diff = 0;
    for (size_t i = 0; i < expected_size; ++i) {
      diff |= (uint8_t)(entry->digest[i] ^ expected[i]);
    }
    matches = (diff == 0);
  }

  StringDelete(expected);

  return matches;
}
