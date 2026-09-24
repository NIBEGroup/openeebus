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
 * @brief shippairing evaluator implementation declarations
 */

#ifndef SRC_SHIP_SHIP_PAIRING_SHIP_PAIRING_H_
#define SRC_SHIP_SHIP_PAIRING_SHIP_PAIRING_H_

#include "src/common/eebus_malloc.h"
#include "src/ship/api/ship_pairing_interface.h"
#include "src/ship/api/tls_certificate_interface.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @brief Minutes without SHIP Message Exchange before addCu-requests are
 * processed again, section 4.3.1
 */
#define SHIP_PAIRING_REACTIVATION_MINUTES 15

/**
 * @brief Creates a shippairing evaluator
 *
 * The SHIP ID and the certificate are this node's own: a request names them, in
 * the keys "forId" and "forPar", to address itself to this node. The
 * fingerprint of the certificate is calculated once here rather than being
 * passed in, so that it cannot disagree with the certificate actually presented
 * during a TLS handshake.
 *
 * Every curve of [SHIP] 1.1.0 is supported until told otherwise, which is what
 * both certificate backends of this library can do. Narrow it with
 * SHIP_PAIRING_SET_SUPPORTED_CURVES() on a node that cannot.
 *
 * Until a secret is set with SHIP_PAIRING_SET_SECRET(), no request can be
 * authenticated and every one of them is rejected.
 *
 * @param own_ship_id SHIP ID of this node
 * @param tls_certificate Certificate of this node
 * @return New evaluator on success, NULL on error
 */
ShipPairingObject* ShipPairingCreate(const char* own_ship_id, const TlsCertificateObject* tls_certificate);

/**
 * @brief Destructs the evaluator and deallocates it
 * @param ship_pairing Evaluator to delete, may be NULL
 */
static inline void ShipPairingDelete(ShipPairingObject* ship_pairing) {
  if (ship_pairing != NULL) {
    SHIP_PAIRING_DESTRUCT(ship_pairing);
    EEBUS_FREE(ship_pairing);
  }
}

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SHIP_SHIP_PAIRING_SHIP_PAIRING_H_
