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

#include "ship_hello_state_messages.inc"
#include "tests/src/ship/ship_connection/ship_connection/ship_connection_test_suite.h"

class SmeHelloAbortStateTests : public ShipConnectionTestSuite {
 protected:
  void SetUp() override {
    ShipConnectionTestSuite::SetUp();
    SetShipConnectionState(kSmeHelloStateAbort);
    ASSERT_EQ(MessageBufferInitHelper(&msg_buf, kSmeHelloStateAbortMsg), kEebusErrorOk) << "Wrong test input!";
    ShipConnectionWebsocketCallback(kWebsocketCallbackTypeRead, msg_buf.data, msg_buf.data_size, &sc);
  }

  MessageBuffer msg_buf{0};
};

TEST_F(SmeHelloAbortStateTests, MessageSuccessfullySent) {
  // Arrange: Expect abort message to be sent successfully
  ExpectWebsocketWrite(msg_buf, true);
  ExpectStateUpdate(kSmeStateError);
  ExpectConnectionClose("SME Hello state connection aborted", false);

  // Act: Abort connection
  SmeHelloStateAbort(&sc);

  // Assert: SME is in kSmeStateError
  EXPECT_EQ(SHIP_CONNECTION_GET_SHIP_STATE(&sc, nullptr), kSmeStateError);
}

TEST_F(SmeHelloAbortStateTests, MessageFailedToSend) {
  // Arrange: Expect abort message to fail to send
  ExpectWebsocketWrite(msg_buf, false);
  ExpectStateUpdate(kSmeStateError);
  ExpectConnectionClose("SME Hello state connection aborted", false);

  // Act: Abort connection
  SmeHelloStateAbort(&sc);

  // Assert: SME is in kSmeStateError
  EXPECT_EQ(SHIP_CONNECTION_GET_SHIP_STATE(&sc, nullptr), kSmeStateError);
}
