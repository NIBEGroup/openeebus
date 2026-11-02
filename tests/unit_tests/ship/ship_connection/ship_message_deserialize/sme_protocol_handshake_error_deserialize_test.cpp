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
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <string_view>

#include "src/common/eebus_malloc.h"
#include "src/ship/ship_connection/ship_message_deserialize.h"
#include "tests/unit_tests/message_buffer.h"

using namespace std::literals;

MATCHER_P(SmeProtHandshakeEq, error, "") {
  *result_listener << "error = " << arg->error;

  return error == arg->error;
}

struct SmeProtocolHandshakeErrorDeserializeTestInput {
  std::string_view description;
  std::string_view msg;
  MsgValueType value_type;
  MessageProtocolHandshakeErrorType error;
};

std::ostream& operator<<(std::ostream& os, SmeProtocolHandshakeErrorDeserializeTestInput test_input) {
  return os << test_input.description;
}

class SmeProtocolHandshakeErrorDeserializeTests
    : public ::testing::TestWithParam<SmeProtocolHandshakeErrorDeserializeTestInput> {};

TEST(SmeProtocolHandshakeErrorDeserializeTests, SmeProtocolHandshakeErrorDeserializeInputArgs) {
  auto deserialize = ShipMessageDeserializeCreate(nullptr);

  void* const value    = SHIP_MESSAGE_DESERIALIZE_GET_VALUE(deserialize);
  auto sme_prot_hs_err = reinterpret_cast<MessageProtocolHandshakeError*>(value);

  const MsgValueType value_type = SHIP_MESSAGE_DESERIALIZE_GET_VALUE_TYPE(deserialize);

  EXPECT_EQ(sme_prot_hs_err, nullptr);
  EXPECT_EQ(value_type, kValueUndefined);

  ShipMessageDeserializeDelete(deserialize);
}

TEST_P(SmeProtocolHandshakeErrorDeserializeTests, SmeProtocolHandshakeErrorDeserializeTests) {
  // Arrange: Initialize the SmeProtocolHandshakeErrorDeserialize() parameters from test input
  MessageBuffer buf;
  MessageBufferInitWithStringView(&buf, GetParam().msg);

  // Act: Run the SmeHelloDeserialize()
  auto deserialize = ShipMessageDeserializeCreate(&buf);

  void* const value    = SHIP_MESSAGE_DESERIALIZE_GET_VALUE(deserialize);
  auto sme_prot_hs_err = reinterpret_cast<MessageProtocolHandshakeError*>(value);

  const MsgValueType value_type = SHIP_MESSAGE_DESERIALIZE_GET_VALUE_TYPE(deserialize);

  // Assert: Verify with expected return value and output message value
  EXPECT_EQ(value_type, GetParam().value_type);
  if ((value_type == kSmeProtocolHandshakeError) && (sme_prot_hs_err != nullptr)) {
    EXPECT_THAT(sme_prot_hs_err, SmeProtHandshakeEq(GetParam().error));
  } else {
    EXPECT_EQ(sme_prot_hs_err, nullptr);
  }

  MessageBufferRelease(&buf);
  ShipMessageDeserializeDelete(deserialize);
}

INSTANTIATE_TEST_SUITE_P(
    SmeProtocolHandshakeErrorDeserialize,
    SmeProtocolHandshakeErrorDeserializeTests,
    ::testing::Values(
        SmeProtocolHandshakeErrorDeserializeTestInput{
            .description = "Test error value missing"sv,
            .msg         = "\001{\"messageProtocolHandshakeError\":[{}]}"sv,
            .value_type  = kValueUndefined,
            .error       = static_cast<MessageProtocolHandshakeErrorType>(0),
        },
        SmeProtocolHandshakeErrorDeserializeTestInput{
            .description = "Test error value out of range (-1)"sv,
            .msg         = "\001{\"messageProtocolHandshakeError\":[{\"error\":-1}]}"sv,
            .value_type  = kValueUndefined,
            .error       = static_cast<MessageProtocolHandshakeErrorType>(0),
        },
        SmeProtocolHandshakeErrorDeserializeTestInput{
            .description = "Test error value out for range (4)"sv,
            .msg         = "\001{\"messageProtocolHandshakeError\":[{\"error\":4}]}"sv,
            .value_type  = kValueUndefined,
            .error       = static_cast<MessageProtocolHandshakeErrorType>(0),
        },
        SmeProtocolHandshakeErrorDeserializeTestInput{
            .description = "Test error = RFU"sv,
            .msg         = "\001{\"messageProtocolHandshakeError\":[{\"error\":0}]}"sv,
            .value_type  = kSmeProtocolHandshakeError,
            .error       = kMessageProtocolHandshakeErrorTypeRFU,
        },
        SmeProtocolHandshakeErrorDeserializeTestInput{
            .description = "Test error = Timeout"sv,
            .msg         = "\001{\"messageProtocolHandshakeError\":[{\"error\":1}]}"sv,
            .value_type  = kSmeProtocolHandshakeError,
            .error       = kMessageProtocolHandshakeErrorTypeTimeout,
        },
        SmeProtocolHandshakeErrorDeserializeTestInput{
            .description = "Test error = UnexpectedMessage"sv,
            .msg         = "\001{\"messageProtocolHandshakeError\":[{\"error\":2}]}"sv,
            .value_type  = kSmeProtocolHandshakeError,
            .error       = kMessageProtocolHandshakeErrorTypeUnexpectedMessage,
        },
        SmeProtocolHandshakeErrorDeserializeTestInput{
            .description = "Test error = SelectionMismatch"sv,
            .msg         = "\001{\"messageProtocolHandshakeError\":[{\"error\":3}]}"sv,
            .value_type  = kSmeProtocolHandshakeError,
            .error       = kMessageProtocolHandshakeErrorTypeSelectionMismatch,
        }
    )
);
