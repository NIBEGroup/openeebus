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
 * @brief Node Connection implementation
 */

#include "node_connection.h"

#include "src/common/eebus_malloc.h"
#include "src/common/eebus_timer/eebus_timer.h"
#include "src/common/service_details.h"
#include "src/common/string_util.h"
#include "src/ship/api/info_provider_interface.h"
#include "src/ship/ship_connection/ship_connection.h"

typedef struct NodeConnection NodeConnection;

struct NodeConnection {
  /** Implements the Node Connection Interface */
  NodeConnectionObject obj;

  char* ski;
  ShipConnectionObject* connection;
  int attempt_cnt;
  bool is_attempt_running;
  bool handshake_complete;
  ServiceDetails* service_details;
  struct ShipNode* owner;
  EebusTimerObject* retry_timer;
};

#define NODE_CONNECTION(obj) ((NodeConnection*)(obj))

static void Destruct(NodeConnectionObject* self);
static const char* GetSki(const NodeConnectionObject* self);
static struct ShipNode* GetOwner(const NodeConnectionObject* self);
static bool IsAttemptRunning(const NodeConnectionObject* self);
static bool OwnsConnection(const NodeConnectionObject* self, const ShipConnectionObject* sc);
static ShipConnectionObject* ReleaseShipConnection(NodeConnectionObject* self);
static int OnConnectionClosed(NodeConnectionObject* self);
static void StopRetryTimer(NodeConnectionObject* self);
static void ScheduleRetry(NodeConnectionObject* self, uint32_t delay_ms);
static bool OnHandshakeComplete(NodeConnectionObject* self);
static EebusError ClientConnect(NodeConnectionObject* self, WebsocketCreatorObject* wsc, const char* ship_id);
static EebusError ServerConnect(NodeConnectionObject* self, WebsocketCreatorObject* wsc, const char* ship_id);

static const NodeConnectionInterface node_connection_methods = {
    .destruct                = Destruct,
    .get_ski                 = GetSki,
    .get_owner               = GetOwner,
    .is_attempt_running      = IsAttemptRunning,
    .owns_connection         = OwnsConnection,
    .release_ship_connection = ReleaseShipConnection,
    .on_connection_closed    = OnConnectionClosed,
    .stop_retry_timer        = StopRetryTimer,
    .schedule_retry          = ScheduleRetry,
    .on_handshake_complete   = OnHandshakeComplete,
    .client_connect          = ClientConnect,
    .server_connect          = ServerConnect,
};

static EebusError
NodeConnectionConstruct(NodeConnection* nc, const char* ski, struct ShipNode* owner, NodeConnectionRetryFn retry_fn);

EebusError
NodeConnectionConstruct(NodeConnection* nc, const char* ski, struct ShipNode* owner, NodeConnectionRetryFn retry_fn) {
  // Override "virtual function table"
  NODE_CONNECTION_INTERFACE(nc) = &node_connection_methods;

  nc->ski                = NULL;
  nc->connection         = NULL;
  nc->attempt_cnt        = 0;
  nc->is_attempt_running = false;
  nc->handshake_complete = false;
  nc->service_details    = NULL;
  nc->owner              = owner;
  nc->retry_timer        = NULL;

  if (retry_fn == NULL) {
    return kEebusErrorInputArgumentNull;
  }

  nc->ski = StringCopy(ski);
  if (nc->ski == NULL) {
    return kEebusErrorMemoryAllocate;
  }

  nc->retry_timer = EebusTimerCreate(retry_fn, NODE_CONNECTION_OBJECT(nc));
  if (nc->retry_timer == NULL) {
    return kEebusErrorInit;
  }

  return kEebusErrorOk;
}

NodeConnectionObject* NodeConnectionCreate(const char* ski, struct ShipNode* owner, NodeConnectionRetryFn retry_fn) {
  NodeConnection* const nc = (NodeConnection*)EEBUS_MALLOC(sizeof(NodeConnection));
  if (nc == NULL) {
    return NULL;
  }

  if (NodeConnectionConstruct(nc, ski, owner, retry_fn) != kEebusErrorOk) {
    NodeConnectionDelete(NODE_CONNECTION_OBJECT(nc));
    return NULL;
  }

  return NODE_CONNECTION_OBJECT(nc);
}

