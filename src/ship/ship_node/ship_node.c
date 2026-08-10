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
#include "src/common/eebus_malloc.h"
#include "src/common/eebus_timer/eebus_timer.h"
#include "src/common/eebus_mutex/eebus_mutex.h"
#include "src/common/eebus_queue/eebus_queue.h"
#include "src/common/eebus_thread/eebus_thread.h"
#include "src/common/service_details.h"
#include "src/common/string_lut.h"
#include "src/common/string_util.h"
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
  kShipNodeQueueMsgTypeShipCancelPairingSki,
};

typedef enum ShipNodeQueueMsgType ShipNodeQueueMsgType;

typedef struct ShipNodeQueueMessage ShipNodeQueueMessage;

struct ShipNodeQueueMessage {
  ShipNodeQueueMsgType  type;
  ShipConnectionObject* ship_connection;
  bool                  had_error;
  char*                 ski;
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
static bool ShipNodeFindServiceForSki(ShipNode* self, const char* ski, MdnsEntry* found_entry);
static void ShipNodeConnectToService(ShipNode* self, ConnectionMapping* m, const MdnsEntry* found_entry);
static void ShipNodeConnectToAllPendingSkis(ShipNode* self);
static void* ShipNodeConnectionLoop(void* ctx);
static int
ShipNodeOnWebsocketServerConnectionCallback(const char* ski, WebsocketCreatorObject* websocket_creator, void* ctx);
static bool ShipNodeIsClientSupported(ShipNode* self);
static bool ShipNodeIsServerSupported(ShipNode* self);
static uint32_t ShipNodeRetryDelayMs(int attempt_cnt);
static void     ShipNodeRetryTimerCallback(void* ctx);

/* ── ConnectionMapping helpers ─────────────────────────────────────────── */

static void ConnectionsMappingDestroy(void* p) {
  if (p == NULL) {
    return;
  }
  ConnectionMapping* m = (ConnectionMapping*)p;
  /* connection must be stopped/deleted before removing the entry */
  EEBUS_FREE(m->ski);
  EEBUS_FREE(m);
}

static ConnectionMapping* ConnectionsFindBySki(ShipNode* self, const char* ski) {
  return (ConnectionMapping*)StringLutFind(&self->connections, ski);
}

static ConnectionMapping* ConnectionsGetOrCreate(ShipNode* self, const char* ski) {
  ConnectionMapping* m = ConnectionsFindBySki(self, ski);
  if (m != NULL) {
    return m;
  }

  m = (ConnectionMapping*)EEBUS_MALLOC(sizeof(ConnectionMapping));
  if (m == NULL) {
    return NULL;
  }
  memset(m, 0, sizeof(*m));
  m->ski = StringCopy(ski);
  if (m->ski == NULL) {
    EEBUS_FREE(m);
    return NULL;
  }

  if (StringLutInsert(&self->connections, ski, m, ConnectionsMappingDestroy) != kEebusErrorOk) {
    EEBUS_FREE(m->ski);
    EEBUS_FREE(m);
    return NULL;
  }

  return m;
}

static void ConnectionsRemoveBySki(ShipNode* self, const char* ski) {
  StringLutRemove(&self->connections, ski);
}

static bool ConnectionsIsSkiTrusted(const ShipNode* self, const char* ski) {
  return StringLutFind(&self->connections, ski) != NULL;
}

static bool ConnectionsIsSkiConnected(const ShipNode* self, const char* ski) {
  const ConnectionMapping* const m = (const ConnectionMapping*)StringLutFind(&self->connections, ski);
  return (m != NULL) && (m->connection != NULL);
}

static bool ConnectionsIsAtLimit(const ShipNode* self) {
  return StringLutGetSize(&self->connections) >= self->max_connections;
}

/* ── Queue helpers ─────────────────────────────────────────────────────── */

static void ShipNodeQueueMsgDeallocator(void* msg) {
  if (msg == NULL) {
    return;
  }

  ShipNodeQueueMessage* queue_msg = (ShipNodeQueueMessage*)msg;
  StringDelete(queue_msg->ski);
  queue_msg->ski = NULL;
}

static void ShipNodePostConnectionClose(ShipNode* sn, ShipConnectionObject* sc, bool had_error) {
  ShipNodeQueueMessage queue_msg = {
      .type            = kShipNodeQueueMsgTypeShipConnectionClosed,
      .ship_connection = sc,
      .had_error       = had_error,
      .ski             = NULL,
  };
  EEBUS_QUEUE_SEND(sn->msg_queue, &queue_msg, kTimeoutInfinite);
}

/* ── Retry helpers ─────────────────────────────────────────────────────── */

static uint32_t ShipNodeRetryDelayMs(int attempt_cnt) {
  if (attempt_cnt <= 1) return 0;
  if (attempt_cnt == 2) return 3000;
  return 10000;
}

static void ShipNodeRetryTimerCallback(void* ctx) {
  ShipNode* const sn = (ShipNode*)ctx;
  if (sn->cancel) {
    return;
  }
  ShipNodeQueueMessage queue_msg = {
      .type            = kShipNodeQueueMsgTypeMdnsEntriesFound,
      .ship_connection = NULL,
      .had_error       = false,
      .ski             = NULL,
  };
  EEBUS_QUEUE_SEND(sn->msg_queue, &queue_msg, kTimeoutInfinite);
}

/* ── Construction ──────────────────────────────────────────────────────── */

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

