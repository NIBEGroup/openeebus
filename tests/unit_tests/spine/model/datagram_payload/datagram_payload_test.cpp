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

#include "src/spine/model/datagram.h"
#include "src/spine/model/datagram_internal.h"
#include "tests/unit_tests/json.h"
#include "tests/unit_tests/spine/model/datagram_payload/sma_destination_list_data_read.inc"
#include "tests/unit_tests/spine/model/datagram_payload/sma_destination_list_data_reply.inc"
#include "tests/unit_tests/spine/model/datagram_payload/sma_discovery_data_read.inc"
#include "tests/unit_tests/spine/model/datagram_payload/sma_discovery_data_reply.inc"
#include "tests/unit_tests/spine/model/datagram_payload/sma_result_data_result.inc"
#include "tests/unit_tests/spine/model/datagram_payload/sma_subscription_req_call.inc"
#include "tests/unit_tests/spine/model/datagram_payload/sma_use_case_data_read.inc"
#include "tests/unit_tests/spine/model/datagram_payload/sma_use_case_data_reply.inc"

using namespace std::literals;

struct PayloadTestInput {
  std::string_view description = ""sv;
  std::string_view msg         = ""sv;
};

std::ostream& operator<<(std::ostream& os, PayloadTestInput test_input) {
  return os << test_input.description;
}

class PayloadTests : public ::testing::TestWithParam<PayloadTestInput> {};

TEST_P(PayloadTests, PayloadTests) {
  // Arrange: Initialize the message buffer with parameters from test input
  std::unique_ptr<char[], decltype(&JsonFree)> s{JsonUnformat(GetParam().msg), JsonFree};
  ASSERT_NE(s, nullptr) << "Wrong test input!";

  // Act: Run the datagram parsing
  std::unique_ptr<DatagramType, decltype(&DatagramDelete)> datagram{DatagramParse(s.get()), DatagramDelete};

  // Assert: Verify with expected datagram fields,
  // then try to copy and serialize the obtained data
  ASSERT_NE(datagram, nullptr);
  ASSERT_NE(datagram->payload, nullptr);

  std::unique_ptr<DatagramType, decltype(&DatagramDelete)> datagram_copy{DatagramCopy(datagram.get()), DatagramDelete};
  datagram.reset();

  std::unique_ptr<char[], decltype(&JsonFree)> serialized{DatagramPrintUnformatted(datagram_copy.get()), JsonFree};
  EXPECT_STREQ(serialized.get(), s.get());
}

INSTANTIATE_TEST_SUITE_P(
    PayloadTests,
    PayloadTests,
    ::testing::Values(
        PayloadTestInput{
            .description = "Test SMA 2.0 Node Management Detailed Discovery Data read"sv,
            .msg         = sma_discovery_data_read,
        },
        PayloadTestInput{
            .description = "Test SMA 2.0 Node Management Detailed Discovery Data reply"sv,
            .msg         = sma_discovery_data_reply,
        },
        PayloadTestInput{
            .description = "Test SMA 2.0 Node Management Subscription Request call"sv,
            .msg         = sma_subscription_request_call,
        },
        PayloadTestInput{
            .description = "Test SMA 2.0 Node Management Use Case Data read"sv,
            .msg         = sma_use_case_data_read,
        },
        PayloadTestInput{
            .description = "Test SMA 2.0 Node Management Use Case Data reply"sv,
            .msg         = sma_use_case_data_reply,
        },
        PayloadTestInput{
            .description = "Test SMA 2.0 Result Data result"sv,
            .msg         = sma_result_data_result,
        },
        PayloadTestInput{
            .description = "Test SMA 2.0 Destination List Data read"sv,
            .msg         = sma_destination_list_data_read,
        },
        PayloadTestInput{
            .description = "Test SMA 2.0 Destination List Data reply"sv,
            .msg         = sma_destination_list_data_reply,
        }
    )
);
