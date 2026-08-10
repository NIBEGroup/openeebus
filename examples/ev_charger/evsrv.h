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
 * @brief EEBUS EV Charger Service
 */

#ifndef EXAMPLES_EV_CHARGER_EVSRV_H_
#define EXAMPLES_EV_CHARGER_EVSRV_H_

#include <stdint.h>

#include "src/common/eebus_errors.h"
#include "src/common/eebus_malloc.h"
#include "src/service/api/service_reader_interface.h"
#include "src/ship/api/tls_certificate_interface.h"

typedef struct EvsrvObject EvsrvObject;

struct EvsrvObject {
  ServiceReaderObject service_reader;
};

#define EVSRV_OBJECT(obj) ((EvsrvObject*)(obj))

/**
 * @brief Open the EEBUS EV Charger Service
 * @param port Port to be used in mDNS server
 * @param role Role "client", "server" or "auto"
 * @param tls_certificate TLS Certificate object
 * @return Pointer to the EV Charger Service instance, or NULL on failure
 */
EvsrvObject* EvsrvOpen(int32_t port, const char* role, TlsCertificateObject* tls_certificate);

static inline void EvsrvClose(EvsrvObject* evsrv) {
  if (evsrv != NULL) {
    SERVICE_READER_DESTRUCT(SERVICE_READER_OBJECT(evsrv));
    EEBUS_FREE(evsrv);
  }
}

/**
 * @brief Register a remote SKI
 * @param self EV Charger Service instance
 * @param ski Remote SKI string
 */
void EvsrvRegisterRemoteSki(EvsrvObject* self, const char* ski);

/**
 * @brief Unregister a remote SKI
 * @param self EV Charger Service instance
 * @param ski Remote SKI string
 */
void EvsrvUnregisterRemoteSki(EvsrvObject* self, const char* ski);

/**
 * @brief Set the total active power measurement (MU-MPC reporting to HEMS)
 * @param self EV Charger Service instance
 * @param power_total Total power in W scaled by 10^(-2) (e.g. 99000 = 990.00 W)
 * @return kEebusErrorOk on success
 */
EebusError EvsrvSetPowerTotal(EvsrvObject* self, int32_t power_total);

/**
 * @brief Handle a CLI command
 * @param self EV Charger Service instance
 * @param cmd Command string
 */
void EvsrvHandleCmd(EvsrvObject* self, char* cmd);

#endif  // EXAMPLES_EV_CHARGER_EVSRV_H_
