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
#include "src/spine/model/datagram.h"
#include "src/spine/model/datagram_internal.h"
#include "tests/unit_tests/json.h"
#include "tests/unit_tests/spine/model/feature_address_test_data.h"
#include "tests/unit_tests/string_ptr.h"
#include "tests/unit_tests/value_ptr.h"

using namespace std::literals;

struct DatagramDeserializeTestInput {
  std::string_view description                     = ""sv;
  std::string_view msg                             = ""sv;
  bool is_valid                                    = false;
  StringPtr spec_version                           = nullptr;
  ValuePtr<FeatureAddressTestData> src_addr        = nullptr;
  ValuePtr<FeatureAddressTestData> dest_addr       = nullptr;
  ValuePtr<FeatureAddressTestData> originator_addr = nullptr;
  ValuePtr<uint64_t> msg_cnt                       = nullptr;
  ValuePtr<uint64_t> msg_cnt_ref                   = nullptr;
  ValuePtr<CommandClassifierType> cmd_classifier   = nullptr;
  ValuePtr<bool> ack_request                       = nullptr;
  StringPtr timestamp                              = nullptr;
};

std::ostream& operator<<(std::ostream& os, DatagramDeserializeTestInput test_input) {
  return os << test_input.description;
}

class DatagramDeserializeTests : public ::testing::TestWithParam<DatagramDeserializeTestInput> {};

TEST(DatagramDeserializeTests, DatagramDeserializeInputArgs) {
  std::unique_ptr<DatagramType, decltype(&DatagramDelete)> const datagram{DatagramParse(NULL), DatagramDelete};

  EXPECT_EQ(datagram, nullptr);
}

TEST_P(DatagramDeserializeTests, DatagramDeserializeTests) {
  // Arrange: Initialize the message buffer with parameters from test input
  std::unique_ptr<char[], decltype(&JsonFree)> s{JsonUnformat(GetParam().msg), JsonFree};
  ASSERT_NE(s, nullptr) << "Wrong test input!";

  // Act: Run the datagram parsing
  std::unique_ptr<DatagramType, decltype(&DatagramDelete)> datagram{DatagramParse(s.get()), DatagramDelete};

  // Assert: Verify with expected datagram fields,
  // then try to copy and serialize the obtained data
  ASSERT_NE(datagram, nullptr);
  ASSERT_NE(datagram->header, nullptr);

  EXPECT_EQ(GetParam().is_valid, DatagramHeaderIsValid(datagram->header));
  EXPECT_EQ(GetParam().spec_version, StringPtr(datagram->header->spec_version));
  EXPECT_EQ(GetParam().src_addr, datagram->header->src_addr);
  EXPECT_EQ(GetParam().dest_addr, datagram->header->dest_addr);
  EXPECT_EQ(GetParam().originator_addr, datagram->header->originator_addr);
  EXPECT_EQ(GetParam().msg_cnt, ValuePtr<uint64_t>(datagram->header->msg_cnt));
  EXPECT_EQ(GetParam().msg_cnt_ref, ValuePtr<uint64_t>(datagram->header->msg_cnt_ref));
  auto cmd_classifier = reinterpret_cast<const CommandClassifierType*>(datagram->header->cmd_classifier);
  EXPECT_EQ(GetParam().cmd_classifier, ValuePtr<CommandClassifierType>(cmd_classifier));
  EXPECT_EQ(GetParam().ack_request, ValuePtr<bool>(datagram->header->ack_request));

  std::unique_ptr<DatagramType, decltype(&DatagramDelete)> datagram_copy{DatagramCopy(datagram.get()), DatagramDelete};
  datagram.reset();

  std::unique_ptr<char[], decltype(&JsonFree)> serialized{DatagramPrintUnformatted(datagram_copy.get()), JsonFree};
  EXPECT_STREQ(serialized.get(), s.get());
}

