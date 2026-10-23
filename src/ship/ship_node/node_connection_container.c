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
 * @brief Node Connection Container implementation
 */

#include "node_connection_container.h"

#include <string.h>

#include "node_connection.h"
#include "node_connection_internal.h"
#include "src/common/eebus_malloc.h"
#include "src/common/string_util.h"
#include "src/common/vector.h"

typedef struct NodeConnectionContainer NodeConnectionContainer;

struct NodeConnectionContainer {
  /** Implements the Node Connection Container Interface */
  NodeConnectionContainerObject obj;

  Vector items;
};

#define NODE_CONNECTION_CONTAINER(obj) ((NodeConnectionContainer*)(obj))

static void Destruct(NodeConnectionContainerObject* self);
static NodeConnectionObject* GetOrCreate(
    NodeConnectionContainerObject* self,
    const char* ski,
    struct ShipNode* owner,
    NodeConnectionRetryFn retry_fn
);
static NodeConnectionObject* FindWithSki(NodeConnectionContainerObject* self, const char* ski);
static NodeConnectionObject* FindWithFingerprint(NodeConnectionContainerObject* self, const char* fingerprint);
static NodeConnectionObject* FindWithShipId(NodeConnectionContainerObject* self, const char* ship_id);
static NodeConnectionObject*
FindWithShipConnection(NodeConnectionContainerObject* self, const ShipConnectionObject* sc);
static void Remove(NodeConnectionContainerObject* self, NodeConnectionObject* nc);
static void RemoveWithSki(NodeConnectionContainerObject* self, const char* ski);
static bool IsSkiTrusted(const NodeConnectionContainerObject* self, const char* ski);
static bool IsSkiConnected(const NodeConnectionContainerObject* self, const char* ski);
static size_t GetSize(const NodeConnectionContainerObject* self);
static NodeConnectionObject* GetWithIndex(NodeConnectionContainerObject* self, size_t i);

static const NodeConnectionContainerInterface node_connection_container_methods = {
    .destruct                  = Destruct,
    .get_or_create             = GetOrCreate,
    .find_with_ski             = FindWithSki,
    .find_with_fingerprint     = FindWithFingerprint,
    .find_with_ship_id         = FindWithShipId,
    .find_with_ship_connection = FindWithShipConnection,
    .remove                    = Remove,
    .remove_with_ski           = RemoveWithSki,
    .is_ski_trusted            = IsSkiTrusted,
    .is_ski_connected          = IsSkiConnected,
    .get_size                  = GetSize,
    .get_with_index            = GetWithIndex,
};

static void NodeConnectionDeleteItem(void* p) {
  NodeConnectionDelete((NodeConnectionObject*)p);
}

static EebusError NodeConnectionContainerConstruct(NodeConnectionContainer* self);

EebusError NodeConnectionContainerConstruct(NodeConnectionContainer* self) {
  // Override "virtual functions table"
  NODE_CONNECTION_CONTAINER_INTERFACE(self) = &node_connection_container_methods;

  VectorConstructWithDeallocator(&self->items, NodeConnectionDeleteItem);
  return kEebusErrorOk;
}

NodeConnectionContainerObject* NodeConnectionContainerCreate(void) {
  NodeConnectionContainer* const ncc = (NodeConnectionContainer*)EEBUS_MALLOC(sizeof(NodeConnectionContainer));
  if (ncc == NULL) {
    return NULL;
  }

  if (NodeConnectionContainerConstruct(ncc) != kEebusErrorOk) {
    NodeConnectionContainerDelete(NODE_CONNECTION_CONTAINER_OBJECT(ncc));
    return NULL;
  }

  return NODE_CONNECTION_CONTAINER_OBJECT(ncc);
}

void Destruct(NodeConnectionContainerObject* self) {
  NodeConnectionContainer* const ncc = NODE_CONNECTION_CONTAINER(self);

  VectorFreeElements(&ncc->items);
  VectorDestruct(&ncc->items);
}

