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

struct ProtHandshakeServerListenProposalTestInput {
  std::string_view description{""sv};
  const char* close_error_msg{"Error sending ship message"};
  ShipConnectionQueueMsgType queue_msg_type{kShipConnectionQueueMsgTypeDataReceived};
  std::string_view msg{""sv};
  std::string_view abort_msg{""sv};
  SmeState expected_sme_state{kSmeStateError};
  bool msg_send_successful{false};
};

class ProtHandshakeServerListenProposalEvaluateReceivedMessageTypeTests
    : public ShipConnectionTestSuite,
      public ::testing::WithParamInterface<ProtHandshakeServerListenProposalTestInput> {};

class ProtHandshakeServerListenProposalEvaluateReceivedMessageContentsTests
    : public ShipConnectionTestSuite,
      public ::testing::WithParamInterface<ProtHandshakeServerListenProposalTestInput> {};

class ProtHandshakeServerListenProposalAgreementMessageSendTests
    : public ShipConnectionTestSuite,
      public ::testing::WithParamInterface<ProtHandshakeServerListenProposalTestInput> {};

std::ostream& operator<<(std::ostream& os, const ProtHandshakeServerListenProposalTestInput& input) {
  return os << input.description;
}

TEST_P(ProtHandshakeServerListenProposalEvaluateReceivedMessageTypeTests, ReceiveUnexpectedMessage) {
  // Arrange:
  // Init queue message
  ShipConnectionQueueMessage queue_msg{GetParam().queue_msg_type, {nullptr}};
  ASSERT_EQ(MessageBufferInitHelper(&queue_msg.msg_buf, GetParam().msg), kEebusErrorOk) << "Wrong test input!";

  // Add message to queue
  EEBUS_QUEUE_SEND(sc.msg_queue, &queue_msg, sizeof(queue_msg));

  // Expect timer function calls
  EXPECT_CALL(*wfr_timer_mock->gmock, Start(sc.wait_for_ready_timer, cmiTimeout, false));
  EXPECT_CALL(*wfr_timer_mock->gmock, Stop(sc.wait_for_ready_timer)).Times(3);
  EXPECT_CALL(*spr_timer_mock->gmock, Stop(sc.send_prolongation_request_timer));
  EXPECT_CALL(*prr_timer_mock->gmock, Stop(sc.prolongation_request_reply_timer));

  MessageBuffer abort_msg{0};
  ASSERT_EQ(MessageBufferInitHelper(&abort_msg, kProtHandshakeAbortUnexpectedMessage), kEebusErrorOk)
      << "Wrong test input!";

  ExpectWebsocketWrite(abort_msg, false);
  ExpectStateUpdate(kSmeStateError);
  ExpectWebsocketClose("Error sending ship message", false);

  // Act: Handle proposal message
  SmeProtHandshakeStateServerListenProposal(&sc);

  // Assert: SME is in kSmeStateError
  EXPECT_EQ(SHIP_CONNECTION_GET_SHIP_STATE(&sc, nullptr), kSmeStateError);
}

INSTANTIATE_TEST_SUITE_P(
    ProtHandshakeServerListenProposalEvaluateReceivedMessageTypeTests,
    ProtHandshakeServerListenProposalEvaluateReceivedMessageTypeTests,
    ::testing::Values(
        ProtHandshakeServerListenProposalTestInput{
            .description    = "Spine data to send message received"sv,
            .queue_msg_type = kShipConnectionQueueMsgTypeSpineDataToSend,
            .msg            = kProtHandshakeAnnounceMaxMsg,
        },
        ProtHandshakeServerListenProposalTestInput{
            .description    = "Timeout Message received"sv,
            .queue_msg_type = kShipConnectionQueueMsgTypeTimeout,
            .msg            = kProtHandshakeAnnounceMaxMsg,
        },
        ProtHandshakeServerListenProposalTestInput{
            .description    = "Websocket error message received"sv,
            .queue_msg_type = kShipConnectionQueueMsgTypeWebsocketError,
            .msg            = kProtHandshakeAnnounceMaxMsg,
        },
        ProtHandshakeServerListenProposalTestInput{
            .description    = "Websocket close message received"sv,
            .queue_msg_type = kShipConnectionQueueMsgTypeWebsocketClose,
            .msg            = kProtHandshakeAnnounceMaxMsg,
        },
        ProtHandshakeServerListenProposalTestInput{
            .description    = "Cancel message received"sv,
            .queue_msg_type = kShipConnectionQueueMsgTypeCancel,
            .msg            = kProtHandshakeAnnounceMaxMsg,
        },
        ProtHandshakeServerListenProposalTestInput{
            .description = "Wrong message value type received"sv,
            .msg         = kProtHandshakeAnnounceMaxInvalidMessageValueType,
        },
        ProtHandshakeServerListenProposalTestInput{
            .description = "No message value received"sv,
            .msg         = kProtHandshakeNoValueMsg,
        }
    )
);