  static const size_t kQueueMaxMsg = 20;

  self->msg_queue = EebusQueueCreate(kQueueMaxMsg, sizeof(ShipNodeQueueMessage), ShipNodeQueueMsgDeallocator);

  self->mdns_entries          = VectorCreateWithDeallocator(MdnsEntryDeallocator);
  self->mutex                 = EebusMutexCreate();
  self->search_for_remote_ski = false;
  self->cancel                = false;
  self->connection_thread     = NULL;

  StringLutInit(&self->connections);
  self->max_connections    = SHIP_NODE_MAX_CONNECTIONS;
  self->retry_timer        = EebusTimerCreate(ShipNodeRetryTimerCallback, self);
  self->ship_node_reader   = ship_node_reader;
  self->tsl_certificate    = tsl_certificate;
  self->local_service_details = local_service_details;

  self->http_server = HttpServerCreate(port, tsl_certificate, ShipNodeOnWebsocketServerConnectionCallback, self);

  self->websocket_creator = NULL;

  if (strcmp(role, "server") == 0) {
    self->role = kShipRoleServer;
  } else if (strcmp(role, "client") == 0) {
    self->role = kShipRoleClient;
  } else {
    self->role = kShipRoleAuto;
  }
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

  EebusTimerDelete(sn->retry_timer);
  sn->retry_timer = NULL;

  /* Stop and delete every active connection before releasing the table */
  for (size_t i = 0; i < StringLutGetSize(&sn->connections); ++i) {
    ConnectionMapping* m = (ConnectionMapping*)StringLutGetElementValue(&sn->connections, i);
    if (m->connection != NULL) {
      SHIP_CONNECTION_STOP(m->connection);
      SHIP_CONNECTION_DESTRUCT(m->connection);
      EEBUS_FREE(m->connection);
      m->connection = NULL;
    }
  }
  StringLutRelease(&sn->connections);

  EebusQueueDelete(sn->msg_queue);
  sn->msg_queue = NULL;

  SHIP_NODE_DEBUG_PRINTF("ShipNode::%s(): end\n", __func__);
}

/* ── mDNS callback ─────────────────────────────────────────────────────── */

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

/* ── InfoProvider callbacks ────────────────────────────────────────────── */

bool IsRemoteServiceForSkiPaired(InfoProviderObject* self, const char* ski) {
  UNUSED(self);
  UNUSED(ski);
  return false;
}

