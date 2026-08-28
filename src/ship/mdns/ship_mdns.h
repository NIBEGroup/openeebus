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
 * @brief Mdns implementation declarations
 */

#ifndef SRC_SHIP_MDNS_MDNS_H_
#define SRC_SHIP_MDNS_MDNS_H_

#include <stddef.h>

#include "src/common/eebus_device_info.h"
#include "src/ship/api/ship_mdns_interface.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

// TODO: parameter types shall be adjusted
ShipMdnsObject* ShipMdnsCreate(
    const char* ski,
    const EebusDeviceInfo* device_info,
    const char* service_name,
    int port,
    OnMdnsEntriesFoundCallback cb,
    void* ctx
);

static inline void MdnsDelete(ShipMdnsObject* mdns) {
    if (mdns != NULL) {
        SHIP_MDNS_DESTRUCT(mdns);
        EEBUS_FREE(mdns);
    }
}

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SHIP_MDNS_MDNS_H_
