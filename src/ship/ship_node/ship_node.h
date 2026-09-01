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
 * @brief Ship Node owns and handles all connections to remote services
 */

#ifndef SRC_SHIP_SHIP_NODE_SHIP_NODE_H_
#define SRC_SHIP_SHIP_NODE_SHIP_NODE_H_

#include <stddef.h>

#include "src/common/eebus_device_info.h"
#include "src/common/service_details.h"
#include "src/ship/api/ship_mdns_interface.h"
#include "src/ship/api/ship_node_interface.h"
#include "src/ship/api/ship_node_reader_interface.h"
#include "src/ship/api/tls_certificate_interface.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @brief Create the new ShipNode instance
 * @param ssl_cert The loaded into memory SSL certificate
 * @param ssl_cert_size SSL certificate buffer size
 * Hint: see lws_context_creation_info Struct at:
 * https://libwebsockets.org/lws-api-doc-v4.0-stable/html/structlws__context__creation__info.html,
 * client_ssl_ca_mem and client_ssl_ca_mem fields
 * https://github.com/bertrandmartel/ssl-cert-generator-lib/blob/master/sslcertgenerator/main.cpp
 * ssl_gen variable
 */
ShipNodeObject* ShipNodeCreate(
    const char* ski,
    const char* role,
    const EebusDeviceInfo* device_info,
    const char* service_name,
    int port,
    const TlsCertificateObject* tls_certificate,
    ShipNodeReaderObject* ship_node_reader,
    ServiceDetails* local_service_details
);

static inline void ShipNodeDelete(ShipNodeObject* ship_node) {
    if (ship_node != NULL) {
        INFO_PROVIDER_DESTRUCT(INFO_PROVIDER_OBJECT(ship_node));
        EEBUS_FREE(ship_node);
    }
}

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SHIP_SHIP_NODE_SHIP_NODE_H_
