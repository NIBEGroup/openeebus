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
 * @brief Node Connection Container interface declarations
 */

#ifndef SRC_SHIP_API_NODE_CONNECTION_CONTAINER_INTERFACE_H_
#define SRC_SHIP_API_NODE_CONNECTION_CONTAINER_INTERFACE_H_

#include <stdbool.h>
#include <stddef.h>

#include "src/ship/api/node_connection_interface.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

struct ShipNode;

/**
 * @brief Node Connection Container Interface
 * (Node Connection Container "virtual functions table" declaration)
 */
typedef struct NodeConnectionContainerInterface NodeConnectionContainerInterface;

/**
 * @brief Node Connection Container Object type definition
 * ("abstract class", has no members but only pointer to
 * "virtual functions table")
 */
typedef struct NodeConnectionContainerObject NodeConnectionContainerObject;

/**
 * @brief Node Connection Container Interface Structure
 */
struct NodeConnectionContainerInterface {
  void (*destruct)(NodeConnectionContainerObject* self);
  NodeConnectionObject* (*get_or_create)(
      NodeConnectionContainerObject* self,
      const char* ski,
      struct ShipNode* owner,
      NodeConnectionRetryFn retry_fn
  );
  NodeConnectionObject* (*find_with_ski)(NodeConnectionContainerObject* self, const char* ski);
  NodeConnectionObject* (*find_with_ship_connection)(
      NodeConnectionContainerObject* self,
      const ShipConnectionObject* sc
  );
  void (*remove_with_ski)(NodeConnectionContainerObject* self, const char* ski);
  bool (*is_ski_trusted)(const NodeConnectionContainerObject* self, const char* ski);
  bool (*is_ski_connected)(const NodeConnectionContainerObject* self, const char* ski);
  size_t (*get_size)(const NodeConnectionContainerObject* self);
  NodeConnectionObject* (*get_with_index)(NodeConnectionContainerObject* self, size_t i);
};

/**
 * @brief Node Connection Container Object Structure
 */
struct NodeConnectionContainerObject {
  const NodeConnectionContainerInterface* interface_;
};

/**
 * @brief Node Connection Container pointer typecast
 */
#define NODE_CONNECTION_CONTAINER_OBJECT(obj) ((NodeConnectionContainerObject*)(obj))

/**
 * @brief Node Connection Container Interface class pointer typecast
 */
#define NODE_CONNECTION_CONTAINER_INTERFACE(obj) (NODE_CONNECTION_CONTAINER_OBJECT(obj)->interface_)

/**
 * @brief Node Connection Container Destruct caller definition
 */
#define NODE_CONNECTION_CONTAINER_DESTRUCT(obj) (NODE_CONNECTION_CONTAINER_INTERFACE(obj)->destruct(obj))

/**
 * @brief Node Connection Container Get Or Create caller definition
 */
#define NODE_CONNECTION_CONTAINER_GET_OR_CREATE(obj, ski, owner, retry_fn) \
  (NODE_CONNECTION_CONTAINER_INTERFACE(obj)->get_or_create(obj, ski, owner, retry_fn))

/**
 * @brief Node Connection Container Find With SKI caller definition
 */
#define NODE_CONNECTION_CONTAINER_FIND_WITH_SKI(obj, ski) \
  (NODE_CONNECTION_CONTAINER_INTERFACE(obj)->find_with_ski(obj, ski))

/**
 * @brief Node Connection Container Find With Ship Connection caller definition
 */
#define NODE_CONNECTION_CONTAINER_FIND_WITH_SHIP_CONNECTION(obj, sc) \
  (NODE_CONNECTION_CONTAINER_INTERFACE(obj)->find_with_ship_connection(obj, sc))

/**
 * @brief Node Connection Container Remove With SKI caller definition
 */
#define NODE_CONNECTION_CONTAINER_REMOVE_WITH_SKI(obj, ski) \
  (NODE_CONNECTION_CONTAINER_INTERFACE(obj)->remove_with_ski(obj, ski))

/**
 * @brief Node Connection Container Is SKI Trusted caller definition
 */
#define NODE_CONNECTION_CONTAINER_IS_SKI_TRUSTED(obj, ski) \
  (NODE_CONNECTION_CONTAINER_INTERFACE(obj)->is_ski_trusted(obj, ski))

/**
 * @brief Node Connection Container Is SKI Connected caller definition
 */
#define NODE_CONNECTION_CONTAINER_IS_SKI_CONNECTED(obj, ski) \
  (NODE_CONNECTION_CONTAINER_INTERFACE(obj)->is_ski_connected(obj, ski))

/**
 * @brief Node Connection Container Get Size caller definition
 */
#define NODE_CONNECTION_CONTAINER_GET_SIZE(obj) (NODE_CONNECTION_CONTAINER_INTERFACE(obj)->get_size(obj))

/**
 * @brief Node Connection Container Get With Index caller definition
 */
#define NODE_CONNECTION_CONTAINER_GET_WITH_INDEX(obj, i) \
  (NODE_CONNECTION_CONTAINER_INTERFACE(obj)->get_with_index(obj, i))

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SHIP_API_NODE_CONNECTION_CONTAINER_INTERFACE_H_
