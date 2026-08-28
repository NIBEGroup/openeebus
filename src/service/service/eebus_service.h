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
 * @brief Service implementation declarations
 */

#ifndef SRC_SERVICE_SERVICE_EEBUS_SERVICE_H_
#define SRC_SERVICE_SERVICE_EEBUS_SERVICE_H_

#include <stddef.h>

#include "src/common/eebus_malloc.h"
#include "src/service/api/eebus_service_interface.h"
#include "src/service/api/service_reader_interface.h"
#include "src/ship/api/ship_node_interface.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

EebusServiceObject* EebusServiceCreate(
    const EebusServiceConfig* cfg,
    const char* role,
    const TlsCertificateObject* tls_certificate,
    ServiceReaderObject* service_reader
);

static inline void EebusServiceDelete(EebusServiceObject* eebus_service) {
    if (eebus_service != NULL) {
        SHIP_NODE_READER_DESTRUCT(SHIP_NODE_READER_OBJECT(eebus_service));
        EEBUS_FREE(eebus_service);
    }
}

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SERVICE_SERVICE_EEBUS_SERVICE_H_
