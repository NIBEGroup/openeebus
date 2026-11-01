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
#include "src/spine/model/feature_types.h"
#include "tests/unit_tests/json.h"
#include "tests/unit_tests/spine/device/sender/sender_test_suite.h"
#include "tests/unit_tests/spine/model/feature_address_test_data.h"

using namespace std::literals;

struct SenderCallUnbindTestInput {
  std::string_view description                 = ""sv;
  ValuePtr<FeatureAddressTestData> sender_addr = nullptr;
  ValuePtr<FeatureAddressTestData> dest_addr   = nullptr;
  uint64_t msg_cnt                             = 0;
  std::string_view msg                         = ""sv;
};

std::ostream& operator<<(std::ostream& os, SenderCallUnbindTestInput test_input) {
  return os << test_input.description;
}

class SenderCallUnbindTests : public SenderTestSuite,
                              public ::testing::WithParamInterface<SenderCallUnbindTestInput> {};

TEST_P(SenderCallUnbindTests, SenderCallUnbindTests) {
  // Arrange: Initialize the sender address and destination address from test input
  std::unique_ptr<FeatureAddressType, decltype(&FeatureAddressDelete)> sender_addr{
      TestDataToFeatureAddress(GetParam().sender_addr.get()),
      FeatureAddressDelete
  };
  std::unique_ptr<FeatureAddressType, decltype(&FeatureAddressDelete)> dest_addr{
      TestDataToFeatureAddress(GetParam().dest_addr.get()),
      FeatureAddressDelete
  };

  SenderObject* sender = GetSender();
  SenderSetMsgCounter(sender, GetParam().msg_cnt);

  ExpectMessageWrite(GetParam().msg);

  // Act: Run the CallUnbind()
  const EebusError ret = SEND_CALL_UNBIND(sender, sender_addr.get(), dest_addr.get());

  // Assert: Verify with expected return value,
  // Note: output message checks are done within ExpectMessageWrite()
  EXPECT_EQ(ret, kEebusErrorOk);
}

INSTANTIATE_TEST_SUITE_P(
    SenderCallUnbindTests,
    SenderCallUnbindTests,
    ::testing::Values(
        SenderCallUnbindTestInput{
            .description = "Test Node Management Bind Delete call"sv,
            .sender_addr = FEATURE_ADDRESS_TEST_DATA("d:_i:Demo_EVSE-234567890", {0}, 0),
            .dest_addr   = FEATURE_ADDRESS_TEST_DATA("d:_i:36013_3019197057", {0}, 0),
            .msg_cnt     = 0,
            .msg         = R"({"datagram":[
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
                               {"cmdClassifier":"call"},
                               {"ackRequest":true}
                             ]},
                             {"payload":[
                               {"cmd":[
                                 [
                                   {"nodeManagementBindingDeleteCall":[
                                     {"bindingDelete":[
                                       {"clientAddress":[
                                         {"device":"d:_i:Demo_EVSE-234567890"},
                                         {"entity":[0]},
                                         {"feature":0}
                                       ]},
                                       {"serverAddress":[
                                         {"device":"d:_i:36013_3019197057"},
                                         {"entity":[0]},
                                         {"feature":0}
                                       ]}
                                     ]}
                                   ]}
                                 ]
                               ]}
                             ]}
                           ]})"sv,
        },
        SenderCallUnbindTestInput{
            .description = "Test SMA 2.0 Node Management Subscription Request call (another attempt)"sv,
            .sender_addr = FEATURE_ADDRESS_TEST_DATA("d:_i:Demo_EVSE-234567890", {0}, 0),
            .dest_addr   = FEATURE_ADDRESS_TEST_DATA("d:_i:36013_3019197057", {0}, 0),
            .msg_cnt     = 1,
            .msg         = R"({"datagram":[
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
                               {"cmdClassifier":"call"},
                               {"ackRequest":true}
                             ]},
                             {"payload":[
                               {"cmd":[
                                 [
                                   {"nodeManagementBindingDeleteCall":[
                                     {"bindingDelete":[
                                       {"clientAddress":[
                                         {"device":"d:_i:Demo_EVSE-234567890"},
                                         {"entity":[0]},
                                         {"feature":0}
                                       ]},
                                       {"serverAddress":[
                                         {"device":"d:_i:36013_3019197057"},
                                         {"entity":[0]},
                                         {"feature":0}
                                       ]}
                                     ]}
                                   ]}
                                 ]
                               ]}
                             ]}
                           ]})"sv,
        }
    )
);
