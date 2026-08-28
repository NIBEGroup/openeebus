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
 * @brief Http Server interface declarations
 */

#ifndef SRC_SHIP_API_HTTP_SERVER_INTERFACE_H_
#define SRC_SHIP_API_HTTP_SERVER_INTERFACE_H_

#include "src/common/eebus_errors.h"
#include "src/ship/api/websocket_creator_interface.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @brief On server connection established callback type definition
 */
typedef int (*WebsocketServerCallbackType)(const char* ski, WebsocketCreatorObject* wsc, void* ctx);

/**
 * @brief Http Server Interface
 * (Http Server "virtual functions table" declaration)
 */
typedef struct HttpServerInterface HttpServerInterface;

/**
 * @brief Http Server Object type definition
 * ("abstract class", has no members but only pointer to
 * "virtual functions table")
 */
typedef struct HttpServerObject HttpServerObject;

/**
 * @brief HttpServer Interface Structure
 */
struct HttpServerInterface {
    void (*destruct)(HttpServerObject* self);
    EebusError (*start)(HttpServerObject* self);
    void (*stop)(HttpServerObject* self);
};

/**
 * @brief Http Server Object Structure
 */
struct HttpServerObject {
    const HttpServerInterface* interface_;
};

/**
 * @brief Http Server pointer typecast
 */
#define HTTP_SERVER_OBJECT(obj) ((HttpServerObject*)(obj))

/**
 * @brief Http Server Interface class pointer typecast
 */
#define HTTP_SERVER_INTERFACE(obj) (HTTP_SERVER_OBJECT(obj)->interface_)

/**
 * @brief Http Server Destruct caller definition
 */
#define HTTP_SERVER_DESTRUCT(obj) (HTTP_SERVER_INTERFACE(obj)->destruct(obj))

/**
 * @brief Http Server Start caller definition
 */
#define HTTP_SERVER_START(obj) (HTTP_SERVER_INTERFACE(obj)->start(obj))

/**
 * @brief Http Server Stop caller definition
 */
#define HTTP_SERVER_STOP(obj) (HTTP_SERVER_INTERFACE(obj)->stop(obj))

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SHIP_API_HTTP_SERVER_INTERFACE_H_
