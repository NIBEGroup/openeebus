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

MATCHER_P3(AccessMethodsEq, id, dns_sd_mdns, uri, "") {
  *result_listener << "id = " << arg->id;
  *result_listener << ", dns_sd_mdsn = " << arg->dns_sd_mdns;
  *result_listener << ", dns.uri = " << arg->dns.uri;

  bool match = (arg->id == id);

  match = match && (arg->dns_sd_mdns == dns_sd_mdns);

  if (arg->dns.uri == nullptr) {
    match = match && (uri == nullptr);
  } else {
    match = match && (uri != nullptr);
    match = match && (*uri == arg->dns.uri);
  }

  return match;
}

struct SmeConnectionAccessMethodsDeserializeTestInput {
  std::string_view description;
  std::string_view msg;
  MsgValueType value_type;
  std::string_view id;
  bool dns_sd_mdsn;
  std::shared_ptr<std::string_view> uri;
};

std::ostream& operator<<(std::ostream& os, SmeConnectionAccessMethodsDeserializeTestInput test_input) {
  return os << test_input.description;
}

class SmeConnectionAccessMethodsDeserializeTests
    : public ::testing::TestWithParam<SmeConnectionAccessMethodsDeserializeTestInput> {};

TEST(SmeConnectionAccessMethodsDeserializeTests, SmeConnectionAccessMethodsDeserializeInputArgs) {
  auto deserialize = ShipMessageDeserializeCreate(nullptr);

  void* const value       = SHIP_MESSAGE_DESERIALIZE_GET_VALUE(deserialize);
  auto sme_access_methods = reinterpret_cast<AccessMethods*>(value);

  const MsgValueType value_type = SHIP_MESSAGE_DESERIALIZE_GET_VALUE_TYPE(deserialize);

  EXPECT_EQ(sme_access_methods, nullptr);
  EXPECT_EQ(value_type, kValueUndefined);

  ShipMessageDeserializeDelete(deserialize);
}

TEST_P(SmeConnectionAccessMethodsDeserializeTests, SmeConnectionAccessMethodsDeserializeTests) {
  // Arrange: Initialize the message buffer with parameters from test input
  MessageBuffer buf;
  MessageBufferInitWithStringView(&buf, GetParam().msg);

  // Act: Run the AccessMethods deserialization
  auto deserialize = ShipMessageDeserializeCreate(&buf);

  void* const value       = SHIP_MESSAGE_DESERIALIZE_GET_VALUE(deserialize);
  auto sme_access_methods = reinterpret_cast<AccessMethods*>(value);

  const MsgValueType value_type = SHIP_MESSAGE_DESERIALIZE_GET_VALUE_TYPE(deserialize);

  // Assert: Verify with expected return value and output message value
  EXPECT_EQ(value_type, GetParam().value_type);
  if ((value_type == kSmeConnectionAccessMethods) && (sme_access_methods != nullptr)) {
    EXPECT_THAT(sme_access_methods, AccessMethodsEq(GetParam().id, GetParam().dns_sd_mdsn, GetParam().uri));
  } else {
    EXPECT_EQ(sme_access_methods, nullptr);
  }

  MessageBufferRelease(&buf);
  ShipMessageDeserializeDelete(deserialize);
}

INSTANTIATE_TEST_SUITE_P(
    SmeConnectionAccessMethodsDeserialize,
    SmeConnectionAccessMethodsDeserializeTests,
    ::testing::Values(
        SmeConnectionAccessMethodsDeserializeTestInput{
            .description = "Test id missing"sv,
            .msg         = "\001{\"accessMethods\":[]}"sv,
            .value_type  = kValueUndefined,
            .dns_sd_mdsn = false,
            .uri         = nullptr,
        },
        SmeConnectionAccessMethodsDeserializeTestInput{
            .description = "Test access methods request with empty ID"sv,
            .msg         = "\001{\"accessMethods\":[{\"id\":\"\"}]}"sv,
            .value_type  = kSmeConnectionAccessMethods,
            .id          = ""sv,
            .dns_sd_mdsn = false,
            .uri         = nullptr,
        },
        SmeConnectionAccessMethodsDeserializeTestInput{
            .description = "Test access methods request"sv,
            .msg         = "\001{\"accessMethods\":[{\"id\":\"RemoteShipID\"}]}"sv,
            .value_type  = kSmeConnectionAccessMethods,
            .id          = "RemoteShipID"sv,
            .dns_sd_mdsn = false,
            .uri         = nullptr,
        },
        SmeConnectionAccessMethodsDeserializeTestInput{
            .description = "Test access methods request with dns_sd_mdsn set"sv,
            .msg         = "\001{\"accessMethods\":[{\"id\":\"RemoteShipID\"},{\"dnsSd_mDns\":[]}]}"sv,
            .value_type  = kSmeConnectionAccessMethods,
            .id          = "RemoteShipID"sv,
            .dns_sd_mdsn = true,
            .uri         = nullptr,
        },
        SmeConnectionAccessMethodsDeserializeTestInput{
            .description = "Test access methods request with uri set"sv,
            .msg         = "\001{\"accessMethods\":"
                           "[{\"id\":\"RemoteShipID\"},"
                           "{\"dns\":[{\"uri\":\"wss://DESKTOP-IAKQS71.local:4769\"}]}]}"sv,
            .value_type  = kSmeConnectionAccessMethods,
            .id          = "RemoteShipID"sv,
            .dns_sd_mdsn = false,
            .uri         = std::make_shared<std::string_view>("wss://DESKTOP-IAKQS71.local:4769"),
        },
        SmeConnectionAccessMethodsDeserializeTestInput{
            .description = "Test access methods request with dns_sd_mdsn and uri set"sv,
            .msg         = "\001{\"accessMethods\":"
                           "[{\"id\":\"RemoteShipID\"},"
                           "{\"dnsSd_mDns\":[]},"
                           "{\"dns\":[{\"uri\":\"wss://DESKTOP-IAKQS71.local:4769\"}]}]}"sv,
            .value_type  = kSmeConnectionAccessMethods,
            .id          = "RemoteShipID"sv,
            .dns_sd_mdsn = true,
            .uri         = std::make_shared<std::string_view>("wss://DESKTOP-IAKQS71.local:4769"),
        }
    )
);
