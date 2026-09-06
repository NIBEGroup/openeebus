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
#include "src/ship/ship_node/node_connection_container.h"

#include <gtest/gtest.h>

#include <memory>

#include "src/ship/ship_connection/ship_connection.h"
#include "tests/src/memory_leak.inc"
#include "tests/src/mocks/common/eebus_timer/eebus_timer_mock.h"
#include "tests/src/mocks/ship/ship_connection/ship_connection_mock.h"

static ShipConnectionMock* ship_connection_mock = nullptr;

ShipConnectionObject* ShipConnectionCreate(InfoProviderObject*, ShipRole, const char*, const char*, const char*) {
  ship_connection_mock = ShipConnectionMockCreate();
  return SHIP_CONNECTION_OBJECT(ship_connection_mock);
}

static void NoOpRetryFn(void*) {}

class NodeConnectionContainerTest : public ::testing::Test {
 protected:
  void SetUp() override {
    ncc_.reset(NodeConnectionContainerCreate());
    ASSERT_NE(ncc_.get(), nullptr);
  }

  void TearDown() override {
    ncc_.reset();
    EXPECT_EQ(heap_used, 0);
    CheckForMemoryLeaks();
  }

  NodeConnectionObject* GetOrCreate(const char* ski) {
    return NODE_CONNECTION_CONTAINER_GET_OR_CREATE(ncc_.get(), ski, nullptr, NoOpRetryFn);
  }

  std::unique_ptr<NodeConnectionContainerObject, decltype(&NodeConnectionContainerDelete)> ncc_{
      nullptr,
      NodeConnectionContainerDelete
  };
};

TEST_F(NodeConnectionContainerTest, GetOrCreateInsertsNewEntry) {
  NodeConnectionObject* nc = GetOrCreate("ski_a");
  ASSERT_NE(nc, nullptr);
  EXPECT_EQ(NODE_CONNECTION_CONTAINER_GET_SIZE(ncc_.get()), 1u);
}

TEST_F(NodeConnectionContainerTest, GetOrCreateIdempotent) {
  NodeConnectionObject* nc1 = GetOrCreate("ski_b");
  NodeConnectionObject* nc2 = GetOrCreate("ski_b");
  EXPECT_EQ(nc1, nc2);
  EXPECT_EQ(NODE_CONNECTION_CONTAINER_GET_SIZE(ncc_.get()), 1u);
}

TEST_F(NodeConnectionContainerTest, DifferentSkisGetSeparateEntries) {
  NodeConnectionObject* nc1 = GetOrCreate("ski_x");
  NodeConnectionObject* nc2 = GetOrCreate("ski_y");
  EXPECT_NE(nc1, nc2);
  EXPECT_EQ(NODE_CONNECTION_CONTAINER_GET_SIZE(ncc_.get()), 2u);
}

TEST_F(NodeConnectionContainerTest, FindWithSki) {
  NodeConnectionObject* nc = GetOrCreate("ski_c");
  EXPECT_EQ(NODE_CONNECTION_CONTAINER_FIND_WITH_SKI(ncc_.get(), "ski_c"), nc);
  EXPECT_EQ(NODE_CONNECTION_CONTAINER_FIND_WITH_SKI(ncc_.get(), "no_such_ski"), nullptr);
}

TEST_F(NodeConnectionContainerTest, RemoveWithSki) {
  GetOrCreate("ski_d");
  EXPECT_EQ(NODE_CONNECTION_CONTAINER_GET_SIZE(ncc_.get()), 1u);

  NODE_CONNECTION_CONTAINER_REMOVE_WITH_SKI(ncc_.get(), "ski_d");
  EXPECT_EQ(NODE_CONNECTION_CONTAINER_GET_SIZE(ncc_.get()), 0u);
  EXPECT_EQ(NODE_CONNECTION_CONTAINER_FIND_WITH_SKI(ncc_.get(), "ski_d"), nullptr);
}

TEST_F(NodeConnectionContainerTest, IsSkiTrusted) {
  EXPECT_FALSE(NODE_CONNECTION_CONTAINER_IS_SKI_TRUSTED(ncc_.get(), "ski_e"));

  GetOrCreate("ski_e");
  EXPECT_TRUE(NODE_CONNECTION_CONTAINER_IS_SKI_TRUSTED(ncc_.get(), "ski_e"));
}

TEST_F(NodeConnectionContainerTest, IsSkiConnected) {
  NodeConnectionObject* nc = GetOrCreate("ski_f");
  EXPECT_FALSE(NODE_CONNECTION_CONTAINER_IS_SKI_CONNECTED(ncc_.get(), "ski_f"));

  ship_connection_mock = nullptr;
  NODE_CONNECTION_SERVER_CONNECT(nc, nullptr, "ship_id");
  ShipConnectionMockGuard sc_guard{ship_connection_mock};
  EXPECT_TRUE(NODE_CONNECTION_CONTAINER_IS_SKI_CONNECTED(ncc_.get(), "ski_f"));

  ShipConnectionObject* sc_released = NODE_CONNECTION_RELEASE_SHIP_CONNECTION(nc);
  EXPECT_EQ(sc_released, SHIP_CONNECTION_OBJECT(ship_connection_mock));
}

TEST_F(NodeConnectionContainerTest, FindWithShipConnection) {
  NodeConnectionObject* nc = GetOrCreate("ski_g");
  ship_connection_mock     = nullptr;
  NODE_CONNECTION_SERVER_CONNECT(nc, nullptr, "ship_id");
  ShipConnectionMockGuard sc_guard{ship_connection_mock};
  ShipConnectionObject* sc = SHIP_CONNECTION_OBJECT(ship_connection_mock);
  ASSERT_NE(sc, nullptr);

  EXPECT_EQ(NODE_CONNECTION_CONTAINER_FIND_WITH_SHIP_CONNECTION(ncc_.get(), sc), nc);

  ShipConnectionObject other{};
  EXPECT_EQ(NODE_CONNECTION_CONTAINER_FIND_WITH_SHIP_CONNECTION(ncc_.get(), &other), nullptr);

  ShipConnectionObject* sc_released = NODE_CONNECTION_RELEASE_SHIP_CONNECTION(nc);
  EXPECT_EQ(sc_released, SHIP_CONNECTION_OBJECT(ship_connection_mock));
}
