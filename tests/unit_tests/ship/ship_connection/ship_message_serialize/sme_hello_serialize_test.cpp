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

TEST(ShipMessageParseTest, SmeHelloSerializeInputArgs) {
  auto serialize = ShipMessageSerializeCreate(nullptr, kValueUndefined);
  EXPECT_EQ(SHIP_MESSAGE_SERIALIZE_GET_BUFFER(serialize), nullptr);
  ShipMessageSerializeDelete(serialize);

  serialize = ShipMessageSerializeCreate(nullptr, kSmeHello);
  EXPECT_EQ(SHIP_MESSAGE_SERIALIZE_GET_BUFFER(serialize), nullptr);
  ShipMessageSerializeDelete(serialize);

  ConnectionHello sme_hello_value_0;
  serialize = ShipMessageSerializeCreate(&sme_hello_value_0, kValueUndefined);
  EXPECT_EQ(SHIP_MESSAGE_SERIALIZE_GET_BUFFER(serialize), nullptr);
  ShipMessageSerializeDelete(serialize);
}

struct SmeHelloSerializeTestInput {
  std::string_view description;
  ConnectionHelloPhase phase;
  std::shared_ptr<int> waiting;
  std::shared_ptr<bool> prolongation_request;
  std::string_view msg;
};

std::ostream& operator<<(std::ostream& os, SmeHelloSerializeTestInput Test) {
  return os << Test.description;
}

class SmeHelloSerializeTests : public ::testing::TestWithParam<SmeHelloSerializeTestInput> {};

TEST_P(SmeHelloSerializeTests, SmeHelloSerializeTests) {
  // Arrange: Initialize the SmeHelloSerialize() parameters from test input
  ConnectionHello sme_hello;

  sme_hello.phase = GetParam().phase;
  if (GetParam().waiting != nullptr) {
    sme_hello.waiting_val = *GetParam().waiting;
    sme_hello.waiting     = &sme_hello.waiting_val;
  } else {
    sme_hello.waiting = nullptr;
  }

  if (GetParam().prolongation_request != nullptr) {
    sme_hello.prolongation_request_val = *GetParam().prolongation_request;
    sme_hello.prolongation_request     = &sme_hello.prolongation_request_val;
  } else {
    sme_hello.prolongation_request = nullptr;
  }

  // Act: Run the SmeHello serialize procedure
  auto serialize = ShipMessageSerializeCreate(&sme_hello, kSmeHello);

  const MessageBuffer* const buf = SHIP_MESSAGE_SERIALIZE_GET_BUFFER(serialize);

  // Assert: Verify with expected output buffer value
  if (buf != nullptr) {
    EXPECT_THAT(buf, BufDataEq(GetParam().msg));
  }

  ShipMessageSerializeDelete(serialize);
}

INSTANTIATE_TEST_SUITE_P(
    SmeHelloSerialize,
    SmeHelloSerializeTests,
    ::testing::Values(
        SmeHelloSerializeTestInput{
            .description          = "Test phase = ready"sv,
            .phase                = kConnectionHelloPhaseReady,
            .waiting              = nullptr,
            .prolongation_request = nullptr,
            .msg                  = "\001{\"connectionHello\":[{\"phase\":\"ready\"}]}"sv,
        },
        SmeHelloSerializeTestInput{
            .description          = "Test phase = ready, waiting = 6000"sv,
            .phase                = kConnectionHelloPhaseReady,
            .waiting              = std::make_shared<int>(6000),
            .prolongation_request = nullptr,
            .msg                  = "\001{\"connectionHello\":[{\"phase\":\"ready\"},{\"waiting\":6000}]}"sv,
        },
        SmeHelloSerializeTestInput{
            .description          = "Test phase = aborted"sv,
            .phase                = kConnectionHelloPhaseAborted,
            .waiting              = nullptr,
            .prolongation_request = nullptr,
            .msg                  = "\001{\"connectionHello\":[{\"phase\":\"aborted\"}]}"sv,
        },
        SmeHelloSerializeTestInput{
            .description          = "Test phase = pending"sv,
            .phase                = kConnectionHelloPhasePending,
            .waiting              = nullptr,
            .prolongation_request = nullptr,
            .msg                  = "\001{\"connectionHello\":[{\"phase\":\"pending\"}]}"sv,
        },
        SmeHelloSerializeTestInput{
            .description          = "Test phase = pending, prolongation request = true"sv,
            .phase                = kConnectionHelloPhasePending,
            .waiting              = nullptr,
            .prolongation_request = std::make_shared<bool>(true),
            .msg                  = "\001{\"connectionHello\":"
                                    "[{\"phase\":\"pending\"},{\"prolongationRequest\":true}]}"sv,
        },
        SmeHelloSerializeTestInput{
            .description          = "Test phase = pending, prolongation request = false"sv,
            .phase                = kConnectionHelloPhasePending,
            .waiting              = nullptr,
            .prolongation_request = std::make_shared<bool>(false),
            .msg                  = "\001{\"connectionHello\":"
                                    "[{\"phase\":\"pending\"},{\"prolongationRequest\":false}]}"sv,
        }
    )
);
