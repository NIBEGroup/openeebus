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

#include <stdbool.h>
#include <string.h>

#include "ship_node_internal.h"
#include "src/common/eebus_arguments.h"
#include "src/common/eebus_device_info.h"
#include "src/common/eebus_mutex/eebus_mutex.h"
#include "src/common/eebus_queue/eebus_queue.h"
#include "src/common/eebus_thread/eebus_thread.h"
#include "src/common/service_details.h"
#include "src/common/vector.h"
#include "src/ship/api/http_server_interface.h"
#include "src/ship/api/ship_node_interface.h"
#include "src/ship/api/ship_node_reader_interface.h"
#include "src/ship/api/tls_certificate_interface.h"
#include "src/ship/mdns/ship_mdns.h"
#include "src/ship/ship_connection/ship_connection.h"
#include "src/ship/websocket/http_server.h"
#include "src/ship/websocket/websocket_client_creator.h"

/** Set SHIP_NODE_DEBUG 1 to enable debug prints */
#ifndef SHIP_NODE_DEBUG
#define SHIP_NODE_DEBUG 0
#endif

/** Ship node debug printf(), enabled whith SHIP_NODE_DEBUG = 1 */
#if SHIP_NODE_DEBUG
#define SHIP_NODE_DEBUG_PRINTF(fmt, ...) DebugPrintf(fmt, ##__VA_ARGS__)
#else
#define SHIP_NODE_DEBUG_PRINTF(fmt, ...)
#endif  // SHIP_NODE_DEBUG

enum ShipNodeQueueMsgType {
  kShipNodeQueueMsgTypeCancel,
  kShipNodeQueueMsgTypeMdnsEntriesFound,
  kShipNodeQueueMsgTypeShipConnectionClosed,
  kShipNodeQueueMsgTypeShipUnregisterSki,
  kShipNodeQueueMsgTypeShipRegisterSki,
  kShipNodeQueueMsgTypeDiscardSuperseded,
  kShipNodeQueueMsgTypeShipCancelPairingSki,
};

typedef enum ShipNodeQueueMsgType ShipNodeQueueMsgType;

typedef struct ShipNodeQueueMessage ShipNodeQueueMessage;

struct ShipNodeQueueMessage {
  ShipNodeQueueMsgType type;
  ShipConnectionObject* ship_connection;
  bool had_error;
  char* ski;
};

typedef struct ShipIncomingConnectionAction ShipIncomingConnectionAction;

struct ShipIncomingConnectionAction {
  /** Connection to pass to SHIP_CONNECTION_START */
  ShipConnectionObject* connection_to_start;
  /** Connection for DiscardSuperseded queue message, or NULL */
  ShipConnectionObject* connection_to_discard;
};

static void Destruct(InfoProviderObject* self);
static bool IsRemoteServiceForSkiPaired(InfoProviderObject* self, const char* ski);
static void HandleConnectionClosed(InfoProviderObject* self, ShipConnectionObject* sc, bool had_error);
static void ReportServiceShipId(InfoProviderObject* self, const char* service_id, const char* ship_id);
static bool IsWaitingForTrustAllowed(InfoProviderObject* self, const char* ski);
static void HandleShipStateUpdate(InfoProviderObject* self, const char* ski, SmeState state, const char* err);
static DataReaderObject* SetupRemoteDevice(InfoProviderObject* self, const char* ski, DataWriterObject* data_writer);
static void Start(ShipNodeObject* self);
static void Stop(ShipNodeObject* self);
static void RegisterRemoteSki(ShipNodeObject* self, const char* ski, bool is_trusted);
static void UnregisterRemoteSki(ShipNodeObject* self, const char* ski);
static void CancelPairingWithSki(ShipNodeObject* self, const char* ski);
static void ShipNodeUnregisterSki(ShipNodeObject* self, const char* ski);
static void ShipNodeCancelPairingSki(ShipNodeObject* self, const char* ski);
static void ShipNodeRegisterSki(ShipNodeObject* self, const char* ski, bool is_trusted);

