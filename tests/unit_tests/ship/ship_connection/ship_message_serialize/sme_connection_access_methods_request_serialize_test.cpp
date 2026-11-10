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

TEST(SmeConnectionAccessMethodsRequestSerializeTests, SmeConnectionAccessMethodsRequestInputArgs) {
  auto serialize = ShipMessageSerializeCreate(nullptr, kValueUndefined);
  EXPECT_EQ(SHIP_MESSAGE_SERIALIZE_GET_BUFFER(serialize), nullptr);
  ShipMessageSerializeDelete(serialize);

  serialize = ShipMessageSerializeCreate(nullptr, kSmeConnectionAccessMethodsRequest);
  EXPECT_EQ(SHIP_MESSAGE_SERIALIZE_GET_BUFFER(serialize), nullptr);
  ShipMessageSerializeDelete(serialize);

  AccessMethodsRequest sme_access_methods_req{};
  serialize = ShipMessageSerializeCreate(&sme_access_methods_req, kValueUndefined);
  EXPECT_EQ(SHIP_MESSAGE_SERIALIZE_GET_BUFFER(serialize), nullptr);
  ShipMessageSerializeDelete(serialize);
}

struct SmeConnectionAccessMethodsRequestSerializeTestInput {
  std::string_view description;
  std::string_view msg;
};

std::ostream& operator<<(std::ostream& os, SmeConnectionAccessMethodsRequestSerializeTestInput Test) {
  return os << Test.description;
}

class SmeConnectionAccessMethodsRequestSerializeTests
    : public ::testing::TestWithParam<SmeConnectionAccessMethodsRequestSerializeTestInput> {};

TEST_P(SmeConnectionAccessMethodsRequestSerializeTests, SmeConnectionAccessMethodsRequestSerializeTests) {
  // Arrange: Initialize the AccessMethodsRequest from test input
  AccessMethodsRequest sme_access_methods_req = {};

  // Act: Run the AccessMethodsRequest serialization procedure
  auto serialize = ShipMessageSerializeCreate(&sme_access_methods_req, kSmeConnectionAccessMethodsRequest);

  const MessageBuffer* const buf = SHIP_MESSAGE_SERIALIZE_GET_BUFFER(serialize);

  // Assert: Verify with expected output buffer value
  if (buf != nullptr) {
    EXPECT_THAT(buf, BufDataEq(GetParam().msg));
  }

  ShipMessageSerializeDelete(serialize);
}

INSTANTIATE_TEST_SUITE_P(
    SmeConnectionAccessMethodsRequestSerialize,
    SmeConnectionAccessMethodsRequestSerializeTests,
    ::testing::Values(SmeConnectionAccessMethodsRequestSerializeTestInput{
        .description = "Test access methods request"sv,
        .msg         = "\001{\"accessMethodsRequest\":[]}"sv,
    })
);
