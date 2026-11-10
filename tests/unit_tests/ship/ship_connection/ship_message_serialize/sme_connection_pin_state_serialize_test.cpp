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

#include "src/common/eebus_malloc.h"
#include "src/ship/ship_connection/ship_message_serialize.h"
#include "tests/unit_tests/ship/ship_connection/ship_message_serialize/buf_data_matcher.h"

using namespace std::literals;

TEST(ShipMessageParseTest, SmeConnectionPinStateInputArgs) {
  auto serialize = ShipMessageSerializeCreate(nullptr, kValueUndefined);
  EXPECT_EQ(SHIP_MESSAGE_SERIALIZE_GET_BUFFER(serialize), nullptr);
  ShipMessageSerializeDelete(serialize);

  serialize = ShipMessageSerializeCreate(nullptr, kSmeHello);
  EXPECT_EQ(SHIP_MESSAGE_SERIALIZE_GET_BUFFER(serialize), nullptr);
  ShipMessageSerializeDelete(serialize);

  ConnectionHello sme_hello_value_0;
  serialize = ShipMessageSerializeCreate(&sme_hello_value_0, kValueUndefined);
  EXPECT_EQ(SHIP_MESSAGE_SERIALIZE_GET_BUFFER(serialize), nullptr);
  ShipMessageSerializeDelete(serialize);
}

struct SmeConnectionPinStateSerializeTestInput {
  std::string_view description;
  PinStateType pin_state;
  std::shared_ptr<PinInputPermissionType> input_permission;
  std::string_view msg;
};

std::ostream& operator<<(std::ostream& os, SmeConnectionPinStateSerializeTestInput Test) {
  return os << Test.description;
}

class SmeConnectionPinStateSerializeTest : public ::testing::TestWithParam<SmeConnectionPinStateSerializeTestInput> {};

TEST_P(SmeConnectionPinStateSerializeTest, SmeConnectionPinStateDeserializeInputArgs) {
  // Arrange: Initialize the SmeConnectionPinState() parameters from test input
  ConnectionPinState sme_pin_state;

  sme_pin_state.pin_state = GetParam().pin_state;
  if (GetParam().input_permission != nullptr) {
    sme_pin_state.input_permission_val = *GetParam().input_permission;
    sme_pin_state.input_permission     = &sme_pin_state.input_permission_val;
  } else {
    sme_pin_state.input_permission = nullptr;
  }

  // Act: Run the SmeHello serialize procedure
  auto serialize = ShipMessageSerializeCreate(&sme_pin_state, kSmeConnectionPinState);

  const MessageBuffer* const buf = SHIP_MESSAGE_SERIALIZE_GET_BUFFER(serialize);

  // Assert: Verify with expected output buffer value
  if (buf != nullptr) {
    EXPECT_THAT(buf, BufDataEq(GetParam().msg));
  }

  ShipMessageSerializeDelete(serialize);
}

INSTANTIATE_TEST_SUITE_P(
    SmeConnectionPinStateSerialize,
    SmeConnectionPinStateSerializeTest,
    ::testing::Values(
        SmeConnectionPinStateSerializeTestInput{
            .description      = "Test pin state = required"sv,
            .pin_state        = kPinStateTypeRequired,
            .input_permission = nullptr,
            .msg              = "\001{\"connectionPinState\":[{\"pinState\":\"required\"}]}"sv,
        },
        SmeConnectionPinStateSerializeTestInput{
            .description      = "Test pin state = optional"sv,
            .pin_state        = kPinStateTypeOptional,
            .input_permission = nullptr,
            .msg              = "\001{\"connectionPinState\":[{\"pinState\":\"optional\"}]}"sv,
        },
        SmeConnectionPinStateSerializeTestInput{
            .description      = "Test pin state = pinOk"sv,
            .pin_state        = kPinStateTypePinOk,
            .input_permission = nullptr,
            .msg              = "\001{\"connectionPinState\":[{\"pinState\":\"pinOk\"}]}"sv,
        },
        SmeConnectionPinStateSerializeTestInput{
            .description      = "Test pin state = none"sv,
            .pin_state        = kPinStateTypeNone,
            .input_permission = nullptr,
            .msg              = "\001{\"connectionPinState\":[{\"pinState\":\"none\"}]}"sv,
        },
        SmeConnectionPinStateSerializeTestInput{
            .description      = "Test pin state = none, input permission = busy"sv,
            .pin_state        = kPinStateTypeNone,
            .input_permission = std::make_shared<PinInputPermissionType>(kPinInputPermissionTypeBusy),
            .msg = "\001{\"connectionPinState\":[{\"pinState\":\"none\"},{\"inputPermission\":\"busy\"}]}"sv,
        },
        SmeConnectionPinStateSerializeTestInput{
            .description      = "Test pin state = none, input permission = busy"sv,
            .pin_state        = kPinStateTypeNone,
            .input_permission = std::make_shared<PinInputPermissionType>(kPinInputPermissionTypeOk),
            .msg              = "\001{\"connectionPinState\":[{\"pinState\":\"none\"},{\"inputPermission\":\"ok\"}]}"sv,
        }
    )
);