static const ShipNodeInterface ship_node_methods = {
    .info_provider_interface = {
        .destruct                         = Destruct,
        .is_remote_service_for_ski_paired = IsRemoteServiceForSkiPaired,
        .handle_connection_closed         = HandleConnectionClosed,
        .report_service_ship_id           = ReportServiceShipId,
        .is_waiting_for_trust_allowed     = IsWaitingForTrustAllowed,
        .handle_ship_state_update         = HandleShipStateUpdate,
        .setup_remote_device              = SetupRemoteDevice,
    },

    .start                   = Start,
    .stop                    = Stop,
    .register_remote_ski     = RegisterRemoteSki,
    .unregister_remote_ski   = UnregisterRemoteSki,
    .cancel_pairing_with_ski = CancelPairingWithSki,
};

static void ShipNodeConstruct(
    ShipNode* self,
    const char* ski,
    const char* role,
    const EebusDeviceInfo* device_info,
    const char* service_name,
    int port,
    const TlsCertificateObject* tsl_certificate,
    ShipNodeReaderObject* ship_node_reader,
    ServiceDetails* local_service_details
);

static void ShipNodeOnMdnsEntriesFoundCallback(Vector* found_entries, void* ctx);
static bool SkiMatches(const char* ski_a, const char* ski_b);
static void CloseShipConnection(ShipNode* self, ShipConnectionObject* sc, bool had_error);
static bool ShipNodeFindService(ShipNode* self, MdnsEntry* found_entry);
static void ShipNodeConnectToService(ShipNode* self, const MdnsEntry* found_entry);
static void ShipNodeConnectToRemoteSki(ShipNode* self);
static void* ShipNodeConnectionLoop(void* ctx);
static int ShipNodeDecideSimopen(ShipNode* sn, const char* ski, ShipIncomingConnectionAction* action);
static int ShipNodeDecideIncomingConnection(ShipNode* sn, const char* ski, ShipIncomingConnectionAction* action);
static int
ShipNodeOnWebsocketServerConnectionCallback(const char* ski, WebsocketCreatorObject* websocket_creator, void* ctx);
static bool ShipNodeIsClientSupported(ShipNode* self);
static bool ShipNodeIsServerSupported(ShipNode* self);

static void ShipNodeQueueMsgDeallocator(void* msg) {
  if (msg == NULL) {
    return;
  }

  ShipNodeQueueMessage* queue_msg = (ShipNodeQueueMessage*)msg;
  StringDelete(queue_msg->ski);
  queue_msg->ski = NULL;
}

void ShipNodeConstruct(
    ShipNode* self,
    const char* ski,
    const char* role,
    const EebusDeviceInfo* device_info,
    const char* service_name,
    int port,
    const TlsCertificateObject* tsl_certificate,
    ShipNodeReaderObject* ship_node_reader,
    ServiceDetails* local_service_details
) {
  // Override "virtual function table"
  SHIP_NODE_INTERFACE(self) = &ship_node_methods;

  self->mdns = ShipMdnsCreate(ski, device_info, service_name, port, ShipNodeOnMdnsEntriesFoundCallback, self);

  static const size_t kQueueMaxMsg = 10;

  self->msg_queue = EebusQueueCreate(kQueueMaxMsg, sizeof(ShipNodeQueueMessage), ShipNodeQueueMsgDeallocator);

  self->mdns_entries          = VectorCreateWithDeallocator(MdnsEntryDeallocator);
  self->mutex                 = EebusMutexCreate();
  self->search_for_remote_ski = false;
  self->cancel                = false;
  self->connection_thread     = NULL;

  self->remote_ski = NULL;

  self->connections_table     = NULL;
  self->ship_node_reader      = ship_node_reader;
  self->tsl_certificate       = tsl_certificate;
  self->local_service_details = local_service_details;

  self->http_server = HttpServerCreate(port, tsl_certificate, ShipNodeOnWebsocketServerConnectionCallback, self);

  self->websocket_creator          = NULL;
  self->connection_attempt_running = false;
  self->client_connection_running  = false;
  self->superseded_connection      = NULL;

  if (strcmp(role, "server") == 0) {
    self->role = kShipRoleServer;
  } else if (strcmp(role, "client") == 0) {
    self->role = kShipRoleClient;
  } else {
    self->role = kShipRoleAuto;
  }

  self->ship_connection = NULL;
}

