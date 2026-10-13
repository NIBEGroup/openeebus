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
 * @brief EEBUS CLI SHIP Pairing Service commands handling
 */

#ifndef SRC_CLI_EEBUS_CLI_PAIRING_H_
#define SRC_CLI_EEBUS_CLI_PAIRING_H_

#include "src/cli/eebus_cli_handler_interface.h"
#include "src/common/eebus_malloc.h"
#include "src/service/service/eebus_service.h"
#include "src/ship/api/tls_certificate_interface.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

EebusCliHandlerObject*
PairingCliCreate(EebusServiceObject* service, const TlsCertificateObject* tls_certificate, const char* ship_id);

static inline void PairingCliDelete(EebusCliHandlerObject* pairing_cli) {
  if (pairing_cli != NULL) {
    EEBUS_CLI_HANDLER_DESTRUCT(pairing_cli);
    EEBUS_FREE(pairing_cli);
  }
}

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_CLI_EEBUS_CLI_PAIRING_H_
