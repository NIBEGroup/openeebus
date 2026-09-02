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

struct ClientListenChoiceTestInput {
  std::string_view description{""sv};
  const char* close_error_msg{"Error sending ship message"};
  ShipConnectionQueueMsgType queue_msg_type{kShipConnectionQueueMsgTypeDataReceived};
  std::string_view msg{""sv};
  SmeState expected_state{kSmeStateError};
};

class ProtocolHandshakeClientListenChoiceInvalidMessageFormatTests
    : public ShipConnectionTestSuite,
      public ::testing::WithParamInterface<ClientListenChoiceTestInput> {};

class ProtocolHandshakeClientListenChoiceInvalidMessageContentTests
    : public ShipConnectionTestSuite,
      public ::testing::WithParamInterface<ClientListenChoiceTestInput> {};

class ProtocolHandshakeClientSelectVersionTests : public ShipConnectionTestSuite,
                                                  public ::testing::WithParamInterface<ClientListenChoiceTestInput> {};

std::ostream& operator<<(std::ostream& os, const ClientListenChoiceTestInput& input) {
  return os << input.description;
}

TEST_P(
    ProtocolHandshakeClientListenChoiceInvalidMessageFormatTests,
    ProtocolHandshakeClientListenChoiceInvalidMessageFormatTests
) {
  // Arrange:
  // Init queue message
  ShipConnectionQueueMessage queue_msg{GetParam().queue_msg_type, {nullptr}};
  ASSERT_EQ(MessageBufferInitHelper(&queue_msg.msg_buf, GetParam().msg), kEebusErrorOk) << "Wrong test input!";

  // Send message to queue
  EEBUS_QUEUE_SEND(sc.msg_queue, &queue_msg, sizeof(queue_msg));

  // Expect timer function calls
  EXPECT_CALL(*wfr_timer_mock->gmock, Start(sc.wait_for_ready_timer, cmiTimeout, false));
  EXPECT_CALL(*wfr_timer_mock->gmock, Stop(sc.wait_for_ready_timer)).Times(3);
  EXPECT_CALL(*spr_timer_mock->gmock, Stop(sc.send_prolongation_request_timer));
  EXPECT_CALL(*prr_timer_mock->gmock, Stop(sc.prolongation_request_reply_timer));

  MessageBuffer msg_buf{0};
  ASSERT_EQ(MessageBufferInitHelper(&msg_buf, kProtHandshakeAbortUnexpectedMessage), kEebusErrorOk)
      << "Wrong test input!";
  ExpectWebsocketWrite(msg_buf, false);

  // Expect state update function calls
  ExpectStateUpdate(kSmeStateError);
  ExpectWebsocketClose("Error sending ship message", false);

  // Act: Check if wrong message format is handled
  SmeProtHandshakeStateClientListenChoice(&sc);

  // Assert: SME is in kSmeStateError
  EXPECT_EQ(SHIP_CONNECTION_GET_SHIP_STATE(&sc, nullptr), kSmeStateError);
}

INSTANTIATE_TEST_SUITE_P(
    ProtocolHandshakeClientListenChoiceInvalidMessageFormatTests,
    ProtocolHandshakeClientListenChoiceInvalidMessageFormatTests,
    ::testing::Values(
        ClientListenChoiceTestInput{
            .description    = "Timed out waiting for message"sv,
            .queue_msg_type = kShipConnectionQueueMsgTypeTimeout,
            .msg            = kProtHandshakeSelectVersionMsg,
        },
        ClientListenChoiceTestInput{
            .description    = "Canceled waiting for message"sv,
            .queue_msg_type = kShipConnectionQueueMsgTypeCancel,
            .msg            = kProtHandshakeSelectVersionMsg,
        },
        ClientListenChoiceTestInput{
            .description    = "Websocket error while waiting for message"sv,
            .queue_msg_type = kShipConnectionQueueMsgTypeWebsocketError,
            .msg            = kProtHandshakeSelectVersionMsg,
        },
        ClientListenChoiceTestInput{
            .description    = "Websocket close while waiting for message"sv,
            .queue_msg_type = kShipConnectionQueueMsgTypeWebsocketClose,
            .msg            = kProtHandshakeSelectVersionMsg,
        },
        ClientListenChoiceTestInput{
            .description    = "Spine data to send received while waiting for message"sv,
            .queue_msg_type = kShipConnectionQueueMsgTypeSpineDataToSend,
            .msg            = kProtHandshakeSelectVersionMsg,
        },
        ClientListenChoiceTestInput{
            .description = "Wrong message value type received"sv,
            .msg         = kProtHandshakeAnnounceMaxMsg,
        },
        ClientListenChoiceTestInput{
            .description = "No message value received"sv,
            .msg         = kProtHandshakeNoValueMsg,
        }
    )
);