void Destruct(NodeConnectionObject* self) {
  NodeConnection* const nc = NODE_CONNECTION(self);

  if (nc->retry_timer != NULL) {
    EEBUS_TIMER_STOP(nc->retry_timer);
    EebusTimerDelete(nc->retry_timer);
    nc->retry_timer = NULL;
  }

  EEBUS_FREE(nc->ski);
}

const char* GetSki(const NodeConnectionObject* self) {
  return NODE_CONNECTION(self)->ski;
}

struct ShipNode* GetOwner(const NodeConnectionObject* self) {
  return NODE_CONNECTION(self)->owner;
}

bool IsAttemptRunning(const NodeConnectionObject* self) {
  return NODE_CONNECTION(self)->is_attempt_running;
}

bool OwnsConnection(const NodeConnectionObject* self, const ShipConnectionObject* sc) {
  return NODE_CONNECTION(self)->connection == sc;
}

ShipConnectionObject* ReleaseShipConnection(NodeConnectionObject* self) {
  NodeConnection* const nc = NODE_CONNECTION(self);

  ShipConnectionObject* sc = nc->connection;

  nc->connection = NULL;

  return sc;
}

int OnConnectionClosed(NodeConnectionObject* self) {
  NodeConnection* const nc = NODE_CONNECTION(self);

  nc->connection         = NULL;
  nc->is_attempt_running = false;
  nc->handshake_complete = false;

  nc->attempt_cnt++;

  return nc->attempt_cnt;
}

void StopRetryTimer(NodeConnectionObject* self) {
  const NodeConnection* const nc = NODE_CONNECTION(self);

  if (nc->retry_timer != NULL) {
    EEBUS_TIMER_STOP(nc->retry_timer);
  }
}

void ScheduleRetry(NodeConnectionObject* self, uint32_t delay_ms) {
  const NodeConnection* const nc = NODE_CONNECTION(self);

  if (nc->retry_timer == NULL) {
    return;
  }

  EEBUS_TIMER_STOP(nc->retry_timer);
  EEBUS_TIMER_START(nc->retry_timer, delay_ms, false);
}

bool OnHandshakeComplete(NodeConnectionObject* self) {
  NodeConnection* const nc = NODE_CONNECTION(self);

  if (nc->handshake_complete) {
    return false;
  }

  nc->handshake_complete = true;
  nc->attempt_cnt        = 0;

  return true;
}

EebusError ClientConnect(NodeConnectionObject* self, WebsocketCreatorObject* wsc, const char* ship_id) {
  NodeConnection* const nc = NODE_CONNECTION(self);

  if (wsc == NULL) {
    return kEebusErrorInputArgumentNull;
  }

  if (nc->is_attempt_running) {
    return kEebusErrorCommunicationBusy;
  }

  nc->connection = ShipConnectionCreate(INFO_PROVIDER_OBJECT(nc->owner), kShipRoleClient, ship_id, nc->ski, "");
  if (nc->connection == NULL) {
    return kEebusErrorMemoryAllocate;
  }

  const EebusError start_err = SHIP_CONNECTION_START(nc->connection, wsc);
  if (start_err == kEebusErrorOk) {
    nc->is_attempt_running = true;
    return kEebusErrorOk;
  }

  ShipConnectionDelete(nc->connection);
  nc->connection = NULL;
  return start_err;
}

EebusError ServerConnect(NodeConnectionObject* self, WebsocketCreatorObject* wsc, const char* ship_id) {
  NodeConnection* const nc = NODE_CONNECTION(self);

  nc->connection = ShipConnectionCreate(INFO_PROVIDER_OBJECT(nc->owner), kShipRoleServer, ship_id, nc->ski, "");
  if (nc->connection == NULL) {
    return kEebusErrorMemoryAllocate;
  }

  nc->is_attempt_running = true;
  SHIP_CONNECTION_START(nc->connection, wsc);
  return kEebusErrorOk;
}
