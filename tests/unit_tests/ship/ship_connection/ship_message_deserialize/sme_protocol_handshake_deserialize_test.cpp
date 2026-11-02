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

MATCHER_P3(SmeProtHandshakeEq, handshake_type, version, formats, "") {
  *result_listener << "handshake_type = " << arg->handshake_type;
  *result_listener << ", version = {" << static_cast<int>(arg->version.major);
  *result_listener << ", " << static_cast<int>(arg->version.minor) << "}, ";
  *result_listener << ", formats = {";
  for (size_t i = 0; i < arg->formats.format_size; ++i) {
    if (i != 0) {
      *result_listener << ", ";
    }

    *result_listener << arg->formats.format[i];
  }

  *result_listener << "}";

  bool match = (arg->handshake_type == handshake_type);

  match = match && (arg->version.major == version.major);
  match = match && (arg->version.minor == version.minor);
  match = match && (arg->formats.format_size == formats.format_size);
  if (match) {
    for (size_t i = 0; i < arg->formats.format_size; ++i) {
      if (arg->formats.format[i] != formats.format[i]) {
        match = false;
        break;
      }
    }
  }

  return match;
}

struct SmeProtocolHandshakeDeserializeTestInput {
  std::string_view description;
  std::string_view msg;
  MsgValueType value_type;
  ProtocolHandshakeType handshake_type;
  Version version;
  MessageProtocolFormats formats;
};

std::ostream& operator<<(std::ostream& os, SmeProtocolHandshakeDeserializeTestInput test_input) {
  return os << test_input.description;
}

class SmeProtocolHandshakeDeserializeTests : public ::testing::TestWithParam<SmeProtocolHandshakeDeserializeTestInput> {
};

TEST(ShipMessageParseTest, SmeProtocolHandshakeDeserializeInputArgs) {
  auto deserialize = ShipMessageDeserializeCreate(nullptr);

  void* const value = SHIP_MESSAGE_DESERIALIZE_GET_VALUE(deserialize);
  auto sme_prot_hs  = reinterpret_cast<MessageProtocolHandshake*>(value);

  const MsgValueType value_type = SHIP_MESSAGE_DESERIALIZE_GET_VALUE_TYPE(deserialize);

  EXPECT_EQ(sme_prot_hs, nullptr);
  EXPECT_EQ(value_type, kValueUndefined);

  ShipMessageDeserializeDelete(deserialize);
}

TEST_P(SmeProtocolHandshakeDeserializeTests, SmeProtocolHandshakeDeserializeTests) {
  // Arrange: Initialize the SmeProtocolHandshakeDeserialize() parameters from test input
  MessageBuffer buf;
  MessageBufferInitWithStringView(&buf, GetParam().msg);

  // Act: Run the SmeHelloDeserialize()
  auto deserialize = ShipMessageDeserializeCreate(&buf);

  void* const value = SHIP_MESSAGE_DESERIALIZE_GET_VALUE(deserialize);
  auto sme_prot_hs  = reinterpret_cast<MessageProtocolHandshake*>(value);

  const MsgValueType value_type = SHIP_MESSAGE_DESERIALIZE_GET_VALUE_TYPE(deserialize);

  // Assert: Verify with expected return value and output message value
  EXPECT_EQ(value_type, GetParam().value_type);
  if ((value_type == kSmeProtocolHandshake) && (sme_prot_hs != nullptr)) {
    EXPECT_THAT(sme_prot_hs, SmeProtHandshakeEq(GetParam().handshake_type, GetParam().version, GetParam().formats));
  } else {
    EXPECT_EQ(sme_prot_hs, nullptr);
  }

  MessageBufferRelease(&buf);
  ShipMessageDeserializeDelete(deserialize);
}

INSTANTIATE_TEST_SUITE_P(
    SmeProtocolHandshakeDeserialize,
    SmeProtocolHandshakeDeserializeTests,
    ::testing::Values(
        SmeProtocolHandshakeDeserializeTestInput{
            .description    = "Test missing type, version = {1, 0}, formats = {JSON-UTF8}"sv,
            .msg            = "\001{\"messageProtocolHandshake\":["
                              "{\"version\":[{\"major\":1},{\"minor\":0}]},"
                              "{\"formats\":[{\"format\":[\"JSON-UTF8\"]}]}]}"sv,
            .value_type     = kValueUndefined,
            .handshake_type = static_cast<ProtocolHandshakeType>(0),
            .version        = { 1, 0},
            .formats        = {{}, 1},
},
        SmeProtocolHandshakeDeserializeTestInput{
            .description    = "Test type = select, missing version, formats = {JSON-UTF8}"sv,
            .msg            = "\001{\"messageProtocolHandshake\":["
                              "{\"handshakeType\":\"select\"},"
                              "{\"formats\":[{\"format\":[\"JSON-UTF8\"]}]}]}"sv,
            .value_type     = kValueUndefined,
            .handshake_type = static_cast<ProtocolHandshakeType>(0),
            .version        = {1, 0},
            .formats        = {{}, 1},
        },
        SmeProtocolHandshakeDeserializeTestInput{
            .description    = "Test type = select, version = {1, 0}, missing formats"sv,
            .msg            = "\001{\"messageProtocolHandshake\":["
                              "{\"handshakeType\":\"select\"},"
                              "{\"version\":[{\"major\":1},{\"minor\":0}]}],"sv,
            .value_type     = kValueUndefined,
            .handshake_type = static_cast<ProtocolHandshakeType>(0),
            .version        = {1, 0},
            .formats        = {{}, 1},
        },
        SmeProtocolHandshakeDeserializeTestInput{
            .description    = "Test type = select, version = {1, 0}, formats = {JSON-UTF8}"sv,
            .msg            = "\001{\"messageProtocolHandshake\":["
                              "{\"handshakeType\":\"select\"},"
                              "{\"version\":[{\"major\":1},{\"minor\":0}]},"
                              "{\"formats\":[{\"format\":[\"JSON-UTF8\"]}]}]}"sv,
            .value_type     = kSmeProtocolHandshake,
            .handshake_type = kProtocolHandshakeTypeSelect,
            .version        = {1, 0},
            .formats        = {{kMessageProtocolFormatTypeUTF8}, 1},
        },
        SmeProtocolHandshakeDeserializeTestInput{
            .description    = "Test type = select, version = {2, 3}, formats = {JSON-UTF8,JSON-UTF16}"sv,
            .msg            = "\001{\"messageProtocolHandshake\":["
                              "{\"handshakeType\":\"select\"},"
                              "{\"version\":[{\"major\":2},{\"minor\":3}]},"
                              "{\"formats\":[{\"format\":[\"JSON-UTF8\",\"JSON-UTF16\"]}]}]}"sv,
            .value_type     = kSmeProtocolHandshake,
            .handshake_type = kProtocolHandshakeTypeSelect,
            .version        = {2, 3},
            .formats        = {{kMessageProtocolFormatTypeUTF8, kMessageProtocolFormatTypeUTF16}, 2},
        },
        SmeProtocolHandshakeDeserializeTestInput{
            .description    = "Test type = announceMax, version = {0, 1}, formats = null"sv,
            .msg            = "\001{\"messageProtocolHandshake\":["
                              "{\"handshakeType\":\"select\"},"
                              "{\"version\":[{\"major\":0},{\"minor\":1}]},"
                              "{\"formats\":[{\"format\":null}]}]}"sv,
            .value_type     = kSmeProtocolHandshake,
            .handshake_type = kProtocolHandshakeTypeSelect,
            .version        = {0, 1},
            .formats        = {{}, 0},
        }
    )
);
