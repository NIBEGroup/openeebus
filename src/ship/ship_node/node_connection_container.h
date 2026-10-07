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
 * @brief Node Connection Container implementation declarations
 */

#ifndef SRC_SHIP_SHIP_NODE_NODE_CONNECTION_CONTAINER_H_
#define SRC_SHIP_SHIP_NODE_NODE_CONNECTION_CONTAINER_H_

#include "src/common/eebus_malloc.h"
#include "src/ship/api/node_connection_container_interface.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

NodeConnectionContainerObject* NodeConnectionContainerCreate(void);

static inline void NodeConnectionContainerDelete(NodeConnectionContainerObject* node_connection_container) {
  if (node_connection_container != NULL) {
    NODE_CONNECTION_CONTAINER_DESTRUCT(node_connection_container);
    EEBUS_FREE(node_connection_container);
  }
}

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SHIP_SHIP_NODE_NODE_CONNECTION_CONTAINER_H_