ShipNodeObject* ShipNodeCreate(
    const char* ski,
    const char* role,
    const EebusDeviceInfo* device_info,
    const char* service_name,
    int port,
    const TlsCertificateObject* tls_certificate,
    ShipNodeReaderObject* ship_node_reader,
    ServiceDetails* local_service_details
) {
  ShipNode* const sn = (ShipNode*)EEBUS_MALLOC(sizeof(ShipNode));

  ShipNodeConstruct(
      sn,
      ski,
      role,
      device_info,
      service_name,
      port,
      tls_certificate,
      ship_node_reader,
      local_service_details
  );

  return SHIP_NODE_OBJECT(sn);
}

void Destruct(InfoProviderObject* self) {
  ShipNode* const sn = SHIP_NODE(self);

  SHIP_NODE_DEBUG_PRINTF("ShipNode::%s(): begin\n", __func__);

  StringDelete(sn->remote_ski);
  sn->remote_ski = NULL;

  if (sn->mdns != NULL) {
    SHIP_MDNS_DESTRUCT(sn->mdns);
    EEBUS_FREE(sn->mdns);
    sn->mdns = NULL;
  }

  if (sn->mdns_entries != NULL) {
    VectorFreeElements(sn->mdns_entries);
    VectorDestruct(sn->mdns_entries);
    EEBUS_FREE(sn->mdns_entries);
    sn->mdns_entries = NULL;
  }

  EebusMutexDelete(sn->mutex);
  sn->mutex = NULL;

  if (sn->http_server != NULL) {
    HttpServerDelete(sn->http_server);
    sn->http_server = NULL;
  }

  if (sn->superseded_connection != NULL) {
    SHIP_CONNECTION_STOP(sn->superseded_connection);
    ShipConnectionDelete(sn->superseded_connection);
    sn->superseded_connection = NULL;
  }

  if (sn->ship_connection != NULL) {
    SHIP_CONNECTION_STOP(sn->ship_connection);
    SHIP_CONNECTION_DESTRUCT(sn->ship_connection);
    EEBUS_FREE(sn->ship_connection);
    sn->ship_connection = NULL;
  }

  EebusQueueDelete(sn->msg_queue);
  sn->msg_queue = NULL;

  sn->connection_attempt_running = false;
  SHIP_NODE_DEBUG_PRINTF("ShipNode::%s(): end\n", __func__);
}

void ShipNodeOnMdnsEntriesFoundCallback(Vector* found_entries, void* ctx) {
  ShipNode* const sn = (ShipNode*)ctx;

  if (sn->cancel) {
    return;
  }

  if (found_entries == NULL) {
    return;
  }

  EEBUS_MUTEX_LOCK(sn->mutex);
  VectorFreeElements(sn->mdns_entries);
  VectorMove(sn->mdns_entries, found_entries);
  EEBUS_FREE(found_entries);
  EEBUS_MUTEX_UNLOCK(sn->mutex);

  sn->search_for_remote_ski = true;
  if (sn->ship_node_reader != NULL) {
    SHIP_NODE_READER_ON_REMOTE_SERVICES_UPDATE(sn->ship_node_reader, sn->mdns_entries);
  }

  if (ShipNodeIsClientSupported(sn)) {
    ShipNodeQueueMessage queue_msg = {
        .type            = kShipNodeQueueMsgTypeMdnsEntriesFound,
        .ship_connection = NULL,
        .had_error       = false,
        .ski             = NULL,
    };

    EEBUS_QUEUE_SEND(sn->msg_queue, &queue_msg, kTimeoutInfinite);
  }
}

bool IsRemoteServiceForSkiPaired(InfoProviderObject* self, const char* ski) {
  UNUSED(self);
  UNUSED(ski);
  // TODO: Implement method
  return false;
}

