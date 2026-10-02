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
#include "src/common/service_details.h"

#include <gtest/gtest.h>

#include "tests/src/memory_leak.inc"

class ServiceDetailsTest : public ::testing::Test {
 protected:
  void TearDown() override {
    EXPECT_EQ(heap_used, 0);
    CheckForMemoryLeaks();
  }
};

TEST_F(ServiceDetailsTest, CreateCopiesInputs) {
  ServiceDetails* const sd = ServiceDetailsCreate("ski", "ship-id", "EnergyManagementSystem", true);
  ASSERT_NE(sd, nullptr);
  EXPECT_STREQ(sd->ski, "ski");
  EXPECT_STREQ(sd->ship_id, "ship-id");
  EXPECT_STREQ(sd->device_type, "EnergyManagementSystem");
  EXPECT_EQ(sd->ipv4, nullptr);
  EXPECT_TRUE(sd->auto_accept);
  EXPECT_FALSE(sd->is_trusted);
  ServiceDetailsDelete(sd);
}

// ServiceDetailsCreate() releases the struct with ServiceDetailsDelete() when
// construction fails. An invalid argument must not leave uninitialised pointers
// behind for that release to free.
TEST_F(ServiceDetailsTest, CreateWithInvalidArgumentsReturnsNull) {
  EXPECT_EQ(ServiceDetailsCreate(nullptr, "ship-id", "EnergyManagementSystem", false), nullptr);
  EXPECT_EQ(ServiceDetailsCreate("", "ship-id", "EnergyManagementSystem", false), nullptr);
  EXPECT_EQ(ServiceDetailsCreate("ski", "", "EnergyManagementSystem", false), nullptr);
  EXPECT_EQ(ServiceDetailsCreate("ski", "ship-id", nullptr, false), nullptr);
}
