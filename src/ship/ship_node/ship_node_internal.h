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
#include <stddef.h>

#include "src/common/api/eebus_mutex_interface.h"
#include "src/common/api/eebus_queue_interface.h"
#include "src/common/api/eebus_thread_interface.h"
#include "src/common/service_details.h"
#include "src/common/api/eebus_timer_interface.h"
#include "src/common/string_lut.h"
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
  char*                  ski;               /* owned copy */
  ShipConnectionObject*  connection;        /* NULL = not connected */
  int                    attempt_cnt;
  bool                   is_attempt_running;
  bool                   handshake_complete; /* true once kDataExchange confirmed */
  ServiceDetails*        service_details;   /* owned */
};

#define SHIP_NODE_MAX_CONNECTIONS 10

typedef struct ShipNode ShipNode;

struct ShipNode {
  /** Implements the Ship Node Interface */
  ShipNodeObject sc_object;

  EebusQueueObject* msg_queue;
  ShipMdnsObject* mdns;
  Vector* mdns_entries;
  EebusMutexObject* mutex;
  bool search_for_remote_ski;
  bool cancel;
  EebusThreadObject* connection_thread;

  StringLut           connections;     /* SKI → ConnectionMapping* (owned) */
  size_t              max_connections; /* default SHIP_NODE_MAX_CONNECTIONS */
  EebusTimerObject*   retry_timer;     /* single deferred retry timer */
  ShipNodeReaderObject* ship_node_reader;
  const TlsCertificateObject* tsl_certificate;
  ServiceDetails* local_service_details;
  WebsocketCreatorObject* websocket_creator;
  HttpServerObject* http_server;
  ShipRole role;
};

#define SHIP_NODE(obj) ((ShipNode*)(obj))

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SHIP_SHIP_NODE_SHIP_NODE_INTERNAL_H_
