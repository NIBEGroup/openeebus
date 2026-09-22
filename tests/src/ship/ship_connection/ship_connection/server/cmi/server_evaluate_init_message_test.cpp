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

#include "src/common/array_util.h"
#include "src/ship/ship_connection/server.h"
#include "tests/src/ship/ship_connection/ship_connection/ship_connection_test_suite.h"

using testing::Return;
using std::literals::string_view_literals::operator""sv;
constexpr const char* kErrorMsgInvalidInitMessage{"Invalid init message received"};
constexpr size_t kCmiInitMessageSize{ARRAY_SIZE(kShipInitMessage)};

struct InvalidInitMessageContentTestInput {
  std::string_view description{""sv};
  uint8_t msg_values[kCmiInitMessageSize]{kMsgTypeInit, 0x00};
};

std::ostream& operator<<(std::ostream& os, const InvalidInitMessageContentTestInput& input) {
  return os << input.description;
}

class CmiServerEvaluateInvalidInitMessageContentTests
    : public ShipConnectionTestSuite,
      public ::testing::WithParamInterface<InvalidInitMessageContentTestInput> {};

struct InvalidInitMessageFormatTestInput {
  std::string_view description{""sv};
  bool use_correct_msg_size{true};
};

std::ostream& operator<<(std::ostream& os, const InvalidInitMessageFormatTestInput& input) {
  return os << input.description;
}

class CmiServerEvaluateInvalidInitMessageFormatTests
    : public ShipConnectionTestSuite,
      public ::testing::WithParamInterface<InvalidInitMessageFormatTestInput> {};

class CmiServerSuccessfullyEvaluateMessageTests : public ShipConnectionTestSuite {};

TEST_F(CmiServerSuccessfullyEvaluateMessageTests, CmiServerSuccessfullyEvaluateMessageTests) {
  // Arrange: Init message buffer and expect message evaluation to be successful
  uint8_t* const msg{reinterpret_cast<uint8_t*>(EEBUS_MALLOC(kCmiInitMessageSize))};
  memcpy(msg, kShipInitMessage, kCmiInitMessageSize);
  MessageBufferInit(&sc.msg, msg, kCmiInitMessageSize);

  EXPECT_CALL(*websocket_mock->gmock, Write(sc.websocket, MemEq(msg, kCmiInitMessageSize), kCmiInitMessageSize))
      .WillOnce(Return(static_cast<int32_t>(kCmiInitMessageSize)));

  ExpectStateUpdate(kSmeHelloState);

  // Act: Evaluate message
  CmiStateServerEvaluate(&sc);

  // Assert: SME is in kSmeHelloState and message buffer is released
  EXPECT_EQ(SHIP_CONNECTION_GET_SHIP_STATE(&sc, nullptr), kSmeHelloState);
  EXPECT_EQ(sc.msg.data, nullptr);
  EXPECT_EQ(sc.msg.data_size, 0);
  ExpectConnectionClose("", false);
}

TEST_P(CmiServerEvaluateInvalidInitMessageFormatTests, CmiServerEvaluateInvalidInitMessageFormatTests) {
  // Arrange: Create and initialize message buffer and expect error state update
  const size_t msg_size{GetParam().use_correct_msg_size ? kCmiInitMessageSize : 0};

  // Test both cases where message pointer is null and where message size is zero, as both are invalid formats
  uint8_t* const msg{msg_size ? nullptr : reinterpret_cast<uint8_t*>(EEBUS_MALLOC(1))};
  MessageBufferInit(&sc.msg, msg, msg_size);

  ExpectStateUpdate(kSmeStateError);
  ExpectConnectionClose(kErrorMsgInvalidInitMessage, false);

  // Act: Evaluate message
  CmiStateServerEvaluate(&sc);

  // Assert: SME is in kSmeStateError and message buffer is released
  EXPECT_EQ(SHIP_CONNECTION_GET_SHIP_STATE(&sc, nullptr), kSmeStateError);
  EXPECT_EQ(sc.msg.data, nullptr);
  EXPECT_EQ(sc.msg.data_size, 0);
}

INSTANTIATE_TEST_SUITE_P(
    CmiServerEvaluateInvalidInitMessageFormatTests,
    CmiServerEvaluateInvalidInitMessageFormatTests,
    ::testing::Values(
        InvalidInitMessageFormatTestInput{
            .description          = "Wrong init message size"sv,
            .use_correct_msg_size = false,
        },
        InvalidInitMessageFormatTestInput{
            .description          = "No message provided test"sv,
            .use_correct_msg_size = true,
        }
    )
);

TEST_P(CmiServerEvaluateInvalidInitMessageContentTests, CmiServerEvaluateInvalidInitMessageContentTests) {
  // Arrange: Init message buffer and expect error state update
  uint8_t* const msg{reinterpret_cast<uint8_t*>(EEBUS_MALLOC(kCmiInitMessageSize))};
  memcpy(msg, GetParam().msg_values, kCmiInitMessageSize);
  MessageBufferInit(&sc.msg, msg, kCmiInitMessageSize);

  ExpectStateUpdate(kSmeStateError);
  ExpectConnectionClose(kErrorMsgInvalidInitMessage, false);

  // Act: Evaluate message
  CmiStateServerEvaluate(&sc);

  // Assert: SME is in kSmeStateError and message buffer is released
  EXPECT_EQ(SHIP_CONNECTION_GET_SHIP_STATE(&sc, nullptr), kSmeStateError);
  EXPECT_EQ(sc.msg.data, nullptr);
  EXPECT_EQ(sc.msg.data_size, 0);
}

INSTANTIATE_TEST_SUITE_P(
    CmiServerEvaluateInvalidInitMessageContentTests,
    CmiServerEvaluateInvalidInitMessageContentTests,
    ::testing::Values(
        InvalidInitMessageContentTestInput{
            .description = "Wrong init message type (End)"sv,
            .msg_values  = {kMsgTypeEnd, 0x00},
},
        InvalidInitMessageContentTestInput{
            .description = "Wrong init message type (Data)"sv,
            .msg_values  = {kMsgTypeData, 0x00},
        },
        InvalidInitMessageContentTestInput{
            .description = "Wrong init message type (Control)"sv,
            .msg_values  = {kMsgTypeControl, 0x00},
        },
        InvalidInitMessageContentTestInput{
            .description = "Wrong init message data"sv,
            .msg_values  = {kMsgTypeInit, 0x01},
        }
    )
);
