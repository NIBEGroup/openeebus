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
#include "src/ship/ship_connection/server.h"
#include "tests/src/ship/ship_connection/ship_connection/ship_connection_test_suite.h"

struct ServerListenChoiceTestInput {
  std::string_view description{""sv};
  ShipConnectionQueueMsgType queue_msg_type{kShipConnectionQueueMsgTypeDataReceived};
  std::string_view msg{""sv};
  std::string_view abort_err_msg{kProtHandshakeAbortUnexpectedMessage};
};

class ServerConfirmReceivedMessageTests : public ShipConnectionTestSuite,
                                          public ::testing::WithParamInterface<ServerListenChoiceTestInput> {};

class ServerCorrectVersionSelectMessageReceivedTest : public ShipConnectionTestSuite {};

std::ostream& operator<<(std::ostream& os, const ServerListenChoiceTestInput& input) {
  return os << input.description;
}

TEST_F(ServerCorrectVersionSelectMessageReceivedTest, ServerCorrectVersionSelectMessageReceivedTest) {
  // Arrange:
  // Init message buffer
  MessageBuffer msg_buf{0};
  ASSERT_EQ(MessageBufferInitHelper(&msg_buf, kProtHandshakeSelectVersionMsg), kEebusErrorOk) << "Wrong test input!";
  ShipConnectionWebsocketCallback(kWebsocketCallbackTypeRead, msg_buf.data, msg_buf.data_size, &sc);

  EXPECT_CALL(*wfr_timer_mock->gmock, Start(sc.wait_for_ready_timer, cmiTimeout, false));
  EXPECT_CALL(*wfr_timer_mock->gmock, Stop(sc.wait_for_ready_timer)).Times(2);
  ExpectStateUpdate(kSmeProtHStateServerOk);

  // Act: Check if the message content is handled correctly
  SmeProtHandshakeStateServerListenConfirm(&sc);

  // Assert: SME is in kSmeProtHStateServerOk
  EXPECT_EQ(SHIP_CONNECTION_GET_SHIP_STATE(&sc, nullptr), kSmeProtHStateServerOk);
  ExpectConnectionClose("", false);
}

TEST_P(ServerConfirmReceivedMessageTests, ProtHandshakeServerListenConfirmWrongMessageReceivedTest) {
  // Arrange:
  // Init message buffer
  ShipConnectionQueueMessage queue_msg{GetParam().queue_msg_type, {nullptr}};
  ASSERT_EQ(MessageBufferInitHelper(&queue_msg.msg_buf, GetParam().msg), kEebusErrorOk) << "Wrong test input!";

  // Add message to queue
  EEBUS_QUEUE_SEND(sc.msg_queue, &queue_msg, sizeof(queue_msg));

  EXPECT_CALL(*wfr_timer_mock->gmock, Start(sc.wait_for_ready_timer, cmiTimeout, false));
  EXPECT_CALL(*wfr_timer_mock->gmock, Stop(sc.wait_for_ready_timer)).Times(3);
  EXPECT_CALL(*spr_timer_mock->gmock, Stop(sc.send_prolongation_request_timer));
  EXPECT_CALL(*prr_timer_mock->gmock, Stop(sc.prolongation_request_reply_timer));

  MessageBuffer abort_msg_buf{0};
  ASSERT_EQ(MessageBufferInitHelper(&abort_msg_buf, GetParam().abort_err_msg), kEebusErrorOk) << "Wrong test input!";
  ExpectWebsocketWrite(abort_msg_buf, false);
  ExpectStateUpdate(kSmeStateError);
  ExpectWebsocketClose("Error sending ship message", false);

  // Act: Check if wrong message format is handled
  SmeProtHandshakeStateServerListenConfirm(&sc);

  // Assert: SME is in kSmeStateError
  EXPECT_EQ(SHIP_CONNECTION_GET_SHIP_STATE(&sc, nullptr), kSmeStateError);
}

INSTANTIATE_TEST_SUITE_P(
    ServerConfirmReceivedMessageTests,
    ServerConfirmReceivedMessageTests,
    ::testing::Values(
        ServerListenChoiceTestInput{
            .description    = "Bad queue message type received (data to send)"sv,
            .queue_msg_type = kShipConnectionQueueMsgTypeSpineDataToSend,
            .msg            = kProtHandshakeSelectVersionMsg,
        },
        ServerListenChoiceTestInput{
            .description    = "Bad queue message type received (timeout)"sv,
            .queue_msg_type = kShipConnectionQueueMsgTypeTimeout,
            .msg            = kProtHandshakeSelectVersionMsg,
        },
        ServerListenChoiceTestInput{
            .description    = "Bad queue message type received (websocket error)"sv,
            .queue_msg_type = kShipConnectionQueueMsgTypeWebsocketError,
            .msg            = kProtHandshakeSelectVersionMsg,
        },
        ServerListenChoiceTestInput{
            .description    = "Bad queue message type received (websocket close)"sv,
            .queue_msg_type = kShipConnectionQueueMsgTypeWebsocketClose,
            .msg            = kProtHandshakeSelectVersionMsg,
        },
        ServerListenChoiceTestInput{
            .description    = "Bad queue message type received (cancel)"sv,
            .queue_msg_type = kShipConnectionQueueMsgTypeCancel,
            .msg            = kProtHandshakeSelectVersionMsg,
        },
        ServerListenChoiceTestInput{
            .description   = "Wrong message value type received"sv,
            .msg           = kProtHandshakeAnnounceMaxMsg,
            .abort_err_msg = kProtHandshakeAbortDataMismatchMsg,
        },
        ServerListenChoiceTestInput{
            .description   = "No message value received"sv,
            .msg           = kProtHandshakeNoValueMsg,
            .abort_err_msg = kProtHandshakeAbortDataMismatchMsg,
        }
    )
);
