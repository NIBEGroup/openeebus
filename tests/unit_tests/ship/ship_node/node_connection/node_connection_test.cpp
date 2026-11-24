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
#include "src/ship/ship_node/node_connection.h"

#include <gtest/gtest.h>

#include "src/ship/ship_connection/ship_connection.h"
#include "src/ship/ship_node/node_connection_internal.h"
#include "tests/unit_tests/memory_leak.inc"
#include "tests/unit_tests/mocks/common/eebus_timer/eebus_timer_mock.h"
#include "tests/unit_tests/mocks/ship/ship_connection/ship_connection_mock.h"

static ShipConnectionMock* ship_connection_mock = nullptr;

ShipConnectionObject* ShipConnectionCreate(InfoProviderObject*, ShipRole, const char*, const char*, const char*) {
  ship_connection_mock = ShipConnectionMockCreate();
  return SHIP_CONNECTION_OBJECT(ship_connection_mock);
}

static void NoOpRetryFn(void*) {}

class NodeConnectionTest : public ::testing::Test {
 protected:
  void SetUp() override {
    nc_ = NodeConnectionCreate("test_ski", nullptr, NoOpRetryFn);
    ASSERT_NE(nc_, nullptr);
  }

  void TearDown() override {
    NodeConnectionDelete(nc_);
    EXPECT_EQ(heap_used, 0);
    CheckForMemoryLeaks();
  }

  NodeConnectionObject* nc_ = nullptr;
};

TEST_F(NodeConnectionTest, GetSki) {
  EXPECT_STREQ(NODE_CONNECTION_GET_SKI(nc_), "test_ski");
}

TEST_F(NodeConnectionTest, IsAttemptRunningInitiallyFalse) {
  EXPECT_FALSE(NODE_CONNECTION_IS_ATTEMPT_RUNNING(nc_));
}

TEST_F(NodeConnectionTest, HandshakeCompleteIdempotent) {
  EXPECT_TRUE(NODE_CONNECTION_ON_HANDSHAKE_COMPLETE(nc_));
  EXPECT_FALSE(NODE_CONNECTION_ON_HANDSHAKE_COMPLETE(nc_));
}

TEST_F(NodeConnectionTest, HandshakeCompleteResetsAttemptCnt) {
  NODE_CONNECTION_ON_CONNECTION_CLOSED(nc_);
  NODE_CONNECTION_ON_CONNECTION_CLOSED(nc_);
  NODE_CONNECTION_ON_HANDSHAKE_COMPLETE(nc_);
  EXPECT_EQ(NODE_CONNECTION_ON_CONNECTION_CLOSED(nc_), 0u);
}

TEST_F(NodeConnectionTest, OnConnectionClosedIncrements) {
  EXPECT_EQ(NODE_CONNECTION_ON_CONNECTION_CLOSED(nc_), 0u);
  EXPECT_EQ(NODE_CONNECTION_ON_CONNECTION_CLOSED(nc_), 3000u);
  EXPECT_FALSE(NODE_CONNECTION_IS_ATTEMPT_RUNNING(nc_));
}

TEST_F(NodeConnectionTest, ReleasedApprovalCannotAuthorizeAReplacementAtTheSameAddress) {
  NodeConnection* peer = NODE_CONNECTION(nc_);
  ShipConnectionObject connection{};
  peer->connection          = &connection;
  peer->pending_approval_id = 7;
  EXPECT_TRUE(NodeConnectionHasPendingApproval(peer, &connection, 7));

  EXPECT_EQ(NODE_CONNECTION_RELEASE_SHIP_CONNECTION(nc_), &connection);
  EXPECT_EQ(peer->pending_approval_id, 0u);
  // Model allocator address reuse: a replacement has a new approval token.
  peer->connection          = &connection;
  peer->pending_approval_id = 8;
  EXPECT_FALSE(NodeConnectionHasPendingApproval(peer, &connection, 7));
  EXPECT_TRUE(NodeConnectionHasPendingApproval(peer, &connection, 8));
  NODE_CONNECTION_RELEASE_SHIP_CONNECTION(nc_);
}

TEST_F(NodeConnectionTest, ClosingUnapprovedConnectionClearsItsTokenWithoutGrantingTrust) {
  NodeConnection* peer = NODE_CONNECTION(nc_);
  ShipConnectionObject connection{};
  peer->connection          = &connection;
  peer->pending_approval_id = 7;
  peer->provisional         = true;

  NODE_CONNECTION_ON_CONNECTION_CLOSED(nc_);

  EXPECT_EQ(peer->pending_approval_id, 0u);
  EXPECT_FALSE(NodeConnectionHasPendingApproval(peer, &connection, 7));
  EXPECT_FALSE(peer->is_trusted);
  EXPECT_TRUE(peer->provisional);
}

TEST_F(NodeConnectionTest, DeferredEventCannotMatchAReplacementAtTheSameAddress) {
  NodeConnection* peer = NODE_CONNECTION(nc_);
  ShipConnectionObject connection{};

  peer->connection    = &connection;
  peer->connection_id = 11;
  EXPECT_TRUE(NodeConnectionIsCurrentConnection(peer, &connection, 11));
  EXPECT_FALSE(NodeConnectionIsCurrentConnection(peer, &connection, 10));

  // Model allocator address reuse: the replacement gets a new connection id.
  peer->connection_id = 12;
  EXPECT_FALSE(NodeConnectionIsCurrentConnection(peer, &connection, 11));
  EXPECT_TRUE(NodeConnectionIsCurrentConnection(peer, &connection, 12));

  peer->connection    = nullptr;
  peer->connection_id = 0;
}

