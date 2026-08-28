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
 * @brief Websocket Client Creator with URI parameter implementation
 */

#include "src/common/eebus_malloc.h"
#include "src/common/string_util.h"
#include "src/ship/api/tls_certificate_interface.h"
#include "src/ship/api/websocket_creator_interface.h"
#include "src/ship/websocket/websocket_client.h"

typedef struct WebsocketClientCreator WebsocketClientCreator;

struct WebsocketClientCreator {
    /** Implements the Websocket Creator Uri Interface */
    WebsocketCreatorObject obj;

    const char* uri;
    TlsCertificateObject* tls_cert;
    const char* remote_ski;
};

#define WEBSOCKET_CLIENT_CREATOR(obj) ((WebsocketClientCreator*)(obj))

static void Destruct(WebsocketCreatorObject* self);
WebsocketObject* Create(WebsocketCreatorObject* self, WebsocketCallback cb, void* ctx);

static const WebsocketCreatorInterface websocket_creator_methods = {
    .destruct         = Destruct,
    .create_websocket = Create,
};

static void WebsocketClientCreatorConstruct(
    WebsocketClientCreator* self,
    const char* uri,
    TlsCertificateObject* tls_cert,
    const char* remote_ski
);

void WebsocketClientCreatorConstruct(
    WebsocketClientCreator* self,
    const char* uri,
    TlsCertificateObject* tls_cert,
    const char* remote_ski
) {
    // Override "virtual functions table"
    WEBSOCKET_CREATOR_INTERFACE(self) = &websocket_creator_methods;

    self->uri        = StringCopy(uri);
    self->tls_cert   = tls_cert;
    self->remote_ski = StringCopy(remote_ski);
}

WebsocketCreatorObject*
WebsocketClientCreatorCreate(const char* uri, TlsCertificateObject* tls_cert, const char* remote_ski) {
    WebsocketClientCreator* const websocket_creator
        = (WebsocketClientCreator*)EEBUS_MALLOC(sizeof(WebsocketClientCreator));

    WebsocketClientCreatorConstruct(websocket_creator, uri, tls_cert, remote_ski);

    return WEBSOCKET_CREATOR_OBJECT(websocket_creator);
}

void Destruct(WebsocketCreatorObject* self) {
    WebsocketClientCreator* const ws = WEBSOCKET_CLIENT_CREATOR(self);
    StringDelete((char*)ws->uri);
    ws->uri = NULL;

    StringDelete((char*)ws->remote_ski);
    ws->remote_ski = NULL;
}

WebsocketObject* Create(WebsocketCreatorObject* self, WebsocketCallback cb, void* ctx) {
    WebsocketClientCreator* const wsc = WEBSOCKET_CLIENT_CREATOR(self);
    return WebsocketClientOpen(wsc->uri, wsc->tls_cert, wsc->remote_ski, cb, ctx);
}
