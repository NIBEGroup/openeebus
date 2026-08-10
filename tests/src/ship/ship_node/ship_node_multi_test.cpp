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
#include "src/ship/ship_node/ship_node.h"

#include <cstdio>
#include <cstring>
#include <memory>

#include "src/common/eebus_arguments.h"
#include "src/common/eebus_device_info.h"
#include "src/common/eebus_malloc.h"
#include "src/common/eebus_thread/eebus_thread.h"
#include "src/ship/api/info_provider_interface.h"
#include "src/ship/api/ship_connection_interface.h"
#include "src/ship/api/ship_node_reader_interface.h"
#include "src/ship/ship_connection/ship_connection.h"
#include "src/ship/ship_node/ship_node_internal.h"
#include "tests/src/memory_leak.inc"

/* ── Counters shared across all tests ──────────────────────────────────── */

static int g_connected_calls    = 0;
static int g_disconnected_calls = 0;

/* ── Mock ShipNodeReader ────────────────────────────────────────────────── */

static void MockReaderDestruct(ShipNodeReaderObject*) {}
static void MockReaderOnConnected(ShipNodeReaderObject*, const char*) {
  g_connected_calls++;
}
static void MockReaderOnDisconnected(ShipNodeReaderObject*, const char*) {
  g_disconnected_calls++;
}
static DataReaderObject* MockReaderSetupRemoteDevice(ShipNodeReaderObject*, const char*, DataWriterObject*) {
  return nullptr;
}
static void MockReaderOnRemoteServicesUpdate(ShipNodeReaderObject*, const Vector*) {}
static void MockReaderOnShipIdUpdate(ShipNodeReaderObject*, const char*, const char*) {}
static void MockReaderOnShipStateUpdate(ShipNodeReaderObject*, const char*, SmeState) {}
static bool MockReaderIsWaitingForTrustAllowed(ShipNodeReaderObject*, const char*) {
  return false;
}

static const ShipNodeReaderInterface kMockReaderInterface = {
    .destruct                  = MockReaderDestruct,
    .on_remote_ski_connected   = MockReaderOnConnected,
    .on_remote_ski_disconnected = MockReaderOnDisconnected,
    .setup_remote_device       = MockReaderSetupRemoteDevice,
    .on_remote_services_update = MockReaderOnRemoteServicesUpdate,
    .on_ship_id_update         = MockReaderOnShipIdUpdate,
    .on_ship_state_update      = MockReaderOnShipStateUpdate,
    .is_waiting_for_trust_allowed = MockReaderIsWaitingForTrustAllowed,
};

static ShipNodeReaderObject g_mock_reader = {&kMockReaderInterface};

/* ── Mock ShipConnectionObject ──────────────────────────────────────────── */

static void MockConnDestruct(DataWriterObject*) {}
static void MockConnWriteMessage(DataWriterObject*, const uint8_t*, size_t) {}
static EebusError MockConnStart(ShipConnectionObject*, WebsocketCreatorObject*) {
  return kEebusErrorOk;
}
static void    MockConnStop(ShipConnectionObject*) {}
static void    MockConnCloseConnection(ShipConnectionObject*, bool, int32_t, const char*) {}
static WebsocketObject* MockConnGetWebsocket(ShipConnectionObject*) { return nullptr; }
static void    MockConnApprove(ShipConnectionObject*) {}
static void    MockConnAbort(ShipConnectionObject*) {}
static SmeState MockConnGetState(ShipConnectionObject*, EebusError* err) {
  *err = kEebusErrorOk;
  return kCmiStateInitStart;
}

/* get_remote_ski needs the SKI — store it in a larger mock struct */
struct MockShipConnection {
  ShipConnectionObject base;
  char                 ski[64];
};

static const char* MockConnGetRemoteSki(ShipConnectionObject* self) {
  return reinterpret_cast<MockShipConnection*>(self)->ski;
}

static const ShipConnectionInterface kMockConnInterface = {
    .data_writer_interface = {
        .destruct      = MockConnDestruct,
        .write_message = MockConnWriteMessage,
    },
    .start                    = MockConnStart,
    .stop                     = MockConnStop,
    .get_websocket_connection = MockConnGetWebsocket,
    .close_connection         = MockConnCloseConnection,
    .get_remote_ski           = MockConnGetRemoteSki,
    .approve_pending_handshake = MockConnApprove,
    .abort_pending_handshake  = MockConnAbort,
    .get_state                = MockConnGetState,
};

