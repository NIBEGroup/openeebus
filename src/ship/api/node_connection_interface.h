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
 * @brief Node Connection interface declarations
 */

#ifndef SRC_SHIP_API_NODE_CONNECTION_INTERFACE_H_
#define SRC_SHIP_API_NODE_CONNECTION_INTERFACE_H_

#include <stdbool.h>
#include <stdint.h>

#include "src/common/eebus_errors.h"
#include "src/ship/api/ship_connection_interface.h"
#include "src/ship/api/websocket_creator_interface.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

struct ShipNode;

typedef void (*NodeConnectionRetryFn)(void* ctx);

/**
 * @brief Node Connection Interface
 * (Node Connection "virtual functions table" declaration)
 */
typedef struct NodeConnectionInterface NodeConnectionInterface;

/**
 * @brief Node Connection Object type definition
 * ("abstract class", has no members but only pointer to
 * "virtual functions table")
 */
typedef struct NodeConnectionObject NodeConnectionObject;

/**
 * @brief Node Connection Interface Structure
 */
struct NodeConnectionInterface {
  void (*destruct)(NodeConnectionObject* self);
  const char* (*get_ski)(const NodeConnectionObject* self);
  struct ShipNode* (*get_owner)(const NodeConnectionObject* self);
  bool (*is_attempt_running)(const NodeConnectionObject* self);
  bool (*owns_connection)(const NodeConnectionObject* self, const ShipConnectionObject* sc);
  ShipConnectionObject* (*release_ship_connection)(NodeConnectionObject* self);
  int (*on_connection_closed)(NodeConnectionObject* self);
  void (*stop_retry_timer)(NodeConnectionObject* self);
  void (*schedule_retry)(NodeConnectionObject* self, uint32_t delay_ms);
  bool (*on_handshake_complete)(NodeConnectionObject* self);
  EebusError (*client_connect)(NodeConnectionObject* self, WebsocketCreatorObject* wsc, const char* ship_id);
  EebusError (*server_connect)(NodeConnectionObject* self, WebsocketCreatorObject* wsc, const char* ship_id);
};

/**
 * @brief Node Connection Object Structure
 */
struct NodeConnectionObject {
  const NodeConnectionInterface* interface_;
};

/**
 * @brief Node Connection pointer typecast
 */
#define NODE_CONNECTION_OBJECT(obj) ((NodeConnectionObject*)(obj))

/**
 * @brief Node Connection Interface class pointer typecast
 */
#define NODE_CONNECTION_INTERFACE(obj) (NODE_CONNECTION_OBJECT(obj)->interface_)

/**
 * @brief Node Connection Destruct caller definition
 */
#define NODE_CONNECTION_DESTRUCT(obj) (NODE_CONNECTION_INTERFACE(obj)->destruct(obj))

/**
 * @brief Node Connection Get SKI caller definition
 */
#define NODE_CONNECTION_GET_SKI(obj) (NODE_CONNECTION_INTERFACE(obj)->get_ski(obj))

/**
 * @brief Node Connection Get Owner caller definition
 */
#define NODE_CONNECTION_GET_OWNER(obj) (NODE_CONNECTION_INTERFACE(obj)->get_owner(obj))

/**
 * @brief Node Connection Is Attempt Running caller definition
 */
#define NODE_CONNECTION_IS_ATTEMPT_RUNNING(obj) (NODE_CONNECTION_INTERFACE(obj)->is_attempt_running(obj))

/**
 * @brief Node Connection Owns Connection caller definition
 */
#define NODE_CONNECTION_OWNS_CONNECTION(obj, sc) (NODE_CONNECTION_INTERFACE(obj)->owns_connection(obj, sc))

/**
 * @brief Node Connection Release Ship Connection caller definition
 */
#define NODE_CONNECTION_RELEASE_SHIP_CONNECTION(obj) (NODE_CONNECTION_INTERFACE(obj)->release_ship_connection(obj))

/**
 * @brief Node Connection On Connection Closed caller definition
 */
#define NODE_CONNECTION_ON_CONNECTION_CLOSED(obj) (NODE_CONNECTION_INTERFACE(obj)->on_connection_closed(obj))

/**
 * @brief Node Connection Stop Retry Timer caller definition
 */
#define NODE_CONNECTION_STOP_RETRY_TIMER(obj) (NODE_CONNECTION_INTERFACE(obj)->stop_retry_timer(obj))

/**
 * @brief Node Connection Schedule Retry caller definition
 */
#define NODE_CONNECTION_SCHEDULE_RETRY(obj, delay_ms) (NODE_CONNECTION_INTERFACE(obj)->schedule_retry(obj, delay_ms))

/**
 * @brief Node Connection On Handshake Complete caller definition
 */
#define NODE_CONNECTION_ON_HANDSHAKE_COMPLETE(obj) (NODE_CONNECTION_INTERFACE(obj)->on_handshake_complete(obj))

/**
 * @brief Node Connection Client Connect caller definition
 */
#define NODE_CONNECTION_CLIENT_CONNECT(obj, wsc, ship_id) \
  (NODE_CONNECTION_INTERFACE(obj)->client_connect(obj, wsc, ship_id))

/**
 * @brief Node Connection Server Connect caller definition
 */
#define NODE_CONNECTION_SERVER_CONNECT(obj, wsc, ship_id) \
  (NODE_CONNECTION_INTERFACE(obj)->server_connect(obj, wsc, ship_id))

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SHIP_API_NODE_CONNECTION_INTERFACE_H_
