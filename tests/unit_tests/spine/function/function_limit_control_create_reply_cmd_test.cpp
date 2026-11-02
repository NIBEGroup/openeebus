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

#include "src/spine/model/function_types.h"
#include "tests/unit_tests/spine/function/function_create_reply_cmd_test.h"

using std::literals::string_view_literals::operator""sv;

INSTANTIATE_TEST_SUITE_P(
    LoadControlCreateReplyCmdTests,
    FunctionCreateReplyCmdTests,
    ::testing::Values(
        FunctionCreateReplyCmdTestInput{
            .description   = "Test Load Control Limit create the reply command 1"sv,
            .function_type = kFunctionTypeLoadControlLimitListData,
            .data_txt      = R"({"loadControlLimitListData": [
                               {"loadControlLimitData": [
                                 [
                                   {"limitId": 10},
                                   {"value": [
                                     {"number": 121},
                                     {"scale": 10}
                                   ]}
                                 ]
                               ]}
                             ]})"sv,
            .cmd_txt       = R"({"cmd": [
                               {"loadControlLimitListData": [
                                 {"loadControlLimitData": [
                                   [
                                     {"limitId": 10},
                                     {"value": [
                                       {"number": 121},
                                       {"scale": 10}
                                     ]}
                                   ]
                                 ]}
                               ]}
                             ]})"sv,
        },
        FunctionCreateReplyCmdTestInput{
            .description   = "Test Load Control Limit create the reply command 2"sv,
            .function_type = kFunctionTypeLoadControlLimitListData,
            .data_txt      = R"({"loadControlLimitListData": [
                               {"loadControlLimitData": [
                                 [
                                   {"limitId": 25},
                                   {"value": [
                                     {"number": 121},
                                     {"scale": 10}
                                   ]}
                                 ],
                                 [
                                   {"limitId": 113},
                                   {"value": [
                                     {"number": 121},
                                     {"scale": 10}
                                   ]}
                                 ],
                                 [
                                   {"limitId": 10},
                                   {"value": [
                                     {"number": 121},
                                     {"scale": 10}
                                   ]}
                                 ]
                               ]}
                             ]})"sv,
            .cmd_txt       = R"({"cmd": [
                               {"loadControlLimitListData": [
                                 {"loadControlLimitData": [
                                   [
                                     {"limitId": 25},
                                     {"value": [
                                       {"number": 121},
                                       {"scale": 10}
                                     ]}
                                   ],
                                   [
                                     {"limitId": 113},
                                     {"value": [
                                       {"number": 121},
                                       {"scale": 10}
                                     ]}
                                   ],
                                   [
                                     {"limitId": 10},
                                     {"value": [
                                       {"number": 121},
                                       {"scale": 10}
                                     ]}
                                   ]
                                 ]}
                               ]}
                             ]})"sv,
        }
    )
);
