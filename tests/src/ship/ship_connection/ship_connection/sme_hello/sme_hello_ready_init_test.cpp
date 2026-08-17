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

class SmeHelloStateReadyInitTests : public ShipConnectionTestSuite {};

TEST_F(SmeHelloStateReadyInitTests, ReadyInitMessageSuccessfullySent) {
  // Arrange:
  // Unformat JSON message
  MessageBuffer msg_buf{0};
  ASSERT_EQ(MessageBufferInitHelper(&msg_buf, kSmeHelloReadyMsg), kEebusErrorOk) << "Wrong test input!";
  ShipConnectionWebsocketCallback(kWebsocketCallbackTypeRead, msg_buf.data, msg_buf.data_size, &sc);

  // Expect timer function calls
  EXPECT_CALL(*prr_timer_mock->gmock, Stop(sc.prolongation_request_reply_timer));
  EXPECT_CALL(*spr_timer_mock->gmock, Stop(sc.send_prolongation_request_timer));

  // Calculate message size
  ExpectWebsocketWrite(msg_buf, true);
  ExpectStateUpdate(kSmeHelloStateReadyListen);

  // Act: Send ready message
  SmeHelloStateReadyInit(&sc);

  // Assert: SME state changed accordingly
  EXPECT_EQ(SHIP_CONNECTION_GET_SHIP_STATE(&sc, nullptr), kSmeHelloStateReadyListen);
  ExpectConnectionClose("", false);
}

TEST_F(SmeHelloStateReadyInitTests, ReadyInitMessageFailedToSend) {
  // Arrange:
  // Unformat JSON message
  MessageBuffer msg_buf{0};
  ASSERT_EQ(MessageBufferInitHelper(&msg_buf, kSmeHelloReadyMsg), kEebusErrorOk) << "Wrong test input!";
  ShipConnectionWebsocketCallback(kWebsocketCallbackTypeRead, msg_buf.data, msg_buf.data_size, &sc);

  // Expect timer function calls
  EXPECT_CALL(*wfr_timer_mock->gmock, Stop(sc.wait_for_ready_timer));
  EXPECT_CALL(*prr_timer_mock->gmock, Stop(sc.prolongation_request_reply_timer)).Times(2);
  EXPECT_CALL(*spr_timer_mock->gmock, Stop(sc.send_prolongation_request_timer)).Times(2);

  // Calculate message size
  ExpectWebsocketWrite(msg_buf, false);
  ExpectStateUpdate(kSmeHelloStateAbort);
  ExpectWebsocketClose("", false);

  // Act: Send ready message
  SmeHelloStateReadyInit(&sc);

  // Assert: SME state changed accordingly
  EXPECT_EQ(SHIP_CONNECTION_GET_SHIP_STATE(&sc, nullptr), kSmeHelloStateAbort);
}
