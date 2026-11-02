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

MATCHER_P3(SmeHelloEq, phase, waiting, prolongation_request, "") {
  *result_listener << "phase = " << arg->phase;
  *result_listener << ", waiting = ";
  if (arg->waiting != nullptr) {
    *result_listener << *(arg->waiting);
  } else {
    *result_listener << nullptr;
  }

  *result_listener << ", prolongation_request = ";
  if (arg->prolongation_request != nullptr) {
    *result_listener << *(arg->prolongation_request);
  } else {
    *result_listener << nullptr;
  }

  bool match = (arg->phase == phase);
  if (arg->waiting == nullptr) {
    match = match && (waiting == nullptr);
  } else {
    match = match && (waiting != nullptr);
    match = match && (*arg->waiting == static_cast<uint32_t>(*waiting));
  }

  if (arg->prolongation_request == nullptr) {
    match = match && (prolongation_request == nullptr);
  } else {
    match = match && (prolongation_request != nullptr);
    match = match && (*arg->prolongation_request == *prolongation_request);
  }

  return match;
}

struct SmeHelloDeserializeTestInput {
  std::string_view description;
  std::string_view msg;
  MsgValueType value_type;
  ConnectionHelloPhase phase;
  std::shared_ptr<int> waiting;
  std::shared_ptr<bool> prolongation_request;
};

std::ostream& operator<<(std::ostream& os, SmeHelloDeserializeTestInput test_input) {
  return os << test_input.description;
}

class SmeHelloDeserializeTests : public ::testing::TestWithParam<SmeHelloDeserializeTestInput> {};

TEST(ShipMessageParseTest, SmeHelloDeserializeInputArgs) {
  auto deserialize = ShipMessageDeserializeCreate(nullptr);

  void* const value = SHIP_MESSAGE_DESERIALIZE_GET_VALUE(deserialize);
  auto sme_hello    = reinterpret_cast<ConnectionHello*>(value);

  const MsgValueType value_type = SHIP_MESSAGE_DESERIALIZE_GET_VALUE_TYPE(deserialize);

  EXPECT_EQ(sme_hello, nullptr);
  EXPECT_EQ(value_type, kValueUndefined);

  ShipMessageDeserializeDelete(deserialize);
}

TEST_P(SmeHelloDeserializeTests, SmeHelloDeserializeTests) {
  // Arrange: Initialize the SmeHelloDeserialize() parameters from test input
  MessageBuffer buf;
  MessageBufferInitWithStringView(&buf, GetParam().msg);

  // Act: Run the SmeHelloDeserialize()
  auto deserialize = ShipMessageDeserializeCreate(&buf);

  void* const value = SHIP_MESSAGE_DESERIALIZE_GET_VALUE(deserialize);
  auto sme_hello    = reinterpret_cast<ConnectionHello*>(value);

  const MsgValueType value_type = SHIP_MESSAGE_DESERIALIZE_GET_VALUE_TYPE(deserialize);

  // Assert: Verify with expected return value and output message value
  EXPECT_EQ(value_type, GetParam().value_type);
  if ((value_type == kSmeHello) && (sme_hello != nullptr)) {
    EXPECT_THAT(sme_hello, SmeHelloEq(GetParam().phase, GetParam().waiting, GetParam().prolongation_request));
  } else {
    EXPECT_EQ(sme_hello, nullptr);
  }

  MessageBufferRelease(&buf);
  ShipMessageDeserializeDelete(deserialize);
}

INSTANTIATE_TEST_SUITE_P(
    SmeHelloDeserialize,
    SmeHelloDeserializeTests,
    ::testing::Values(
        SmeHelloDeserializeTestInput{
            .description          = "Test missing phase, waiting = 6000"sv,
            .msg                  = "\001{\"connectionHello\":[{\"waiting\":6000}]}"sv,
            .value_type           = kValueUndefined,
            .phase                = kConnectionHelloPhaseReady,
            .waiting              = std::make_shared<int>(6000),
            .prolongation_request = nullptr,
        },
        SmeHelloDeserializeTestInput{
            .description          = "Test phase = ready"sv,
            .msg                  = "\001{\"connectionHello\":[{\"phase\":\"ready\"}]}"sv,
            .value_type           = kSmeHello,
            .phase                = kConnectionHelloPhaseReady,
            .waiting              = nullptr,
            .prolongation_request = nullptr,
        },
        SmeHelloDeserializeTestInput{
            .description          = "Test phase = ready, waiting = 6000"sv,
            .msg                  = "\001{\"connectionHello\":[{\"phase\":\"ready\"},{\"waiting\":6000}]}"sv,
            .value_type           = kSmeHello,
            .phase                = kConnectionHelloPhaseReady,
            .waiting              = std::make_shared<int>(6000),
            .prolongation_request = nullptr,
        },
        SmeHelloDeserializeTestInput{
            .description          = "Test phase = aborted"sv,
            .msg                  = "\001{\"connectionHello\":[{\"phase\":\"aborted\"}]}"sv,
            .value_type           = kSmeHello,
            .phase                = kConnectionHelloPhaseAborted,
            .waiting              = nullptr,
            .prolongation_request = nullptr,
        },
        SmeHelloDeserializeTestInput{
            .description          = "Test phase = pending"sv,
            .msg                  = "\001{\"connectionHello\":[{\"phase\":\"pending\"}]}"sv,
            .value_type           = kSmeHello,
            .phase                = kConnectionHelloPhasePending,
            .waiting              = nullptr,
            .prolongation_request = nullptr,
        },
        SmeHelloDeserializeTestInput{
            .description          = "Test phase = pending, prolongation request = true"sv,
            .msg                  = "\001{\"connectionHello\":"
                                    "[{\"phase\":\"pending\"},{\"prolongationRequest\":true}]}"sv,
            .value_type           = kSmeHello,
            .phase                = kConnectionHelloPhasePending,
            .waiting              = nullptr,
            .prolongation_request = std::make_shared<bool>(true),
        },
        SmeHelloDeserializeTestInput{
            .description          = "Test phase = pending, prolongation request = false"sv,
            .msg                  = "\001{\"connectionHello\":"
                                    "[{\"phase\":\"pending\"},{\"prolongationRequest\":false}]}"sv,
            .value_type           = kSmeHello,
            .phase                = kConnectionHelloPhasePending,
            .waiting              = nullptr,
            .prolongation_request = std::make_shared<bool>(false),
        }
    )
);
