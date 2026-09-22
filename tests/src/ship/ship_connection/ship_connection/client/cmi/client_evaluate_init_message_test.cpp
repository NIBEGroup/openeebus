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
#include "src/common/eebus_malloc.h"
#include "src/ship/ship_connection/client.h"
#include "tests/src/ship/ship_connection/ship_connection/ship_connection_test_suite.h"

using std::literals::string_view_literals::operator""sv;
constexpr const char* kErrorMsgInvalidInitMessage{"Invalid init message received"};
constexpr size_t kCmiInitMessageSize{ARRAY_SIZE(kShipInitMessage)};

class CmiClientEvaluateMessageTests : public ShipConnectionTestSuite {};

struct CmiInvalidMessageFormatTestInput {
  std::string_view description{""sv};
  bool use_correct_msg_size{true};
};

std::ostream& operator<<(std::ostream& os, const CmiInvalidMessageFormatTestInput& input) {
  return os << input.description;
}

class CmiClientEvaluateInvalidMessageFormatTests
    : public ShipConnectionTestSuite,
      public ::testing::WithParamInterface<CmiInvalidMessageFormatTestInput> {};

struct CmiInvalidMessageContentTestInput {
  std::string_view description{""sv};
  uint8_t msg_values[kCmiInitMessageSize]{kMsgTypeInit, 0x00};
};

std::ostream& operator<<(std::ostream& os, const CmiInvalidMessageContentTestInput& input) {
  return os << input.description;
}

class CmiClientEvaluateInvalidMessageContentTests
    : public ShipConnectionTestSuite,
      public ::testing::WithParamInterface<CmiInvalidMessageContentTestInput> {};

TEST_F(CmiClientEvaluateMessageTests, MessageSuccessfullyEvaluated) {
  // Arrange: Create message and init message buffer
  uint8_t* const msg{reinterpret_cast<uint8_t*>(EEBUS_MALLOC(kCmiInitMessageSize))};
  memcpy(msg, kShipInitMessage, kCmiInitMessageSize);
  MessageBufferInit(&sc.msg, msg, kCmiInitMessageSize);

  ExpectStateUpdate(kSmeHelloState);

  // Act: Evaluate message
  CmiStateClientEvaluate(&sc);

  // Assert: SME is in kSmeHelloState and message buffer is released
  EXPECT_EQ(SHIP_CONNECTION_GET_SHIP_STATE(&sc, nullptr), kSmeHelloState);
  EXPECT_EQ(sc.msg.data, nullptr);
  EXPECT_EQ(sc.msg.data_size, 0);
  ExpectConnectionClose("", false);
}

TEST_P(CmiClientEvaluateInvalidMessageFormatTests, InvalidMessageFormatReceived) {
  // Arrange: Create message and init message buffer
  const size_t msg_size{GetParam().use_correct_msg_size ? kCmiInitMessageSize : 0};

  // Test both cases where message pointer is null and where message size is zero, as both are invalid formats
  uint8_t* const msg{msg_size ? nullptr : reinterpret_cast<uint8_t*>(EEBUS_MALLOC(1))};
  MessageBufferInit(&sc.msg, msg, msg_size);

  ExpectStateUpdate(kSmeStateError);
  ExpectConnectionClose(kErrorMsgInvalidInitMessage, false);

  // Act: Evaluate message
  CmiStateClientEvaluate(&sc);

  // Assert: SME is in kSmeStateError and message buffer is released
  EXPECT_EQ(SHIP_CONNECTION_GET_SHIP_STATE(&sc, nullptr), kSmeStateError);
  EXPECT_EQ(sc.msg.data, nullptr);
  EXPECT_EQ(sc.msg.data_size, 0);
}

INSTANTIATE_TEST_SUITE_P(
    CmiClientEvaluateInvalidMessageFormatTests,
    CmiClientEvaluateInvalidMessageFormatTests,
    ::testing::Values(
        CmiInvalidMessageFormatTestInput{
            .description          = "Invalid message size"sv,
            .use_correct_msg_size = false,
        },
        CmiInvalidMessageFormatTestInput{
            .description          = "No message provided"sv,
            .use_correct_msg_size = true,
        }
    )
);

TEST_P(CmiClientEvaluateInvalidMessageContentTests, InvalidMessageContentsReceived) {
  // Arrange: Create message and init message buffer
  uint8_t* const msg{reinterpret_cast<uint8_t*>(EEBUS_MALLOC(kCmiInitMessageSize))};
  memcpy(msg, GetParam().msg_values, kCmiInitMessageSize);
  MessageBufferInit(&sc.msg, msg, kCmiInitMessageSize);

  ExpectStateUpdate(kSmeStateError);
  ExpectConnectionClose(kErrorMsgInvalidInitMessage, false);

  // Act: Evaluate message
  CmiStateClientEvaluate(&sc);

  // Assert: SME is in kSmeStateError and message buffer is released
  EXPECT_EQ(SHIP_CONNECTION_GET_SHIP_STATE(&sc, nullptr), kSmeStateError);
  EXPECT_EQ(sc.msg.data, nullptr);
  EXPECT_EQ(sc.msg.data_size, 0);
}

INSTANTIATE_TEST_SUITE_P(
    CmiClientEvaluateInvalidMessageContentTests,
    CmiClientEvaluateInvalidMessageContentTests,
    ::testing::Values(
        CmiInvalidMessageContentTestInput{
            .description = "Invalid message type (Control)"sv,
            .msg_values  = {kMsgTypeControl, 0x00},
},
        CmiInvalidMessageContentTestInput{
            .description = "Invalid message type (Data)"sv,
            .msg_values  = {kMsgTypeData, 0x00},
        },
        CmiInvalidMessageContentTestInput{
            .description = "Invalid message type (End)"sv,
            .msg_values  = {kMsgTypeEnd, 0x00},
        },
        CmiInvalidMessageContentTestInput{
            .description = "Invalid message data"sv,
            .msg_values  = {kMsgTypeInit, 0x01},
        }
    )
);
