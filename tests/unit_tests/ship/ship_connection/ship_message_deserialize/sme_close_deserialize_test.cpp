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

MATCHER_P3(ConnectionCloseEq, phase, max_time, reason, "") {
  *result_listener << "phase = " << arg->phase;
  *result_listener << ", max_time = ";
  if (arg->max_time != nullptr) {
    *result_listener << *(arg->max_time);
  } else {
    *result_listener << nullptr;
  }

  *result_listener << ", reason = " << arg->reason;

  bool match = (arg->phase == phase);
  if (arg->max_time == nullptr) {
    match = match && (max_time == nullptr);
  } else {
    match = match && (max_time != nullptr);
    match = match && (*max_time == *(arg->max_time));
  }

  return match;
}

struct SmeCloseCloseDeserializeTestInput {
  std::string_view description;
  std::string_view msg;
  MsgValueType value_type;
  ConnectionClosePhaseType phase;
  std::shared_ptr<uint32_t> max_time;
  std::shared_ptr<std::string_view> reason;
};

std::ostream& operator<<(std::ostream& os, SmeCloseCloseDeserializeTestInput test_input) {
  return os << test_input.description;
}

class SmeCloseCloseDeserializeTests : public ::testing::TestWithParam<SmeCloseCloseDeserializeTestInput> {};

TEST(SmeCloseCloseDeserializeTests, SmeCloseCloseDeserializeInputArgs) {
  auto deserialize = ShipMessageDeserializeCreate(nullptr);

  void* const value = SHIP_MESSAGE_DESERIALIZE_GET_VALUE(deserialize);
  auto sme_close    = reinterpret_cast<ConnectionClose*>(value);

  const MsgValueType value_type = SHIP_MESSAGE_DESERIALIZE_GET_VALUE_TYPE(deserialize);

  EXPECT_EQ(sme_close, nullptr);
  EXPECT_EQ(value_type, kValueUndefined);

  ShipMessageDeserializeDelete(deserialize);
}

TEST_P(SmeCloseCloseDeserializeTests, SmeCloseCloseDeserializeTests) {
  // Arrange: Initialize the message buffer with parameters from test input
  MessageBuffer buf;
  MessageBufferInitWithStringView(&buf, GetParam().msg);

  // Act: Run the ConnectionClose deserialization
  auto deserialize = ShipMessageDeserializeCreate(&buf);

  void* const value     = SHIP_MESSAGE_DESERIALIZE_GET_VALUE(deserialize);
  auto sme_close_metods = reinterpret_cast<ConnectionClose*>(value);

  const MsgValueType value_type = SHIP_MESSAGE_DESERIALIZE_GET_VALUE_TYPE(deserialize);

  // Assert: Verify with expected return value and output message value
  EXPECT_EQ(value_type, GetParam().value_type);
  if ((value_type == kSmeClose) && (sme_close_metods != nullptr)) {
    EXPECT_THAT(sme_close_metods, ConnectionCloseEq(GetParam().phase, GetParam().max_time, GetParam().reason));
  } else {
    EXPECT_EQ(sme_close_metods, nullptr);
  }

  MessageBufferRelease(&buf);
  ShipMessageDeserializeDelete(deserialize);
}

INSTANTIATE_TEST_SUITE_P(
    SmeCloseCloseDeserializeTests,
    SmeCloseCloseDeserializeTests,
    ::testing::Values(
        SmeCloseCloseDeserializeTestInput{
            .description = "Test phase is missing"sv,
            .msg         = "\003{\"connectionClose\":[]}"sv,
            .value_type  = kValueUndefined,
            .phase       = static_cast<ConnectionClosePhaseType>(0),
            .max_time    = nullptr,
            .reason      = nullptr,
        },
        SmeCloseCloseDeserializeTestInput{
            .description = "Test connection close with phase = announce"sv,
            .msg         = "\003{\"connectionClose\":[{\"phase\":\"announce\"}]}"sv,
            .value_type  = kSmeClose,
            .phase       = kConnectionClosePhaseTypeAnnounce,
            .max_time    = nullptr,
            .reason      = nullptr,
        },
        SmeCloseCloseDeserializeTestInput{
            .description = "Test connection close with phase = confirm"sv,
            .msg         = "\003{\"connectionClose\":[{\"phase\":\"confirm\"}]}"sv,
            .value_type  = kSmeClose,
            .phase       = kConnectionClosePhaseTypeConfirm,
            .max_time    = nullptr,
            .reason      = nullptr,
        },
        SmeCloseCloseDeserializeTestInput{
            .description = "Test connection close with maxTime set"sv,
            .msg         = "\003{\"connectionClose\":[{\"phase\":\"announce\"},{\"maxTime\":10000}]}"sv,
            .value_type  = kSmeClose,
            .phase       = kConnectionClosePhaseTypeAnnounce,
            .max_time    = std::make_shared<uint32_t>(10000),
            .reason      = nullptr,
        },
        SmeCloseCloseDeserializeTestInput{
            .description = "Test connection close with reason set"sv,
            .msg         = "\003{\"connectionClose\":"
                           "[{\"phase\":\"confirm\"},"
                           "{\"reason\":\"Closed by user\"}]}"sv,
            .value_type  = kSmeClose,
            .phase       = kConnectionClosePhaseTypeConfirm,
            .max_time    = nullptr,
            .reason      = std::make_shared<std::string_view>("Closed by user"),
        },
        SmeCloseCloseDeserializeTestInput{
            .description = "Test connection close with maxTime and reason set"sv,
            .msg         = "\003{\"connectionClose\":"
                           "[{\"phase\":\"announce\"},"
                           "{\"maxTime\":20000},"
                           "{\"reason\":\"Unexpected error\"}]}"sv,
            .value_type  = kSmeClose,
            .phase       = kConnectionClosePhaseTypeAnnounce,
            .max_time    = std::make_shared<uint32_t>(20000),
            .reason      = std::make_shared<std::string_view>("Unexpected error"),
        }
    )
);
