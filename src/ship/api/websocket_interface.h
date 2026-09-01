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
 * @brief Transformed from api/websocket.go in a specific way
 * WebsocketDataWriterInterface is replaced with WebsocketInterface
 * WebsocketDataReaderInterface is replaced with related callback
 *
 * Websocket Interface declarations
 *
 * Interface for handling the actual remote device data connection
 *
 * Implemented by Websocket, used by ShipConnection
 */

#ifndef SRC_SHIP_API_WEBSOCKET_INTERFACE_H_
#define SRC_SHIP_API_WEBSOCKET_INTERFACE_H_

#include <stdbool.h>
#include <stdint.h>

#include "src/ship/model/types.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

enum WebsocketCallbackType {
    kWebsocketCallbackTypeError,
    kWebsocketCallbackTypeRead,
    kWebsocketCallbackTypeClose,
};

typedef enum WebsocketCallbackType WebsocketCallbackType;

/**
 * @brief On message read callback type definition
 */
typedef void (*WebsocketCallback)(WebsocketCallbackType type, const void* in, size_t size, void* ctx);

/**
 * @brief Websocket Interface
 * (Websocket "virtual functions table" declaration)
 *
 * Transformed from WebsocketDataWriterInterface and
 * WebsocketDataReaderInterface
 */
typedef struct WebsocketInterface WebsocketInterface;

/**
 * @brief Websocket Object type definition
 * ("abstract class", has no members but only pointer to
 * "virtual functions table")
 */
typedef struct WebsocketObject WebsocketObject;

/**
 * @brief Websocket Interface Structure
 */
struct WebsocketInterface {
    void (*destruct)(WebsocketObject* self);
    int32_t (*write)(WebsocketObject* self, const uint8_t* msg, size_t msg_size);
    void (*close)(WebsocketObject* self, int32_t close_code, const char* reason);
    bool (*is_closed)(const WebsocketObject* self);
    int32_t (*get_close_error)(const WebsocketObject* self);
    void (*schedule_write)(WebsocketObject* self);
};

/**
 * @brief Websocket Object Structure
 */
struct WebsocketObject {
    const WebsocketInterface* interface_;
};

/**
 * @brief Websocket pointer typecast
 */
#define WEBSOCKET_OBJECT(obj) ((WebsocketObject*)(obj))

/**
 * @brief Websocket Interface class pointer typecast
 */
#define WEBSOCKET_INTERFACE(obj) (WEBSOCKET_OBJECT(obj)->interface_)

/**
 * @brief Websocket Destruct caller definition
 */
#define WEBSOCKET_DESTRUCT(obj) (WEBSOCKET_INTERFACE(obj)->destruct(obj))

/**
 * @brief Websocket Write caller definition
 */
#define WEBSOCKET_WRITE(obj, msg, msg_size) (WEBSOCKET_INTERFACE(obj)->write(obj, msg, msg_size))

/**
 * @brief Websocket Close caller definition
 */
#define WEBSOCKET_CLOSE(obj, close_code, reason) (WEBSOCKET_INTERFACE(obj)->close(obj, close_code, reason))

/**
 * @brief Websocket Is Closed caller definition
 */
#define WEBSOCKET_IS_CLOSED(obj) (WEBSOCKET_INTERFACE(obj)->is_closed(obj))

/**
 * @brief Websocket Get Close Error caller definition
 */
#define WEBSOCKET_GET_CLOSE_ERROR(obj) (WEBSOCKET_INTERFACE(obj)->get_close_error(obj))

/**
 * @brief Websocket Schedule Write caller definition
 */
#define WEBSOCKET_SCHEDULE_WRITE(obj) (WEBSOCKET_INTERFACE(obj)->schedule_write(obj))

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SHIP_API_WEBSOCKET_INTERFACE_H_
