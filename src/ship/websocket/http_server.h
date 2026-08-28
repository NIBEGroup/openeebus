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
 * @brief Http Server implementation declarations
 */

#ifndef SRC_SHIP_WEBSOCKET_HTTP_SERVER_H_
#define SRC_SHIP_WEBSOCKET_HTTP_SERVER_H_

#include <stddef.h>

#include "src/common/eebus_malloc.h"
#include "src/ship/api/http_server_interface.h"
#include "src/ship/api/tls_certificate_interface.h"
#include "src/ship/api/websocket_creator_interface.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

HttpServerObject* HttpServerCreate(
    int port,
    const TlsCertificateObject* tls_cert,
    WebsocketServerCallbackType conn_establish_cb,
    void* conn_establish_ctx
);

static inline void HttpServerDelete(HttpServerObject* http_server) {
    if (http_server != NULL) {
        HTTP_SERVER_DESTRUCT(http_server);
        EEBUS_FREE(http_server);
    }
}

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SHIP_WEBSOCKET_HTTP_SERVER_H_
