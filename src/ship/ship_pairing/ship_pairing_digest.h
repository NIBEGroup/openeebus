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
 * @brief shippairing digest calculation
 *
 * The digest authenticates a shippairing request: it proves the sender knows a
 * secret of the node the request is addressed to. It is calculated the same way
 * by the node that announces a request and by the node that evaluates one
 * (SHIP Pairing Service TS 1.0.0, chapter 7), so both roles share this.
 */

#ifndef SRC_SHIP_SHIP_PAIRING_SHIP_PAIRING_DIGEST_H_
#define SRC_SHIP_SHIP_PAIRING_SHIP_PAIRING_DIGEST_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "src/ship/api/ship_pairing_entry.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @brief Builds the message the digest is calculated over
 *
 * The message is the ordered concatenation of ten of the TXT record keys with
 * their values and a semicolon after each (section 7.4). Neither the order nor
 * the set of keys may vary, so it is built from the entry's own fields rather
 * than from the record as it arrived.
 *
 * @param entry Entry holding the keys, all ten of which must be set
 * @return Dynamically allocated message, or NULL if a key is missing or on
 *         error. The caller is responsible for deallocating it with
 *         StringDelete().
 */
char* ShipPairingBuildMessage(const ShipPairingEntry* entry);

/**
 * @brief Calculates the digest of a shippairing request
 *
 * The key is the binary concatenation of the secret of the addressed node and
 * the nonce the request carries (section 7.3); the message is
 * ShipPairingBuildMessage() (section 7.4).
 *
 * @param entry Entry holding the keys, all of which except "digest" must be set
 * @param secret Secret of the addressed node
 * @param secret_size Size of @p secret in octets, SHIP_PAIRING_SECRET_SIZE
 * @return Dynamically allocated uppercase hexadecimal string of 64 digits, or
 *         NULL on error. The caller is responsible for deallocating it with
 *         StringDelete().
 */
char* ShipPairingCalcDigest(const ShipPairingEntry* entry, const uint8_t* secret, size_t secret_size);

/**
 * @brief Reports whether the digest an entry carries is the expected one
 *
 * The comparison does not stop at the first differing octet, so how long it
 * takes does not depend on how much of a wrong digest was correct.
 *
 * @param entry Entry to verify
 * @param secret Secret of the addressed node
 * @param secret_size Size of @p secret in octets, SHIP_PAIRING_SECRET_SIZE
 * @return true if the digest matches
 */
bool ShipPairingVerifyDigest(const ShipPairingEntry* entry, const uint8_t* secret, size_t secret_size);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SHIP_SHIP_PAIRING_SHIP_PAIRING_DIGEST_H_
