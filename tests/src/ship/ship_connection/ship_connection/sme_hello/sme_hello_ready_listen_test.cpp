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

struct HelloReadyListenTestInput {
  std::string_view description{""sv};
  ShipConnectionQueueMsgType queue_msg_type{kShipConnectionQueueMsgTypeDataReceived};
  std::string_view msg{kSmeHelloReadyMsg};
  SmeState expected_sme_state{kSmeHelloStateAbort};
};

class SmeHelloStateReadyListenTests : public ShipConnectionTestSuite,
                                      public ::testing::WithParamInterface<HelloReadyListenTestInput> {};

class SmeHelloStateEvaluateReceivedMessageTests : public ShipConnectionTestSuite,
                                                  public ::testing::WithParamInterface<HelloReadyListenTestInput> {};

class SmeHelloStateEvaluateReceivedPendingMessageTests
    : public ShipConnectionTestSuite,
      public ::testing::WithParamInterface<HelloReadyListenTestInput> {};

class HelloStateEvaluateReceivedPendingRequestMessageTests
    : public ShipConnectionTestSuite,
      public ::testing::WithParamInterface<HelloReadyListenTestInput> {};

std::ostream& operator<<(std::ostream& os, const HelloReadyListenTestInput& input) {
  return os << input.description;
}

TEST_P(SmeHelloStateReadyListenTests, WrongQueueMessageTypeReceived) {
  // Arrange:
  // Init message buffer
  ShipConnectionQueueMessage queue_msg{GetParam().queue_msg_type, {nullptr}};
  ASSERT_EQ(MessageBufferInitHelper(&queue_msg.msg_buf, kSmeHelloReadyMsg), kEebusErrorOk) << "Wrong test input!";

  // Add message to queue
  EEBUS_QUEUE_SEND(sc.msg_queue, &queue_msg, sizeof(queue_msg));

  EXPECT_CALL(*wfr_timer_mock->gmock, Start(sc.wait_for_ready_timer, tHelloInit, false));
  EXPECT_CALL(*wfr_timer_mock->gmock, Stop(sc.wait_for_ready_timer)).Times(2);
  EXPECT_CALL(*spr_timer_mock->gmock, Stop(sc.send_prolongation_request_timer));
  EXPECT_CALL(*prr_timer_mock->gmock, Stop(sc.prolongation_request_reply_timer));
  ExpectStateUpdate(GetParam().expected_sme_state);
  ExpectWebsocketClose("", false);

  // Act: Listen and process the received message
  SmeHelloStateReadyListen(&sc);

  // Assert: SME state changed accordingly
  EXPECT_EQ(SHIP_CONNECTION_GET_SHIP_STATE(&sc, nullptr), GetParam().expected_sme_state);
}

INSTANTIATE_TEST_SUITE_P(
    SmeHelloStateReadyListenTests,
    SmeHelloStateReadyListenTests,
    ::testing::Values(
        HelloReadyListenTestInput{
            .description        = "Timeout message received"sv,
            .queue_msg_type     = kShipConnectionQueueMsgTypeTimeout,
            .expected_sme_state = kSmeHelloStateReadyTimeout,
        },
        HelloReadyListenTestInput{
            .description    = "Spine data to send message received"sv,
            .queue_msg_type = kShipConnectionQueueMsgTypeSpineDataToSend,
        },
        HelloReadyListenTestInput{
            .description    = "Websocket error message received"sv,
            .queue_msg_type = kShipConnectionQueueMsgTypeWebsocketError,
        },
        HelloReadyListenTestInput{
            .description    = "Websocket close message received"sv,
            .queue_msg_type = kShipConnectionQueueMsgTypeWebsocketClose,
        },
        HelloReadyListenTestInput{
            .description    = "Cancel message received"sv,
            .queue_msg_type = kShipConnectionQueueMsgTypeCancel,
        }
    )
);

TEST_P(SmeHelloStateEvaluateReceivedMessageTests, ReadyOrAbortMessageReceived) {
  // Arrange:
  // Init message buffer
  MessageBuffer msg_buf{0};
  ASSERT_EQ(MessageBufferInitHelper(&msg_buf, GetParam().msg), kEebusErrorOk) << "Wrong test input!";
  ShipConnectionWebsocketCallback(kWebsocketCallbackTypeRead, msg_buf.data, msg_buf.data_size, &sc);

  // Expect timer function calls
  EXPECT_CALL(*wfr_timer_mock->gmock, Start(sc.wait_for_ready_timer, tHelloInit, false));
  EXPECT_CALL(*wfr_timer_mock->gmock, Stop(sc.wait_for_ready_timer)).Times(2);
  EXPECT_CALL(*spr_timer_mock->gmock, Stop(sc.send_prolongation_request_timer));
  EXPECT_CALL(*prr_timer_mock->gmock, Stop(sc.prolongation_request_reply_timer));

  // Calculate message size
  ExpectStateUpdate(GetParam().expected_sme_state);
  ExpectWebsocketClose("", false);

  // Act: Receive and process the hello message
  SmeHelloStateReadyListen(&sc);

  // Assert: Verify that the state changed accordingly
  EXPECT_EQ(SHIP_CONNECTION_GET_SHIP_STATE(&sc, nullptr), GetParam().expected_sme_state);
}

INSTANTIATE_TEST_SUITE_P(
    SmeHelloStateEvaluateReceivedMessageTests,
    SmeHelloStateEvaluateReceivedMessageTests,
    ::testing::Values(
        HelloReadyListenTestInput{
            .description        = "Ready phase message received"sv,
            .expected_sme_state = kSmeHelloStateOk,
        },
        HelloReadyListenTestInput{
            .description = "Abort phase message received"sv,
            .msg         = kSmeHelloStateAbortMsg,
        }
    )
);