INSTANTIATE_TEST_SUITE_P(
    DatagramDeserializeTests,
    DatagramDeserializeTests,
    ::testing::Values(
        DatagramDeserializeTestInput{
            .description = "Test datagram header no specification version"sv,
            .msg =
                R"({"datagram": [
                  {"header": [
                    {"addressSource": [{"entity": [0]}, {"feature": 0}]},
                    {"addressDestination": [{"entity": [0]}, {"feature": 0}]},
                    {"msgCounter": 1},
                    {"cmdClassifier": "reply"}
                  ]},
                  {"payload": []}
                ]})"sv,
            .is_valid       = false,
            .src_addr       = FEATURE_ADDRESS_TEST_DATA(nullptr, {0}, 0),
            .dest_addr      = FEATURE_ADDRESS_TEST_DATA(nullptr, {0}, 0),
            .msg_cnt        = ValuePtrCreate<uint64_t>(1),
            .cmd_classifier = ValuePtrCreate<CommandClassifierType>(kCommandClassifierTypeReply),
        },
        DatagramDeserializeTestInput{
            .description = "Test datagram header no address source"sv,
            .msg =
                R"({"datagram": [
                  {"header": [
                    {"specificationVersion": "1.1.1"},
                    {"addressDestination": [{"entity": [0]}, {"feature": 0}]},
                    {"msgCounter": 2},
                    {"cmdClassifier": "reply"}
                  ]},
                  {"payload": []}
                ]})"sv,
            .is_valid       = false,
            .spec_version   = "1.1.1",
            .dest_addr      = FEATURE_ADDRESS_TEST_DATA(nullptr, {0}, 0),
            .msg_cnt        = ValuePtrCreate<uint64_t>(2),
            .cmd_classifier = ValuePtrCreate<CommandClassifierType>(kCommandClassifierTypeReply),
        },
        DatagramDeserializeTestInput{
            .description = "Test datagram header no address destination"sv,
            .msg =
                R"({"datagram": [
                  {"header": [
                    {"specificationVersion": "1.1.1"},
                    {"addressSource": [{"entity": [0]}, {"feature": 0}]},
                    {"msgCounter": 3},
                    {"cmdClassifier": "reply"}
                  ]},
                  {"payload": []}
                ]})"sv,
            .is_valid       = false,
            .spec_version   = "1.1.1",
            .src_addr       = FEATURE_ADDRESS_TEST_DATA(nullptr, {0}, 0),
            .msg_cnt        = ValuePtrCreate<uint64_t>(3),
            .cmd_classifier = ValuePtrCreate<CommandClassifierType>(kCommandClassifierTypeReply),
        },
        DatagramDeserializeTestInput{
            .description = "Test datagram header no message counter"sv,
            .msg =
                R"({"datagram": [
                  {"header": [
                    {"specificationVersion": "1.1.1"},
                    {"addressSource": [{"entity": [0]}, {"feature": 0}]},
                    {"addressDestination": [{"entity": [0]}, {"feature": 0}]},
                    {"cmdClassifier": "reply"}
                  ]},
                  {"payload": []}
                ]})"sv,
            .is_valid       = false,
            .spec_version   = "1.1.1",
            .src_addr       = FEATURE_ADDRESS_TEST_DATA(nullptr, {0}, 0),
            .dest_addr      = FEATURE_ADDRESS_TEST_DATA(nullptr, {0}, 0),
            .cmd_classifier = ValuePtrCreate<CommandClassifierType>(kCommandClassifierTypeReply),
        },
        DatagramDeserializeTestInput{
            .description = "Test datagram header no entity in address destination"sv,
            .msg =
                R"({"datagram": [
                  {"header": [
                    {"specificationVersion": "1.1.1"},
                    {"addressSource": [{"entity": [0]}, {"feature": 0}]},
                    {"addressDestination": [{"feature": 0}]},
                    {"msgCounter": 4},
                    {"cmdClassifier": "reply"}
                  ]},
                  {"payload": []}
                ]})"sv,
            .is_valid       = false,
            .spec_version   = "1.1.1",
            .src_addr       = FEATURE_ADDRESS_TEST_DATA(nullptr, {0}, 0),
            .dest_addr      = FEATURE_ADDRESS_TEST_DATA(nullptr, {}, 0),
            .msg_cnt        = ValuePtrCreate<uint64_t>(4),
            .cmd_classifier = ValuePtrCreate<CommandClassifierType>(kCommandClassifierTypeReply),
        },
        DatagramDeserializeTestInput{
            .description = "Test datagram header no feature in address destination"sv,
            .msg =
                R"({"datagram": [
                  {"header": [
                    {"specificationVersion": "1.1.1"},
                    {"addressSource": [{"entity": [0]}, {"feature": 0}]},
                    {"addressDestination": [{"entity": [0]}]},
                    {"msgCounter": 5},
                    {"cmdClassifier": "reply"}
                  ]},
                  {"payload": []}
                ]})"sv,
            .is_valid       = false,
            .spec_version   = "1.1.1",
            .src_addr       = FEATURE_ADDRESS_TEST_DATA(nullptr, {0}, 0),
            .dest_addr      = ValuePtrCreate<FeatureAddressTestData>(nullptr, std::vector<uint32_t>({0}), nullptr),
            .msg_cnt        = ValuePtrCreate<uint64_t>(5),
            .cmd_classifier = ValuePtrCreate<CommandClassifierType>(kCommandClassifierTypeReply),
        },
        DatagramDeserializeTestInput{
            .description = "Test datagram header without classifier"sv,
            .msg =
                R"({"datagram": [
                  {"header": [
                    {"specificationVersion": "1.1.1"},
                    {"addressSource": [{"entity": [0]}, {"feature": 0}]},
                    {"addressDestination": [{"entity": [0]}, {"feature": 0}]},
                    {"msgCounter": 6}
                  ]},
                  {"payload": []}
                ]})"sv,
            .is_valid     = false,
            .spec_version = "1.1.1",
            .src_addr     = FEATURE_ADDRESS_TEST_DATA(nullptr, {0}, 0),
            .dest_addr    = FEATURE_ADDRESS_TEST_DATA(nullptr, {0}, 0),
            .msg_cnt      = ValuePtrCreate<uint64_t>(6),
        },
        DatagramDeserializeTestInput{
            .description = "Test datagram header no device in ordinator address"sv,
            .msg =
                R"({"datagram": [
                  {"header": [
                    {"specificationVersion": "1.1.1"},
                    {"addressSource": [{"entity": [0]}, {"feature": 0}]},
                    {"addressDestination": [{"entity": [0]}, {"feature": 0}]},
                    {"addressOriginator": [{"entity": [0]}, {"feature": 0}]},
                    {"msgCounter": 7},
                    {"cmdClassifier": "reply"}
                  ]},
                  {"payload": []}
                ]})"sv,
            .is_valid        = false,
            .spec_version    = "1.1.1",
            .src_addr        = FEATURE_ADDRESS_TEST_DATA(nullptr, {0}, 0),
            .dest_addr       = FEATURE_ADDRESS_TEST_DATA(nullptr, {0}, 0),
            .originator_addr = FEATURE_ADDRESS_TEST_DATA(nullptr, {0}, 0),
            .msg_cnt         = ValuePtrCreate<uint64_t>(7),
            .cmd_classifier  = ValuePtrCreate<CommandClassifierType>(kCommandClassifierTypeReply),
        },
        DatagramDeserializeTestInput{
            .description = "Test datagram header minimal valid setup with cmd classifier = result"sv,
            .msg =
                R"({"datagram": [
                  {"header": [
                    {"specificationVersion": "1.1.1"},
                    {"addressSource": [{"entity": [8]}, {"feature": 7}]},
                    {"addressDestination": [{"entity": [1]}, {"feature": 2}]},
                    {"msgCounter": 8},
                    {"cmdClassifier": "result"}
                  ]},
                  {"payload": []}
                ]})"sv,
            .is_valid       = true,
            .spec_version   = "1.1.1",
            .src_addr       = FEATURE_ADDRESS_TEST_DATA(nullptr, {8}, 7),
            .dest_addr      = FEATURE_ADDRESS_TEST_DATA(nullptr, {1}, 2),
            .msg_cnt        = ValuePtrCreate<uint64_t>(8),
            .cmd_classifier = ValuePtrCreate<CommandClassifierType>(kCommandClassifierTypeResult),
        },
        DatagramDeserializeTestInput{
            .description = "Test datagram header minimal valid setup with ack request = true"sv,
            .msg =
                R"({"datagram": [
                  {"header": [
                    {"specificationVersion": "1.7.8"},
                    {"addressSource": [{"entity": [9]}, {"feature": 7}]},
                    {"addressDestination": [{"entity": [4]}, {"feature": 33}]},
                    {"msgCounter": 9},
                    {"cmdClassifier": "notify"},
                    {"ackRequest": true}
                  ]},
                  {"payload": []}
                ]})"sv,
            .is_valid       = true,
            .spec_version   = "1.7.8",
            .src_addr       = FEATURE_ADDRESS_TEST_DATA(nullptr, {9}, 7),
            .dest_addr      = FEATURE_ADDRESS_TEST_DATA(nullptr, {4}, 33),
            .msg_cnt        = ValuePtrCreate<uint64_t>(9),
            .cmd_classifier = ValuePtrCreate<CommandClassifierType>(kCommandClassifierTypeNotify),
            .ack_request    = ValuePtrCreate<bool>(true),
        },
        DatagramDeserializeTestInput{
            .description = "Test datagram header minimal valid setup with ack request = false"sv,
            .msg =
                R"({"datagram": [
                  {"header": [
                    {"specificationVersion": "1.1.1"},
                    {"addressSource": [{"entity": [14]}, {"feature": 0}]},
                    {"addressDestination": [{"device": "d:_i:EVSE-123"}, {"entity": [3]}, {"feature": 1}]},
                    {"msgCounter": 9},
                    {"cmdClassifier": "read"},
                    {"ackRequest": false}
                  ]},
                  {"payload": []}
                ]})"sv,
            .is_valid       = true,
            .spec_version   = "1.1.1",
            .src_addr       = FEATURE_ADDRESS_TEST_DATA(nullptr, {14}, 0),
            .dest_addr      = FEATURE_ADDRESS_TEST_DATA("d:_i:EVSE-123", {3}, 1),
            .msg_cnt        = ValuePtrCreate<uint64_t>(9),
            .cmd_classifier = ValuePtrCreate<CommandClassifierType>(kCommandClassifierTypeRead),
            .ack_request    = ValuePtrCreate<bool>(false),
        },
        DatagramDeserializeTestInput{
            .description = "Test datagram header ordinator address"sv,
            .msg =
                R"({"datagram": [
                  {"header": [
                    {"specificationVersion": "1.2.1"},
                    {"addressSource": [{"entity": [81]}, {"feature": 17}]},
                    {"addressDestination": [{"entity": [11]}, {"feature": 23}]},
                    {"addressOriginator": [{"device": "d:_i:EVSE-123"}, {"entity": [17]}, {"feature": 20}]},
                    {"msgCounter": 9},
                    {"cmdClassifier": "write"}
                  ]},
                  {"payload": []}
                ]})"sv,
            .is_valid        = true,
            .spec_version    = "1.2.1",
            .src_addr        = FEATURE_ADDRESS_TEST_DATA(nullptr, {81}, 17),
            .dest_addr       = FEATURE_ADDRESS_TEST_DATA(nullptr, {11}, 23),
            .originator_addr = FEATURE_ADDRESS_TEST_DATA("d:_i:EVSE-123", {17}, 20),
            .msg_cnt         = ValuePtrCreate<uint64_t>(9),
            .cmd_classifier  = ValuePtrCreate<CommandClassifierType>(kCommandClassifierTypeWrite),
        },
        DatagramDeserializeTestInput{
            .description = "Test datagram header SMA discovery data"sv,
            .msg =
                R"({"datagram": [
                  {"header": [
                    {"specificationVersion": "1.1.1"},
                    {"addressSource": [{"device": "d:_i:12_345"}, {"entity": [0]}, {"feature": 0}]},
                    {"addressDestination": [{"device": "d:_i:EVSE-123"}, {"entity": [0]}, {"feature": 0}]},
                    {"msgCounter": 100},
                    {"msgCounterReference": 1},
                    {"cmdClassifier": "call"}
                  ]},
                  {"payload": []}
                ]})"sv,
            .is_valid       = true,
            .spec_version   = "1.1.1",
            .src_addr       = FEATURE_ADDRESS_TEST_DATA("d:_i:12_345", {0}, 0),
            .dest_addr      = FEATURE_ADDRESS_TEST_DATA("d:_i:EVSE-123", {0}, 0),
            .msg_cnt        = ValuePtrCreate<uint64_t>(100),
            .msg_cnt_ref    = ValuePtrCreate<uint64_t>(1),
            .cmd_classifier = ValuePtrCreate<CommandClassifierType>(kCommandClassifierTypeCall),
        }
    )
);
