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

TEST(SmeConnectionAccessMethodsSerializeTests, SmeConnectionAccessMethodsInputArgs) {
  auto serialize = ShipMessageSerializeCreate(nullptr, kValueUndefined);
  EXPECT_EQ(SHIP_MESSAGE_SERIALIZE_GET_BUFFER(serialize), nullptr);
  ShipMessageSerializeDelete(serialize);

  serialize = ShipMessageSerializeCreate(nullptr, kSmeConnectionAccessMethods);
  EXPECT_EQ(SHIP_MESSAGE_SERIALIZE_GET_BUFFER(serialize), nullptr);
  ShipMessageSerializeDelete(serialize);

  AccessMethods sme_access_methods;
  serialize = ShipMessageSerializeCreate(&sme_access_methods, kValueUndefined);
  EXPECT_EQ(SHIP_MESSAGE_SERIALIZE_GET_BUFFER(serialize), nullptr);
  ShipMessageSerializeDelete(serialize);
}

struct SmeConnectionAccessMethodsSerializeTestInput {
  std::string_view description;
  const char* id;
  bool dns_sd_mdsn;
  const char* uri;
  std::string_view msg;
};

std::ostream& operator<<(std::ostream& os, SmeConnectionAccessMethodsSerializeTestInput Test) {
  return os << Test.description;
}

class SmeConnectionAccessMethodsSerializeTests
    : public ::testing::TestWithParam<SmeConnectionAccessMethodsSerializeTestInput> {};

TEST_P(SmeConnectionAccessMethodsSerializeTests, SmeConnectionAccessMethodsSerializeTests) {
  // Arrange: Initialize the AccessMethods from test input
  AccessMethods sme_access_methods = {
      .id          = GetParam().id,
      .dns_sd_mdns = GetParam().dns_sd_mdsn,
      .dns         = {.uri = GetParam().uri},
  };

  // Act: Run the AccessMethods serialization procedure
  auto serialize = ShipMessageSerializeCreate(&sme_access_methods, kSmeConnectionAccessMethods);

  const MessageBuffer* const buf = SHIP_MESSAGE_SERIALIZE_GET_BUFFER(serialize);

  // Assert: Verify with expected output buffer value
  if (buf != nullptr) {
    EXPECT_THAT(buf, BufDataEq(GetParam().msg));
  }

  ShipMessageSerializeDelete(serialize);
}

INSTANTIATE_TEST_SUITE_P(
    SmeConnectionAccessMethodsSerialize,
    SmeConnectionAccessMethodsSerializeTests,
    ::testing::Values(
        SmeConnectionAccessMethodsSerializeTestInput{
            .description = "Test access methods request with empty ID"sv,
            .id          = "",
            .dns_sd_mdsn = false,
            .uri         = nullptr,
            .msg         = "\001{\"accessMethods\":[{\"id\":\"\"}]}"sv,
        },
        SmeConnectionAccessMethodsSerializeTestInput{
            .description = "Test access methods request"sv,
            .id          = "RemoteShipID",
            .dns_sd_mdsn = false,
            .uri         = nullptr,
            .msg         = "\001{\"accessMethods\":[{\"id\":\"RemoteShipID\"}]}"sv,
        },
        SmeConnectionAccessMethodsSerializeTestInput{
            .description = "Test access methods request with dns_sd_mdsn set"sv,
            .id          = "RemoteShipID",
            .dns_sd_mdsn = true,
            .uri         = nullptr,
            .msg         = "\001{\"accessMethods\":[{\"id\":\"RemoteShipID\"},{\"dnsSd_mDns\":[]}]}"sv,
        },
        SmeConnectionAccessMethodsSerializeTestInput{
            .description = "Test access methods request with uri set"sv,
            .id          = "RemoteShipID",
            .dns_sd_mdsn = false,
            .uri         = "wss://DESKTOP-IAKQS71.local:4769",
            .msg         = "\001{\"accessMethods\":"
                           "[{\"id\":\"RemoteShipID\"},"
                           "{\"dns\":[{\"uri\":\"wss://DESKTOP-IAKQS71.local:4769\"}]}]}"sv,
        },
        SmeConnectionAccessMethodsSerializeTestInput{
            .description = "Test access methods request with dns_sd_mdsn and uri set"sv,
            .id          = "RemoteShipID",
            .dns_sd_mdsn = true,
            .uri         = "wss://DESKTOP-IAKQS71.local:4769",
            .msg         = "\001{\"accessMethods\":"
                           "[{\"id\":\"RemoteShipID\"},"
                           "{\"dnsSd_mDns\":[]},"
                           "{\"dns\":[{\"uri\":\"wss://DESKTOP-IAKQS71.local:4769\"}]}]}"sv,
        }
    )
);