void CloseShipConnection(ShipNode* self, ShipConnectionObject* sc, bool had_error) {
  UNUSED(had_error);

  if (sc == NULL) {
    return;
  }

  /* Scan by pointer identity — never dereferences sc, safe even for dangling
   * pointers from a double-close event on a superseded connection. */
  EEBUS_MUTEX_LOCK(self->mutex);

  const char* ski_copy  = NULL;
  bool        is_current = false;
  uint32_t    retry_delay = 0;

  for (size_t i = 0; i < StringLutGetSize(&self->connections); ++i) {
    ConnectionMapping* m = (ConnectionMapping*)StringLutGetElementValue(&self->connections, i);
    if (m->connection == sc) {
      m->connection         = NULL;
      m->is_attempt_running = false;
      m->handshake_complete = false;
      m->attempt_cnt++;
      retry_delay           = ShipNodeRetryDelayMs(m->attempt_cnt);
      ski_copy              = m->ski;
      is_current            = true;
      break;
    }
  }

  EEBUS_MUTEX_UNLOCK(self->mutex);

  if (is_current) {
    SHIP_CONNECTION_STOP(sc);
    SHIP_NODE_DEBUG_PRINTF("%s(), connection closed\n", __func__);
    SHIP_NODE_READER_ON_REMOTE_SKI_DISCONNECTED(self->ship_node_reader, ski_copy);
    ShipConnectionDelete(sc);

    if (!self->cancel) {
      if (retry_delay == 0) {
        ShipNodeConnectToAllPendingSkis(self);
      } else if (self->retry_timer != NULL) {
        EEBUS_TIMER_STOP(self->retry_timer);
        EEBUS_TIMER_START(self->retry_timer, retry_delay, false);
      }
    }
  }
  /* else: orphaned/double close — sc must NOT be dereferenced */
}

void HandleConnectionClosed(InfoProviderObject* self, ShipConnectionObject* sc, bool had_error) {
  ShipNode* const sn = SHIP_NODE(self);

  if (sn->cancel) {
    return;
  }

  ShipNodePostConnectionClose(sn, sc, had_error);
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
  ShipNode* const sn = SHIP_NODE(self);

  SHIP_NODE_READER_ON_SHIP_STATE_UPDATE(sn->ship_node_reader, ski, state);

  if (state == kDataExchange) {
    bool just_completed = false;
    EEBUS_MUTEX_LOCK(sn->mutex);
    ConnectionMapping* m = ConnectionsFindBySki(sn, ski);
    if ((m != NULL) && !m->handshake_complete) {
      m->handshake_complete = true;
      m->attempt_cnt        = 0;
      just_completed        = true;
    }
    EEBUS_MUTEX_UNLOCK(sn->mutex);
    if (just_completed) {
      SHIP_NODE_READER_ON_REMOTE_SKI_CONNECTED(sn->ship_node_reader, ski);
    }
  }
}

DataReaderObject* SetupRemoteDevice(InfoProviderObject* self, const char* ski, DataWriterObject* data_writer) {
  const ShipNode* const sn = SHIP_NODE(self);

  return SHIP_NODE_READER_SETUP_REMOTE_DEVICE(sn->ship_node_reader, ski, data_writer);
}

/* ── mDNS service search ───────────────────────────────────────────────── */

bool SkiMatches(const char* ski_a, const char* ski_b) {
  if (StringIsEmpty(ski_a) || StringIsEmpty(ski_b)) {
    return false;
  }

  return strcmp(ski_a, ski_b) == 0;
}

static bool ShipNodeFindServiceForSki(ShipNode* self, const char* ski, MdnsEntry* found_entry) {
  if (self->cancel) {
    return false;
  }

  const size_t size = VectorGetSize(self->mdns_entries);
  if (size == 0) {
    return false;
  }

  for (size_t i = 0; i < size; i++) {
    MdnsEntry* entry = (MdnsEntry*)VectorGetElement(self->mdns_entries, i);
    if (SkiMatches(entry->ski, ski)) {
      *found_entry = *entry;
      return true;
    }
  }

  return false;
}

