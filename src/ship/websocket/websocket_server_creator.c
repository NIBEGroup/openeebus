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
 * @brief Websocket Creator with WSI parameter implementation
 */

#include <libwebsockets.h>

#include "src/common/eebus_arguments.h"
#include "src/common/eebus_malloc.h"
#include "src/ship/api/http_server_interface.h"
#include "src/ship/api/websocket_creator_interface.h"
#include "src/ship/websocket/websocket_server.h"

typedef struct WebsocketCreatorWsi WebsocketCreatorWsi;

struct WebsocketCreatorWsi {
    /** Implements the Websocket Creator Wsi Interface */
    WebsocketCreatorObject obj;
    HttpServerObject* srv;
    struct lws* wsi;
};

#define WEBSOCKET_SERVER_CREATOR(obj) ((WebsocketCreatorWsi*)(obj))

static void Destruct(WebsocketCreatorObject* self);
static WebsocketObject* Create(WebsocketCreatorObject* self, WebsocketCallback cb, void* ctx);

static const WebsocketCreatorInterface websocket_creator_methods = {
    .destruct         = Destruct,
    .create_websocket = Create,
};

static void WebsocketCreatorConstruct(WebsocketCreatorWsi* self, HttpServerObject* srv, struct lws* wsi);

void WebsocketCreatorConstruct(WebsocketCreatorWsi* self, HttpServerObject* srv, struct lws* wsi) {
    // Override "virtual functions table"
    WEBSOCKET_CREATOR_INTERFACE(self) = &websocket_creator_methods;

    self->srv = srv;
    self->wsi = wsi;
}

WebsocketCreatorObject* WebsocketServerCreatorCreate(HttpServerObject* srv, struct lws* wsi) {
    WebsocketCreatorWsi* const websocket_creator = (WebsocketCreatorWsi*)EEBUS_MALLOC(sizeof(WebsocketCreatorWsi));

    WebsocketCreatorConstruct(websocket_creator, srv, wsi);

    return WEBSOCKET_CREATOR_OBJECT(websocket_creator);
}

void Destruct(WebsocketCreatorObject* self) {
    UNUSED(self);
    // WSI release is handled by the WebsocketServer, since it provides the WSI
}

WebsocketObject* Create(WebsocketCreatorObject* self, WebsocketCallback cb, void* ctx) {
    WebsocketCreatorWsi* const wsc = WEBSOCKET_SERVER_CREATOR(self);
    return WebsocketServerOpen(wsc->srv, wsc->wsi, cb, ctx);
}
