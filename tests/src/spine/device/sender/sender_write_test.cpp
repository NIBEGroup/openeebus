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
#include "src/spine/model/actuator_level_types.h"
#include "src/spine/model/model.h"
#include "tests/src/json.h"
#include "tests/src/spine/device/sender/sender_test_suite.h"
#include "tests/src/spine/model/feature_address_test_data.h"

using namespace std::literals;

struct SenderWriteTestInput {
  std::string_view description                 = ""sv;
  ValuePtr<FeatureAddressTestData> sender_addr = nullptr;
  ValuePtr<FeatureAddressTestData> dest_addr   = nullptr;
  FunctionType data_type_id                    = static_cast<FunctionType>(0);
  uint64_t msg_cnt                             = 0;
  std::string_view msg                         = ""sv;
};

std::ostream& operator<<(std::ostream& os, SenderWriteTestInput test_input) {
  return os << test_input.description;
}

class SenderWriteTests : public SenderTestSuite, public ::testing::WithParamInterface<SenderWriteTestInput> {};

TEST_P(SenderWriteTests, SenderWriteTests) {
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

  // Act: Run the Write()
  const EebusError ret = SEND_WRITE(sender, sender_addr.get(), dest_addr.get(), &cmd, NULL);

  // Assert: Verify with expected return value,
  // Note: output message checks are done within mock expectation call
  EXPECT_EQ(ret, kEebusErrorOk);
}

TEST_F(SenderWriteTests, WriteFillsOutMsgCnt) {
  std::unique_ptr<FeatureAddressType, decltype(&FeatureAddressDelete)> sender_addr{
      TestDataToFeatureAddress(FEATURE_ADDRESS_TEST_DATA("d:_i:Demo_EVSE-234567890", {5}, 5).get()),
      FeatureAddressDelete
  };
  std::unique_ptr<FeatureAddressType, decltype(&FeatureAddressDelete)> dest_addr{
      TestDataToFeatureAddress(FEATURE_ADDRESS_TEST_DATA("d:_i:36013_3019197057", {10}, 15).get()),
      FeatureAddressDelete
  };

  std::unique_ptr<void, std::function<void(void*)>> spine_data{
      ModelFunctionDataCreateEmpty(kFunctionTypeActuatorLevelData),
      [](void* p) -> void { ModelFunctionDataDelete(kFunctionTypeActuatorLevelData, p); }
  };

  ASSERT_NE(spine_data, nullptr);

  CmdType cmd = {
      .data_choice         = spine_data.get(),
      .data_choice_type_id = kFunctionTypeActuatorLevelData,
  };

  SenderObject* sender = GetSender();
  SenderSetMsgCounter(sender, 4);

  ExpectMessageWrite(R"({"datagram":[
    {"header":[
      {"specificationVersion":"1.3.0"},
      {"addressSource":[
        {"device":"d:_i:Demo_EVSE-234567890"},
        {"entity":[5]},
        {"feature":5}
      ]},
      {"addressDestination":[
        {"device":"d:_i:36013_3019197057"},
        {"entity":[10]},
        {"feature":15}
      ]},
      {"msgCounter":5},
      {"cmdClassifier":"write"},
      {"ackRequest":true}
    ]},
    {"payload":[
      {"cmd":[
        [{"actuatorLevelData":[]}]
      ]}
    ]}
  ]})"sv);

  uint64_t msg_cnt     = 0;
  const EebusError ret = SEND_WRITE(sender, sender_addr.get(), dest_addr.get(), &cmd, &msg_cnt);

  EXPECT_EQ(ret, kEebusErrorOk);
  EXPECT_EQ(msg_cnt, 5u);
}

INSTANTIATE_TEST_SUITE_P(
    SenderWriteTests,
    SenderWriteTests,
    ::testing::Values(
        SenderWriteTestInput{
            .description  = "Test Actuator Level Data write (skipped cmd data)"sv,
            .sender_addr  = FEATURE_ADDRESS_TEST_DATA("d:_i:Demo_EVSE-234567890", {5}, 5),
            .dest_addr    = FEATURE_ADDRESS_TEST_DATA("d:_i:36013_3019197057", {10}, 15),
            .data_type_id = kFunctionTypeActuatorLevelData,
            .msg_cnt      = 0,
            .msg          = R"({"datagram":[
                              {"header":[
                                {"specificationVersion":"1.3.0"},
                                {"addressSource":[
                                  {"device":"d:_i:Demo_EVSE-234567890"},
                                  {"entity":[5]},
                                  {"feature":5}
                                ]},
                                {"addressDestination":[
                                  {"device":"d:_i:36013_3019197057"},
                                  {"entity":[10]},
                                  {"feature":15}
                                ]},
                                {"msgCounter":1},
                                {"cmdClassifier":"write"},
                                {"ackRequest":true}
                              ]},
                              {"payload":[
                                {"cmd":[
                                  [{"actuatorLevelData":[]}]
                                ]}
                              ]}
                            ]})"sv,
        },
        SenderWriteTestInput{
            .description  = "Test Actuator Level Data write (skipped cmd data, another attempt)"sv,
            .sender_addr  = FEATURE_ADDRESS_TEST_DATA("d:_i:Demo_EVSE-234567890", {20}, 25),
            .dest_addr    = FEATURE_ADDRESS_TEST_DATA("d:_i:36013_3019197057", {31}, 71),
            .data_type_id = kFunctionTypeActuatorLevelDescriptionData,
            .msg_cnt      = 1,
            .msg          = R"({"datagram":[
                              {"header":[
                                {"specificationVersion":"1.3.0"},
                                {"addressSource":[
                                  {"device":"d:_i:Demo_EVSE-234567890"},
                                  {"entity":[20]},
                                  {"feature":25}
                                ]},
                                {"addressDestination":[
                                  {"device":"d:_i:36013_3019197057"},
                                  {"entity":[31]},
                                  {"feature":71}
                                ]},
                                {"msgCounter":2},
                                {"cmdClassifier":"write"},
                                {"ackRequest":true}
                              ]},
                              {"payload":[
                                {"cmd":[
                                  [{"actuatorLevelDescriptionData":[]}]
                                ]}
                              ]}
                            ]})"sv,
        }
    )
);