void CloseShipConnection(ShipNode* self, ShipConnectionObject* sc, bool had_error) {
  UNUSED(had_error);

  if (sc == NULL) {
    return;
  }

  // Determine the role of sc under the mutex and update shared state atomically.
  // SHIP_CONNECTION_STOP is intentionally called outside the mutex — it blocks on
  // a thread join and must not be held across that wait.
  EEBUS_MUTEX_LOCK(self->mutex);

  const bool is_superseded_connection = (sc == self->superseded_connection);
  const bool is_current_connection    = (sc == self->ship_connection);
  if (is_superseded_connection) {
    self->superseded_connection = NULL;
  } else if (is_current_connection) {
    self->ship_connection            = NULL;
    self->connection_attempt_running = false;
    self->client_connection_running  = false;
  }

  EEBUS_MUTEX_UNLOCK(self->mutex);

  if (is_superseded_connection) {
    SHIP_NODE_DEBUG_PRINTF("[SHIP-SIMOPEN] %s(), superseded connection closed\n", __func__);
    SHIP_CONNECTION_STOP(sc);
    ShipConnectionDelete(sc);
  } else if (is_current_connection) {
    SHIP_CONNECTION_STOP(sc);
    SHIP_NODE_DEBUG_PRINTF("%s(), connection closed\n", __func__);
    SHIP_NODE_READER_ON_REMOTE_SKI_DISCONNECTED(self->ship_node_reader, SHIP_CONNECTION_GET_REMOTE_SKI(sc));
    ShipConnectionDelete(sc);
  }
}

void HandleConnectionClosed(InfoProviderObject* self, ShipConnectionObject* sc, bool had_error) {
  ShipNode* const sn = SHIP_NODE(self);

  if (sn->cancel) {
    return;
  }

  ShipNodeQueueMessage queue_msg = {
      .type            = kShipNodeQueueMsgTypeShipConnectionClosed,
      .ship_connection = sc,
      .had_error       = had_error,
      .ski             = NULL,
  };

  EEBUS_QUEUE_SEND(sn->msg_queue, &queue_msg, kTimeoutInfinite);
}

void ReportServiceShipId(InfoProviderObject* self, const char* service_id, const char* ship_id) {
  const ShipNode* const sn = SHIP_NODE(self);
  SHIP_NODE_READER_ON_SHIP_ID_UPDATE(sn->ship_node_reader, service_id, ship_id);
}

bool IsWaitingForTrustAllowed(InfoProviderObject* self, const char* ski) {
  const ShipNode* const sn = SHIP_NODE(self);
  return SHIP_NODE_READER_IS_WAITING_FOR_TRUST_ALLOWED(sn->ship_node_reader, ski);
}

void HandleShipStateUpdate(InfoProviderObject* self, const char* ski, SmeState state, const char* err) {
  UNUSED(err);
  const ShipNode* const sn = SHIP_NODE(self);

  SHIP_NODE_READER_ON_SHIP_STATE_UPDATE(sn->ship_node_reader, ski, state);

  if (state == kDataExchange) {
    SHIP_NODE_READER_ON_REMOTE_SKI_CONNECTED(sn->ship_node_reader, ski);
  }
}

DataReaderObject* SetupRemoteDevice(InfoProviderObject* self, const char* ski, DataWriterObject* data_writer) {
  const ShipNode* const sn = SHIP_NODE(self);

  return SHIP_NODE_READER_SETUP_REMOTE_DEVICE(sn->ship_node_reader, ski, data_writer);
}

bool SkiMatches(const char* ski_a, const char* ski_b) {
  if (StringIsEmpty(ski_a) || StringIsEmpty(ski_b)) {
    return false;
  }

  return strcmp(ski_a, ski_b) == 0;
}

