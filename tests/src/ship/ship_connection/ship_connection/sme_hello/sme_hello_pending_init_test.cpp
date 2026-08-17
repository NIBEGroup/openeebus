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

class SmeHelloPendingInitTests : public ShipConnectionTestSuite {
 protected:
  void SetUp() override {
    ShipConnectionTestSuite::SetUp();
    ASSERT_EQ(MessageBufferInitHelper(&msg_buf, kSmeHelloPendingInitMsg), kEebusErrorOk) << "Wrong test input!";
    ShipConnectionWebsocketCallback(kWebsocketCallbackTypeRead, msg_buf.data, msg_buf.data_size, &sc);
  }

  MessageBuffer msg_buf{0};
};

TEST_F(SmeHelloPendingInitTests, PendingInitMessageSentSuccessfully) {
  // Arrange: Expect pending init message to be sent successfully
  ExpectWebsocketWrite(msg_buf, true);
  ExpectStateUpdate(kSmeHelloStatePendingListen);
  EXPECT_CALL(*spr_timer_mock->gmock, Stop(sc.send_prolongation_request_timer));
  EXPECT_CALL(*prr_timer_mock->gmock, Stop(sc.prolongation_request_reply_timer));

  // Act: Try to send pending init message
  SmeHelloStatePendingInit(&sc);

  // Assert: SME is in kSmeHelloStatePendingListen
  EXPECT_EQ(SHIP_CONNECTION_GET_SHIP_STATE(&sc, nullptr), kSmeHelloStatePendingListen);
  ExpectConnectionClose("", false);
}

TEST_F(SmeHelloPendingInitTests, PendingInitMessageFailedToSend) {
  // Arrange: Expect pending init message to fail to send
  ExpectWebsocketWrite(msg_buf, false);
  EXPECT_CALL(*wfr_timer_mock->gmock, Stop(sc.wait_for_ready_timer));
  EXPECT_CALL(*spr_timer_mock->gmock, Stop(sc.send_prolongation_request_timer)).Times(2);
  EXPECT_CALL(*prr_timer_mock->gmock, Stop(sc.prolongation_request_reply_timer)).Times(2);
  ExpectStateUpdate(kSmeHelloStateAbort);
  ExpectWebsocketClose("", false);

  // Act: Send pending init message
  SmeHelloStatePendingInit(&sc);

  // Assert: SME state changed accordingly
  EXPECT_EQ(SHIP_CONNECTION_GET_SHIP_STATE(&sc, nullptr), kSmeHelloStateAbort);
}
