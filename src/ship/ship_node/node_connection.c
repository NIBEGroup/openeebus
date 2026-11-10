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
#include "node_connection_internal.h"

#include "src/common/eebus_malloc.h"
#include "src/common/eebus_timer/eebus_timer.h"
#include "src/common/string_util.h"
#include "src/ship/api/info_provider_interface.h"
#include "src/ship/ship_connection/ship_connection.h"

static void Destruct(NodeConnectionObject* self);
static const char* GetSki(const NodeConnectionObject* self);
static struct ShipNode* GetOwner(const NodeConnectionObject* self);
static bool IsAttemptRunning(const NodeConnectionObject* self);
static bool OwnsConnection(const NodeConnectionObject* self, const ShipConnectionObject* sc);
static ShipConnectionObject* ReleaseShipConnection(NodeConnectionObject* self);
static uint32_t OnConnectionClosed(NodeConnectionObject* self);
static void StopRetryTimer(NodeConnectionObject* self);
static void ScheduleRetry(NodeConnectionObject* self, uint32_t delay_ms);
static bool OnHandshakeComplete(NodeConnectionObject* self);
static EebusError ClientConnect(NodeConnectionObject* self, WebsocketCreatorObject* wsc, const char* ship_id);
static EebusError ServerConnect(NodeConnectionObject* self, WebsocketCreatorObject* wsc, const char* ship_id);
static bool IsOutboundInFlight(const NodeConnectionObject* self);
static bool IsOutboundEstablished(const NodeConnectionObject* self);
static bool ShouldYield(NodeConnectionObject* self);
static ShipConnectionObject* SupersedeClient(NodeConnectionObject* self);
static void RestoreSuperseded(NodeConnectionObject* self);
static bool IsSuperseded(const NodeConnectionObject* self, const ShipConnectionObject* sc);
static void ReleaseSuperseded(NodeConnectionObject* self, const ShipConnectionObject* sc);
static ShipConnectionObject* TakeSuperseded(NodeConnectionObject* self);
static ShipConnectionObject* DetachConnection(NodeConnectionObject* self, bool* was_established);
static void ClearAttempt(NodeConnectionObject* self);
static void OnInboundHandshake(NodeConnectionObject* self, bool started);
static bool IsInboundHandshake(const NodeConnectionObject* self);

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
    .is_outbound_in_flight   = IsOutboundInFlight,
    .is_outbound_established = IsOutboundEstablished,
    .should_yield            = ShouldYield,
    .supersede_client        = SupersedeClient,
    .restore_superseded      = RestoreSuperseded,
    .is_superseded           = IsSuperseded,
    .release_superseded      = ReleaseSuperseded,
    .take_superseded         = TakeSuperseded,
    .detach_connection       = DetachConnection,
    .clear_attempt           = ClearAttempt,
    .on_inbound_handshake    = OnInboundHandshake,
    .is_inbound_handshake    = IsInboundHandshake,
};

static EebusError
NodeConnectionConstruct(NodeConnection* nc, const char* ski, struct ShipNode* owner, NodeConnectionRetryFn retry_fn);
static uint32_t RetryDelayMs(int attempt_cnt);