TEST_P(SmeHelloStateEvaluateReceivedPendingMessageTests, PendingRequestFieldMissingOrFalse) {
  // Arrange:
  // Init message buffer
  MessageBuffer msg_buf{0};
  ASSERT_EQ(MessageBufferInitHelper(&msg_buf, GetParam().msg), kEebusErrorOk) << "Wrong test input!";
  ShipConnectionWebsocketCallback(kWebsocketCallbackTypeRead, msg_buf.data, msg_buf.data_size, &sc);

  SetShipConnectionState(kSmeHelloStateReadyListen);

  // Expect function calls
  EXPECT_CALL(*wfr_timer_mock->gmock, Start(sc.wait_for_ready_timer, tHelloInit, false));
  EXPECT_CALL(*wfr_timer_mock->gmock, Stop(sc.wait_for_ready_timer));

  // Act: Receive and process the hello message
  SmeHelloStateReadyListen(&sc);

  // Assert: SME state changed accordingly
  EXPECT_EQ(SHIP_CONNECTION_GET_SHIP_STATE(&sc, nullptr), kSmeHelloStateReadyListen);
  ExpectConnectionClose("", false);
}

INSTANTIATE_TEST_SUITE_P(
    SmeHelloStateEvaluateReceivedPendingMessageTests,
    SmeHelloStateEvaluateReceivedPendingMessageTests,
    ::testing::Values(
        HelloReadyListenTestInput{
            .description = "Pending phase without prolongation request"sv,
            .msg         = kSmeHelloPendingMsgMissingProlongationRequest,
        },
        HelloReadyListenTestInput{
            .description = "Pending phase prolongation request false"sv,
            .msg         = kSmeHelloPendingMsgWithProlongationRequestFalse,
        }
    )
);

TEST_F(SmeHelloStateEvaluateReceivedMessageTests, PendingMessageReceivedRemainingTimeMsgSent) {
  // Arrange:
  // Set initial SME state
  SetShipConnectionState(kSmeHelloStateReadyListen);

  // Init message buffer
  MessageBuffer msg_buf{0};
  ASSERT_EQ(MessageBufferInitHelper(&msg_buf, kSmeHelloPendingMsgWithProlongationRequestTrue), kEebusErrorOk)
      << "Wrong test input!";
  ShipConnectionWebsocketCallback(kWebsocketCallbackTypeRead, msg_buf.data, msg_buf.data_size, &sc);

  // Expect function calls
  EXPECT_CALL(*wfr_timer_mock->gmock, GetTimerState(sc.wait_for_ready_timer));
  EXPECT_CALL(*wfr_timer_mock->gmock, GetRemainingTime(sc.wait_for_ready_timer));

  MessageBuffer msg_buf_expected{0};
  ASSERT_EQ(MessageBufferInitHelper(&msg_buf_expected, kSmeHelloPendingMsgMissingProlongationRequest), kEebusErrorOk)
      << "Wrong test input!";

  ExpectWebsocketWrite(msg_buf_expected, true);

  EXPECT_CALL(*wfr_timer_mock->gmock, Start(sc.wait_for_ready_timer, tHelloInit, false)).Times(2);
  EXPECT_CALL(*wfr_timer_mock->gmock, Stop(sc.wait_for_ready_timer));

  // Act: Receive and process the hello message
  SmeHelloStateReadyListen(&sc);

  // Assert: SME state changed accordingly
  EXPECT_EQ(SHIP_CONNECTION_GET_SHIP_STATE(&sc, nullptr), kSmeHelloStateReadyListen);
  ExpectConnectionClose("", false);
}

TEST_F(SmeHelloStateEvaluateReceivedMessageTests, PendingMessageReceivedRemainingTimeMsgFailedToSend) {
  // Arrange:
  // Set initial SME state
  SetShipConnectionState(kSmeHelloStateReadyListen);

  // Init message buffer
  MessageBuffer msg_buf{0};
  ASSERT_EQ(MessageBufferInitHelper(&msg_buf, kSmeHelloPendingMsgWithProlongationRequestTrue), kEebusErrorOk)
      << "Wrong test input!";
  ShipConnectionWebsocketCallback(kWebsocketCallbackTypeRead, msg_buf.data, msg_buf.data_size, &sc);

  // Expect function calls
  EXPECT_CALL(*wfr_timer_mock->gmock, GetTimerState(sc.wait_for_ready_timer));
  EXPECT_CALL(*wfr_timer_mock->gmock, GetRemainingTime(sc.wait_for_ready_timer));

  MessageBuffer msg_buf_expected{0};
  ASSERT_EQ(MessageBufferInitHelper(&msg_buf_expected, kSmeHelloPendingMsgMissingProlongationRequest), kEebusErrorOk)
      << "Wrong test input!";

  ExpectWebsocketWrite(msg_buf_expected, false);

  ExpectStateUpdate(kSmeHelloStateAbort);
  EXPECT_CALL(*wfr_timer_mock->gmock, Start(sc.wait_for_ready_timer, tHelloInit, false)).Times(2);
  EXPECT_CALL(*wfr_timer_mock->gmock, Stop(sc.wait_for_ready_timer)).Times(2);
  EXPECT_CALL(*spr_timer_mock->gmock, Stop(sc.send_prolongation_request_timer));
  EXPECT_CALL(*prr_timer_mock->gmock, Stop(sc.prolongation_request_reply_timer));
  ExpectWebsocketClose("", false);

  // Act: Receive and process the hello message
  SmeHelloStateReadyListen(&sc);

  // Assert: SME state changed accordingly
  EXPECT_EQ(SHIP_CONNECTION_GET_SHIP_STATE(&sc, nullptr), kSmeHelloStateAbort);
}