static bool ShipNodeFindService(ShipNode* self, MdnsEntry* found_entry) {
  if (self->cancel) {
    return false;
  }

  size_t size = VectorGetSize(self->mdns_entries);
  if (size == 0) {
    return false;
  }

  bool entry_found = false;
  MdnsEntry* entry = NULL;

  // Search for the service with the remote ski
  for (size_t i = 0; i < size; i++) {
    entry = (MdnsEntry*)VectorGetElement(self->mdns_entries, i);
    if (SkiMatches(entry->ski, self->remote_ski)) {
      *found_entry = *entry;
      entry_found  = true;
      break;
    }
  }

  return entry_found;
}

static void ShipNodeConnectToService(ShipNode* self, const MdnsEntry* found_entry) {
  if (self->connection_attempt_running) {
    return;
  }

  size_t len = strlen(found_entry->host);
  if (len <= 1) {
    return;
  }

  if (found_entry->host[len - 1] == '.') {
    --len;
  }

  const char* const uri
      = StringFmtSprintf("wss://%.*s:%d%s", len, found_entry->host, found_entry->port, found_entry->path);
  if (uri == NULL) {
    return;
  }

  self->websocket_creator = WebsocketClientCreatorCreate(uri, self->tsl_certificate, self->remote_ski);
  StringDelete((char*)uri);
  if (self->websocket_creator == NULL) {
    return;
  }

  self->ship_connection = ShipConnectionCreate(
      INFO_PROVIDER_OBJECT(self),
      kShipRoleClient,
      self->local_service_details->ship_id,
      found_entry->ski,
      ""
  );

  if (self->ship_connection != NULL) {
    const EebusError err = SHIP_CONNECTION_START(self->ship_connection, self->websocket_creator);

    self->connection_attempt_running = (err == kEebusErrorOk);
    self->client_connection_running  = self->connection_attempt_running;
  }

  if ((self->connection_attempt_running == false) && (self->ship_connection != NULL)) {
    ShipConnectionDelete(self->ship_connection);
    self->ship_connection = NULL;
  }

  WebsocketCreatorDelete(self->websocket_creator);
  self->websocket_creator = NULL;
}

static void ShipNodeConnectToRemoteSki(ShipNode* self) {
  MdnsEntry found_entry;
  EEBUS_MUTEX_LOCK(self->mutex);
  if (ShipNodeFindService(self, &found_entry)) {
    ShipNodeConnectToService(self, &found_entry);
  }

  self->search_for_remote_ski = false;
  EEBUS_MUTEX_UNLOCK(self->mutex);
}

void* ShipNodeConnectionLoop(void* ctx) {
  ShipNode* const sn             = (ShipNode*)ctx;
  ShipNodeQueueMessage queue_msg = {0};
  EebusError err                 = kEebusErrorOk;

  while (!sn->cancel) {
    err = EEBUS_QUEUE_RECEIVE(sn->msg_queue, &queue_msg, kTimeoutInfinite);
    if (err != kEebusErrorOk) {
      continue;
    }

    if (queue_msg.type == kShipNodeQueueMsgTypeMdnsEntriesFound) {
      ShipNodeConnectToRemoteSki(sn);
    } else if (queue_msg.type == kShipNodeQueueMsgTypeShipConnectionClosed) {
      CloseShipConnection(sn, queue_msg.ship_connection, queue_msg.had_error);
    } else if (queue_msg.type == kShipNodeQueueMsgTypeShipUnregisterSki) {
      ShipNodeUnregisterSki(SHIP_NODE_OBJECT(sn), queue_msg.ski);
    } else if (queue_msg.type == kShipNodeQueueMsgTypeShipRegisterSki) {
      ShipNodeRegisterSki(SHIP_NODE_OBJECT(sn), queue_msg.ski, true);
      ShipNodeConnectToRemoteSki(sn);
    } else if (queue_msg.type == kShipNodeQueueMsgTypeDiscardSuperseded) {
      CloseShipConnection(sn, queue_msg.ship_connection, queue_msg.had_error);
    } else if (queue_msg.type == kShipNodeQueueMsgTypeShipCancelPairingSki) {
      ShipNodeCancelPairingSki(SHIP_NODE_OBJECT(sn), queue_msg.ski);
    }

    ShipNodeQueueMsgDeallocator(&queue_msg);
  }

  return NULL;
}

