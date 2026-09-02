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

#include "src/ship/ship_connection/server.h"
#include "tests/src/ship/ship_connection/ship_connection/ship_connection_test_suite.h"

using std::literals::string_view_literals::operator""sv;

struct ShipCmiServerWaitStateTestInput {
  std::string_view description{""sv};
  const char* close_error_msg{"CMI server wait failed"};
  ShipConnectionQueueMsgType msg_type{kShipConnectionQueueMsgTypeDataReceived};
  SmeState expected_sme_state{kSmeStateError};
};

class ShipConnectionServerCmiWaitStateTests : public ShipConnectionTestSuite,
                                              public ::testing::WithParamInterface<ShipCmiServerWaitStateTestInput> {};

std::ostream& operator<<(std::ostream& os, const ShipCmiServerWaitStateTestInput& input) {
  return os << input.description;
}

TEST_P(ShipConnectionServerCmiWaitStateTests, CmiServerWaitReceivedReplyTest) {
  // Arrange: Create message queue and add message to it
  ShipConnectionQueueMessage queue_msg{GetParam().msg_type, {nullptr}};
  EEBUS_QUEUE_SEND(sc.msg_queue, &queue_msg, sizeof(queue_msg));

  // Expect timer function calls
  EXPECT_CALL(*wfr_timer_mock->gmock, Start(sc.wait_for_ready_timer, cmiTimeout, false));
  EXPECT_CALL(*wfr_timer_mock->gmock, Stop(sc.wait_for_ready_timer)).Times(2);
  EXPECT_CALL(*prr_timer_mock->gmock, Stop(sc.prolongation_request_reply_timer));
  EXPECT_CALL(*spr_timer_mock->gmock, Stop(sc.send_prolongation_request_timer));
  EXPECT_CALL(
      *ifp_mock->gmock,
      HandleShipStateUpdate(
          sc.info_provider,
          testing::StrCaseEq(TEST_REMOTE_SKI),
          GetParam().expected_sme_state,
          testing::StrCaseEq("")
      )
  );
  ExpectCloseWithError(GetParam().close_error_msg, false);

  // Act: Wait for message
  CmiStateServerWait(&sc);

  // Assert: SME state changed accordingly
  EXPECT_EQ(SHIP_CONNECTION_GET_SHIP_STATE(&sc, nullptr), GetParam().expected_sme_state);
}

INSTANTIATE_TEST_SUITE_P(
    ShipConnectionServerCmiWaitStateTests,
    ShipConnectionServerCmiWaitStateTests,
    ::testing::Values(
        ShipCmiServerWaitStateTestInput{
            .description = "Init message with cancel message received"sv,
            .msg_type    = kShipConnectionQueueMsgTypeCancel,
        },
        ShipCmiServerWaitStateTestInput{
            .description = "Init message with timeout message received"sv,
            .msg_type    = kShipConnectionQueueMsgTypeTimeout,
        },
        ShipCmiServerWaitStateTestInput{
            .description        = "Init message successfully received"sv,
            .close_error_msg    = "",
            .expected_sme_state = kCmiStateServerEvaluate,
        }
    )
);
