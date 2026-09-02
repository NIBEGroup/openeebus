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
 * @brief EEBUS Home Energy Management System Service
 */

#ifndef EXAMPLES_HEMS_HEMS_H_
#define EXAMPLES_HEMS_HEMS_H_

#include <stddef.h>
#include <stdint.h>

#include "src/common/eebus_malloc.h"
#include "src/service/api/service_reader_interface.h"
#include "src/ship/api/tls_certificate_interface.h"
#include "src/use_case/model/scaled_value.h"

typedef struct HemsObject HemsObject;

struct HemsObject {
  /** Implement service reader interface */
  ServiceReaderObject service_reader;
};

#define HEMS_OBJECT(obj) ((HemsObject*)(obj))

/**
 * @brief Open the EEBUS Home Energy Management System Service
 * @param port Port to be used in mDNS server
 * @param role Role "client", "server" or "auto" to be used in SHIP connection
 * @param tls_certificate TLS Certificate object containing the certificate and private key
 * @return Pointer to the EEBUS Home Energy Management System Service instance
 */
HemsObject* HemsOpen(int32_t port, const char* role, TlsCertificateObject* tls_certificate);

static inline void HemsClose(HemsObject* hpsrv) {
  if (hpsrv != NULL) {
    SERVICE_READER_DESTRUCT(SERVICE_READER_OBJECT(hpsrv));
    EEBUS_FREE(hpsrv);
  }
}

/**
 * @brief Register remote SKI to the EEBUS Home Energy Management System Service instance
 * @param self HEMS instance to register the remote SKI for
 * @param ski Remote SKI to be registered
 */
void HemsRegisterRemoteSki(HemsObject* self, const char* ski);

/**
 * @brief Unregister remote SKI from the EEBUS Home Energy Management System Service instance
 * @param self HEMS instance to unregister the remote SKI for
 * @param ski Remote SKI to be unregistered
 */
void HemsUnregisterRemoteSki(HemsObject* self, const char* ski);

/**
 * @brief Set the EG LPC remote entity address to be used for sending the power limit
 * @param self HEMS instance to set the remote entity for
 * @param entity_addr Pointer to the entity address
 */
void HemsSetEgLpcRemoteEntity(HemsObject* self, const EntityAddressType* entity_addr);

/**
 * @brief Set the EG LPP remote entity address to be used for sending the power limit
 * @param self HEMS instance to set the remote entity for
 * @param entity_addr Pointer to the entity address
 */
void HemsSetEgLppRemoteEntity(HemsObject* self, const EntityAddressType* entity_addr);

/**
 * @brief Add the MA MPC remote entity address to be used for sending the power limit
 * @param self HEMS instance to add the remote entity for
 * @param entity_addr Pointer to the entity address
 */
void HemsAddMaMpcRemoteEntity(HemsObject* self, const EntityAddressType* entity_addr);

/**
 * @brief Remove the MA MPC remote entity address
 * @param self HEMS instance to remove the remote entity for
 * @param entity_addr Pointer to the entity address
 */
void HemsRemoveMaMpcRemoteEntity(HemsObject* self, const EntityAddressType* entity_addr);

/**
 * @brief Set the MA MGCP remote entity address
 * @param self HEMS instance to set the remote entity for
 * @param entity_addr Pointer to the entity address (NULL = disconnected)
 */
void HemsSetMaMgcpRemoteEntity(HemsObject* self, const EntityAddressType* entity_addr);

/**
 * @brief Handle command line input
 * @param self HEMS instance to handle the command for
 * @param cmd Command string to be handled
 */
void HemsHandleCmd(HemsObject* self, char* cmd);

#endif  // EXAMPLES_HEMS_HEMS_H_
