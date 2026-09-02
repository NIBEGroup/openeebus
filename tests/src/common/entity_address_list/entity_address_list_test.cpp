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
#include "src/common/entity_address_list.h"

#include <gtest/gtest.h>

#include <memory>

#include "src/spine/model/entity_types.h"
#include "tests/src/memory_leak.inc"

using EntityAddressPtr = std::unique_ptr<EntityAddressType, decltype(&EntityAddressDelete)>;

static EntityAddressPtr MakeAddr(uint32_t id) {
  return EntityAddressPtr{EntityAddressCreate("dev", &id, 1), &EntityAddressDelete};
}

class EntityAddressListTest : public ::testing::Test {
 protected:
  void SetUp() override {
    EntityAddressListInit(&list_);
  }

  void TearDown() override {
    EntityAddressListRelease(&list_);
    EXPECT_EQ(heap_used, 0);
    CheckForMemoryLeaks();
  }

  EntityAddressList list_{};
};

TEST_F(EntityAddressListTest, InitiallyEmpty) {
  EXPECT_EQ(EntityAddressListGetSize(&list_), 0u);
}

TEST_F(EntityAddressListTest, AddOne) {
  EntityAddressPtr addr{MakeAddr(1)};
  ASSERT_NE(addr.get(), nullptr);

  EXPECT_EQ(EntityAddressListAdd(&list_, addr.get()), kEebusErrorOk);
  EXPECT_EQ(EntityAddressListGetSize(&list_), 1u);
}

TEST_F(EntityAddressListTest, AddIdempotent) {
  EntityAddressPtr addr{MakeAddr(2)};
  ASSERT_NE(addr.get(), nullptr);

  EXPECT_EQ(EntityAddressListAdd(&list_, addr.get()), kEebusErrorOk);
  EXPECT_EQ(EntityAddressListAdd(&list_, addr.get()), kEebusErrorOk);
  EXPECT_EQ(EntityAddressListGetSize(&list_), 1u);
}

TEST_F(EntityAddressListTest, AddTwoDifferent) {
  EntityAddressPtr addr1{MakeAddr(1)};
  EntityAddressPtr addr2{MakeAddr(2)};
  ASSERT_NE(addr1.get(), nullptr);
  ASSERT_NE(addr2.get(), nullptr);

  EntityAddressListAdd(&list_, addr1.get());
  EntityAddressListAdd(&list_, addr2.get());
  EXPECT_EQ(EntityAddressListGetSize(&list_), 2u);
}

TEST_F(EntityAddressListTest, GetReturnsAddedAddress) {
  EntityAddressPtr addr{MakeAddr(3)};
  ASSERT_NE(addr.get(), nullptr);

  EntityAddressListAdd(&list_, addr.get());
  const EntityAddressType* got{EntityAddressListGet(&list_, 0)};
  ASSERT_NE(got, nullptr);
  EXPECT_TRUE(EntityAddressCompare(got, addr.get()));
}

TEST_F(EntityAddressListTest, RemoveReducesSize) {
  EntityAddressPtr addr{MakeAddr(4)};
  ASSERT_NE(addr.get(), nullptr);

  EntityAddressListAdd(&list_, addr.get());
  EXPECT_EQ(EntityAddressListGetSize(&list_), 1u);

  EntityAddressListRemove(&list_, addr.get());
  EXPECT_EQ(EntityAddressListGetSize(&list_), 0u);
}

TEST_F(EntityAddressListTest, RemoveUnknownAddrIsNoop) {
  EntityAddressPtr addr{MakeAddr(5)};
  ASSERT_NE(addr.get(), nullptr);

  EntityAddressListRemove(&list_, addr.get());
  EXPECT_EQ(EntityAddressListGetSize(&list_), 0u);
}

TEST_F(EntityAddressListTest, AddNullReturnsError) {
  EXPECT_EQ(EntityAddressListAdd(&list_, nullptr), kEebusErrorInputArgument);
  EXPECT_EQ(EntityAddressListGetSize(&list_), 0u);
}

TEST_F(EntityAddressListTest, RemoveNullIsNoop) {
  EntityAddressListRemove(&list_, nullptr);
  EXPECT_EQ(EntityAddressListGetSize(&list_), 0u);
}