TEST_P(
    ProtHandshakeServerListenProposalEvaluateReceivedMessageContentsTests,
    ProtHandshakeServerListenProposalEvaluateReceivedMessageContentsTest
) {
  // Arrange:
  // Init message buffer
  ShipConnectionQueueMessage queue_msg{GetParam().queue_msg_type, {nullptr}};
  ASSERT_EQ(MessageBufferInitHelper(&queue_msg.msg_buf, GetParam().msg), kEebusErrorOk) << "Wrong test input!";

  // Add message to queue
  EEBUS_QUEUE_SEND(sc.msg_queue, &queue_msg, sizeof(queue_msg));

  // Expect timer function calls
  EXPECT_CALL(*wfr_timer_mock->gmock, Start(sc.wait_for_ready_timer, cmiTimeout, false));
  EXPECT_CALL(*wfr_timer_mock->gmock, Stop(sc.wait_for_ready_timer)).Times(4);
  EXPECT_CALL(*spr_timer_mock->gmock, Stop(sc.send_prolongation_request_timer));
  EXPECT_CALL(*prr_timer_mock->gmock, Stop(sc.prolongation_request_reply_timer));

  MessageBuffer abort_msg{0};
  ASSERT_EQ(MessageBufferInitHelper(&abort_msg, GetParam().abort_msg), kEebusErrorOk) << "Wrong test input!";
  ExpectWebsocketWrite(abort_msg, false);
  ExpectStateUpdate(kSmeStateError);
  ExpectWebsocketClose("Error sending ship message", false);

  // Act: Verify the received message content is appropriate
  SmeProtHandshakeStateServerListenProposal(&sc);

  // Assert: SME is in kSmeStateError
  EXPECT_EQ(SHIP_CONNECTION_GET_SHIP_STATE(&sc, nullptr), kSmeStateError);
}

INSTANTIATE_TEST_SUITE_P(
    ProtHandshakeServerListenProposalEvaluateReceivedMessageContentsTests,
    ProtHandshakeServerListenProposalEvaluateReceivedMessageContentsTests,
    ::testing::Values(
        ProtHandshakeServerListenProposalTestInput{
            .description = "Wrong handshake type received"sv,
            .msg         = kProtHandshakeSelectVersionMsg,
            .abort_msg   = kProtHandshakeAbortUnexpectedMessage,
        },
        ProtHandshakeServerListenProposalTestInput{
            .description = "Unsupported major version received"sv,
            .msg         = kProtocolHandshakeAnnounceUnsupportedMajorVersionMsg,
            .abort_msg   = kProtHandshakeAbortDataMismatchMsg
        },
        ProtHandshakeServerListenProposalTestInput{
            .description = "Unsupported minor version received"sv,
            .msg         = kProtocolHandshakeAnnounceUnsupportedMinorVersionMsg,
            .abort_msg   = kProtHandshakeAbortDataMismatchMsg
        }
    )
);

TEST_P(
    ProtHandshakeServerListenProposalAgreementMessageSendTests,
    ProtHandshakeServerListenProposalAgreementMessageSendTest
) {
  // Arrange:
  // Init message buffer
  ShipConnectionQueueMessage queue_msg{GetParam().queue_msg_type, {nullptr}};
  ASSERT_EQ(MessageBufferInitHelper(&queue_msg.msg_buf, GetParam().msg), kEebusErrorOk) << "Wrong test input!";

  // Send message to queue
  EEBUS_QUEUE_SEND(sc.msg_queue, &queue_msg, sizeof(queue_msg));

  MessageBuffer version_msg_buf{0};
  ASSERT_EQ(MessageBufferInitHelper(&version_msg_buf, kProtHandshakeSelectVersionMsg), kEebusErrorOk)
      << "Wrong test input!";
  ExpectWebsocketWrite(version_msg_buf, GetParam().msg_send_successful);

  EXPECT_CALL(*wfr_timer_mock->gmock, Start(sc.wait_for_ready_timer, cmiTimeout, false));
  EXPECT_CALL(*wfr_timer_mock->gmock, Stop(sc.wait_for_ready_timer)).Times(3);
  EXPECT_CALL(*spr_timer_mock->gmock, Stop(sc.send_prolongation_request_timer));
  EXPECT_CALL(*prr_timer_mock->gmock, Stop(sc.prolongation_request_reply_timer));

  ExpectStateUpdate(GetParam().expected_sme_state);
  ExpectWebsocketClose(GetParam().close_error_msg, false);

  // Act: Check message send handling
  SmeProtHandshakeStateServerListenProposal(&sc);

  // Assert: SME is in the expected state
  EXPECT_EQ(SHIP_CONNECTION_GET_SHIP_STATE(&sc, nullptr), GetParam().expected_sme_state);
}

INSTANTIATE_TEST_SUITE_P(
    ProtHandshakeServerListenProposalAgreementMessageSendTests,
    ProtHandshakeServerListenProposalAgreementMessageSendTests,
    ::testing::Values(
        ProtHandshakeServerListenProposalTestInput{
            .description         = "Proper version message received and reply sent"sv,
            .close_error_msg     = "",
            .msg                 = kProtHandshakeAnnounceMaxMsg,
            .expected_sme_state  = kSmeProtHStateServerListenConfirm,
            .msg_send_successful = true,
        },
        ProtHandshakeServerListenProposalTestInput{
            .description     = "Proper version message received, error while sending agreement message"sv,
            .close_error_msg = "Error serializing protocol handshake ship message",
            .msg             = kProtHandshakeAnnounceMaxMsg,
        }
    )
);
