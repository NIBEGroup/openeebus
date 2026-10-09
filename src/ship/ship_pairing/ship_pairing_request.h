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
 * @brief Creating and announcing a shippairing request
 *
 * The node that asks to be trusted: it builds a request for one other node from
 * that node's SHIP ID, certificate fingerprint and secret, and it decides how
 * long the request keeps being announced
 * (SHIP Pairing Service TS 1.0.0, chapter 8 and section 4.2).
 *
 * An administrator supplies what a request is built from. Nothing here obtains
 * it, and nothing here announces a request that was not configured: section 4.5
 * recommends against a node re-initiating this on its own.
 */

#ifndef SRC_SHIP_SHIP_PAIRING_SHIP_PAIRING_REQUEST_H_
#define SRC_SHIP_SHIP_PAIRING_SHIP_PAIRING_REQUEST_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "src/common/eebus_malloc.h"
#include "src/ship/api/ship_pairing_entry.h"
#include "src/ship/api/tls_certificate_interface.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @brief Minutes of uninterrupted SHIP connection after which announcing stops,
 * section 4.2
 */
#define SHIP_PAIRING_REQUEST_SETTLED_MINUTES 15

/**
 * @brief What an administrator configures a request with
 */
typedef struct {
  /** @brief Service instance name to announce under, section 5.2 */
  const char* instance_name;

  /** @brief SHIP ID of the node being asked to trust this one */
  const char* for_id;

  /** @brief SHA-256 fingerprint of that node's certificate, 64 uppercase digits */
  const char* for_par;

  /** @brief Secret of that node, section 6.2 */
  const uint8_t* secret;

  /** @brief Size of @ref secret in octets */
  size_t secret_size;

  /** @brief SHIP ID of this node */
  const char* trust_id;

  /**
   * @brief Certificate of this node
   *
   * Its fingerprint and curve are calculated from it rather than supplied, so
   * that what a request claims about this node cannot disagree with what it
   * presents during a TLS handshake.
   */
  const TlsCertificateObject* tls_certificate;
} ShipPairingRequestConfig;

typedef struct ShipPairingRequest ShipPairingRequest;

/**
 * @brief Builds a shippairing request, chapter 8
 *
 * A new nonce is created for every request, so re-running the process with the
 * same node never reuses one (section 6.2).
 *
 * @param config What the request is built from
 * @return New request on success, NULL if the configuration is incomplete or a
 *         nonce could not be created
 */
ShipPairingRequest* ShipPairingRequestCreate(const ShipPairingRequestConfig* config);

/**
 * @brief Destructs the request and deallocates it
 * @param request Request to delete, may be NULL
 */
void ShipPairingRequestDelete(ShipPairingRequest* request);

/**
 * @brief Gets the service instance to announce
 * @param request Request to read
 * @return Entry holding the eleven TXT record keys, owned by @p request
 */
const ShipPairingEntry* ShipPairingRequestGetEntry(const ShipPairingRequest* request);

/**
 * @brief Reports whether the request should currently be announced
 *
 * Polled rather than reported through a callback, because what acts on it
 * registers and deregisters an mDNS service, which must not happen on a timer
 * callback's thread.
 *
 * @param request Request to read
 * @return true while the request is to be announced
 */
bool ShipPairingRequestIsAnnouncing(const ShipPairingRequest* request);

/**
 * @brief Reports that a SHIP connection with the addressed node was established
 *
 * Starts the fifteen minutes of section 4.2 counting. They measure an
 * uninterrupted connection, so a later disconnection resets them to zero.
 *
 * @param request Request to update
 */
void ShipPairingRequestNotifyConnected(ShipPairingRequest* request);

/**
 * @brief Reports that the SHIP connection with the addressed node was lost
 *
 * The next connection starts the fifteen minutes again from zero, which is also
 * what happens if this node restarts.
 *
 * @param request Request to update
 */
void ShipPairingRequestNotifyDisconnected(ShipPairingRequest* request);

/**
 * @brief Stops announcing the request
 *
 * For the case of section 4.2 where an administrator withdraws trust in the
 * addressed node while the request is still being announced, which has to take
 * effect immediately. A stopped request is never announced again; a corrected
 * one is a new request under a new instance name (section 5.5).
 *
 * @param request Request to stop
 */
void ShipPairingRequestStop(ShipPairingRequest* request);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SHIP_SHIP_PAIRING_SHIP_PAIRING_REQUEST_H_
