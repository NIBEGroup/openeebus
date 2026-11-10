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
  uint32_t (*on_connection_closed)(NodeConnectionObject* self);
  void (*stop_retry_timer)(NodeConnectionObject* self);
  void (*schedule_retry)(NodeConnectionObject* self, uint32_t delay_ms);
  bool (*on_handshake_complete)(NodeConnectionObject* self);
  EebusError (*client_connect)(NodeConnectionObject* self, WebsocketCreatorObject* wsc, const char* ship_id);
  EebusError (*server_connect)(NodeConnectionObject* self, WebsocketCreatorObject* wsc, const char* ship_id);
  bool (*is_outbound_in_flight)(const NodeConnectionObject* self);
  bool (*is_outbound_established)(const NodeConnectionObject* self);
  bool (*should_yield)(NodeConnectionObject* self);
  ShipConnectionObject* (*supersede_client)(NodeConnectionObject* self);
  void (*restore_superseded)(NodeConnectionObject* self);
  bool (*is_superseded)(const NodeConnectionObject* self, const ShipConnectionObject* sc);
  void (*release_superseded)(NodeConnectionObject* self, const ShipConnectionObject* sc);
  ShipConnectionObject* (*take_superseded)(NodeConnectionObject* self);
  ShipConnectionObject* (*detach_connection)(NodeConnectionObject* self, bool* was_established);
  void (*clear_attempt)(NodeConnectionObject* self);
  void (*on_inbound_handshake)(NodeConnectionObject* self, bool started);
  bool (*is_inbound_handshake)(const NodeConnectionObject* self);
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

/**
 * @brief Node Connection Is Outbound In Flight caller definition: our own
 * outbound (client) connection to this SKI is running and has not completed
 * its SHIP handshake yet. This is the "simultaneous open" case when an inbound
 * connection from the same SKI arrives.
 */
#define NODE_CONNECTION_IS_OUTBOUND_IN_FLIGHT(obj) (NODE_CONNECTION_INTERFACE(obj)->is_outbound_in_flight(obj))

/**
 * @brief Node Connection Is Outbound Established caller definition: our own
 * outbound (client) connection to this SKI has completed its SHIP handshake.
 * When an inbound connection from the same SKI arrives now, it is a second
 * connection to a peer that already has a working one.
 */
#define NODE_CONNECTION_IS_OUTBOUND_ESTABLISHED(obj) (NODE_CONNECTION_INTERFACE(obj)->is_outbound_established(obj))

/**
 * @brief Node Connection Should Yield caller definition: whether the smaller-SKI
 * side keeps yielding (refusing the inbound connection) to its own outbound
 * attempt. Bounded: yields once, then gives way, so a peer that keeps dialing in
 * while our attempt goes nowhere (e.g. a stale mDNS entry) is not refused forever.
 * Counts the yield, so call it once per inbound connection.
 */
#define NODE_CONNECTION_SHOULD_YIELD(obj) (NODE_CONNECTION_INTERFACE(obj)->should_yield(obj))

/**
 * @brief Node Connection Supersede Client caller definition: detach the in-flight
 * outbound connection into the "superseded" slot, to be replaced by a server
 * connection. Returns it, or NULL (nothing changed) if there is none or the slot
 * is still occupied. The caller must hand it to the node for closing
 * (see ShipNode's DiscardSuperseded message).
 */
#define NODE_CONNECTION_SUPERSEDE_CLIENT(obj) (NODE_CONNECTION_INTERFACE(obj)->supersede_client(obj))

/**
 * @brief Node Connection Restore Superseded caller definition: undo
 * NODE_CONNECTION_SUPERSEDE_CLIENT() when the replacing server connection could
 * not be created.
 */
#define NODE_CONNECTION_RESTORE_SUPERSEDED(obj) (NODE_CONNECTION_INTERFACE(obj)->restore_superseded(obj))

/**
 * @brief Node Connection Is Superseded caller definition
 */
#define NODE_CONNECTION_IS_SUPERSEDED(obj, sc) (NODE_CONNECTION_INTERFACE(obj)->is_superseded(obj, sc))

/**
 * @brief Node Connection Release Superseded caller definition: forget the
 * superseded connection sc (the caller then stops and deletes it).
 */
#define NODE_CONNECTION_RELEASE_SUPERSEDED(obj, sc) (NODE_CONNECTION_INTERFACE(obj)->release_superseded(obj, sc))

/**
 * @brief Node Connection Take Superseded caller definition: remove and return
 * the superseded connection, or NULL if there is none (used on shutdown).
 */
#define NODE_CONNECTION_TAKE_SUPERSEDED(obj) (NODE_CONNECTION_INTERFACE(obj)->take_superseded(obj))

/**
 * @brief Node Connection Detach Connection caller definition: take the current
 * connection (running or established) away so that a newer inbound connection
 * from the same SKI can replace it - SHIP 12.2.2, the newest connection wins.
 * Returns it (NULL if there is none) and whether it had completed its SHIP
 * handshake (the application was told it connected). The attempt stays marked as
 * running, so no competing attempt can start until the replacing connection is
 * created (NODE_CONNECTION_SERVER_CONNECT()) or the replacement is given up
 * (NODE_CONNECTION_CLEAR_ATTEMPT()). The caller must stop and delete the
 * returned connection.
 */
#define NODE_CONNECTION_DETACH_CONNECTION(obj, was_established) \
  (NODE_CONNECTION_INTERFACE(obj)->detach_connection(obj, was_established))

/**
 * @brief Node Connection Clear Attempt caller definition: no connection is
 * running any more (a replacement that could not be created).
 */
#define NODE_CONNECTION_CLEAR_ATTEMPT(obj) (NODE_CONNECTION_INTERFACE(obj)->clear_attempt(obj))

/**
 * @brief Node Connection On Inbound Handshake caller definition: the peer started (started == true) or finished
 * (started == false) connecting in. Handshakes are counted, so overlapping ones from the same peer are tracked.
 */
#define NODE_CONNECTION_ON_INBOUND_HANDSHAKE(obj, started) \
  (NODE_CONNECTION_INTERFACE(obj)->on_inbound_handshake(obj, started))

/**
 * @brief Node Connection Is Inbound Handshake caller definition: true while at least one inbound handshake from the
 * peer is running. Our own outbound attempt should be held back meanwhile.
 */
#define NODE_CONNECTION_IS_INBOUND_HANDSHAKE(obj) (NODE_CONNECTION_INTERFACE(obj)->is_inbound_handshake(obj))

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SHIP_API_NODE_CONNECTION_INTERFACE_H_
