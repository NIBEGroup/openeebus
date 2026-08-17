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

#include "src/ship/ship_connection/client.h"
#include "tests/src/ship/ship_connection/ship_connection/ship_connection_test_suite.h"

using std::literals::string_view_literals::operator""sv;

class CmiClientWaitStateSuccessfulTests : public ShipConnectionTestSuite {};

struct CmiClientWaitTestInput {
  std::string_view description{""sv};
  ShipConnectionQueueMsgType msg_type{kShipConnectionQueueMsgTypeDataReceived};
};

class CmiClientWaitInvalidMessageReceivedTests : public ShipConnectionTestSuite,
                                                 public ::testing::WithParamInterface<CmiClientWaitTestInput> {};

std::ostream& operator<<(std::ostream& os, const CmiClientWaitTestInput& input) {
  return os << input.description;
}

TEST_F(CmiClientWaitStateSuccessfulTests, ValidInitMessageReceived) {
  // Arrange: Create message and send to queue
  ShipConnectionQueueMessage queue_msg{kShipConnectionQueueMsgTypeDataReceived, {nullptr}};
  EEBUS_QUEUE_SEND(sc.msg_queue, &queue_msg, sizeof(queue_msg));

  EXPECT_CALL(*wfr_timer_mock->gmock, Start(sc.wait_for_ready_timer, cmiTimeout, false));
  EXPECT_CALL(*wfr_timer_mock->gmock, Stop(sc.wait_for_ready_timer));
  ExpectStateUpdate(kCmiStateClientEvaluate);

  // Act: Wait for message
  CmiStateClientWait(&sc);

  // Assert: SME is in kCmiStateClientEvaluate
  EXPECT_EQ(SHIP_CONNECTION_GET_SHIP_STATE(&sc, nullptr), kCmiStateClientEvaluate);
  ExpectConnectionClose("", false);
}

TEST_P(CmiClientWaitInvalidMessageReceivedTests, InvalidMessageTypeReceived) {
  // Arrange: Create message and send to queue
  ShipConnectionQueueMessage queue_msg{GetParam().msg_type, {nullptr}};
  EEBUS_QUEUE_SEND(sc.msg_queue, &queue_msg, sizeof(queue_msg));

  EXPECT_CALL(*wfr_timer_mock->gmock, Start(sc.wait_for_ready_timer, cmiTimeout, false));
  EXPECT_CALL(*wfr_timer_mock->gmock, Stop(sc.wait_for_ready_timer)).Times(2);
  EXPECT_CALL(*prr_timer_mock->gmock, Stop(sc.prolongation_request_reply_timer));
  EXPECT_CALL(*spr_timer_mock->gmock, Stop(sc.send_prolongation_request_timer));
  ExpectStateUpdate(kSmeStateError);
  ExpectWebsocketClose("CMI client wait failed", false);

  // Act: Wait for message
  CmiStateClientWait(&sc);

  // Assert: SME is in kSmeStateError
  EXPECT_EQ(SHIP_CONNECTION_GET_SHIP_STATE(&sc, nullptr), kSmeStateError);
}

INSTANTIATE_TEST_SUITE_P(
    CmiClientWaitInvalidMessageReceivedTests,
    CmiClientWaitInvalidMessageReceivedTests,
    ::testing::Values(
        CmiClientWaitTestInput{
            .description = "Invalid message type received (SpineDataToSend)"sv,
            .msg_type    = kShipConnectionQueueMsgTypeSpineDataToSend,
        },
        CmiClientWaitTestInput{
            .description = "Invalid message type received (Timeout)"sv,
            .msg_type    = kShipConnectionQueueMsgTypeTimeout,
        },
        CmiClientWaitTestInput{
            .description = "Invalid message type received (WebsocketError)"sv,
            .msg_type    = kShipConnectionQueueMsgTypeWebsocketError,
        },
        CmiClientWaitTestInput{
            .description = "Invalid message type received (WebsocketClose)"sv,
            .msg_type    = kShipConnectionQueueMsgTypeWebsocketClose,
        },
        CmiClientWaitTestInput{
            .description = "Invalid message type received (Cancel)"sv,
            .msg_type    = kShipConnectionQueueMsgTypeCancel,
        }
    )
);