EebusError
NodeConnectionConstruct(NodeConnection* nc, const char* ski, struct ShipNode* owner, NodeConnectionRetryFn retry_fn) {
  // Override "virtual function table"
  NODE_CONNECTION_INTERFACE(nc) = &node_connection_methods;

  nc->ski                  = NULL;
  nc->ship_id              = NULL;
  nc->connection           = NULL;
  nc->pending_approval_id  = 0;
  nc->attempt_cnt          = 0;
  nc->is_attempt_running   = false;
  nc->is_outbound          = false;
  nc->handshake_complete   = false;
  nc->yield_cnt            = 0;
  nc->inbound_hs_cnt       = 0;
  nc->superseded           = NULL;
  nc->is_trusted           = false;
  nc->provisional          = false;
  nc->trusted_by_pairing   = false;
  nc->expected_fingerprint = NULL;
  nc->peer_fingerprint     = NULL;
  nc->service_details      = NULL;
  nc->owner                = owner;
  nc->retry_timer          = NULL;

  if (retry_fn == NULL) {
    return kEebusErrorInputArgumentNull;
  }

  nc->ski = StringCopy(ski);
  if ((ski != NULL) && (nc->ski == NULL)) {
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

  StringDelete((char*)nc->ski);
  StringDelete(nc->ship_id);
  StringDelete(nc->expected_fingerprint);
  StringDelete(nc->peer_fingerprint);
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
  return (NODE_CONNECTION(self)->connection == sc) || IsSuperseded(self, sc);
}

ShipConnectionObject* ReleaseShipConnection(NodeConnectionObject* self) {
  NodeConnection* const nc = NODE_CONNECTION(self);

  ShipConnectionObject* sc = nc->connection;

  nc->connection          = NULL;
  nc->pending_approval_id = 0;

  return sc;
}

static uint32_t RetryDelayMs(int attempt_cnt) {
  if (attempt_cnt <= 1) {
    return 0;
  }

  if (attempt_cnt == 2) {
    return 3000;
  }

  return 10000;
}

uint32_t OnConnectionClosed(NodeConnectionObject* self) {
  NodeConnection* const nc = NODE_CONNECTION(self);

  nc->connection          = NULL;
  nc->pending_approval_id = 0;
  nc->is_attempt_running  = false;
  nc->is_outbound         = false;
  nc->handshake_complete  = false;
  StringDelete(nc->peer_fingerprint);
  nc->peer_fingerprint = NULL;

  nc->attempt_cnt++;

  return RetryDelayMs(nc->attempt_cnt);
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
  nc->yield_cnt          = 0;
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

  nc->pending_approval_id = 0;
  nc->connection = ShipConnectionCreate(INFO_PROVIDER_OBJECT(nc->owner), kShipRoleClient, ship_id, nc->ski, "");
  if (nc->connection == NULL) {
    return kEebusErrorMemoryAllocate;
  }

  const EebusError start_err = SHIP_CONNECTION_START(nc->connection, wsc);
  if (start_err == kEebusErrorOk) {
    nc->is_attempt_running = true;
    nc->is_outbound        = true;
    return kEebusErrorOk;
  }

  ShipConnectionDelete(nc->connection);
  nc->connection = NULL;
  return start_err;
}

EebusError ServerConnect(NodeConnectionObject* self, WebsocketCreatorObject* wsc, const char* ship_id) {
  NodeConnection* const nc = NODE_CONNECTION(self);

  nc->pending_approval_id = 0;
  nc->connection = ShipConnectionCreate(INFO_PROVIDER_OBJECT(nc->owner), kShipRoleServer, ship_id, nc->ski, "");
  if (nc->connection == NULL) {
    return kEebusErrorMemoryAllocate;
  }

  const EebusError start_err = SHIP_CONNECTION_START(nc->connection, wsc);
  if (start_err != kEebusErrorOk) {
    ShipConnectionDelete(nc->connection);
    nc->connection = NULL;
    return start_err;
  }

  nc->is_attempt_running = true;
  nc->is_outbound        = false;
  nc->yield_cnt          = 0;
  return kEebusErrorOk;
}

bool IsOutboundInFlight(const NodeConnectionObject* self) {
  const NodeConnection* const nc = NODE_CONNECTION(self);
  return nc->is_attempt_running && nc->is_outbound && !nc->handshake_complete;
}

bool IsOutboundEstablished(const NodeConnectionObject* self) {
  const NodeConnection* const nc = NODE_CONNECTION(self);
  return nc->is_attempt_running && nc->is_outbound && nc->handshake_complete;
}

bool ShouldYield(NodeConnectionObject* self) {
  NodeConnection* const nc = NODE_CONNECTION(self);

  // Yield once. If the peer dials in again while our outbound attempt still has
  // not completed, that attempt is going nowhere (e.g. a stale mDNS entry), so
  // give way. The count is kept across our own attempts: they restart about as
  // often as the peer retries, and a per-attempt count would refuse every
  // inbound connection that lands on a fresh attempt.
  if (nc->yield_cnt == 0) {
    nc->yield_cnt++;
    return true;
  }

  return false;
}

ShipConnectionObject* SupersedeClient(NodeConnectionObject* self) {
  NodeConnection* const nc = NODE_CONNECTION(self);

  if (!IsOutboundInFlight(self) || (nc->connection == NULL) || (nc->superseded != NULL)) {
    return NULL;
  }

  ShipConnectionObject* const old_client = nc->connection;

  // The outbound connection is being replaced: it no longer counts as a client,
  // but the attempt stays marked as running so that no competing outbound
  // attempt can start while the replacing server connection is set up.
  nc->superseded  = old_client;
  nc->connection  = NULL;
  nc->is_outbound = false;
  return old_client;
}

void RestoreSuperseded(NodeConnectionObject* self) {
  NodeConnection* const nc = NODE_CONNECTION(self);

  if (nc->superseded != NULL) {
    nc->connection  = nc->superseded;
    nc->superseded  = NULL;
    nc->is_outbound = true;
  }
}

bool IsSuperseded(const NodeConnectionObject* self, const ShipConnectionObject* sc) {
  const NodeConnection* const nc = NODE_CONNECTION(self);
  return (sc != NULL) && (nc->superseded == sc);
}

void ReleaseSuperseded(NodeConnectionObject* self, const ShipConnectionObject* sc) {
  NodeConnection* const nc = NODE_CONNECTION(self);

  if ((sc != NULL) && (nc->superseded == sc)) {
    nc->superseded = NULL;
  }
}

ShipConnectionObject* DetachConnection(NodeConnectionObject* self, bool* was_established) {
  NodeConnection* const nc = NODE_CONNECTION(self);

  ShipConnectionObject* const old_connection = nc->connection;
  *was_established                           = (old_connection != NULL) && nc->handshake_complete;

  // The replacing connection starts from scratch: its SHIP handshake completing
  // must be reported to the application again (the old one is reported as
  // disconnected by the caller). is_attempt_running stays as it is.
  nc->connection         = NULL;
  nc->is_outbound        = false;
  nc->handshake_complete = false;
  return old_connection;
}

void ClearAttempt(NodeConnectionObject* self) {
  NodeConnection* const nc = NODE_CONNECTION(self);

  nc->connection         = NULL;
  nc->is_attempt_running = false;
  nc->is_outbound        = false;
  nc->handshake_complete = false;
}

ShipConnectionObject* TakeSuperseded(NodeConnectionObject* self) {
  NodeConnection* const nc       = NODE_CONNECTION(self);
  ShipConnectionObject* const sc = nc->superseded;
  nc->superseded                 = NULL;
  return sc;
}

void OnInboundHandshake(NodeConnectionObject* self, bool started) {
  NodeConnection* const nc = NODE_CONNECTION(self);

  if (started) {
    nc->inbound_hs_cnt++;
  } else if (nc->inbound_hs_cnt > 0) {
    // Does not go below zero: the connection may have been created while the handshake was already running
    nc->inbound_hs_cnt--;
  }
}

bool IsInboundHandshake(const NodeConnectionObject* self) {
  return NODE_CONNECTION(self)->inbound_hs_cnt > 0;
}