int ShipNodeDecideSimopen(ShipNode* sn, const char* ski, ShipIncomingConnectionAction* action) {
  const char* local_ski = sn->local_service_details->ski;
  SHIP_NODE_DEBUG_PRINTF("[SHIP-SIMOPEN] %s(), local ski=%.8s, remote ski=%.8s\n", __func__, local_ski, ski);

  if (strcmp(local_ski, ski) <= 0) {
    // Local SKI is lower — yield; our outgoing client connection will be
    // accepted by the remote side.
    SHIP_NODE_DEBUG_PRINTF("[SHIP-SIMOPEN] %s(), yielding to remote (local SKI <= remote SKI)\n", __func__);
    return -1;
  }

  // Local SKI is higher — switch to server role. Clear client_connection_running
  // now that the outgoing client is being superseded; connection_attempt_running
  // stays true so ShipNodeConnectToService cannot start a competing attempt
  // while the new server connection is being set up.
  SHIP_NODE_DEBUG_PRINTF("[SHIP-SIMOPEN] %s(), taking server role (local SKI > remote SKI)\n", __func__);
  ShipConnectionObject* const old_client = sn->ship_connection;
  sn->client_connection_running          = false;

  sn->ship_connection
      = ShipConnectionCreate(INFO_PROVIDER_OBJECT(sn), kShipRoleServer, sn->local_service_details->ship_id, ski, "");
  if (sn->ship_connection == NULL) {
    SHIP_NODE_DEBUG_PRINTF("%s(), creating server ship connection failed\n", __func__);
    sn->ship_connection           = old_client;
    sn->client_connection_running = true;
    return -1;
  }

  // Store the superseded client so CloseShipConnection can identify and clean it
  // up if it closes naturally before the DiscardSuperseded queue message is processed.
  sn->superseded_connection = old_client;

  action->connection_to_start   = sn->ship_connection;
  action->connection_to_discard = old_client;

  return 0;
}

int ShipNodeDecideIncomingConnection(ShipNode* sn, const char* ski, ShipIncomingConnectionAction* action) {
  if (sn->client_connection_running) {
    return ShipNodeDecideSimopen(sn, ski, action);
  }

  if (sn->connection_attempt_running) {
    // An active server connection already exists — reject; the remote should
    // retry after the existing connection is torn down.
    SHIP_NODE_DEBUG_PRINTF("[SHIP] %s(), rejecting incoming: server connection already running\n", __func__);
    return -1;
  }

  SHIP_NODE_DEBUG_PRINTF("[SHIP] server accept from %.8s... (no simultaneous open)\n", ski);
  sn->ship_connection
      = ShipConnectionCreate(INFO_PROVIDER_OBJECT(sn), kShipRoleServer, sn->local_service_details->ship_id, ski, "");
  if (sn->ship_connection == NULL) {
    SHIP_NODE_DEBUG_PRINTF("%s(), creating ship connection failed\n", __func__);
    return -1;
  }

  sn->connection_attempt_running = true;

  action->connection_to_start = sn->ship_connection;

  return 0;
}