NodeConnectionObject* GetOrCreate(
    NodeConnectionContainerObject* self,
    const char* ski,
    struct ShipNode* owner,
    NodeConnectionRetryFn retry_fn
) {
  NodeConnectionObject* nc = FindWithSki(self, ski);
  if (nc != NULL) {
    return nc;
  }

  nc = NodeConnectionCreate(ski, owner, retry_fn);
  if (nc == NULL) {
    return NULL;
  }

  NodeConnectionContainer* const ncc = NODE_CONNECTION_CONTAINER(self);
  VectorPushBack(&ncc->items, nc);
  return nc;
}

NodeConnectionObject* FindWithSki(NodeConnectionContainerObject* self, const char* ski) {
  if (StringIsEmpty(ski)) {
    return NULL;
  }

  NodeConnectionContainer* const ncc = NODE_CONNECTION_CONTAINER(self);

  for (size_t i = 0; i < VectorGetSize(&ncc->items); ++i) {
    NodeConnectionObject* nc = (NodeConnectionObject*)VectorGetElement(&ncc->items, i);
    if (StringEqualsIgnoreCase(NODE_CONNECTION_GET_SKI(nc), ski)) {
      return nc;
    }
  }

  return NULL;
}

NodeConnectionObject* FindWithShipId(NodeConnectionContainerObject* self, const char* ship_id) {
  if (StringIsEmpty(ship_id)) {
    return NULL;
  }

  for (size_t i = 0; i < GetSize(self); ++i) {
    NodeConnectionObject* nco = GetWithIndex(self, i);

    NodeConnection* nc = NODE_CONNECTION(nco);
    if ((nc->ship_id != NULL) && (strcmp(nc->ship_id, ship_id) == 0)) {
      return nco;
    }
  }

  return NULL;
}

NodeConnectionObject* FindWithFingerprint(NodeConnectionContainerObject* self, const char* fingerprint) {
  if (StringIsEmpty(fingerprint)) {
    return NULL;
  }

  for (size_t i = 0; i < GetSize(self); ++i) {
    NodeConnectionObject* nco = GetWithIndex(self, i);

    NodeConnection* nc = NODE_CONNECTION(nco);
    if (nc->is_trusted && StringEqualsIgnoreCase(nc->expected_fingerprint, fingerprint)) {
      return nco;
    }
  }

  return NULL;
}

NodeConnectionObject* FindWithShipConnection(NodeConnectionContainerObject* self, const ShipConnectionObject* sc) {
  if (sc == NULL) {
    return NULL;
  }

  NodeConnectionContainer* const ncc = NODE_CONNECTION_CONTAINER(self);

  for (size_t i = 0; i < VectorGetSize(&ncc->items); ++i) {
    NodeConnectionObject* nc = (NodeConnectionObject*)VectorGetElement(&ncc->items, i);
    if (NODE_CONNECTION_OWNS_CONNECTION(nc, sc)) {
      return nc;
    }
  }

  return NULL;
}

void Remove(NodeConnectionContainerObject* self, NodeConnectionObject* nc) {
  NodeConnectionContainer* const ncc = NODE_CONNECTION_CONTAINER(self);
  if (nc == NULL) {
    return;
  }

  VectorRemove(&ncc->items, nc);
  NodeConnectionDelete(nc);
}

void RemoveWithSki(NodeConnectionContainerObject* self, const char* ski) {
  NodeConnectionObject* const nc = FindWithSki(self, ski);
  Remove(self, nc);
}

bool IsSkiTrusted(const NodeConnectionContainerObject* self, const char* ski) {
  NodeConnectionObject* nc = FindWithSki((NodeConnectionContainerObject*)self, ski);
  return (nc != NULL) && NODE_CONNECTION(nc)->is_trusted;
}

bool IsSkiConnected(const NodeConnectionContainerObject* self, const char* ski) {
  NodeConnectionObject* const nc = FindWithSki((NodeConnectionContainerObject*)self, ski);
  return (nc != NULL) && NODE_CONNECTION_IS_ATTEMPT_RUNNING(nc);
}

size_t GetSize(const NodeConnectionContainerObject* self) {
  const NodeConnectionContainer* const ncc = NODE_CONNECTION_CONTAINER(self);
  return VectorGetSize(&ncc->items);
}

NodeConnectionObject* GetWithIndex(NodeConnectionContainerObject* self, size_t i) {
  NodeConnectionContainer* const ncc = NODE_CONNECTION_CONTAINER(self);
  return (NodeConnectionObject*)VectorGetElement(&ncc->items, i);
}
