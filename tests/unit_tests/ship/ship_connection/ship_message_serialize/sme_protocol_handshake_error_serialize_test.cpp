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

TEST(ShipMessageParseTest, SmeProtocolHandshakeErrorSerializeInputArgs) {
  auto serialize = ShipMessageSerializeCreate(nullptr, kValueUndefined);
  EXPECT_EQ(SHIP_MESSAGE_SERIALIZE_GET_BUFFER(serialize), nullptr);
  ShipMessageSerializeDelete(serialize);

  serialize = ShipMessageSerializeCreate(nullptr, kSmeProtocolHandshakeError);
  EXPECT_EQ(SHIP_MESSAGE_SERIALIZE_GET_BUFFER(serialize), nullptr);
  ShipMessageSerializeDelete(serialize);

  MessageProtocolHandshakeError sme_prot_hs_err;
  serialize = ShipMessageSerializeCreate(&sme_prot_hs_err, kValueUndefined);
  EXPECT_EQ(SHIP_MESSAGE_SERIALIZE_GET_BUFFER(serialize), nullptr);
  ShipMessageSerializeDelete(serialize);
}

struct SmeProtocolHandshakeErrorSerializeTestInput {
  std::string_view description;
  MessageProtocolHandshakeErrorType error;
  std::string_view msg;
};

std::ostream& operator<<(std::ostream& os, SmeProtocolHandshakeErrorSerializeTestInput Test) {
  return os << Test.description;
}

class SmeProtocolHandshakeErrorSerializeTests
    : public ::testing::TestWithParam<SmeProtocolHandshakeErrorSerializeTestInput> {};

TEST_P(SmeProtocolHandshakeErrorSerializeTests, SmeProtocolHandshakeErrorSerializeTests) {
  // Arrange: Initialize the SmeProtocolHandshakeErrorSerialize() parameters from test input
  MessageProtocolHandshakeError sme_prot_hs_err;

  sme_prot_hs_err.error = GetParam().error;

  // Act: Run the SmeProtocolHandshakeError serialize procedure
  auto serialize = ShipMessageSerializeCreate(&sme_prot_hs_err, kSmeProtocolHandshakeError);

  const MessageBuffer* const buf = SHIP_MESSAGE_SERIALIZE_GET_BUFFER(serialize);

  // Assert: Verify with expected output buffer value
  if (buf != nullptr) {
    EXPECT_THAT(buf, BufDataEq(GetParam().msg));
  }

  ShipMessageSerializeDelete(serialize);
}

INSTANTIATE_TEST_SUITE_P(
    SmeProtocolHandshakeErrorSerialize,
    SmeProtocolHandshakeErrorSerializeTests,
    ::testing::Values(
        SmeProtocolHandshakeErrorSerializeTestInput{
            .description = "Test error = RFU"sv,
            .error       = kMessageProtocolHandshakeErrorTypeRFU,
            .msg         = "\001{\"messageProtocolHandshakeError\":[{\"error\":0}]}"sv,
        },
        SmeProtocolHandshakeErrorSerializeTestInput{
            .description = "Test error = Timeout"sv,
            .error       = kMessageProtocolHandshakeErrorTypeTimeout,
            .msg         = "\001{\"messageProtocolHandshakeError\":[{\"error\":1}]}"sv,
        },
        SmeProtocolHandshakeErrorSerializeTestInput{
            .description = "Test error = UnexpectedMessage"sv,
            .error       = kMessageProtocolHandshakeErrorTypeUnexpectedMessage,
            .msg         = "\001{\"messageProtocolHandshakeError\":[{\"error\":2}]}"sv,
        },
        SmeProtocolHandshakeErrorSerializeTestInput{
            .description = "Test error = SelectionMismatch"sv,
            .error       = kMessageProtocolHandshakeErrorTypeSelectionMismatch,
            .msg         = "\001{\"messageProtocolHandshakeError\":[{\"error\":3}]}"sv,
        }
    )
);