TEST_F(NodeConnectionTest, SupersededEventOnlyMatchesItsOwnConnectionId) {
  NodeConnection* peer = NODE_CONNECTION(nc_);
  ShipConnectionObject connection{};

  peer->superseded    = &connection;
  peer->superseded_id = 7;
  EXPECT_TRUE(NodeConnectionIsSupersededConnection(peer, &connection, 7));
  EXPECT_FALSE(NodeConnectionIsSupersededConnection(peer, &connection, 8));
  EXPECT_FALSE(NodeConnectionIsSupersededConnection(peer, &connection, 0));

  peer->superseded    = nullptr;
  peer->superseded_id = 0;
}

TEST_F(NodeConnectionTest, SupersedeMovesTheConnectionId) {
  ship_connection_mock = nullptr;
  NODE_CONNECTION_SERVER_CONNECT(nc_, nullptr, "ship_id");
  ShipConnectionMockGuard sc_guard{ship_connection_mock};
  ShipConnectionObject* sc = SHIP_CONNECTION_OBJECT(ship_connection_mock);
  ASSERT_NE(sc, nullptr);

  NodeConnection* peer     = NODE_CONNECTION(nc_);
  peer->is_attempt_running = true;
  peer->is_outbound        = true;
  peer->handshake_complete = false;
  peer->connection_id      = 42;

  EXPECT_EQ(NODE_CONNECTION_SUPERSEDE_CLIENT(nc_), sc);
  EXPECT_EQ(peer->connection, nullptr);
  EXPECT_EQ(peer->connection_id, 0u);
  EXPECT_EQ(peer->superseded, sc);
  EXPECT_EQ(peer->superseded_id, 42u);

  NODE_CONNECTION_RESTORE_SUPERSEDED(nc_);
  EXPECT_EQ(peer->connection, sc);
  EXPECT_EQ(peer->connection_id, 42u);
  EXPECT_EQ(peer->superseded, nullptr);
  EXPECT_EQ(peer->superseded_id, 0u);

  ASSERT_EQ(NODE_CONNECTION_SUPERSEDE_CLIENT(nc_), sc);
  NODE_CONNECTION_RELEASE_SUPERSEDED(nc_, sc);
  EXPECT_EQ(peer->superseded, nullptr);
  EXPECT_EQ(peer->superseded_id, 0u);

  // Give the current connection back to the fixture so it is released cleanly.
  peer->connection    = sc;
  peer->connection_id = 42;
  EXPECT_EQ(NODE_CONNECTION_RELEASE_SHIP_CONNECTION(nc_), sc);
}

TEST_F(NodeConnectionTest, OwnsConnection) {
  ship_connection_mock = nullptr;
  NODE_CONNECTION_SERVER_CONNECT(nc_, nullptr, "ship_id");
  ShipConnectionMockGuard sc_guard{ship_connection_mock};
  ShipConnectionObject* sc = SHIP_CONNECTION_OBJECT(ship_connection_mock);
  ASSERT_NE(sc, nullptr);

  EXPECT_TRUE(NODE_CONNECTION_OWNS_CONNECTION(nc_, sc));

  ShipConnectionObject dummy{};
  EXPECT_FALSE(NODE_CONNECTION_OWNS_CONNECTION(nc_, &dummy));

  ShipConnectionObject* released = NODE_CONNECTION_RELEASE_SHIP_CONNECTION(nc_);
  EXPECT_EQ(released, SHIP_CONNECTION_OBJECT(ship_connection_mock));
}

TEST_F(NodeConnectionTest, ReleaseShipConnection) {
  ship_connection_mock = nullptr;
  NODE_CONNECTION_SERVER_CONNECT(nc_, nullptr, "ship_id");
  ShipConnectionMockGuard sc_guard{ship_connection_mock};
  ShipConnectionObject* sc = SHIP_CONNECTION_OBJECT(ship_connection_mock);
  ASSERT_NE(sc, nullptr);

  ShipConnectionObject* released = NODE_CONNECTION_RELEASE_SHIP_CONNECTION(nc_);
  EXPECT_EQ(released, sc);
  EXPECT_EQ(released, SHIP_CONNECTION_OBJECT(ship_connection_mock));
  EXPECT_FALSE(NODE_CONNECTION_OWNS_CONNECTION(nc_, sc));
}

TEST_F(NodeConnectionTest, ClientConnectBusyGuard) {
  ship_connection_mock = nullptr;
  NODE_CONNECTION_SERVER_CONNECT(nc_, nullptr, "ship_id");
  ShipConnectionMockGuard sc_guard{ship_connection_mock};

  EXPECT_TRUE(NODE_CONNECTION_IS_ATTEMPT_RUNNING(nc_));

  WebsocketCreatorObject dummy_wsc{nullptr};
  EXPECT_EQ(NODE_CONNECTION_CLIENT_CONNECT(nc_, &dummy_wsc, "ship_id"), kEebusErrorCommunicationBusy);

  ShipConnectionObject* released = NODE_CONNECTION_RELEASE_SHIP_CONNECTION(nc_);
  EXPECT_EQ(released, SHIP_CONNECTION_OBJECT(ship_connection_mock));
}

TEST(NodeConnectionTimerTest, DeleteWithArmedTimerNoCrash) {
  NodeConnectionObject* nc = NodeConnectionCreate("ski", nullptr, NoOpRetryFn);
  ASSERT_NE(nc, nullptr);

  NODE_CONNECTION_SCHEDULE_RETRY(nc, 3000);

  EXPECT_NO_FATAL_FAILURE(NodeConnectionDelete(nc));
  EXPECT_EQ(heap_used, 0);
  CheckForMemoryLeaks();
}
