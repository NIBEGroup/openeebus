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

TEST(SmeConnectionPinErrorSerializeTests, SmeConnectionPinErrorInputArgs) {
  auto serialize = ShipMessageSerializeCreate(nullptr, kValueUndefined);
  EXPECT_EQ(SHIP_MESSAGE_SERIALIZE_GET_BUFFER(serialize), nullptr);
  ShipMessageSerializeDelete(serialize);

  serialize = ShipMessageSerializeCreate(nullptr, kSmeConnectionPinError);
  EXPECT_EQ(SHIP_MESSAGE_SERIALIZE_GET_BUFFER(serialize), nullptr);
  ShipMessageSerializeDelete(serialize);

  ConnectionPinError sme_pin_error;
  serialize = ShipMessageSerializeCreate(&sme_pin_error, kValueUndefined);
  EXPECT_EQ(SHIP_MESSAGE_SERIALIZE_GET_BUFFER(serialize), nullptr);
  ShipMessageSerializeDelete(serialize);
}

struct SmeConnectionPinErrorSerializeTestInput {
  std::string_view description;
  ConnectionPinErrorType error;
  std::string_view msg;
};

std::ostream& operator<<(std::ostream& os, SmeConnectionPinErrorSerializeTestInput Test) {
  return os << Test.description;
}

class SmeConnectionPinErrorSerializeTests : public ::testing::TestWithParam<SmeConnectionPinErrorSerializeTestInput> {};

TEST_P(SmeConnectionPinErrorSerializeTests, SmeConnectionPinErrorSerializeTests) {
  // Arrange: Initialize the ConnectionPinError from test input
  ConnectionPinError sme_pin_error;

  sme_pin_error.error = GetParam().error;

  // Act: Run the ConnectionPinError serialization procedure
  auto serialize = ShipMessageSerializeCreate(&sme_pin_error, kSmeConnectionPinError);

  const MessageBuffer* const buf = SHIP_MESSAGE_SERIALIZE_GET_BUFFER(serialize);

  // Assert: Verify with expected output buffer value
  if (buf != nullptr) {
    EXPECT_THAT(buf, BufDataEq(GetParam().msg));
  }

  ShipMessageSerializeDelete(serialize);
}

INSTANTIATE_TEST_SUITE_P(
    SmeConnectionPinErrorSerialize,
    SmeConnectionPinErrorSerializeTests,
    ::testing::Values(SmeConnectionPinErrorSerializeTestInput{
        .description = "Test 32 bit pin"sv,
        .error       = kConnectionPinErrorTypeWrongPin,
        .msg         = "\001{\"connectionPinError\":[{\"error\":1}]}"sv,
    })
);