int ShipNodeOnWebsocketServerConnectionCallback(const char* ski, WebsocketCreatorObject* websocket_creator, void* ctx) {
  ShipNode* const sn = (ShipNode*)ctx;

  if (sn->cancel) {
    return -1;
  }

  // SKI check and connection decision share one lock — no state-change window between them.
  // Release before SHIP_CONNECTION_START (blocks on thread join)
  // and before EEBUS_QUEUE_SEND (deadlock risk: websocket thread blocks on full queue
  // while the connection loop thread waits for the mutex to drain it).
  EEBUS_MUTEX_LOCK(sn->mutex);

  bool is_ski_trusted = SkiMatches(ski, sn->remote_ski);
  if (!is_ski_trusted && StringIsEmpty(sn->remote_ski)) {
    // Pairing mode: no remote SKI registered yet.
    // Delegate to the info-provider (service layer) to decide whether to trust this SKI.
    if (INFO_PROVIDER_IS_WAITING_FOR_TRUST_ALLOWED(sn, ski)) {
      StringDelete(sn->remote_ski);
      sn->remote_ski = StringCopy(ski);
      is_ski_trusted = (sn->remote_ski != NULL);
      SHIP_NODE_DEBUG_PRINTF("%s(), Pairing mode: auto-trusting incoming SKI %s\n", __func__, ski);
    }
  }

  if (!is_ski_trusted) {
    EEBUS_MUTEX_UNLOCK(sn->mutex);
    SHIP_NODE_DEBUG_PRINTF("%s(), Remote SKI is not trusted\n", __func__);
    return -1;
  }

  ShipIncomingConnectionAction action = {NULL, NULL};

  const int err = ShipNodeDecideIncomingConnection(sn, ski, &action);

  EEBUS_MUTEX_UNLOCK(sn->mutex);

  if (err) {
    return err;
  }

  if (action.connection_to_discard != NULL) {
    const ShipNodeQueueMessage discard_msg = {
        .type            = kShipNodeQueueMsgTypeDiscardSuperseded,
        .ship_connection = action.connection_to_discard,
        .had_error       = false,
        .ski             = NULL,
    };

    EEBUS_QUEUE_SEND(sn->msg_queue, &discard_msg, kTimeoutInfinite);
  }

  if (action.connection_to_start != NULL) {
    SHIP_CONNECTION_START(action.connection_to_start, websocket_creator);
  }

  return 0;
}

bool ShipNodeIsClientSupported(ShipNode* self) {
  return (self->role == kShipRoleClient) || (self->role == kShipRoleAuto);
}

bool ShipNodeIsServerSupported(ShipNode* self) {
  return (self->role == kShipRoleServer) || (self->role == kShipRoleAuto);
}

void Start(ShipNodeObject* self) {
  ShipNode* const sn = SHIP_NODE(self);

  if (ShipNodeIsServerSupported(sn)) {
    HTTP_SERVER_START(sn->http_server);
  }

  SHIP_MDNS_START(sn->mdns);

  sn->connection_thread = EebusThreadCreate(ShipNodeConnectionLoop, sn, 4 * 1024);
  if (sn->connection_thread == NULL) {
    SHIP_NODE_DEBUG_PRINTF("%s(), client connection thread creation failed\n", __func__);
  }
}

void Stop(ShipNodeObject* self) {
  ShipNode* const sn = SHIP_NODE(self);

  SHIP_NODE_DEBUG_PRINTF("ShipNode::%s(): begin\n", __func__);
  sn->cancel = true;

  if (sn->connection_thread != NULL) {
    ShipNodeQueueMessage queue_msg = {.type = kShipNodeQueueMsgTypeCancel, .ski = NULL};
    EEBUS_QUEUE_SEND(sn->msg_queue, &queue_msg, kTimeoutInfinite);
    EEBUS_THREAD_JOIN(sn->connection_thread);
    EebusThreadDelete(sn->connection_thread);
    sn->connection_thread = NULL;
  }

  // ShipNodeConnectionLoop may have exited via kCancel before processing a
  // kShipConnectionClosed that arrived during the concurrent close handshake
  // (e.g. during the 500 ms wait in DataExchangeHandleClose). Stop and delete
  // the connection here so Destruct never encounters a live ship_connection.
  EEBUS_MUTEX_LOCK(sn->mutex);
  ShipConnectionObject* const sc = sn->ship_connection;

  sn->ship_connection = NULL;
  EEBUS_MUTEX_UNLOCK(sn->mutex);

  if (sc != NULL) {
    SHIP_CONNECTION_STOP(sc);
    SHIP_NODE_READER_ON_REMOTE_SKI_DISCONNECTED(sn->ship_node_reader, SHIP_CONNECTION_GET_REMOTE_SKI(sc));
    ShipConnectionDelete(sc);
  }

  SHIP_MDNS_STOP(sn->mdns);

  if (ShipNodeIsServerSupported(sn)) {
    HTTP_SERVER_STOP(sn->http_server);
  }

  SHIP_NODE_DEBUG_PRINTF("ShipNode::%s(): end\n", __func__);
}

