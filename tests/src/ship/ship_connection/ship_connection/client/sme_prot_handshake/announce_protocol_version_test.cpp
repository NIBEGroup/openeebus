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

#include "protocol_handshake_messages.inc"
#include "src/ship/ship_connection/client.h"
#include "tests/src/ship/ship_connection/ship_connection/ship_connection_test_suite.h"

class ProtocolHandshakeClientInitTests : public ShipConnectionTestSuite {
 protected:
  void SetUp() override {
    ShipConnectionTestSuite::SetUp();
    ASSERT_EQ(MessageBufferInitHelper(&msg_buf, kProtHandshakeAnnounceMaxVersionMsg), kEebusErrorOk)
        << "Wrong test input!";
  }

  MessageBuffer msg_buf{0};
};

TEST_F(ProtocolHandshakeClientInitTests, MessageSuccessfullySent) {
  // Arrange: Send client Init protocol handshake message successfully
  EXPECT_CALL(*wfr_timer_mock->gmock, Stop(sc.wait_for_ready_timer));
  ExpectWebsocketWrite(msg_buf, true);
  ExpectStateUpdate(kSmeProtHStateClientListenChoice);

  // Act: Try to send client Init protocol handshake message
  SmeProtHandshakeStateClientInit(&sc);

  // Assert: SME is in kSmeProtHStateClientListenChoice
  EXPECT_EQ(SHIP_CONNECTION_GET_SHIP_STATE(&sc, nullptr), kSmeProtHStateClientListenChoice);
  ExpectConnectionClose("", false);
}

TEST_F(ProtocolHandshakeClientInitTests, MessageFailedToSend) {
  // Arrange: Fail to send client Init protocol handshake message
  ExpectWebsocketWrite(msg_buf, false);
  ExpectStateUpdate(kSmeStateError);
  EXPECT_CALL(*wfr_timer_mock->gmock, Stop(sc.wait_for_ready_timer)).Times(2);
  EXPECT_CALL(*spr_timer_mock->gmock, Stop(sc.send_prolongation_request_timer));
  EXPECT_CALL(*prr_timer_mock->gmock, Stop(sc.prolongation_request_reply_timer));
  ExpectWebsocketClose("Error serializing protocol handshake ship message", false);

  // Act: Try to send client Init protocol handshake message
  SmeProtHandshakeStateClientInit(&sc);

  // Assert: SME is in kSmeStateError
  EXPECT_EQ(SHIP_CONNECTION_GET_SHIP_STATE(&sc, nullptr), kSmeStateError);
}
