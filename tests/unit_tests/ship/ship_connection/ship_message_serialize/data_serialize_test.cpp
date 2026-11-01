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

TEST(DataSerializeTests, DataInputArgs) {
  auto serialize = ShipMessageSerializeCreate(nullptr, kValueUndefined);
  EXPECT_EQ(SHIP_MESSAGE_SERIALIZE_GET_BUFFER(serialize), nullptr);
  ShipMessageSerializeDelete(serialize);

  serialize = ShipMessageSerializeCreate(nullptr, kData);
  EXPECT_EQ(SHIP_MESSAGE_SERIALIZE_GET_BUFFER(serialize), nullptr);
  ShipMessageSerializeDelete(serialize);

  Data data;
  serialize = ShipMessageSerializeCreate(&data, kValueUndefined);
  EXPECT_EQ(SHIP_MESSAGE_SERIALIZE_GET_BUFFER(serialize), nullptr);
  ShipMessageSerializeDelete(serialize);
}

struct DataSerializeTestInput {
  std::string_view description;
  std::string_view protocol_id;
  std::string_view payload;
  std::string_view msg;
};

std::ostream& operator<<(std::ostream& os, DataSerializeTestInput Test) {
  return os << Test.description;
}

class DataSerializeTests : public ::testing::TestWithParam<DataSerializeTestInput> {};

TEST_P(DataSerializeTests, DataSerializeTests) {
  // Arrange: Initialize the Data from test input
  Data data;

  strcpy(data.header.protocol_id, GetParam().protocol_id.data());
  char* payload = const_cast<char*>(GetParam().payload.data());

  MessageBufferInitWithDeallocator(&data.payload, reinterpret_cast<uint8_t*>(payload), strlen(payload) + 1, NULL);

  // Act: Run the Data serialization procedure
  auto serialize = ShipMessageSerializeCreate(&data, kData);

  const MessageBuffer* const buf = SHIP_MESSAGE_SERIALIZE_GET_BUFFER(serialize);

  // Assert: Verify with expected output buffer value
  if (buf != nullptr) {
    EXPECT_THAT(buf, BufDataEq(GetParam().msg));
  }

  MessageBufferRelease(&data.payload);
  ShipMessageSerializeDelete(serialize);
}

INSTANTIATE_TEST_SUITE_P(
    DataSerializeTests,
    DataSerializeTests,
    ::testing::Values(
        DataSerializeTestInput{
            .description = "Test data with protocol_id = \"ee1.0\","
                           " payload = :{\"datagram\":[]}"sv,
            .protocol_id = "ee1.0"sv,
            .payload     = "{\"datagram\":[]}\0"sv,
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
        },
        DataSerializeTestInput{
            .description = "Test data with protocol_id = \"ee3.7\","
                           " payload = :{\"datagram\":[{\"header\":[]}]}"sv,
            .protocol_id = "ee3.7"sv,
            .payload     = "{\"datagram\":[{\"header\":[]}]}\0"sv,
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
        }
    )
);