TEST_P(
    ProtocolHandshakeClientListenChoiceInvalidMessageContentTests,
    ProtocolHandshakeClientListenChoiceInvalidMessageContentTests
) {
  // Arrange: Init message buffer
  MessageBuffer msg_buf{0};
  ASSERT_EQ(MessageBufferInitHelper(&msg_buf, GetParam().msg), kEebusErrorOk) << "Wrong test input!";
  ShipConnectionWebsocketCallback(kWebsocketCallbackTypeRead, msg_buf.data, msg_buf.data_size, &sc);

  // Expect timer function calls
  EXPECT_CALL(*wfr_timer_mock->gmock, Start(sc.wait_for_ready_timer, cmiTimeout, false));
  EXPECT_CALL(*wfr_timer_mock->gmock, Stop(sc.wait_for_ready_timer)).Times(4);
  EXPECT_CALL(*spr_timer_mock->gmock, Stop(sc.send_prolongation_request_timer));
  EXPECT_CALL(*prr_timer_mock->gmock, Stop(sc.prolongation_request_reply_timer));

  // Expect error message send function calls
  MessageBuffer err_msg_buf{0};
  ASSERT_EQ(MessageBufferInitHelper(&err_msg_buf, kProtHandshakeAbortDataMismatchMsg), kEebusErrorOk)
      << "Wrong test input!";
  ExpectWebsocketWrite(err_msg_buf, false);

  // Expect state update function calls
  ExpectStateUpdate(kSmeStateError);
  ExpectWebsocketClose("Error sending ship message", false);

  // Act: Check message format errors
  SmeProtHandshakeStateClientListenChoice(&sc);

  // Assert: SME state changed accordingly
  EXPECT_EQ(SHIP_CONNECTION_GET_SHIP_STATE(&sc, nullptr), kSmeStateError);
}

INSTANTIATE_TEST_SUITE_P(
    ProtocolHandshakeClientListenChoiceInvalidMessageContentTests,
    ProtocolHandshakeClientListenChoiceInvalidMessageContentTests,
    ::testing::Values(
        ClientListenChoiceTestInput{
            .description = "Invalid protocol handshake response"sv,
            .msg         = kProtHandshakeInvalidAnnounceMaxMsg,
        },
        ClientListenChoiceTestInput{
            .description = "Unsupported protocol major version"sv,
            .msg         = kProtHandshakeUnsupportedMajorVersionMsg,
        },
        ClientListenChoiceTestInput{
            .description = "Unsupported protocol minor version"sv,
            .msg         = kProtHandshakeUnsupportedMinorVersionMsg,
        },
        ClientListenChoiceTestInput{
            .description = "Missing format in message"sv,
            .msg         = kProtHandshakeMissingFormatMsg,
        },
        ClientListenChoiceTestInput{
            .description = "Unsupported format in message"sv,
            .msg         = kProtHandshakeUnsupportedFormatMsg,
        }
    )
);

TEST_P(ProtocolHandshakeClientSelectVersionTests, ProtocolHandshakeClientSelectVersionTests) {
  // Arrange: Init message buffer
  MessageBuffer msg_buf{0};
  ASSERT_EQ(MessageBufferInitHelper(&msg_buf, kProtHandshakeSelectVersionMsg), kEebusErrorOk) << "Wrong test input!";
  ShipConnectionWebsocketCallback(kWebsocketCallbackTypeRead, msg_buf.data, msg_buf.data_size, &sc);

  // Expect message send function calls
  ExpectWebsocketWrite(msg_buf, GetParam().expected_state != kSmeStateError);
  EXPECT_CALL(*wfr_timer_mock->gmock, Start(sc.wait_for_ready_timer, cmiTimeout, false));

  // Expect state update function calls
  ExpectStateUpdate(GetParam().expected_state);
  EXPECT_CALL(*wfr_timer_mock->gmock, Stop(sc.wait_for_ready_timer)).Times(3);
  EXPECT_CALL(*spr_timer_mock->gmock, Stop(sc.send_prolongation_request_timer));
  EXPECT_CALL(*prr_timer_mock->gmock, Stop(sc.prolongation_request_reply_timer));
  ExpectWebsocketClose(GetParam().close_error_msg, false);

  // Act: Send message
  SmeProtHandshakeStateClientListenChoice(&sc);

  // Assert: SME state changed accordingly
  EXPECT_EQ(SHIP_CONNECTION_GET_SHIP_STATE(&sc, nullptr), GetParam().expected_state);
}

INSTANTIATE_TEST_SUITE_P(
    ProtocolHandshakeClientSelectVersionTests,
    ProtocolHandshakeClientSelectVersionTests,
    ::testing::Values(
        ClientListenChoiceTestInput{
            .description     = "Version message failed to send"sv,
            .close_error_msg = "Error serializing protocol handshake ship message",
        },
        ClientListenChoiceTestInput{
            .description     = "Version message successfully sent"sv,
            .close_error_msg = "",
            .expected_state  = kSmeProtHStateClientOk,
        }
    )
);
