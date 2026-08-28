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
 * @brief Websocket Creator interface declarations
 */

#ifndef SRC_SHIP_API_WEBSOCKET_CREATOR_INTERFACE_H_
#define SRC_SHIP_API_WEBSOCKET_CREATOR_INTERFACE_H_

#include "src/ship/api/websocket_interface.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @brief Websocket Creator Interface
 * (Websocket Creator "virtual functions table" declaration)
 */
typedef struct WebsocketCreatorInterface WebsocketCreatorInterface;

/**
 * @brief Websocket Creator Object type definition
 * ("abstract class", has no members but only pointer to
 * "virtual functions table")
 */
typedef struct WebsocketCreatorObject WebsocketCreatorObject;

/**
 * @brief WebsocketCreator Interface Structure
 */
struct WebsocketCreatorInterface {
    void (*destruct)(WebsocketCreatorObject* self);
    WebsocketObject* (*create_websocket)(WebsocketCreatorObject* self, WebsocketCallback cb, void* ctx);
};

/**
 * @brief Websocket Creator Object Structure
 */
struct WebsocketCreatorObject {
    const WebsocketCreatorInterface* interface_;
};

/**
 * @brief Websocket Creator pointer typecast
 */
#define WEBSOCKET_CREATOR_OBJECT(obj) ((WebsocketCreatorObject*)(obj))

/**
 * @brief Websocket Creator Interface class pointer typecast
 */
#define WEBSOCKET_CREATOR_INTERFACE(obj) (WEBSOCKET_CREATOR_OBJECT(obj)->interface_)

/**
 * @brief Websocket Creator Destruct caller definition
 */
#define WEBSOCKET_CREATOR_DESTRUCT(obj) (WEBSOCKET_CREATOR_INTERFACE(obj)->destruct(obj))

/**
 * @brief Websocket Creator Create Websocket caller definition
 */
#define WEBSOCKET_CREATOR_CREATE_WEBSOCKET(obj, cb, ctx) \
    (WEBSOCKET_CREATOR_INTERFACE(obj)->create_websocket(obj, cb, ctx))

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SHIP_API_WEBSOCKET_CREATOR_INTERFACE_H_