static void ShipNodeConnectToService(ShipNode* self, ConnectionMapping* m, const MdnsEntry* found_entry) {
  if (m->is_attempt_running) {
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

  self->websocket_creator = WebsocketClientCreatorCreate(uri, self->tsl_certificate, m->ski);
  StringDelete((char*)uri);
  if (self->websocket_creator == NULL) {
    return;
  }

  m->connection = ShipConnectionCreate(
      INFO_PROVIDER_OBJECT(self),
      kShipRoleClient,
      self->local_service_details->ship_id,
      found_entry->ski,
      ""
  );

  if (m->connection != NULL) {
    const EebusError start_err = SHIP_CONNECTION_START(m->connection, self->websocket_creator);
    m->is_attempt_running      = (start_err == kEebusErrorOk);
  }

  if (!m->is_attempt_running && (m->connection != NULL)) {
    ShipConnectionDelete(m->connection);
    m->connection = NULL;
  }

  WebsocketCreatorDelete(self->websocket_creator);
  self->websocket_creator = NULL;
}

static void ShipNodeConnectToAllPendingSkis(ShipNode* self) {
  EEBUS_MUTEX_LOCK(self->mutex);

  for (size_t i = 0; i < StringLutGetSize(&self->connections); ++i) {
    ConnectionMapping* m = (ConnectionMapping*)StringLutGetElementValue(&self->connections, i);
    if ((m->connection != NULL) || m->is_attempt_running) {
      continue;
    }
    if (ConnectionsIsAtLimit(self)) {
      break;
    }
    MdnsEntry found = {0};
    if (ShipNodeFindServiceForSki(self, m->ski, &found)) {
      ShipNodeConnectToService(self, m, &found);
    }
  }

  self->search_for_remote_ski = false;
  EEBUS_MUTEX_UNLOCK(self->mutex);
}

/* ── Connection loop ───────────────────────────────────────────────────── */

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
      ShipNodeConnectToAllPendingSkis(sn);
    } else if (queue_msg.type == kShipNodeQueueMsgTypeShipConnectionClosed) {
      CloseShipConnection(sn, queue_msg.ship_connection, queue_msg.had_error);
    } else if (queue_msg.type == kShipNodeQueueMsgTypeShipUnregisterSki) {
      ShipNodeUnregisterSki(SHIP_NODE_OBJECT(sn), queue_msg.ski);
    } else if (queue_msg.type == kShipNodeQueueMsgTypeShipRegisterSki) {
      ShipNodeRegisterSki(SHIP_NODE_OBJECT(sn), queue_msg.ski, true);
      ShipNodeConnectToAllPendingSkis(sn);
    } else if (queue_msg.type == kShipNodeQueueMsgTypeShipCancelPairingSki) {
      ShipNodeCancelPairingSki(SHIP_NODE_OBJECT(sn), queue_msg.ski);
    }

    ShipNodeQueueMsgDeallocator(&queue_msg);
  }

  return NULL;
}

/* ── Websocket server callback ─────────────────────────────────────────── */

