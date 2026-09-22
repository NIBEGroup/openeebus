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

struct SmeHelloPendingListenTestInput {
  std::string_view description{""sv};
  ShipConnectionQueueMsgType queue_msg_type{kShipConnectionQueueMsgTypeDataReceived};
  std::string_view msg{kSmeHelloReadyMsg};
  SmeState expected_sme_state{kSmeHelloStateAbort};
};

class SmeHelloStatePendingListenTests : public ShipConnectionTestSuite,
                                        public ::testing::WithParamInterface<SmeHelloPendingListenTestInput> {};

class SmeHelloStatePendingListenEvaluateReadyMessageTests : public ShipConnectionTestSuite {};
class SmeHelloStatePendingListenEvaluatePendingMessageTests : public ShipConnectionTestSuite {};

std::ostream& operator<<(std::ostream& os, const SmeHelloPendingListenTestInput& input) {
  return os << input.description;
}

TEST_P(SmeHelloStatePendingListenTests, WrongQueueMessageTypeReceived) {
  // Arrange:
  // Init message buffer
  ShipConnectionQueueMessage queue_msg{GetParam().queue_msg_type, {nullptr}};
  ASSERT_EQ(MessageBufferInitHelper(&queue_msg.msg_buf, GetParam().msg), kEebusErrorOk) << "Wrong test input!";

  // Add message to queue
  EEBUS_QUEUE_SEND(sc.msg_queue, &queue_msg, sizeof(queue_msg));

  // Expect function calls
  EXPECT_CALL(*wfr_timer_mock->gmock, Start(sc.wait_for_ready_timer, tHelloInit, false));
  EXPECT_CALL(*wfr_timer_mock->gmock, Stop(sc.wait_for_ready_timer)).Times(2);
  EXPECT_CALL(*prr_timer_mock->gmock, Stop(sc.prolongation_request_reply_timer));
  EXPECT_CALL(*spr_timer_mock->gmock, Stop(sc.send_prolongation_request_timer));
  ExpectStateUpdate(GetParam().expected_sme_state);
  ExpectWebsocketClose("", false);

  // Act: Listen and process the received message
  SmeHelloStatePendingListen(&sc);

  // Assert: Verify that the state changed accordingly
  EXPECT_EQ(SHIP_CONNECTION_GET_SHIP_STATE(&sc, nullptr), GetParam().expected_sme_state);
}

INSTANTIATE_TEST_SUITE_P(
    SmeHelloStatePendingListenTests,
    SmeHelloStatePendingListenTests,
    ::testing::Values(
        SmeHelloPendingListenTestInput{
            .description        = "Timeout message received"sv,
            .queue_msg_type     = kShipConnectionQueueMsgTypeTimeout,
            .expected_sme_state = kSmeHelloStatePendingTimeout,
        },
        SmeHelloPendingListenTestInput{
            .description    = "Cancel message received"sv,
            .queue_msg_type = kShipConnectionQueueMsgTypeCancel,
        },
        SmeHelloPendingListenTestInput{
            .description    = "Spine data to send message received"sv,
            .queue_msg_type = kShipConnectionQueueMsgTypeSpineDataToSend,
        },
        SmeHelloPendingListenTestInput{
            .description    = "Websocket error message received"sv,
            .queue_msg_type = kShipConnectionQueueMsgTypeWebsocketError,
        },
        SmeHelloPendingListenTestInput{
            .description    = "Websocket close message received"sv,
            .queue_msg_type = kShipConnectionQueueMsgTypeWebsocketClose,
        }
    )
);

