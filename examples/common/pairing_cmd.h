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
 * @brief Console commands for SHIP Pairing Service, shared by the examples
 *
 * Enough to run both halves of the process against each other on one machine:
 * one example prints what an administrator would collect about it, the other is
 * given those values and announces a request built from them
 * (SHIP Pairing Service TS 1.0.0, section 4.2).
 */

#ifndef EXAMPLES_COMMON_PAIRING_CMD_H_
#define EXAMPLES_COMMON_PAIRING_CMD_H_

#include <stdbool.h>

#include "src/service/service/eebus_service.h"
#include "src/ship/api/tls_certificate_interface.h"
#include "src/ship/ship_pairing/ship_pairing_request.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @brief State the console commands keep between calls
 */
typedef struct {
  EebusServiceObject* service;
  const TlsCertificateObject* tls_certificate;
  const char* ship_id;

  /** The request this node is announcing, if it is asking to be trusted */
  ShipPairingRequest* request;
} PairingCmd;

/**
 * @brief Prepares the console commands
 */
void PairingCmdConstruct(
    PairingCmd* self,
    EebusServiceObject* service,
    const TlsCertificateObject* tls_certificate,
    const char* ship_id
);

/**
 * @brief Releases what the console commands own
 */
void PairingCmdDestruct(PairingCmd* self);

/**
 * @brief Handles a console command if it is one of these
 *
 * @param self Command state
 * @param cmd Whole command line
 * @return true if the command was handled here
 */
bool PairingCmdHandle(PairingCmd* self, const char* cmd);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // EXAMPLES_COMMON_PAIRING_CMD_H_
