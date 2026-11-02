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

TEST(SmeCloseSerializeTests, SmeCloseInputArgs) {
  auto serialize = ShipMessageSerializeCreate(nullptr, kValueUndefined);
  EXPECT_EQ(SHIP_MESSAGE_SERIALIZE_GET_BUFFER(serialize), nullptr);
  ShipMessageSerializeDelete(serialize);

  serialize = ShipMessageSerializeCreate(nullptr, kSmeClose);
  EXPECT_EQ(SHIP_MESSAGE_SERIALIZE_GET_BUFFER(serialize), nullptr);
  ShipMessageSerializeDelete(serialize);

  ConnectionClose sme_close;
  serialize = ShipMessageSerializeCreate(&sme_close, kValueUndefined);
  EXPECT_EQ(SHIP_MESSAGE_SERIALIZE_GET_BUFFER(serialize), nullptr);
  ShipMessageSerializeDelete(serialize);
}

struct SmeCloseSerializeTestInput {
  std::string_view description;
  ConnectionClosePhaseType phase;
  std::shared_ptr<uint32_t> max_time;
  std::shared_ptr<std::string_view> reason;
  std::string_view msg;
};

std::ostream& operator<<(std::ostream& os, SmeCloseSerializeTestInput Test) {
  return os << Test.description;
}

class SmeCloseSerializeTests : public ::testing::TestWithParam<SmeCloseSerializeTestInput> {};

TEST_P(SmeCloseSerializeTests, SmeCloseSerializeTests) {
  // Arrange: Initialize the ConnectionClose from test input
  ConnectionClose sme_close;

  sme_close.phase = GetParam().phase;
  if (GetParam().max_time != nullptr) {
    sme_close.max_time_val = *GetParam().max_time;
    sme_close.max_time     = &sme_close.max_time_val;
  } else {
    sme_close.max_time = nullptr;
  }

  if (GetParam().reason != nullptr) {
    sme_close.reason = GetParam().reason->data();
  } else {
    sme_close.reason = nullptr;
  }

  // Act: Run the ConnectionClose serialization procedure
  auto serialize = ShipMessageSerializeCreate(&sme_close, kSmeClose);

  const MessageBuffer* const buf = SHIP_MESSAGE_SERIALIZE_GET_BUFFER(serialize);

  // Assert: Verify with expected output buffer value
  if (buf != nullptr) {
    EXPECT_THAT(buf, BufDataEq(GetParam().msg));
  }

  ShipMessageSerializeDelete(serialize);
}

INSTANTIATE_TEST_SUITE_P(
    SmeCloseSerializeTests,
    SmeCloseSerializeTests,
    ::testing::Values(
        SmeCloseSerializeTestInput{
            .description = "Test connection close with phase = announce"sv,
            .phase       = kConnectionClosePhaseTypeAnnounce,
            .max_time    = nullptr,
            .reason      = nullptr,
            .msg         = "\003{\"connectionClose\":[{\"phase\":\"announce\"}]}"sv,
        },
        SmeCloseSerializeTestInput{
            .description = "Test connection close with phase = confirm"sv,
            .phase       = kConnectionClosePhaseTypeConfirm,
            .max_time    = nullptr,
            .reason      = nullptr,
            .msg         = "\003{\"connectionClose\":[{\"phase\":\"confirm\"}]}"sv,
        },
        SmeCloseSerializeTestInput{
            .description = "Test connection close with maxTime set"sv,
            .phase       = kConnectionClosePhaseTypeAnnounce,
            .max_time    = std::make_shared<uint32_t>(10000),
            .reason      = nullptr,
            .msg         = "\003{\"connectionClose\":[{\"phase\":\"announce\"},{\"maxTime\":10000}]}"sv,
        },
        SmeCloseSerializeTestInput{
            .description = "Test connection close with reason set"sv,
            .phase       = kConnectionClosePhaseTypeConfirm,
            .max_time    = nullptr,
            .reason      = std::make_shared<std::string_view>("Closed by user"),
            .msg         = "\003{\"connectionClose\":"
                           "[{\"phase\":\"confirm\"},"
                           "{\"reason\":\"Closed by user\"}]}"sv,
        },
        SmeCloseSerializeTestInput{
            .description = "Test connection close with maxTime and reason set"sv,
            .phase       = kConnectionClosePhaseTypeAnnounce,
            .max_time    = std::make_shared<uint32_t>(20000),
            .reason      = std::make_shared<std::string_view>("Unexpected error"),
            .msg         = "\003{\"connectionClose\":"
                           "[{\"phase\":\"announce\"},"
                           "{\"maxTime\":20000},"
                           "{\"reason\":\"Unexpected error\"}]}"sv,
        }
    )
);