TEST_F(SmeHelloStatePendingListenEvaluateReadyMessageTests, ReadyPhaseMessageWithoutWaitingReceived) {
  // Arrange:
  // Init message buffer
  MessageBuffer msg_buf{0};
  ASSERT_EQ(MessageBufferInitHelper(&msg_buf, kSmeHelloReadyMsgMissingWaiting), kEebusErrorOk) << "Wrong test input!";
  ShipConnectionWebsocketCallback(kWebsocketCallbackTypeRead, msg_buf.data, msg_buf.data_size, &sc);

  // Set initial SME state
  SetShipConnectionState(kSmeHelloStatePendingListen);
  ExpectStateUpdate(kSmeHelloStateAbort);

  // Expect function calls
  EXPECT_CALL(*wfr_timer_mock->gmock, Start(sc.wait_for_ready_timer, tHelloInit, false));
  EXPECT_CALL(*wfr_timer_mock->gmock, Stop(sc.wait_for_ready_timer)).Times(2);
  EXPECT_CALL(*prr_timer_mock->gmock, Stop(sc.prolongation_request_reply_timer));
  EXPECT_CALL(*spr_timer_mock->gmock, Stop(sc.send_prolongation_request_timer));

  ExpectWebsocketClose("", false);

  // Act: Receive and process the hello message
  SmeHelloStatePendingListen(&sc);

  // Assert: SME state changed accordingly
  EXPECT_EQ(SHIP_CONNECTION_GET_SHIP_STATE(&sc, nullptr), kSmeHelloStateAbort);
}

TEST_F(
    SmeHelloStatePendingListenEvaluateReadyMessageTests,
    ReadyPhaseMessageReceivedWithWaitingValueGreaterThanProlongationThreshold
) {
  // Arrange:
  // Init message buffer
  MessageBuffer msg_buf{0};
  ASSERT_EQ(MessageBufferInitHelper(&msg_buf, kSmeHelloReadyMsg), kEebusErrorOk) << "Wrong test input!";
  ShipConnectionWebsocketCallback(kWebsocketCallbackTypeRead, msg_buf.data, msg_buf.data_size, &sc);

  // Set initial SME state
  SetShipConnectionState(kSmeHelloStatePendingListen);

  // Expect function calls
  EXPECT_CALL(*wfr_timer_mock->gmock, Start(sc.wait_for_ready_timer, tHelloInit, false));
  EXPECT_CALL(*wfr_timer_mock->gmock, Stop(sc.wait_for_ready_timer)).Times(2);
  EXPECT_CALL(*prr_timer_mock->gmock, Stop(sc.prolongation_request_reply_timer));
  EXPECT_CALL(*spr_timer_mock->gmock, Start(sc.send_prolongation_request_timer, 45000, false));

  // Act: Receive and process the hello message
  SmeHelloStatePendingListen(&sc);

  // Assert: SME state changed accordingly
  EXPECT_EQ(SHIP_CONNECTION_GET_SHIP_STATE(&sc, nullptr), kSmeHelloStatePendingListen);
  ExpectConnectionClose("", false);
}

TEST_F(
    SmeHelloStatePendingListenEvaluateReadyMessageTests,
    ReadyPhaseMessageReceivedWithWaitingValueLessThanProlongationThreshold
) {
  // Arrange:
  // Init message buffer
  MessageBuffer msg_buf{0};
  ASSERT_EQ(MessageBufferInitHelper(&msg_buf, kSmeHelloReadyMsgWaitingPeriodLessThanThreshold), kEebusErrorOk)
      << "Wrong test input!";
  ShipConnectionWebsocketCallback(kWebsocketCallbackTypeRead, msg_buf.data, msg_buf.data_size, &sc);

  // Set initial SME state
  SetShipConnectionState(kSmeHelloStatePendingListen);

  // Expect function calls
  EXPECT_CALL(*wfr_timer_mock->gmock, Start(sc.wait_for_ready_timer, tHelloInit, false));
  EXPECT_CALL(*wfr_timer_mock->gmock, Stop(sc.wait_for_ready_timer)).Times(2);
  EXPECT_CALL(*prr_timer_mock->gmock, Stop(sc.prolongation_request_reply_timer));
  EXPECT_CALL(*spr_timer_mock->gmock, Stop(sc.send_prolongation_request_timer));

  // Act: Receive and process the hello message
  SmeHelloStatePendingListen(&sc);

  // Assert: SME state changed accordingly
  EXPECT_EQ(SHIP_CONNECTION_GET_SHIP_STATE(&sc, nullptr), kSmeHelloStatePendingListen);
  ExpectConnectionClose("", false);
}

