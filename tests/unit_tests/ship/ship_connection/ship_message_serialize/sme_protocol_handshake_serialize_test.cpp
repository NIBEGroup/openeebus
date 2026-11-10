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

TEST(ShipMessageParseTest, SmeProtocolHandshakeSerializeInputArgs) {
  ShipMessageSerializeObject* serialize = ShipMessageSerializeCreate(nullptr, kValueUndefined);
  EXPECT_EQ(SHIP_MESSAGE_SERIALIZE_GET_BUFFER(serialize), nullptr);
  ShipMessageSerializeDelete(serialize);

  serialize = ShipMessageSerializeCreate(nullptr, kSmeProtocolHandshake);
  EXPECT_EQ(SHIP_MESSAGE_SERIALIZE_GET_BUFFER(serialize), nullptr);
  ShipMessageSerializeDelete(serialize);

  MessageProtocolHandshake sme_prot_hs;
  serialize = ShipMessageSerializeCreate(&sme_prot_hs, kValueUndefined);
  EXPECT_EQ(SHIP_MESSAGE_SERIALIZE_GET_BUFFER(serialize), nullptr);
  ShipMessageSerializeDelete(serialize);
}

struct SmeProtocolHandshakeSerializeTestInput {
  std::string_view description;
  ProtocolHandshakeType handshake_type;
  Version version;
  MessageProtocolFormats formats;
  std::string_view msg;
};

std::ostream& operator<<(std::ostream& os, SmeProtocolHandshakeSerializeTestInput Test) {
  return os << Test.description;
}

class SmeProtocolHandshakeSerializeTests : public ::testing::TestWithParam<SmeProtocolHandshakeSerializeTestInput> {};

TEST_P(SmeProtocolHandshakeSerializeTests, SmeProtocolHandshakeSerializeTests) {
  // Arrange: Initialize the SmeProtocolHandshakeSerialize() parameters from test input
  MessageProtocolHandshake sme_prot_hs;

  sme_prot_hs.handshake_type = GetParam().handshake_type;
  sme_prot_hs.version        = GetParam().version;
  sme_prot_hs.formats        = GetParam().formats;

  // Act: Run the SmeProtocolHandshakeError serialize procedure
  auto serialize = ShipMessageSerializeCreate(&sme_prot_hs, kSmeProtocolHandshake);

  // Assert: Verify with expected output buffer value
  const MessageBuffer* const buf = SHIP_MESSAGE_SERIALIZE_GET_BUFFER(serialize);
  if (buf != nullptr) {
    EXPECT_THAT(buf, BufDataEq(GetParam().msg));
  }

  ShipMessageSerializeDelete(serialize);
}

INSTANTIATE_TEST_SUITE_P(
    SmeProtocolHandshakeSerialize,
    SmeProtocolHandshakeSerializeTests,
    ::testing::Values(
        SmeProtocolHandshakeSerializeTestInput{
            .description    = "Test type = select, version = {1, 0}, formats = {JSON-UTF8}"sv,
            .handshake_type = kProtocolHandshakeTypeSelect,
            .version        = {                               1, 0},
            .formats        = {{kMessageProtocolFormatTypeUTF8}, 1},
            .msg            = "\001{\"messageProtocolHandshake\":["
                              "{\"handshakeType\":\"select\"},"
                              "{\"version\":[{\"major\":1},{\"minor\":0}]},"
                              "{\"formats\":[{\"format\":[\"JSON-UTF8\"]}]}]}"sv,
},
        SmeProtocolHandshakeSerializeTestInput{
            .description    = "Test type = select, version = {2, 3}, formats = {JSON-UTF8,JSON-UTF16}"sv,
            .handshake_type = kProtocolHandshakeTypeSelect,
            .version        = {2, 3},
            .formats        = {{kMessageProtocolFormatTypeUTF8, kMessageProtocolFormatTypeUTF16}, 2},
            .msg            = "\001{\"messageProtocolHandshake\":["
                              "{\"handshakeType\":\"select\"},"
                              "{\"version\":[{\"major\":2},{\"minor\":3}]},"
                              "{\"formats\":[{\"format\":[\"JSON-UTF8\",\"JSON-UTF16\"]}]}]}"sv,
        },
        SmeProtocolHandshakeSerializeTestInput{
            .description    = "Test type = announceMax, version = {0, 1}, formats = null"sv,
            .handshake_type = kProtocolHandshakeTypeSelect,
            .version        = {0, 1},
            .formats        = {{}, 0},
            .msg            = "\001{\"messageProtocolHandshake\":["
                              "{\"handshakeType\":\"select\"},"
                              "{\"version\":[{\"major\":0},{\"minor\":1}]},"
                              "{\"formats\":[{\"format\":null}]}]}"sv,
        }
    )
);