int ShipNodeOnWebsocketServerConnectionCallback(const char* ski, WebsocketCreatorObject* websocket_creator, void* ctx) {
  ShipNode* const sn = (ShipNode*)ctx;

  if (sn->cancel) {
    return -1;
  }

  /* SKI check and connection decision share one lock — no state-change window between them.
   * Release before SHIP_CONNECTION_START (blocks on thread join)
   * and before EEBUS_QUEUE_SEND (deadlock risk: websocket thread blocks on full queue
   * while the connection loop thread waits for the mutex to drain it). */
  EEBUS_MUTEX_LOCK(sn->mutex);

  bool is_ski_trusted = ConnectionsIsSkiTrusted(sn, ski);
  if (!is_ski_trusted && (StringLutGetSize(&sn->connections) == 0)) {
    /* Pairing mode: no remote SKI registered yet. */
    if (INFO_PROVIDER_IS_WAITING_FOR_TRUST_ALLOWED(sn, ski)) {
      ConnectionMapping* pm = ConnectionsGetOrCreate(sn, ski);
      is_ski_trusted        = (pm != NULL);
      SHIP_NODE_DEBUG_PRINTF("%s(), Pairing mode: auto-trusting incoming SKI %s\n", __func__, ski);
    }
  }

  if (!is_ski_trusted) {
    EEBUS_MUTEX_UNLOCK(sn->mutex);
    SHIP_NODE_DEBUG_PRINTF("%s(), Remote SKI is not trusted\n", __func__);
    return -1;
  }

  const bool already_connected = ConnectionsIsSkiConnected(sn, ski);
  const bool at_limit          = ConnectionsIsAtLimit(sn);

  if (already_connected || at_limit) {
    EEBUS_MUTEX_UNLOCK(sn->mutex);
    SHIP_NODE_DEBUG_PRINTF("%s(), rejecting: connected=%d at_limit=%d\n", __func__, already_connected, at_limit);
    return -1;
  }

  ConnectionMapping* m = ConnectionsGetOrCreate(sn, ski);
  if (m == NULL) {
    EEBUS_MUTEX_UNLOCK(sn->mutex);
    return -1;
  }

  m->connection = ShipConnectionCreate(
      INFO_PROVIDER_OBJECT(sn), kShipRoleServer, sn->local_service_details->ship_id, ski, "");
  if (m->connection == NULL) {
    EEBUS_MUTEX_UNLOCK(sn->mutex);
    SHIP_NODE_DEBUG_PRINTF("%s(), creating ship connection failed\n", __func__);
    return -1;
  }

  m->is_attempt_running = true;

  EEBUS_MUTEX_UNLOCK(sn->mutex);

  SHIP_CONNECTION_START(m->connection, websocket_creator);

  return 0;
}

/* ── Start / Stop ──────────────────────────────────────────────────────── */

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
  if (sn->retry_timer != NULL) {
    EEBUS_TIMER_STOP(sn->retry_timer);
  }

  if (sn->connection_thread != NULL) {
    ShipNodeQueueMessage queue_msg = {.type = kShipNodeQueueMsgTypeCancel, .ski = NULL};
    EEBUS_QUEUE_SEND(sn->msg_queue, &queue_msg, kTimeoutInfinite);
    EEBUS_THREAD_JOIN(sn->connection_thread);
    EebusThreadDelete(sn->connection_thread);
    sn->connection_thread = NULL;
  }

  /* Stop all active connections so Destruct never encounters a live connection.
   * Iterate with mutex held but release before SHIP_CONNECTION_STOP (blocks on
   * thread join) to avoid a deadlock with the connection's own close callback. */
  EEBUS_MUTEX_LOCK(sn->mutex);
  for (size_t i = 0; i < StringLutGetSize(&sn->connections); ++i) {
    ConnectionMapping* m     = (ConnectionMapping*)StringLutGetElementValue(&sn->connections, i);
    ShipConnectionObject* sc = m->connection;
    m->connection            = NULL;
    EEBUS_MUTEX_UNLOCK(sn->mutex);

    if (sc != NULL) {
      SHIP_CONNECTION_STOP(sc);
      SHIP_NODE_READER_ON_REMOTE_SKI_DISCONNECTED(sn->ship_node_reader, SHIP_CONNECTION_GET_REMOTE_SKI(sc));
      ShipConnectionDelete(sc);
    }

    EEBUS_MUTEX_LOCK(sn->mutex);
  }
  EEBUS_MUTEX_UNLOCK(sn->mutex);

  SHIP_MDNS_STOP(sn->mdns);

  if (ShipNodeIsServerSupported(sn)) {
    HTTP_SERVER_STOP(sn->http_server);
  }

  SHIP_NODE_DEBUG_PRINTF("ShipNode::%s(): end\n", __func__);
}

