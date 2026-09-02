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
#include "tests/src/memory_leak.inc"
#include "tests/src/mocks/common/eebus_timer/eebus_timer_mock.h"
#include "tests/src/mocks/ship/ship_connection/ship_connection_mock.h"

static ShipConnectionMock* ship_connection_mock = nullptr;

ShipConnectionObject* ShipConnectionCreate(
    InfoProviderObject*, ShipRole, const char*, const char*, const char*
) {
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
  EXPECT_EQ(NODE_CONNECTION_ON_CONNECTION_CLOSED(nc_), 1);
}

TEST_F(NodeConnectionTest, OnConnectionClosedIncrements) {
  EXPECT_EQ(NODE_CONNECTION_ON_CONNECTION_CLOSED(nc_), 1);
  EXPECT_EQ(NODE_CONNECTION_ON_CONNECTION_CLOSED(nc_), 2);
  EXPECT_FALSE(NODE_CONNECTION_IS_ATTEMPT_RUNNING(nc_));
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