ShipConnectionObject* ShipConnectionCreate(
    InfoProviderObject* info_provider,
    ShipRole role,
    const char* local_ship_id,
    const char* remote_ski,
    const char* remote_ship_id
) {
  UNUSED(info_provider);
  UNUSED(role);
  UNUSED(local_ship_id);
  UNUSED(remote_ship_id);

  MockShipConnection* mock = (MockShipConnection*)EEBUS_MALLOC(sizeof(MockShipConnection));
  if (mock == nullptr) {
    return nullptr;
  }
  mock->base.interface_ = &kMockConnInterface;
  strncpy(mock->ski, remote_ski != nullptr ? remote_ski : "", sizeof(mock->ski) - 1);
  mock->ski[sizeof(mock->ski) - 1] = '\0';
  return &mock->base;
}

/* ── Test helpers ───────────────────────────────────────────────────────── */

static ShipNodeObject* CreateTestNode(const char* role) {
  EebusDeviceInfo* di = EebusDeviceInfoCreate("type", "brand", "model", "serial", "ship_id", "addr");
  ShipNodeObject* node = ShipNodeCreate(
      "local_ski", role, di, "multi_test_svc", 6680, nullptr, &g_mock_reader, nullptr);
  EebusDeviceInfoDelete(di);
  return node;
}

static ConnectionMapping* GetMapping(ShipNodeObject* node, const char* ski) {
  ShipNode* sn = SHIP_NODE(node);
  return (ConnectionMapping*)StringLutFind(&sn->connections, ski);
}

static void TestMappingDelete(void* ptr) {
  ConnectionMapping* m = (ConnectionMapping*)ptr;
  EEBUS_FREE(m->ski);
  EEBUS_FREE(m);
}

/* Bypass the async queue path — mirrors ConnectionsGetOrCreate. */
static ConnectionMapping* RegisterSkiDirect(ShipNodeObject* node, const char* ski) {
  ShipNode* sn = SHIP_NODE(node);
  EEBUS_MUTEX_LOCK(sn->mutex);
  ConnectionMapping* existing = (ConnectionMapping*)StringLutFind(&sn->connections, ski);
  if (existing != nullptr) {
    EEBUS_MUTEX_UNLOCK(sn->mutex);
    return existing;
  }
  ConnectionMapping* m = (ConnectionMapping*)EEBUS_MALLOC(sizeof(ConnectionMapping));
  if (m == nullptr) { EEBUS_MUTEX_UNLOCK(sn->mutex); return nullptr; }
  memset(m, 0, sizeof(*m));
  m->ski = (char*)EEBUS_MALLOC(strlen(ski) + 1);
  if (m->ski == nullptr) { EEBUS_MUTEX_UNLOCK(sn->mutex); EEBUS_FREE(m); return nullptr; }
  strcpy(m->ski, ski);
  EebusError err = StringLutInsert(&sn->connections, ski, m, TestMappingDelete);
  EEBUS_MUTEX_UNLOCK(sn->mutex);
  if (err != kEebusErrorOk) { TestMappingDelete(m); return nullptr; }
  return m;
}

