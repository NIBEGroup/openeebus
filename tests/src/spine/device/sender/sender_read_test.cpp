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
#include "src/spine/model/model.h"
#include "src/spine/model/node_management_types.h"
#include "tests/src/json.h"
#include "tests/src/spine/device/sender/sender_test_suite.h"
#include "tests/src/spine/model/feature_address_test_data.h"

using namespace std::literals;

struct SenderReadTestInput {
  std::string_view description                 = ""sv;
  ValuePtr<FeatureAddressTestData> sender_addr = nullptr;
  ValuePtr<FeatureAddressTestData> dest_addr   = nullptr;
  FunctionType data_type_id                    = static_cast<FunctionType>(0);
  uint64_t msg_cnt                             = 0;
  std::string_view msg                         = ""sv;
};

std::ostream& operator<<(std::ostream& os, SenderReadTestInput test_input) {
  return os << test_input.description;
}

class SenderReadTests : public SenderTestSuite, public ::testing::WithParamInterface<SenderReadTestInput> {};

TEST_P(SenderReadTests, SenderReadTests) {
  // Arrange: Initialize the sender address, destination address
  // and command with parameters from test input
  std::unique_ptr<FeatureAddressType, decltype(&FeatureAddressDelete)> sender_addr{
      TestDataToFeatureAddress(GetParam().sender_addr.get()),
      FeatureAddressDelete
  };
  std::unique_ptr<FeatureAddressType, decltype(&FeatureAddressDelete)> dest_addr{
      TestDataToFeatureAddress(GetParam().dest_addr.get()),
      FeatureAddressDelete
  };

  std::unique_ptr<void, std::function<void(void*)>> spine_data{
      ModelFunctionDataCreateEmpty(GetParam().data_type_id),
      [](void* p) -> void { ModelFunctionDataDelete(GetParam().data_type_id, p); }
  };

  ASSERT_NE(spine_data, nullptr);

  CmdType cmd = {
      .data_choice         = spine_data.get(),
      .data_choice_type_id = GetParam().data_type_id,
  };

  SenderObject* sender = GetSender();
  SenderSetMsgCounter(sender, GetParam().msg_cnt);

  ExpectMessageWrite(GetParam().msg);

  // Act: Run the Read()
  const EebusError ret = SEND_READ(sender, sender_addr.get(), dest_addr.get(), &cmd, nullptr);

  // Assert: Verify with expected return value,
  // Note: output message checks are done within mock expectation call
  EXPECT_EQ(ret, kEebusErrorOk);
}

INSTANTIATE_TEST_SUITE_P(
    SenderReadTests,
    SenderReadTests,
    ::testing::Values(
        SenderReadTestInput{
            .description  = "Test SMA 2.0 Node Management Detailed Discovery Data read"sv,
            .sender_addr  = FEATURE_ADDRESS_TEST_DATA("d:_i:Demo_EVSE-234567890", {0}, 0),
            .dest_addr    = FEATURE_ADDRESS_TEST_DATA(nullptr, {0}, 0),
            .data_type_id = kFunctionTypeNodeManagementDetailedDiscoveryData,
            .msg_cnt      = 0,
            .msg          = R"({"datagram":[
                              {"header":[
                                {"specificationVersion":"1.3.0"},
                                {"addressSource":[
                                  {"device":"d:_i:Demo_EVSE-234567890"},
                                  {"entity":[0]},
                                  {"feature":0}
                                ]},
                                {"addressDestination":[
                                  {"entity":[0]},
                                  {"feature":0}
                                ]},
                                {"msgCounter":1},
                                {"cmdClassifier":"read"}
                              ]},
                              {"payload":[
                                {"cmd":[
                                  [{"nodeManagementDetailedDiscoveryData":[]}]
                                ]}
                              ]}
                            ]})"sv,
        },
        SenderReadTestInput{
            .description  = "Test SMA 2.0 Node Management Use Case Data read"sv,
            .sender_addr  = FEATURE_ADDRESS_TEST_DATA("d:_i:Demo_EVSE-234567890", {0}, 0),
            .dest_addr    = FEATURE_ADDRESS_TEST_DATA(nullptr, {0}, 0),
            .data_type_id = kFunctionTypeNodeManagementUseCaseData,
            .msg_cnt      = 1,
            .msg          = R"({"datagram":[
                              {"header":[
                                {"specificationVersion":"1.3.0"},
                                {"addressSource":[
                                  {"device":"d:_i:Demo_EVSE-234567890"},
                                  {"entity":[0]},
                                  {"feature":0}
                                ]},
                                {"addressDestination":[
                                  {"entity":[0]},
                                  {"feature":0}
                                ]},
                                {"msgCounter":2},
                                {"cmdClassifier":"read"}
                              ]},
                              {"payload":[
                                {"cmd":[
                                  [{"nodeManagementUseCaseData":[]}]
                                ]}
                              ]}
                            ]})"sv,
        },
        SenderReadTestInput{
            .description  = "Test SMA 2.0 Node Management Detailed Discovery Data read (another attempt)"sv,
            .sender_addr  = FEATURE_ADDRESS_TEST_DATA("d:_i:Demo_EVSE-234567890", {0}, 0),
            .dest_addr    = FEATURE_ADDRESS_TEST_DATA(nullptr, {0}, 0),
            .data_type_id = kFunctionTypeNodeManagementDetailedDiscoveryData,
            .msg_cnt      = 2,
            .msg          = R"({"datagram":[
                              {"header":[
                                {"specificationVersion":"1.3.0"},
                                {"addressSource":[
                                  {"device":"d:_i:Demo_EVSE-234567890"},
                                  {"entity":[0]},
                                  {"feature":0}
                                ]},
                                {"addressDestination":[
                                  {"entity":[0]},
                                  {"feature":0}
                                ]},
                                {"msgCounter":3},
                                {"cmdClassifier":"read"}
                              ]},
                              {"payload":[
                                {"cmd":[
                                  [{"nodeManagementDetailedDiscoveryData":[]}]
                                ]}
                              ]}
                            ]})"sv,
        }
    )
);
