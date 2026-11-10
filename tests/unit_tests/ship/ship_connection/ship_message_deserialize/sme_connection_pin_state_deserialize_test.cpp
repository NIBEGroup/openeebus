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

MATCHER_P2(SmeConnPinStateEq, pin_state, input_permission, "") {
  *result_listener << "pin_state = " << arg->pin_state;
  *result_listener << ", input_permission = ";
  if (arg->input_permission != nullptr) {
    *result_listener << *(arg->input_permission);
  } else {
    *result_listener << nullptr;
  }

  bool match = (arg->pin_state == pin_state);
  if (arg->input_permission == nullptr) {
    match = match && (input_permission == nullptr);
  } else {
    match = match && (input_permission != nullptr);
    match = match && (*arg->input_permission == *input_permission);
  }

  return match;
}

struct SmeConnectionPinStateDeserializeTestInput {
  std::string_view description;
  std::string_view msg;
  MsgValueType value_type;
  PinStateType pin_state;
  std::shared_ptr<PinInputPermissionType> input_permission;
};

std::ostream& operator<<(std::ostream& os, SmeConnectionPinStateDeserializeTestInput test_input) {
  return os << test_input.description;
}

class SmeConnectionPinStateDeserializeTests
    : public ::testing::TestWithParam<SmeConnectionPinStateDeserializeTestInput> {};

TEST(SmeConnectionPinStateDeserializeTests, SmeConnectionPinStateDeserializeInputArgs) {
  auto deserialize = ShipMessageDeserializeCreate(nullptr);

  void* const value  = SHIP_MESSAGE_DESERIALIZE_GET_VALUE(deserialize);
  auto sme_pin_state = reinterpret_cast<ConnectionPinState*>(value);

  const MsgValueType value_type = SHIP_MESSAGE_DESERIALIZE_GET_VALUE_TYPE(deserialize);

  EXPECT_EQ(sme_pin_state, nullptr);
  EXPECT_EQ(value_type, kValueUndefined);

  ShipMessageDeserializeDelete(deserialize);
}

TEST_P(SmeConnectionPinStateDeserializeTests, SmeConnectionPinStateDeserializeTests) {
  // Arrange: Initialize the SmeConnectionPinStateDeserialize() parameters from test input
  MessageBuffer buf;
  MessageBufferInitWithStringView(&buf, GetParam().msg);

  // Act: Run the SmeHelloDeserialize()
  auto deserialize = ShipMessageDeserializeCreate(&buf);

  void* const value  = SHIP_MESSAGE_DESERIALIZE_GET_VALUE(deserialize);
  auto sme_pin_state = reinterpret_cast<ConnectionPinState*>(value);

  const MsgValueType value_type = SHIP_MESSAGE_DESERIALIZE_GET_VALUE_TYPE(deserialize);

  // Assert: Verify with expected return value and output message value
  EXPECT_EQ(value_type, GetParam().value_type);
  if ((value_type == kSmeConnectionPinState) && (sme_pin_state != nullptr)) {
    EXPECT_THAT(sme_pin_state, SmeConnPinStateEq(GetParam().pin_state, GetParam().input_permission));
  } else {
    EXPECT_EQ(sme_pin_state, nullptr);
  }

  MessageBufferRelease(&buf);
  ShipMessageDeserializeDelete(deserialize);
}

INSTANTIATE_TEST_SUITE_P(
    SmeConnectionPinStateDeserialize,
    SmeConnectionPinStateDeserializeTests,
    ::testing::Values(
        SmeConnectionPinStateDeserializeTestInput{
            .description      = "Test pin state value missing"sv,
            .msg              = "\001{\"connectionPinState\":[{}]}"sv,
            .value_type       = kValueUndefined,
            .pin_state        = static_cast<PinStateType>(0),
            .input_permission = nullptr,
        },
        SmeConnectionPinStateDeserializeTestInput{
            .description      = "Test pin state unexpected value (nonrequired)"sv,
            .msg              = "\001{\"connectionPinState\":[{\"pinState\":\"nonrequired\"}]}"sv,
            .value_type       = kValueUndefined,
            .pin_state        = static_cast<PinStateType>(0),
            .input_permission = nullptr,
        },
        SmeConnectionPinStateDeserializeTestInput{
            .description      = "Test input permission unexpected value (nok)"sv,
            .msg              = "\001{\"connectionPinState\":"
                                "[{\"pinState\":\"required\"},"
                                "{\"inputPermission\":\"nok\"}]}"sv,
            .value_type       = kValueUndefined,
            .pin_state        = static_cast<PinStateType>(0),
            .input_permission = nullptr,
        },
        SmeConnectionPinStateDeserializeTestInput{
            .description      = "Test pin state = required"sv,
            .msg              = "\001{\"connectionPinState\":[{\"pinState\":\"required\"}]}"sv,
            .value_type       = kSmeConnectionPinState,
            .pin_state        = kPinStateTypeRequired,
            .input_permission = nullptr,
        },
        SmeConnectionPinStateDeserializeTestInput{
            .description      = "Test pin state = optional"sv,
            .msg              = "\001{\"connectionPinState\":[{\"pinState\":\"optional\"}]}"sv,
            .value_type       = kSmeConnectionPinState,
            .pin_state        = kPinStateTypeOptional,
            .input_permission = nullptr,
        },
        SmeConnectionPinStateDeserializeTestInput{
            .description      = "Test pin state = pinOk"sv,
            .msg              = "\001{\"connectionPinState\":[{\"pinState\":\"pinOk\"}]}"sv,
            .value_type       = kSmeConnectionPinState,
            .pin_state        = kPinStateTypePinOk,
            .input_permission = nullptr,
        },
        SmeConnectionPinStateDeserializeTestInput{
            .description      = "Test pin state = none"sv,
            .msg              = "\001{\"connectionPinState\":[{\"pinState\":\"none\"}]}"sv,
            .value_type       = kSmeConnectionPinState,
            .pin_state        = kPinStateTypeNone,
            .input_permission = nullptr,
        },
        SmeConnectionPinStateDeserializeTestInput{
            .description = "Test pin state = none, input permission = busy"sv,
            .msg         = "\001{\"connectionPinState\":[{\"pinState\":\"none\"},{\"inputPermission\":\"busy\"}]}"sv,
            .value_type  = kSmeConnectionPinState,
            .pin_state   = kPinStateTypeNone,
            .input_permission = std::make_shared<PinInputPermissionType>(kPinInputPermissionTypeBusy),
        },
        SmeConnectionPinStateDeserializeTestInput{
            .description      = "Test pin state = none, input permission = busy"sv,
            .msg              = "\001{\"connectionPinState\":[{\"pinState\":\"none\"},{\"inputPermission\":\"ok\"}]}"sv,
            .value_type       = kSmeConnectionPinState,
            .pin_state        = kPinStateTypeNone,
            .input_permission = std::make_shared<PinInputPermissionType>(kPinInputPermissionTypeOk),
        }
    )
);