TEST_F(
    SmeHelloStatePendingListenEvaluatePendingMessageTests,
    EvaluatePendingMessageWithoutWaitingReceivedNoProlongationRequest
) {
  // Arrange:
  // Init message buffer
  MessageBuffer msg_buf{0};
  ASSERT_EQ(MessageBufferInitHelper(&msg_buf, kSmeHelloPendingInitMsg), kEebusErrorOk) << "Wrong test input!";
  ShipConnectionWebsocketCallback(kWebsocketCallbackTypeRead, msg_buf.data, msg_buf.data_size, &sc);

  // Set initial SME state
  SetShipConnectionState(kSmeHelloStatePendingListen);

  // Expect function calls
  EXPECT_CALL(*wfr_timer_mock->gmock, Stop(sc.wait_for_ready_timer));
  EXPECT_CALL(*prr_timer_mock->gmock, Stop(sc.prolongation_request_reply_timer));
  EXPECT_CALL(*wfr_timer_mock->gmock, Start(sc.wait_for_ready_timer, tHelloInit, false));
  EXPECT_CALL(*spr_timer_mock->gmock, Start(sc.send_prolongation_request_timer, 45000, false));

  // Act: Receive and process the hello message
  SmeHelloStatePendingListen(&sc);

  // Assert: SME state changed accordingly
  EXPECT_EQ(SHIP_CONNECTION_GET_SHIP_STATE(&sc, nullptr), kSmeHelloStatePendingListen);
  ExpectConnectionClose("", false);
}

TEST_F(
    SmeHelloStatePendingListenEvaluatePendingMessageTests,
    EvaluatePendingMessageWithoutWaitingReceivedAndProlongationRequestMessageSuccessfullySent
) {
  // Arrange:
  // Init message buffer
  MessageBuffer msg_buf{0};
  ASSERT_EQ(MessageBufferInitHelper(&msg_buf, kSmeHelloPendingMsgWithProlongationRequestTrue), kEebusErrorOk)
      << "Wrong test input!";
  ShipConnectionWebsocketCallback(kWebsocketCallbackTypeRead, msg_buf.data, msg_buf.data_size, &sc);

  // Set initial SME state
  SetShipConnectionState(kSmeHelloStatePendingListen);

  // Expect function calls
  EXPECT_CALL(*wfr_timer_mock->gmock, Stop(sc.wait_for_ready_timer));
  EXPECT_CALL(*wfr_timer_mock->gmock, Start(sc.wait_for_ready_timer, tHelloInit, false)).Times(2);
  EXPECT_CALL(*wfr_timer_mock->gmock, GetTimerState(sc.wait_for_ready_timer));
  EXPECT_CALL(*wfr_timer_mock->gmock, GetRemainingTime(sc.wait_for_ready_timer));

  // Unformat response message
  MessageBuffer sent_msg{0};
  ASSERT_EQ(MessageBufferInitHelper(&sent_msg, kSmeHelloPendingMsgMissingProlongationRequest), kEebusErrorOk)
      << "Wrong test input!";

  ExpectWebsocketWrite(sent_msg, true);

  // Act: Receive and process the hello message
  SmeHelloStatePendingListen(&sc);

  // Assert: SME state changed accordingly
  EXPECT_EQ(SHIP_CONNECTION_GET_SHIP_STATE(&sc, nullptr), kSmeHelloStatePendingListen);
  ExpectConnectionClose("", false);
}

