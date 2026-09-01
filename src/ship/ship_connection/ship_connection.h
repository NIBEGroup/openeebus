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
#ifndef SRC_SHIP_SHIP_CONNECTION_SHIP_CONNECTION_H_
#define SRC_SHIP_SHIP_CONNECTION_SHIP_CONNECTION_H_

#include "src/common/debug.h"
#include "src/common/eebus_malloc.h"
#include "src/ship/api/info_provider_interface.h"
#include "src/ship/api/ship_connection_interface.h"
#include "src/ship/api/tls_certificate_interface.h"
#include "src/ship/ship_connection/types.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

ShipConnectionObject* ShipConnectionCreate(
    InfoProviderObject* info_provider,
    ShipRole role,
    const char* local_ship_id,
    const char* remote_ski,
    const char* remote_ship_id
);

static inline void ShipConnectionDelete(ShipConnectionObject* ship_connection) {
    if (ship_connection != NULL) {
        SHIP_CONNECTION_DESTRUCT(ship_connection);
        EEBUS_FREE(ship_connection);
    }
}

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SHIP_SHIP_CONNECTION_SHIP_CONNECTION_H_