/* ── SKI registration ──────────────────────────────────────────────────── */

void ShipNodeRegisterSki(ShipNodeObject* self, const char* ski, bool is_trusted) {
  UNUSED(is_trusted);
  ShipNode* const sn = SHIP_NODE(self);

  EEBUS_MUTEX_LOCK(sn->mutex);
  ConnectionsGetOrCreate(sn, ski);
  EEBUS_MUTEX_UNLOCK(sn->mutex);
}

void RegisterRemoteSki(ShipNodeObject* self, const char* ski, bool is_trusted) {
  UNUSED(is_trusted);
  ShipNode* const sn = SHIP_NODE(self);

  ShipNodeQueueMessage queue_msg = {
      .type            = kShipNodeQueueMsgTypeShipRegisterSki,
      .ship_connection = NULL,
      .had_error       = false,
      .ski             = StringCopy(ski),
  };

  EEBUS_QUEUE_SEND(sn->msg_queue, &queue_msg, kTimeoutInfinite);
}

void ShipNodeUnregisterSki(ShipNodeObject* self, const char* ski) {
  ShipNode* const sn = SHIP_NODE(self);

  EEBUS_MUTEX_LOCK(sn->mutex);
  ConnectionMapping* m     = ConnectionsFindBySki(sn, ski);
  ShipConnectionObject* sc = (m != NULL) ? m->connection : NULL;
  if (m != NULL) {
    m->connection = NULL;
  }
  EEBUS_MUTEX_UNLOCK(sn->mutex);

  if (sc != NULL) {
    SHIP_CONNECTION_STOP(sc);
    SHIP_NODE_READER_ON_REMOTE_SKI_DISCONNECTED(sn->ship_node_reader, SHIP_CONNECTION_GET_REMOTE_SKI(sc));
    ShipConnectionDelete(sc);
  }

  EEBUS_MUTEX_LOCK(sn->mutex);
  ConnectionsRemoveBySki(sn, ski);
  EEBUS_MUTEX_UNLOCK(sn->mutex);
}

void UnregisterRemoteSki(ShipNodeObject* self, const char* ski) {
  ShipNode* const sn = SHIP_NODE(self);

  if (!ConnectionsIsSkiTrusted(sn, ski)) {
    SHIP_NODE_DEBUG_PRINTF("%s(), SKI not registered\n", __func__);
    return;
  }

  ShipNodeQueueMessage queue_msg = {
      .type            = kShipNodeQueueMsgTypeShipUnregisterSki,
      .ship_connection = NULL,
      .had_error       = false,
      .ski             = StringCopy(ski),
  };

  EEBUS_QUEUE_SEND(sn->msg_queue, &queue_msg, kTimeoutInfinite);
}

/* ── Pairing cancellation ──────────────────────────────────────────────── */

void ShipNodeCancelPairingSki(ShipNodeObject* self, const char* ski) {
  ShipNode* const sn = SHIP_NODE(self);

  if (StringIsEmpty(ski)) {
    return;
  }

  ShipConnectionObject* sc = NULL;

  EEBUS_MUTEX_LOCK(sn->mutex);
  ConnectionMapping* m = ConnectionsFindBySki(sn, ski);
  if (m != NULL) {
    sc            = m->connection;
    m->connection = NULL;
  }
  EEBUS_MUTEX_UNLOCK(sn->mutex);

  if (sc != NULL) {
    SHIP_CONNECTION_CLOSE_CONNECTION(sc, true, 0, "pairing cancelled");
    ShipConnectionDelete(sc);
  }

  EEBUS_MUTEX_LOCK(sn->mutex);
  ConnectionsRemoveBySki(sn, ski);
  EEBUS_MUTEX_UNLOCK(sn->mutex);
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
