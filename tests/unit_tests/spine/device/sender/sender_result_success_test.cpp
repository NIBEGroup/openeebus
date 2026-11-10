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

#include "src/spine/device/sender.h"
#include "src/spine/device/sender_internal.h"
#include "src/spine/model/datagram.h"
#include "src/spine/model/node_management_types.h"
#include "tests/unit_tests/json.h"
#include "tests/unit_tests/spine/device/sender/sender_test_suite.h"
#include "tests/unit_tests/spine/model/feature_address_test_data.h"

using namespace std::literals;

struct SenderResultSuccessTestInput {
  std::string_view description                             = ""sv;
  ValuePtr<FeatureAddressTestData> request_header_src_addr = nullptr;
  ValuePtr<FeatureAddressTestData> request_header_dst_addr = nullptr;
  ValuePtr<uint64_t> request_header_msg_cnt                = 0;
  ValuePtr<FeatureAddressTestData> sender_addr             = nullptr;
  uint64_t msg_cnt                                         = 0;
  std::string_view msg                                     = ""sv;
};

std::ostream& operator<<(std::ostream& os, SenderResultSuccessTestInput test_input) {
  return os << test_input.description;
}

class SenderResultSuccessTests : public SenderTestSuite,
                                 public ::testing::WithParamInterface<SenderResultSuccessTestInput> {};

TEST_P(SenderResultSuccessTests, SenderResultSuccessTests) {
  // Arrange: Initialize the sender address and request header from test input
  std::unique_ptr<FeatureAddressType, decltype(&FeatureAddressDelete)> header_src_addr{
      TestDataToFeatureAddress(GetParam().request_header_src_addr.get()),
      FeatureAddressDelete
  };
  std::unique_ptr<FeatureAddressType, decltype(&FeatureAddressDelete)> header_dst_addr{
      TestDataToFeatureAddress(GetParam().request_header_dst_addr.get()),
      FeatureAddressDelete
  };
  std::unique_ptr<FeatureAddressType, decltype(&FeatureAddressDelete)> sender_addr{
      TestDataToFeatureAddress(GetParam().sender_addr.get()),
      FeatureAddressDelete
  };

  HeaderType header = {
      .src_addr  = header_src_addr.get(),
      .dest_addr = header_dst_addr.get(),
      .msg_cnt   = const_cast<uint64_t*>(GetParam().request_header_msg_cnt.get()),
  };

  SenderObject* sender = GetSender();
  SenderSetMsgCounter(sender, GetParam().msg_cnt);

  ExpectMessageWrite(GetParam().msg);

  // Act: Run the ResultSuccess()
  const EebusError ret = SEND_RESULT_SUCCESS(sender, &header, sender_addr.get());

  // Assert: Verify with expected return value,
  // Note: output message checks are done within mock expectation call
  EXPECT_EQ(ret, kEebusErrorOk);
}

INSTANTIATE_TEST_SUITE_P(
    SenderResultSuccessTests,
    SenderResultSuccessTests,
    ::testing::Values(
        SenderResultSuccessTestInput{
            .description             = "Test SMA 2.0 Node Management result success"sv,
            .request_header_src_addr = FEATURE_ADDRESS_TEST_DATA("d:_i:36013_3019197057", {0}, 0),
            .request_header_dst_addr = FEATURE_ADDRESS_TEST_DATA("d:_i:Demo_EVSE-234567890", {0}, 0),
            .request_header_msg_cnt  = ValuePtrCreate<uint64_t>(5),
            .sender_addr             = FEATURE_ADDRESS_TEST_DATA("d:_i:Demo_EVSE-234567890", {0}, 0),
            .msg_cnt                 = 0,
            .msg                     = R"({"datagram":[
                                         {"header":[
                                           {"specificationVersion":"1.3.0"},
                                           {"addressSource":[
                                             {"device":"d:_i:Demo_EVSE-234567890"},
                                             {"entity":[0]},
                                             {"feature":0}
                                           ]},
                                           {"addressDestination":[
                                             {"device":"d:_i:36013_3019197057"},
                                             {"entity":[0]},
                                             {"feature":0}
                                           ]},
                                           {"msgCounter":1},
                                           {"msgCounterReference":5},
                                           {"cmdClassifier":"result"}
                                         ]},
                                         {"payload":[
                                           {"cmd":[
                                             [{"resultData":[{"errorNumber":0}]}]
                                           ]}
                                         ]}
                                       ]})"sv,
        },
        SenderResultSuccessTestInput{
            .description             = "Test SMA 2.0 Node Management result success (another attempt)"sv,
            .request_header_src_addr = FEATURE_ADDRESS_TEST_DATA("d:_i:36013_3019197057", {0}, 0),
            .request_header_dst_addr = FEATURE_ADDRESS_TEST_DATA("d:_i:Demo_EVSE-234567890", {0}, 0),
            .request_header_msg_cnt  = ValuePtrCreate<uint64_t>(7),
            .sender_addr             = FEATURE_ADDRESS_TEST_DATA("d:_i:Demo_EVSE-234567890", {0}, 0),
            .msg_cnt                 = 1,
            .msg                     = R"({"datagram":[
                                         {"header":[
                                           {"specificationVersion":"1.3.0"},
                                           {"addressSource":[
                                             {"device":"d:_i:Demo_EVSE-234567890"},
                                             {"entity":[0]},
                                             {"feature":0}
                                           ]},
                                           {"addressDestination":[
                                             {"device":"d:_i:36013_3019197057"},
                                             {"entity":[0]},
                                             {"feature":0}
                                           ]},
                                           {"msgCounter":2},
                                           {"msgCounterReference":7},
                                           {"cmdClassifier":"result"}
                                         ]},
                                         {"payload":[
                                           {"cmd":[
                                             [{"resultData":[{"errorNumber":0}]}]
                                           ]}
                                         ]}
                                       ]})"sv,
        }
    )
);
