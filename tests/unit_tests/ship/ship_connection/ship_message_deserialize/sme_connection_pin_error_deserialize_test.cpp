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

MATCHER_P(SmeConnPinErrorEq, error, "") {
  *result_listener << "pin = " << arg->error;

  return (arg->error == error);
}

struct SmeConnectionPinErrorDeserializeTestInput {
  std::string_view description;
  std::string_view msg;
  MsgValueType value_type;
  ConnectionPinErrorType pin;
};

std::ostream& operator<<(std::ostream& os, SmeConnectionPinErrorDeserializeTestInput test_input) {
  return os << test_input.description;
}

class SmeConnectionPinErrorDeserializeTests
    : public ::testing::TestWithParam<SmeConnectionPinErrorDeserializeTestInput> {};

TEST(SmeConnectionPinErrorDeserializeTests, SmeConnectionPinErrorDeserializeInputArgs) {
  auto deserialize = ShipMessageDeserializeCreate(nullptr);

  void* const value  = SHIP_MESSAGE_DESERIALIZE_GET_VALUE(deserialize);
  auto sme_pin_error = reinterpret_cast<ConnectionPinError*>(value);

  const MsgValueType value_type = SHIP_MESSAGE_DESERIALIZE_GET_VALUE_TYPE(deserialize);

  EXPECT_EQ(sme_pin_error, nullptr);
  EXPECT_EQ(value_type, kValueUndefined);

  ShipMessageDeserializeDelete(deserialize);
}

TEST_P(SmeConnectionPinErrorDeserializeTests, SmeConnectionPinErrorDeserializeTests) {
  // Arrange: Initialize the message buffer with parameters from test input
  MessageBuffer buf;
  MessageBufferInitWithStringView(&buf, GetParam().msg);

  // Act: Run the ConnectionPinError deserialization
  auto deserialize = ShipMessageDeserializeCreate(&buf);

  void* const value  = SHIP_MESSAGE_DESERIALIZE_GET_VALUE(deserialize);
  auto sme_pin_error = reinterpret_cast<ConnectionPinError*>(value);

  const MsgValueType value_type = SHIP_MESSAGE_DESERIALIZE_GET_VALUE_TYPE(deserialize);

  // Assert: Verify with expected return value and output message value
  EXPECT_EQ(value_type, GetParam().value_type);
  if ((value_type == kSmeConnectionPinError) && (sme_pin_error != nullptr)) {
    EXPECT_THAT(sme_pin_error, SmeConnPinErrorEq(GetParam().pin));
  } else {
    EXPECT_EQ(sme_pin_error, nullptr);
  }

  MessageBufferRelease(&buf);
  ShipMessageDeserializeDelete(deserialize);
}

INSTANTIATE_TEST_SUITE_P(
    SmeConnectionPinErrorDeserialize,
    SmeConnectionPinErrorDeserializeTests,
    ::testing::Values(
        SmeConnectionPinErrorDeserializeTestInput{
            .description = "Test pin error value missing"sv,
            .msg         = "\001{\"connectionPinError\":[{}]}"sv,
            .value_type  = kValueUndefined,
            .pin         = static_cast<ConnectionPinErrorType>(0),
        },
        SmeConnectionPinErrorDeserializeTestInput{
            .description = "Test pin error unexpected value (5)"sv,
            .msg         = "\001{\"connectionPinError\":[{\"error\":5}]}"sv,
            .value_type  = kValueUndefined,
            .pin         = static_cast<ConnectionPinErrorType>(0),
        },
        SmeConnectionPinErrorDeserializeTestInput{
            .description = "Test kConnectionPinErrorTypeWrongPin (1)"sv,
            .msg         = "\001{\"connectionPinError\":[{\"error\":1}]}"sv,
            .value_type  = kSmeConnectionPinError,
            .pin         = kConnectionPinErrorTypeWrongPin,
        }
    )
);
