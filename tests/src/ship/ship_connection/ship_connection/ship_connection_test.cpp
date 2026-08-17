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
#include "src/ship/ship_connection/ship_connection.h"

#include <gtest/gtest.h>

#include <string_view>

#include "src/common/eebus_malloc.h"
#include "tests/src/ship/ship_connection/ship_connection/ship_connection_test_suite.h"

using std::literals::string_view_literals::operator""sv;

TEST_F(ShipConnectionTestSuite, ShipConnectionCreationTest) {
  EXPECT_EQ(sc.role, kShipRoleAuto);
  EXPECT_EQ(SHIP_CONNECTION_GET_SHIP_STATE(&sc, nullptr), kCmiStateClientSend);
  EXPECT_EQ(sc.sme_error, kEebusErrorOk);
  EXPECT_EQ(std::string_view(SHIP_CONNECTION_GET_REMOTE_SKI(&sc)), kShipRemoteSki);
  ExpectConnectionClose("", false);
}
