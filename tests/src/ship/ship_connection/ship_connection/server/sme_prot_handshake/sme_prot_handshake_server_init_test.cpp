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
#include <gtest/gtest.h>

#include <string_view>

#include "src/ship/ship_connection/server.h"
#include "tests/src/ship/ship_connection/ship_connection/ship_connection_test_suite.h"

TEST_F(ShipConnectionTestSuite, ProtHandshakeServerInitTest) {
  // Arrange: Expect timer stop and state update function calls
  EXPECT_CALL(*wfr_timer_mock->gmock, Stop(sc.wait_for_ready_timer));
  ExpectStateUpdate(kSmeProtHStateServerListenProposal);

  // Act: Stop timer and move to next state
  SmeProtHandshakeStateServerInit(&sc);

  // Assert: SME is in kSmeProtHStateServerListenProposal
  EXPECT_EQ(SHIP_CONNECTION_GET_SHIP_STATE(&sc, nullptr), kSmeProtHStateServerListenProposal);
  ExpectConnectionClose("", false);
}
