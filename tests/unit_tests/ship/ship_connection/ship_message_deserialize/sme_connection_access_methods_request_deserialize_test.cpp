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

struct SmeConnectionAccessMethodsRequestDeserializeTestInput {
  std::string_view description;
  std::string_view msg;
  MsgValueType value_type;
};

std::ostream& operator<<(std::ostream& os, SmeConnectionAccessMethodsRequestDeserializeTestInput test_input) {
  return os << test_input.description;
}

class SmeConnectionAccessMethodsRequestDeserializeTests
    : public ::testing::TestWithParam<SmeConnectionAccessMethodsRequestDeserializeTestInput> {};

TEST(SmeConnectionAccessMethodsRequestDeserializeTests, SmeConnectionAccessMethodsRequestDeserializeInputArgs) {
  auto deserialize = ShipMessageDeserializeCreate(nullptr);

  void* const value           = SHIP_MESSAGE_DESERIALIZE_GET_VALUE(deserialize);
  auto sme_access_methods_req = reinterpret_cast<AccessMethodsRequest*>(value);

  const MsgValueType value_type = SHIP_MESSAGE_DESERIALIZE_GET_VALUE_TYPE(deserialize);

  EXPECT_EQ(sme_access_methods_req, nullptr);
  EXPECT_EQ(value_type, kValueUndefined);

  ShipMessageDeserializeDelete(deserialize);
}

TEST_P(SmeConnectionAccessMethodsRequestDeserializeTests, SmeConnectionAccessMethodsRequestDeserializeTests) {
  // Arrange: Initialize the message buffer with parameters from test input
  MessageBuffer buf;
  MessageBufferInitWithStringView(&buf, GetParam().msg);

  // Act: Run the AccessMethodsRequest deserialization
  auto deserialize = ShipMessageDeserializeCreate(&buf);

  void* const value           = SHIP_MESSAGE_DESERIALIZE_GET_VALUE(deserialize);
  auto sme_access_methods_req = reinterpret_cast<AccessMethodsRequest*>(value);

  const MsgValueType value_type = SHIP_MESSAGE_DESERIALIZE_GET_VALUE_TYPE(deserialize);

  // Assert: Verify with expected return value and output message value
  EXPECT_EQ(value_type, GetParam().value_type);
  if (value_type == kSmeConnectionAccessMethodsRequest) {
    EXPECT_NE(sme_access_methods_req, nullptr);
  } else {
    EXPECT_EQ(sme_access_methods_req, nullptr);
  }

  MessageBufferRelease(&buf);
  ShipMessageDeserializeDelete(deserialize);
}

INSTANTIATE_TEST_SUITE_P(
    SmeConnectionAccessMethodsRequestDeserialize,
    SmeConnectionAccessMethodsRequestDeserializeTests,
    ::testing::Values(
        SmeConnectionAccessMethodsRequestDeserializeTestInput{
            .description = "Test unexpected data within access methods request"sv,
            .msg         = "\001{\"accessMethodsRequest\":[{\"data\":\"unexpected\"}]}"sv,
            .value_type  = kValueUndefined,
        },
        SmeConnectionAccessMethodsRequestDeserializeTestInput{
            .description = "Test access methods request"sv,
            .msg         = "\001{\"accessMethodsRequest\":[]}"sv,
            .value_type  = kSmeConnectionAccessMethodsRequest,
        }
    )
);
