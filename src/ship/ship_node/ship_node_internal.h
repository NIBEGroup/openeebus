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
#ifndef SRC_SHIP_SHIP_NODE_SHIP_NODE_INTERNAL_H_
#define SRC_SHIP_SHIP_NODE_SHIP_NODE_INTERNAL_H_

#include <stdbool.h>

#include "src/common/api/eebus_mutex_interface.h"
#include "src/common/api/eebus_queue_interface.h"
#include "src/common/api/eebus_thread_interface.h"
#include "src/common/service_details.h"
#include "src/ship/api/http_server_interface.h"
#include "src/ship/api/ship_connection_interface.h"
#include "src/ship/api/ship_mdns_interface.h"
#include "src/ship/api/ship_node_interface.h"
#include "src/ship/api/ship_node_reader_interface.h"
#include "src/ship/api/tls_certificate_interface.h"
#include "src/ship/api/websocket_creator_interface.h"
#include "src/ship/ship_connection/types.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct ConnectionMapping ConnectionMapping;

struct ConnectionMapping {
  const char* ski;
  ShipConnectionInterface* connection;
  /** Which attempt is it to initate an connection to the remote SKI */
  int attempt_cnt;
  bool is_attempt_running;
  ServiceDetails* service_details;
};

typedef struct ShipNode ShipNode;

struct ShipNode {
  /** Implements the Ship Node Interface */
  ShipNodeObject sc_object;

  EebusQueueObject* msg_queue;
  char* remote_ski;
  ShipMdnsObject* mdns;
  Vector* mdns_entries;
  EebusMutexObject* mutex;
  bool search_for_remote_ski;
  bool cancel;
  EebusThreadObject* connection_thread;

  ConnectionMapping* connections_table;
  ShipNodeReaderObject* ship_node_reader;
  const TlsCertificateObject* tsl_certificate;
  ServiceDetails* local_service_details;
  // Temporary single SHIP Connection object instance
  // for early stage of Ship Node development and testing.
  // To be replaces with multiple instances handling
  ShipConnectionObject* ship_connection;
  // Old client connection superseded by the simultaneous-open tiebreaker.
  // Stopped and freed by the DiscardSuperseded handler on the connection loop thread.
  ShipConnectionObject* superseded_connection;
  WebsocketCreatorObject* websocket_creator;
  HttpServerObject* http_server;
  bool connection_attempt_running;
  // True only while a CLIENT ShipConnection is alive.  Distinct from
  // connection_attempt_running (which is also true for active servers) so that
  // the SIMOPEN detector can tell whether there is a real concurrent outgoing
  // client, not just an already-established server.
  bool client_connection_running;
  ShipRole role;
};

#define SHIP_NODE(obj) ((ShipNode*)(obj))

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SHIP_SHIP_NODE_SHIP_NODE_INTERNAL_H_
