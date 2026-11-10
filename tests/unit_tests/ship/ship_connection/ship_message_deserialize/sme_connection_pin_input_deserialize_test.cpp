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

MATCHER_P(SmeConnPinInputEq, pin, "") {
  *result_listener << "pin = " << arg->pin;

  return (arg->pin == pin);
}

struct SmeConnectionPinInputDeserializeTestInput {
  std::string_view description;
  std::string_view msg;
  MsgValueType value_type;
  uint64_t pin;
};

std::ostream& operator<<(std::ostream& os, SmeConnectionPinInputDeserializeTestInput test_input) {
  return os << test_input.description;
}

class SmeConnectionPinInputDeserializeTests
    : public ::testing::TestWithParam<SmeConnectionPinInputDeserializeTestInput> {};

TEST(SmeConnectionPinInputDeserializeTests, SmeConnectionPinInputDeserializeInputArgs) {
  auto deserialize = ShipMessageDeserializeCreate(nullptr);

  void* const value  = SHIP_MESSAGE_DESERIALIZE_GET_VALUE(deserialize);
  auto sme_pin_input = reinterpret_cast<ConnectionPinInput*>(value);

  const MsgValueType value_type = SHIP_MESSAGE_DESERIALIZE_GET_VALUE_TYPE(deserialize);

  EXPECT_EQ(sme_pin_input, nullptr);
  EXPECT_EQ(value_type, kValueUndefined);

  ShipMessageDeserializeDelete(deserialize);
}

TEST_P(SmeConnectionPinInputDeserializeTests, SmeConnectionPinInputDeserializeTests) {
  // Arrange: Initialize the message buffer with parameters from test input
  MessageBuffer buf;
  MessageBufferInitWithStringView(&buf, GetParam().msg);

  // Act: Run the ConnectionPinInput deserialization
  auto deserialize = ShipMessageDeserializeCreate(&buf);

  void* const value  = SHIP_MESSAGE_DESERIALIZE_GET_VALUE(deserialize);
  auto sme_pin_input = reinterpret_cast<ConnectionPinInput*>(value);

  const MsgValueType value_type = SHIP_MESSAGE_DESERIALIZE_GET_VALUE_TYPE(deserialize);

  // Assert: Verify with expected return value and output message value
  EXPECT_EQ(value_type, GetParam().value_type);
  if ((value_type == kSmeConnectionPinInput) && (sme_pin_input != nullptr)) {
    EXPECT_THAT(sme_pin_input, SmeConnPinInputEq(GetParam().pin));
  } else {
    EXPECT_EQ(sme_pin_input, nullptr);
  }

  MessageBufferRelease(&buf);
  ShipMessageDeserializeDelete(deserialize);
}

INSTANTIATE_TEST_SUITE_P(
    SmeConnectionPinInputDeserialize,
    SmeConnectionPinInputDeserializeTests,
    ::testing::Values(
        SmeConnectionPinInputDeserializeTestInput{
            .description = "Test pin input value missing"sv,
            .msg         = "\001{\"connectionPinInput\":[{}]}"sv,
            .value_type  = kValueUndefined,
            .pin         = 0,
        },
        SmeConnectionPinInputDeserializeTestInput{
            .description = "Test pin input unexpected value (too short)"sv,
            .msg         = "\001{\"connectionPinInput\":[{\"pin\":\"7777777\"}]}"sv,
            .value_type  = kValueUndefined,
            .pin         = 0,
        },
        SmeConnectionPinInputDeserializeTestInput{
            .description = "Test pin input unexpected value (too long)"sv,
            .msg         = "\001{\"connectionPinInput\":[{\"pin\":\"11717171717171717\"}]}"sv,
            .value_type  = kValueUndefined,
            .pin         = 0,
        },
        SmeConnectionPinInputDeserializeTestInput{
            .description = "Test 32 bit pin"sv,
            .msg         = "\001{\"connectionPinInput\":[{\"pin\":\"55AAAAFF\"}]}"sv,
            .value_type  = kSmeConnectionPinInput,
            .pin         = (uint64_t)0x55AAAAFF,
        },
        SmeConnectionPinInputDeserializeTestInput{
            .description = "Test 40 bit pin"sv,
            .msg         = "\001{\"connectionPinInput\":[{\"pin\":\"5555AAAAFF\"}]}"sv,
            .value_type  = kSmeConnectionPinInput,
            .pin         = (uint64_t)0x5555AAAAFF,
        },
        SmeConnectionPinInputDeserializeTestInput{
            .description = "Test 64 bit pin"sv,
            .msg         = "\001{\"connectionPinInput\":[{\"pin\":\"ABCD735555AAAAFF\"}]}"sv,
            .value_type  = kSmeConnectionPinInput,
            .pin         = (uint64_t)0xABCD735555AAAAFF,
        }
    )
);
