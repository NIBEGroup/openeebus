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

TEST(SmeConnectionPinInputSerializeTests, SmeConnectionPinInputInputArgs) {
  auto serialize = ShipMessageSerializeCreate(nullptr, kValueUndefined);
  EXPECT_EQ(SHIP_MESSAGE_SERIALIZE_GET_BUFFER(serialize), nullptr);
  ShipMessageSerializeDelete(serialize);

  serialize = ShipMessageSerializeCreate(nullptr, kSmeConnectionPinInput);
  EXPECT_EQ(SHIP_MESSAGE_SERIALIZE_GET_BUFFER(serialize), nullptr);
  ShipMessageSerializeDelete(serialize);

  ConnectionPinInput sme_pin_input;
  serialize = ShipMessageSerializeCreate(&sme_pin_input, kValueUndefined);
  EXPECT_EQ(SHIP_MESSAGE_SERIALIZE_GET_BUFFER(serialize), nullptr);
  ShipMessageSerializeDelete(serialize);
}

struct SmeConnectionPinInputSerializeTestInput {
  std::string_view description;
  uint64_t pin;
  std::string_view msg;
};

std::ostream& operator<<(std::ostream& os, SmeConnectionPinInputSerializeTestInput Test) {
  return os << Test.description;
}

class SmeConnectionPinInputSerializeTests : public ::testing::TestWithParam<SmeConnectionPinInputSerializeTestInput> {};

TEST_P(SmeConnectionPinInputSerializeTests, SmeConnectionPinInputSerializeTests) {
  // Arrange: Initialize the ConnectionPinInput from test input
  ConnectionPinInput sme_pin_input;

  sme_pin_input.pin = GetParam().pin;

  // Act: Run the ConnectionPinInput serialization procedure
  auto serialize = ShipMessageSerializeCreate(&sme_pin_input, kSmeConnectionPinInput);

  const MessageBuffer* const buf = SHIP_MESSAGE_SERIALIZE_GET_BUFFER(serialize);

  // Assert: Verify with expected output buffer value
  if (buf != nullptr) {
    EXPECT_THAT(buf, BufDataEq(GetParam().msg));
  }

  ShipMessageSerializeDelete(serialize);
}

INSTANTIATE_TEST_SUITE_P(
    SmeConnectionPinInputSerialize,
    SmeConnectionPinInputSerializeTests,
    ::testing::Values(
        SmeConnectionPinInputSerializeTestInput{
            .description = "Test 32 bit pin"sv,
            .pin         = (uint64_t)0x55AAAAFF,
            .msg         = "\001{\"connectionPinInput\":[{\"pin\":\"55AAAAFF\"}]}"sv,
        },
        SmeConnectionPinInputSerializeTestInput{
            .description = "Test 40 bit pin"sv,
            .pin         = (uint64_t)0x5555AAAAFF,
            .msg         = "\001{\"connectionPinInput\":[{\"pin\":\"5555AAAAFF\"}]}"sv,
        },
        SmeConnectionPinInputSerializeTestInput{
            .description = "Test 64 bit pin"sv,
            .pin         = (uint64_t)0xABCD735555AAAAFF,
            .msg         = "\001{\"connectionPinInput\":[{\"pin\":\"ABCD735555AAAAFF\"}]}"sv,
        }
    )
);