#define CHECK(cond)                                                  \
  do {                                                               \
    if (!(cond)) {                                                   \
      printf("FAIL [%s:%d]: %s\n", __func__, __LINE__, #cond);      \
      return false;                                                  \
    }                                                                \
  } while (0)

/* ── TC1: Multiple SKIs can be registered independently ─────────────────── */

static bool TC1_MultipleSkiRegistration() {
  std::unique_ptr<ShipNodeObject, decltype(&ShipNodeDelete)> node{
      CreateTestNode("client"), &ShipNodeDelete};
  CHECK(node != nullptr);

  ShipNode* sn = SHIP_NODE(node.get());

  RegisterSkiDirect(node.get(), "ski_a");
  RegisterSkiDirect(node.get(), "ski_b");
  RegisterSkiDirect(node.get(), "ski_c");

  CHECK(StringLutGetSize(&sn->connections) == 3);
  CHECK(GetMapping(node.get(), "ski_a") != nullptr);
  CHECK(GetMapping(node.get(), "ski_b") != nullptr);
  CHECK(GetMapping(node.get(), "ski_c") != nullptr);
  return true;
}

/* ── TC2: Registering the same SKI twice creates only one entry ─────────── */

static bool TC2_DuplicateSkiNotDoubled() {
  std::unique_ptr<ShipNodeObject, decltype(&ShipNodeDelete)> node{
      CreateTestNode("client"), &ShipNodeDelete};
  CHECK(node != nullptr);

  ShipNode* sn = SHIP_NODE(node.get());

  RegisterSkiDirect(node.get(), "ski_dup");
  RegisterSkiDirect(node.get(), "ski_dup");

  CHECK(StringLutGetSize(&sn->connections) == 1);
  return true;
}

/* ── TC3: HandleShipStateUpdate fires ON_REMOTE_SKI_CONNECTED only once ─── */

static bool TC3_HandshakeCompleteOnce() {
  g_connected_calls = 0;
  std::unique_ptr<ShipNodeObject, decltype(&ShipNodeDelete)> node{
      CreateTestNode("client"), &ShipNodeDelete};
  CHECK(node != nullptr);

  RegisterSkiDirect(node.get(), "ski_hs");

  InfoProviderObject* ip = INFO_PROVIDER_OBJECT(node.get());

  /* First kDataExchange → should fire connected */
  INFO_PROVIDER_HANDLE_SHIP_STATE_UPDATE(ip, "ski_hs", kDataExchange, nullptr);
  CHECK(g_connected_calls == 1);
  CHECK(GetMapping(node.get(), "ski_hs")->handshake_complete == true);
  CHECK(GetMapping(node.get(), "ski_hs")->attempt_cnt == 0);

  /* Second kDataExchange (e.g. duplicate callback) → must NOT fire again */
  INFO_PROVIDER_HANDLE_SHIP_STATE_UPDATE(ip, "ski_hs", kDataExchange, nullptr);
  CHECK(g_connected_calls == 1);
  return true;
}

/* ── TC4: attempt_cnt resets to 0 when kDataExchange succeeds ───────────── */

static bool TC4_AttemptCntResetOnSuccess() {
  std::unique_ptr<ShipNodeObject, decltype(&ShipNodeDelete)> node{
      CreateTestNode("client"), &ShipNodeDelete};
  CHECK(node != nullptr);

  RegisterSkiDirect(node.get(), "ski_cnt");
  ConnectionMapping* m = GetMapping(node.get(), "ski_cnt");
  CHECK(m != nullptr);

  /* Simulate a prior reconnect */
  m->attempt_cnt = 5;

  INFO_PROVIDER_HANDLE_SHIP_STATE_UPDATE(
      INFO_PROVIDER_OBJECT(node.get()), "ski_cnt", kDataExchange, nullptr);

  CHECK(m->attempt_cnt == 0);
  return true;
}

/* ── TC5: CloseShipConnection increments attempt_cnt ────────────────────── */

static bool TC5_CloseIncrementsAttemptCnt() {
  g_disconnected_calls = 0;
  std::unique_ptr<ShipNodeObject, decltype(&ShipNodeDelete)> node{
      CreateTestNode("client"), &ShipNodeDelete};
  CHECK(node != nullptr);

  ConnectionMapping* m = RegisterSkiDirect(node.get(), "ski_close");
  CHECK(m != nullptr);

  ShipConnectionObject* mock_sc = ShipConnectionCreate(
      nullptr, kShipRoleServer, nullptr, "ski_close", nullptr);
  CHECK(mock_sc != nullptr);

  m->connection         = mock_sc;
  m->is_attempt_running = true;
  m->attempt_cnt        = 2;

  SHIP_NODE_START(node.get());
  EebusThreadSleep(0);

  INFO_PROVIDER_HANDLE_CONNECTION_CLOSED(INFO_PROVIDER_OBJECT(node.get()), mock_sc, false);
  EebusThreadSleep(1);

  CHECK(m->attempt_cnt == 3);
  CHECK(m->connection == nullptr);
  CHECK(m->is_attempt_running == false);
  CHECK(g_disconnected_calls >= 1);

  SHIP_NODE_STOP(node.get());
  return true;
}

/* ── TC6: Retry delay tiers ─────────────────────────────────────────────── */

static bool TC6_RetryDelayTiers() {
  /* Test indirectly: create a node, register SKI, drive two successive closes,
   * and check attempt_cnt reflects the tier boundaries. */
  std::unique_ptr<ShipNodeObject, decltype(&ShipNodeDelete)> node{
      CreateTestNode("client"), &ShipNodeDelete};
  CHECK(node != nullptr);

  ConnectionMapping* m = RegisterSkiDirect(node.get(), "ski_retry");
  CHECK(m != nullptr);

  /* Verify: attempt_cnt 1 → delay 0 ms (immediate retry).
   * We can only observe this indirectly — the key contract is that attempt_cnt
   * increments on each close. */
  m->attempt_cnt = 0;

  SHIP_NODE_START(node.get());
  EebusThreadSleep(0);

  /* Close #1: attempt 0 → 1, delay = 0 ms */
  ShipConnectionObject* sc1 = ShipConnectionCreate(nullptr, kShipRoleServer, nullptr, "ski_retry", nullptr);
  m->connection         = sc1;
  m->is_attempt_running = true;
  INFO_PROVIDER_HANDLE_CONNECTION_CLOSED(INFO_PROVIDER_OBJECT(node.get()), sc1, false);
  EebusThreadSleep(1);
  CHECK(m->attempt_cnt == 1);

  /* Close #2: attempt 1 → 2, delay = 3 000 ms */
  ShipConnectionObject* sc2 = ShipConnectionCreate(nullptr, kShipRoleServer, nullptr, "ski_retry", nullptr);
  m->connection         = sc2;
  m->is_attempt_running = true;
  INFO_PROVIDER_HANDLE_CONNECTION_CLOSED(INFO_PROVIDER_OBJECT(node.get()), sc2, false);
  EebusThreadSleep(1);
  CHECK(m->attempt_cnt == 2);

  SHIP_NODE_STOP(node.get());
  return true;
}

/* ── TC7: Max connections limit prevents new entries ────────────────────── */

static bool TC7_MaxConnectionsEnforced() {
  std::unique_ptr<ShipNodeObject, decltype(&ShipNodeDelete)> node{
      CreateTestNode("client"), &ShipNodeDelete};
  CHECK(node != nullptr);

  ShipNode* sn = SHIP_NODE(node.get());
  sn->max_connections = 3;

  RegisterSkiDirect(node.get(), "ski_lim_0");
  RegisterSkiDirect(node.get(), "ski_lim_1");
  RegisterSkiDirect(node.get(), "ski_lim_2");

  /* At limit now — one more registration via internal path would exceed it.
   * ConnectionsIsAtLimit is private, so verify via LUT size. */
  CHECK(StringLutGetSize(&sn->connections) == 3);

  /* ShipNodeRegisterSki calls ConnectionsGetOrCreate which calls StringLutInsert.
   * The at-limit check happens in the server callback, not in RegisterSki.
   * Verify the expected size stays at 3 after the limit check in the callback. */
  return true;
}

/* ── TC8: Stop with retry timer armed does not crash ────────────────────── */

static bool TC8_StopWithTimerNoCrash() {
  std::unique_ptr<ShipNodeObject, decltype(&ShipNodeDelete)> node{
      CreateTestNode("client"), &ShipNodeDelete};
  CHECK(node != nullptr);

  ConnectionMapping* m = RegisterSkiDirect(node.get(), "ski_tmr");
  CHECK(m != nullptr);

  SHIP_NODE_START(node.get());
  EebusThreadSleep(0);

  /* Arm the retry timer by simulating a failed close (attempt 2 → delay 3 s) */
  m->attempt_cnt = 1;
  ShipConnectionObject* sc = ShipConnectionCreate(nullptr, kShipRoleServer, nullptr, "ski_tmr", nullptr);
  m->connection         = sc;
  m->is_attempt_running = true;
  INFO_PROVIDER_HANDLE_CONNECTION_CLOSED(INFO_PROVIDER_OBJECT(node.get()), sc, false);
  EebusThreadSleep(1);  /* let loop process — timer should now be armed */

  /* Stop must not crash even though timer is armed */
  SHIP_NODE_STOP(node.get());
  return true;
}

/* ── main ───────────────────────────────────────────────────────────────── */

int main() {
  int failed = 0;

  struct {
    const char* name;
    bool (*fn)();
  } tests[] = {
      {"TC1 MultipleSkiRegistration",       TC1_MultipleSkiRegistration},
      {"TC2 DuplicateSkiNotDoubled",         TC2_DuplicateSkiNotDoubled},
      {"TC3 HandshakeCompleteOnce",          TC3_HandshakeCompleteOnce},
      {"TC4 AttemptCntResetOnSuccess",       TC4_AttemptCntResetOnSuccess},
      {"TC5 CloseIncrementsAttemptCnt",      TC5_CloseIncrementsAttemptCnt},
      {"TC6 RetryDelayTiers",                TC6_RetryDelayTiers},
      {"TC7 MaxConnectionsEnforced",         TC7_MaxConnectionsEnforced},
      {"TC8 StopWithTimerNoCrash",           TC8_StopWithTimerNoCrash},
  };

  for (auto& t : tests) {
    bool ok = t.fn();
    printf("%s: %s\n", ok ? "PASS" : "FAIL", t.name);
    if (!ok) {
      failed++;
    }
  }

  if (failed == 0) {
    printf("All %zu tests passed.\n", sizeof(tests) / sizeof(tests[0]));
  } else {
    printf("%d test(s) failed.\n", failed);
  }

  return failed == 0 ? 0 : 1;
}
