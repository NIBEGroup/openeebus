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

MATCHER_P2(DataEq, protocol_id, payload, "") {
  *result_listener << "protocol_id = " << arg->header.protocol_id;
  std::string_view payload_sv(reinterpret_cast<const char*>(arg->payload.data), arg->payload.data_size);
  *result_listener << ", payload = " << payload_sv;

  bool match = (arg->header.protocol_id == protocol_id);
  match      = match && (payload_sv == payload);

  return match;
}

struct DataDeserializeTestInput {
  std::string_view description;
  std::string_view msg;
  MsgValueType value_type;
  std::string_view protocol_id;
  std::string_view payload;
};

std::ostream& operator<<(std::ostream& os, DataDeserializeTestInput test_input) {
  return os << test_input.description;
}

class DataDeserializeTests : public ::testing::TestWithParam<DataDeserializeTestInput> {};

TEST(DataDeserializeTests, DataDeserializeInputArgs) {
  auto deserialize = ShipMessageDeserializeCreate(nullptr);

  void* const value = SHIP_MESSAGE_DESERIALIZE_GET_VALUE(deserialize);
  auto data         = reinterpret_cast<Data*>(value);

  const MsgValueType value_type = SHIP_MESSAGE_DESERIALIZE_GET_VALUE_TYPE(deserialize);

  EXPECT_EQ(data, nullptr);
  EXPECT_EQ(value_type, kValueUndefined);

  ShipMessageDeserializeDelete(deserialize);
}

TEST_P(DataDeserializeTests, DataDeserializeTests) {
  // Arrange: Initialize the message buffer with parameters from test input
  MessageBuffer buf;
  MessageBufferInitWithStringView(&buf, GetParam().msg);

  // Act: Run the Data deserialization
  auto deserialize = ShipMessageDeserializeCreate(&buf);

  void* const value = SHIP_MESSAGE_DESERIALIZE_GET_VALUE(deserialize);
  auto data         = reinterpret_cast<Data*>(value);

  const MsgValueType value_type = SHIP_MESSAGE_DESERIALIZE_GET_VALUE_TYPE(deserialize);

  // Assert: Verify with expected return value and output message value
  EXPECT_EQ(value_type, GetParam().value_type);
  if ((value_type == kData) && (data != nullptr)) {
    EXPECT_THAT(data, DataEq(GetParam().protocol_id, GetParam().payload));
  } else {
    EXPECT_EQ(data, nullptr);
  }

  MessageBufferRelease(&buf);
  ShipMessageDeserializeDelete(deserialize);
}

INSTANTIATE_TEST_SUITE_P(
    DataDeserializeTests,
    DataDeserializeTests,
    ::testing::Values(
        DataDeserializeTestInput{
            .description = "Test protocol ID is missing"sv,
            .msg         = "\002{\"data\":["
                           "{\"header\":[]},"
                           "{\"payload\":{\"datagram\":[]}}]}"sv,
            .value_type  = kValueUndefined,
            .protocol_id = ""sv,
            .payload     = ""sv,
        },
        DataDeserializeTestInput{
            .description = "Test payload is missing"sv,
            .msg         = "\002{\"data\":["
                           "{\"header\":[{\"protocolId\":\"ee1.0\"}]}]}"sv,
            .value_type  = kValueUndefined,
            .protocol_id = ""sv,
            .payload     = ""sv,
        },
        DataDeserializeTestInput{
            .description = "Test data with protocol_id = \"ee1.0\","
                           " payload = :{\"datagram\":[]}"sv,
            .msg         = "\002{\"data\":["
                           ""
                           "{\"header\":["
                           ""
                           ""
                           "{\"protocolId\":\"ee1.0\"}"
                           ""
                           "]},"
                           ""
                           "{\"payload\":"
                           ""
                           ""
                           "{\"datagram\":[]}"
                           ""
                           "}"
                           "]}"sv,
            .value_type  = kData,
            .protocol_id = "ee1.0"sv,
            .payload     = "{\"datagram\":[]}\0"sv,
        },
        DataDeserializeTestInput{
            .description = "Test data with protocol_id = \"ee3.7\","
                           " payload = :{\"datagram\":[{\"header\":[]}]}"sv,
            .msg         = "\002{\"data\":["
                           ""
                           "{\"header\":[{\"protocolId\":\"ee3.7\"}]},"
                           ""
                           "{\"payload\":"
                           ""
                           ""
                           "{\"datagram\":["
                           ""
                           ""
                           ""
                           "{\"header\":[]}"
                           ""
                           ""
                           "]}"
                           ""
                           "}"
                           "]}"sv,
            .value_type  = kData,
            .protocol_id = "ee3.7"sv,
            .payload     = "{\"datagram\":[{\"header\":[]}]}\0"sv,
        }
    )
);