TEST_F(
    SmeHelloStatePendingListenEvaluatePendingMessageTests,
    EvaluatePendingMessageWithoutWaitingReceivedAndProlongationRequestMessageFailedToSend
) {
  // Arrange:
  // Init message buffer
  MessageBuffer msg_buf{0};
  ASSERT_EQ(MessageBufferInitHelper(&msg_buf, kSmeHelloPendingMsgWithProlongationRequestTrue), kEebusErrorOk)
      << "Wrong test input!";
  ShipConnectionWebsocketCallback(kWebsocketCallbackTypeRead, msg_buf.data, msg_buf.data_size, &sc);

  // Set initial SME state
  SetShipConnectionState(kSmeHelloStateAbort);

  // Expect function calls
  EXPECT_CALL(*wfr_timer_mock->gmock, Stop(sc.wait_for_ready_timer)).Times(2);
  EXPECT_CALL(*prr_timer_mock->gmock, Stop(sc.prolongation_request_reply_timer));
  EXPECT_CALL(*spr_timer_mock->gmock, Stop(sc.send_prolongation_request_timer));

  EXPECT_CALL(*wfr_timer_mock->gmock, Start(sc.wait_for_ready_timer, tHelloInit, false)).Times(2);
  EXPECT_CALL(*wfr_timer_mock->gmock, GetTimerState(sc.wait_for_ready_timer));
  EXPECT_CALL(*wfr_timer_mock->gmock, GetRemainingTime(sc.wait_for_ready_timer));

  // Unformat response message
  MessageBuffer sent_msg{0};
  ASSERT_EQ(MessageBufferInitHelper(&sent_msg, kSmeHelloPendingMsgMissingProlongationRequest), kEebusErrorOk)
      << "Wrong test input!";

  ExpectWebsocketWrite(sent_msg, false);
  ExpectWebsocketClose("", false);

  // Act: Receive and process the hello message
  SmeHelloStatePendingListen(&sc);

  // Assert: SME state changed accordingly
  EXPECT_EQ(SHIP_CONNECTION_GET_SHIP_STATE(&sc, nullptr), kSmeHelloStateAbort);
}

TEST_F(
    SmeHelloStatePendingListenEvaluatePendingMessageTests,
    EvaluatePendingMessageReceivedWithWaitingPeriodGreaterThanProlongationThreshold
) {
  // Arrange:
  // Init message buffer
  MessageBuffer msg_buf{0};
  ASSERT_EQ(MessageBufferInitHelper(&msg_buf, kSmeHelloPendingInitMsg), kEebusErrorOk) << "Wrong test input!";
  ShipConnectionWebsocketCallback(kWebsocketCallbackTypeRead, msg_buf.data, msg_buf.data_size, &sc);

  // Set initial SME state
  SetShipConnectionState(kSmeHelloStatePendingListen);
  EXPECT_CALL(*wfr_timer_mock->gmock, Start(sc.wait_for_ready_timer, tHelloInit, false));
  EXPECT_CALL(*wfr_timer_mock->gmock, Stop(sc.wait_for_ready_timer));
  EXPECT_CALL(*prr_timer_mock->gmock, Stop(sc.prolongation_request_reply_timer));
  EXPECT_CALL(*spr_timer_mock->gmock, Start(sc.send_prolongation_request_timer, 45000, false));

  // Act: Receive and process the hello message
  SmeHelloStatePendingListen(&sc);

  // Assert: SME state changed accordingly
  EXPECT_EQ(SHIP_CONNECTION_GET_SHIP_STATE(&sc, nullptr), kSmeHelloStatePendingListen);
  ExpectConnectionClose("", false);
}

TEST_F(
    SmeHelloStatePendingListenEvaluatePendingMessageTests,
    EvaluatePendingMessageReceivedWithWaitingPeriodLessThanProlongationThreshold
) {
  // Arrange:
  // Init message buffer
  MessageBuffer msg_buf{0};
  ASSERT_EQ(MessageBufferInitHelper(&msg_buf, kSmeHelloPendingInitMsgWaitingPeriodLessThanThreshold), kEebusErrorOk)
      << "Wrong test input!";
  ShipConnectionWebsocketCallback(kWebsocketCallbackTypeRead, msg_buf.data, msg_buf.data_size, &sc);

  // Set initial SME state
  SetShipConnectionState(kSmeHelloStatePendingListen);

  // Expect function calls
  EXPECT_CALL(*wfr_timer_mock->gmock, Start(sc.wait_for_ready_timer, tHelloInit, false));
  EXPECT_CALL(*wfr_timer_mock->gmock, Stop(sc.wait_for_ready_timer));
  EXPECT_CALL(*prr_timer_mock->gmock, Stop(sc.prolongation_request_reply_timer));
  EXPECT_CALL(*spr_timer_mock->gmock, Stop(sc.send_prolongation_request_timer));

  // Act: Receive and process the hello message
  SmeHelloStatePendingListen(&sc);

  // Assert: SME state changed accordingly
  EXPECT_EQ(SHIP_CONNECTION_GET_SHIP_STATE(&sc, nullptr), kSmeHelloStatePendingListen);
  ExpectConnectionClose("", false);
}
