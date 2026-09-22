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
#include "tests/src/ship/ship_connection/ship_connection/ship_connection_test_suite.h"

using std::literals::string_view_literals::operator""sv;
constexpr std::string_view kPinInitMessage{R"({"connectionPinState": [{"pinState": "none"}]})"sv};

class PinCheckInitMessageSendTests : public ShipConnectionTestSuite {
 protected:
  void SetUp() override {
    ShipConnectionTestSuite::SetUp();
    SetShipConnectionState(kSmePinStateCheckInit);
    ASSERT_EQ(MessageBufferInitHelper(&msg_buf, kPinInitMessage), kEebusErrorOk) << "Wrong test input!";
    ShipConnectionWebsocketCallback(kWebsocketCallbackTypeRead, msg_buf.data, msg_buf.data_size, &sc);
  }

  MessageBuffer msg_buf{0};
};

TEST_F(PinCheckInitMessageSendTests, MessageSuccessfullySent) {
  // Arrange: Expect PIN requirement message to be sent successfully
  ExpectWebsocketWrite(msg_buf, true);
  EXPECT_CALL(*wfr_timer_mock->gmock, Start(sc.wait_for_ready_timer, cmiTimeout, false));
  EXPECT_CALL(*wfr_timer_mock->gmock, Stop(sc.wait_for_ready_timer));
  ExpectStateUpdate(kSmePinStateCheckOk);

  // Act: Try to send PIN requirement
  SmePinStateCheckInit(&sc);

  // Assert: SME is in kSmePinStateCheckOk
  EXPECT_EQ(SHIP_CONNECTION_GET_SHIP_STATE(&sc, nullptr), kSmePinStateCheckOk);
  ExpectConnectionClose("", false);
}

TEST_F(PinCheckInitMessageSendTests, MessageFailedToSend) {
  // Arrange: Expect PIN requirement message to fail to send and SME to enter error state
  ExpectWebsocketWrite(msg_buf, false);
  ExpectStateUpdate(kSmeStateError);
  ExpectConnectionClose("Error sending PIN requirement message", false);

  // Act: Try to send PIN requirement
  SmePinStateCheckInit(&sc);

  // Assert: SME is in kSmeStateError
  EXPECT_EQ(SHIP_CONNECTION_GET_SHIP_STATE(&sc, nullptr), kSmeStateError);
}
