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

#include <memory>
#include <string_view>

#include "src/spine/function/function.h"
#include "src/spine/model/cmd.h"
#include "src/spine/model/function_types.h"
#include "tests/unit_tests/json.h"
#include "tests/unit_tests/spine/function/cmd_test_data.h"
#include "tests/unit_tests/spine/function/filter_test_data.h"

using std::literals::string_view_literals::operator""sv;

struct FunctionCreateReadCmdTestInput {
  std::string_view description        = ""sv;
  FunctionType function_type          = static_cast<FunctionType>(0);
  std::string_view filter_partial_txt = ""sv;
  std::string_view cmd_txt            = ""sv;
};

std::ostream& operator<<(std::ostream& os, FunctionCreateReadCmdTestInput test_input) {
  return os << test_input.description;
}

class FunctionCreateReadCmdTests : public ::testing::TestWithParam<FunctionCreateReadCmdTestInput> {};

TEST_P(FunctionCreateReadCmdTests, FunctionCreateReadCmdTests) {
  // Arrange: Initialize the message buffer with parameters from test input
  std::unique_ptr<char[], decltype(&JsonFree)> s_cmd{JsonUnformat(GetParam().cmd_txt), JsonFree};
  ASSERT_NE(GetParam().cmd_txt.size() != 0, s_cmd == nullptr) << "Wrong test input!";
  std::unique_ptr<FunctionObject, decltype(&FunctionDelete)> fcn{
      FunctionCreate(GetParam().function_type),
      FunctionDelete
  };
  ASSERT_NE(fcn, nullptr);

  std::unique_ptr<FilterType, decltype(&FilterDelete)> filter_partial
      = FilterTestDataParse(GetParam().filter_partial_txt);
  ASSERT_NE(GetParam().filter_partial_txt.size() != 0, filter_partial == nullptr) << "Wrong Filter Partial input!";

  // Act: Create the read command
  std::unique_ptr<CmdType, decltype(&CmdDelete)> cmd{
      FUNCTION_CREATE_READ_CMD(fcn.get(), filter_partial.get()),
      CmdDelete
  };
  ASSERT_NE(cmd, nullptr);

  // Assert: Verify with expected command
  std::unique_ptr<char[], decltype(&JsonFree)> s_cmd_serialized{CmdPrintUnformatted(cmd.get()), JsonFree};
  EXPECT_STREQ(s_cmd_serialized.get(), s_cmd.get());
}

INSTANTIATE_TEST_SUITE_P(
    FunctionCreateReadCmdTests,
    FunctionCreateReadCmdTests,
    ::testing::Values(
        FunctionCreateReadCmdTestInput{
            .description   = "Test Actuator Level read command"sv,
            .function_type = kFunctionTypeActuatorLevelData,
            .cmd_txt       = R"({"cmd": [
                               {"actuatorLevelData": []}
                             ]})"sv,
        },
        FunctionCreateReadCmdTestInput{
            .description   = "Test Load Control Limit read command"sv,
            .function_type = kFunctionTypeLoadControlLimitListData,
            .cmd_txt       = R"({"cmd": [
                               {"loadControlLimitListData": []}
                             ]})"sv,
        },
        FunctionCreateReadCmdTestInput{
            .description        = "Test Load Control Limit read command with partial selectors"sv,
            .function_type      = kFunctionTypeLoadControlLimitListData,
            .filter_partial_txt = R"({"filter": [
                                    {"loadControlLimitListDataSelectors": [
                                      {"limitId": 113}
                                    ]}
                                  ]})"sv,
            .cmd_txt            = R"({"cmd": [
                                    {"function": "loadControlLimitListData"},
                                    {"filter": [
                                      [
                                        {"cmdControl": [
                                          {"partial": []}
                                        ]},
                                        {"loadControlLimitListDataSelectors": [
                                          {"limitId": 113}
                                        ]}
                                      ]
                                    ]},
                                    {"loadControlLimitListData": []}
                                  ]})"sv,
        },
        FunctionCreateReadCmdTestInput{
            .description        = "Test Load Control Limit read command with partial selectors and elements"sv,
            .function_type      = kFunctionTypeLoadControlLimitListData,
            .filter_partial_txt = R"({"filter": [
                                    {"loadControlLimitListDataSelectors": [
                                      {"limitId": 113}
                                    ]},
                                    {"loadControlLimitDataElements": [
                                      {"value": [
                                        {"number": []}
                                      ]}
                                    ]}
                                  ]})"sv,
            .cmd_txt            = R"({"cmd": [
                                    {"function": "loadControlLimitListData"},
                                    {"filter": [
                                      [
                                        {"cmdControl": [
                                          {"partial": []}
                                        ]},
                                        {"loadControlLimitListDataSelectors": [
                                          {"limitId": 113}
                                        ]},
                                        {"loadControlLimitDataElements": [
                                          {"value": [
                                            {"number": []}
                                          ]}
                                        ]}
                                      ]
                                    ]},
                                    {"loadControlLimitListData": []}
                                  ]})"sv,
        }
    )
);