void ShipNodeRegisterSki(ShipNodeObject* self, const char* ski, bool is_trusted) {
  UNUSED(is_trusted);
  ShipNode* const sn = SHIP_NODE(self);

  EEBUS_MUTEX_LOCK(sn->mutex);
  StringDelete(sn->remote_ski);
  sn->remote_ski = StringCopy(ski);
  EEBUS_MUTEX_UNLOCK(sn->mutex);
}

void RegisterRemoteSki(ShipNodeObject* self, const char* ski, bool is_trusted) {
  UNUSED(is_trusted);
  ShipNode* const sn = SHIP_NODE(self);

  ShipNodeQueueMessage queue_msg = {
      .type            = kShipNodeQueueMsgTypeShipRegisterSki,
      .ship_connection = sn->ship_connection,
      .had_error       = false,
      .ski             = StringCopy(ski),
  };

  EEBUS_QUEUE_SEND(sn->msg_queue, &queue_msg, kTimeoutInfinite);
}

void ShipNodeUnregisterSki(ShipNodeObject* self, const char* ski) {
  UNUSED(ski);
  ShipNode* const sn = SHIP_NODE(self);

  EEBUS_MUTEX_LOCK(sn->mutex);
  StringDelete(sn->remote_ski);
  sn->remote_ski = NULL;
  EEBUS_MUTEX_UNLOCK(sn->mutex);

  // TODO: Fix possible situation that ShipConnection Start() is called
  // from another thread at the same time
  if (sn->ship_connection != NULL) {
    CloseShipConnection(sn, sn->ship_connection, false);
  }
}

void UnregisterRemoteSki(ShipNodeObject* self, const char* ski) {
  ShipNode* const sn = SHIP_NODE(self);

  if (!SkiMatches(ski, sn->remote_ski)) {
    SHIP_NODE_DEBUG_PRINTF("%s(), SKI does not match\n", __func__);
    return;
  }

  ShipNodeQueueMessage queue_msg = {
      .type            = kShipNodeQueueMsgTypeShipUnregisterSki,
      .ship_connection = sn->ship_connection,
      .had_error       = false,
      .ski             = StringCopy(ski),
  };

  EEBUS_QUEUE_SEND(sn->msg_queue, &queue_msg, kTimeoutInfinite);
}

// Runs on the connection loop thread (same context as ShipNodeUnregisterSki)
void ShipNodeCancelPairingSki(ShipNodeObject* self, const char* ski) {
  ShipNode* const sn = SHIP_NODE(self);

  if (StringIsEmpty(ski)) {
    return;
  }

  ShipConnectionObject* sc = NULL;

  EEBUS_MUTEX_LOCK(sn->mutex);
  if (SkiMatches(ski, sn->remote_ski)) {
    StringDelete(sn->remote_ski);
    sn->remote_ski = NULL;
  }

  if (sn->ship_connection != NULL) {
    const char* const conn_ski = SHIP_CONNECTION_GET_REMOTE_SKI(sn->ship_connection);
    if (SkiMatches(ski, conn_ski)) {
      sc = sn->ship_connection;

      sn->ship_connection = NULL;
    }
  }

  EEBUS_MUTEX_UNLOCK(sn->mutex);

  if (sc != NULL) {
    SHIP_CONNECTION_CLOSE_CONNECTION(sc, true, 0, "pairing cancelled");
    ShipConnectionDelete(sc);
  }
}

void CancelPairingWithSki(ShipNodeObject* self, const char* ski) {
  ShipNode* const sn = SHIP_NODE(self);

  if (StringIsEmpty(ski)) {
    return;
  }

  ShipNodeQueueMessage queue_msg = {
      .type            = kShipNodeQueueMsgTypeShipCancelPairingSki,
      .ship_connection = NULL,
      .had_error       = false,
      .ski             = StringCopy(ski),
  };

  EEBUS_QUEUE_SEND(sn->msg_queue, &queue_msg, kTimeoutInfinite);
}
