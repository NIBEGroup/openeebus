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
 * @brief Websocket Uri implementation declarations
 */

#ifndef SRC_SHIP_WEBSOCKET_WEBSOCKET_CLIENT_H_
#define SRC_SHIP_WEBSOCKET_WEBSOCKET_CLIENT_H_

#include <stddef.h>

#include "src/ship/api/tls_certificate_interface.h"
#include "src/ship/api/websocket_interface.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

WebsocketObject* WebsocketClientOpen(
    const char* uri,
    const TlsCertificateObject* tls_cert,
    const char* remote_ski,
    WebsocketCallback cb,
    void* ctx
);

/** Verifies the certificate fingerprint before allowing SHIP messages. */
WebsocketObject* WebsocketClientOpenWithFingerprint(
    const char* uri,
    const TlsCertificateObject* tls_cert,
    const char* remote_ski,
    const char* remote_fingerprint,
    WebsocketCallback cb,
    void* ctx
);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SHIP_WEBSOCKET_WEBSOCKET_CLIENT_H_
