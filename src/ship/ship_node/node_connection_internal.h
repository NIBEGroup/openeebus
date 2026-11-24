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
#ifndef SRC_SHIP_SHIP_NODE_NODE_CONNECTION_INTERNAL_H_
#define SRC_SHIP_SHIP_NODE_NODE_CONNECTION_INTERNAL_H_

#include <stdbool.h>
#include <stdint.h>

#include "src/common/api/eebus_timer_interface.h"
#include "src/common/service_details.h"
#include "src/ship/api/node_connection_interface.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct NodeConnection NodeConnection;

struct NodeConnection {
  /** Implements the Node Connection Interface */
  NodeConnectionObject obj;

  const char* ski;
  char* ship_id;
  ShipConnectionObject* connection;
  /**
   * Id of the connection currently held in connection, 0 if there is none.
   *
   * Deferred events carry the id they were created with. The node compares it
   * together with the pointer, so that an event for a freed connection is not
   * mistaken for one about a new connection that reuses the same address.
   */
  uint64_t connection_id;
  uint64_t pending_approval_id;
  int attempt_cnt;
  bool is_attempt_running;
  bool is_outbound;  // the current connection is our own outbound (client) one
  bool handshake_complete;
  int yield_cnt;                     // inbound connections refused in favour of our outbound attempt (see should_yield)
  ShipConnectionObject* superseded;  // outbound connection replaced by an inbound one, awaiting its close
  /** Id of the connection in the superseded slot, 0 if there is none. */
  uint64_t superseded_id;
  int inbound_hs_cnt;  // inbound handshakes from this SKI that are running
  bool is_trusted;
  bool provisional;
  bool trusted_by_pairing;
  /** Expected certificate fingerprint, from configuration or an accepted pairing request. */
  char* expected_fingerprint;
  /** Fingerprint of the certificate presented on the current incoming connection. */
  char* peer_fingerprint;
  ServiceDetails* service_details;
  struct ShipNode* owner;
  EebusTimerObject* retry_timer;
};

#define NODE_CONNECTION(obj) ((NodeConnection*)(obj))

/** Caller holds the node mutex. A token also distinguishes reused addresses. */
static inline bool
NodeConnectionHasPendingApproval(const NodeConnection* self, const ShipConnectionObject* sc, uint64_t approval_id) {
  return (self != NULL) && (sc != NULL) && (self->connection == sc) && (approval_id != 0)
         && (self->pending_approval_id == approval_id);
}

/**
 * @brief Whether a deferred event refers to the connection currently held by the peer.
 *
 * Caller holds the node mutex. The id also distinguishes a freed connection from
 * a replacement that was allocated at the same address.
 */
static inline bool
NodeConnectionIsCurrentConnection(const NodeConnection* self, const ShipConnectionObject* sc, uint64_t connection_id) {
  return (self != NULL) && (sc != NULL) && (connection_id != 0) && (self->connection == sc)
         && (self->connection_id == connection_id);
}

/**
 * @brief Whether a deferred event refers to the connection in the superseded slot.
 *
 * Caller holds the node mutex.
 */
static inline bool NodeConnectionIsSupersededConnection(
    const NodeConnection* self,
    const ShipConnectionObject* sc,
    uint64_t connection_id
) {
  return (self != NULL) && (sc != NULL) && (connection_id != 0) && (self->superseded == sc)
         && (self->superseded_id == connection_id);
}

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  // SRC_SHIP_SHIP_NODE_NODE_CONNECTION_INTERNAL_H_
