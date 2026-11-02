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

struct SenderCallBindTestInput {
  std::string_view description                 = ""sv;
  ValuePtr<FeatureAddressTestData> sender_addr = nullptr;
  ValuePtr<FeatureAddressTestData> dest_addr   = nullptr;
  FeatureTypeType server_feature_type          = static_cast<FeatureTypeType>(0);
  uint64_t msg_cnt                             = 0;
  std::string_view msg                         = ""sv;
};

std::ostream& operator<<(std::ostream& os, SenderCallBindTestInput test_input) {
  return os << test_input.description;
}

class SenderCallBindTests : public SenderTestSuite, public ::testing::WithParamInterface<SenderCallBindTestInput> {};

TEST_P(SenderCallBindTests, SenderCallBindTests) {
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

  // Act: Run the CallBind()
  const EebusError ret = SEND_CALL_BIND(sender, sender_addr.get(), dest_addr.get(), GetParam().server_feature_type);

  // Assert: Verify with expected return value,
  // Note: output message checks are done within mock expectation call
  EXPECT_EQ(ret, kEebusErrorOk);
}

INSTANTIATE_TEST_SUITE_P(
    SenderCallBindTests,
    SenderCallBindTests,
    ::testing::Values(
        SenderCallBindTestInput{
            .description         = "Test Node Management Bind Request call"sv,
            .sender_addr         = FEATURE_ADDRESS_TEST_DATA("d:_i:Demo_EVSE-234567890", {0}, 0),
            .dest_addr           = FEATURE_ADDRESS_TEST_DATA("d:_i:36013_3019197057", {0}, 0),
            .server_feature_type = kFeatureTypeTypeSetpoint,
            .msg_cnt             = 0,
            .msg                 = R"({"datagram":[
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
                                           {"nodeManagementBindingRequestCall":[
                                             {"bindingRequest":[
                                               {"clientAddress":[
                                                 {"device":"d:_i:Demo_EVSE-234567890"},
                                                 {"entity":[0]},
                                                 {"feature":0}
                                               ]},
                                               {"serverAddress":[
                                                 {"device":"d:_i:36013_3019197057"},
                                                 {"entity":[0]},
                                                 {"feature":0}
                                               ]},
                                               {"serverFeatureType":"Setpoint"}
                                             ]}
                                           ]}
                                         ]
                                       ]}
                                     ]}
                                   ]})"sv,
        },
        SenderCallBindTestInput{
            .description         = "Test Node Management Bind Request call (another attempt)"sv,
            .sender_addr         = FEATURE_ADDRESS_TEST_DATA("d:_i:Demo_EVSE-234567890", {0}, 0),
            .dest_addr           = FEATURE_ADDRESS_TEST_DATA("d:_i:36013_3019197057", {0}, 0),
            .server_feature_type = kFeatureTypeTypeSetpoint,
            .msg_cnt             = 1,
            .msg                 = R"({"datagram":[
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
                                           {"nodeManagementBindingRequestCall":[
                                             {"bindingRequest":[
                                               {"clientAddress":[
                                                 {"device":"d:_i:Demo_EVSE-234567890"},
                                                 {"entity":[0]},
                                                 {"feature":0}
                                               ]},
                                               {"serverAddress":[
                                                 {"device":"d:_i:36013_3019197057"},
                                                 {"entity":[0]},
                                                 {"feature":0}
                                               ]},
                                               {"serverFeatureType":"Setpoint"}
                                             ]}
                                           ]}
                                         ]
                                       ]}
                                     ]}
                                   ]})"sv,
        }
    )
);
