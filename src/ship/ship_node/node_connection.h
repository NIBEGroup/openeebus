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
 * @brief Node Connection implementation declarations
 */

#ifndef SRC_SHIP_SHIP_NODE_NODE_CONNECTION_H_
#define SRC_SHIP_SHIP_NODE_NODE_CONNECTION_H_

#include "src/common/eebus_malloc.h"
#include "src/ship/api/node_connection_interface.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

NodeConnectionObject* NodeConnectionCreate(const char* ski, struct ShipNode* owner, NodeConnectionRetryFn retry_fn);

static inline void NodeConnectionDelete(NodeConnectionObject* node_connection) {
  if (node_connection != NULL) {
    NODE_CONNECTION_DESTRUCT(node_connection);
    EEBUS_FREE(node_connection);
  }
}

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SHIP_SHIP_NODE_NODE_CONNECTION_H_
