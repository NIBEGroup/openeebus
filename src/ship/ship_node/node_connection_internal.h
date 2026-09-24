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
  ShipConnectionObject* connection;
  int attempt_cnt;
  bool is_attempt_running;
  bool handshake_complete;
  ServiceDetails* service_details;
  struct ShipNode* owner;
  EebusTimerObject* retry_timer;
};

#define NODE_CONNECTION(obj) ((NodeConnection*)(obj))

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  // SRC_SHIP_SHIP_NODE_NODE_CONNECTION_INTERNAL_H_
