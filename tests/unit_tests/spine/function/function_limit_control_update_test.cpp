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
#include "tests/unit_tests/spine/function/function_update_test.h"

using std::literals::string_view_literals::operator""sv;

INSTANTIATE_TEST_SUITE_P(
    LoadControlUpdateTests,
    FunctionUpdateTests,
    ::testing::Values(
        FunctionUpdateTestInput{
            .description       = "Test Load Control Limit delete selected item"sv,
            .function_type     = kFunctionTypeLoadControlLimitListData,
            .data_txt          = R"({"loadControlLimitListData": [
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
            .filter_delete_txt = R"({"filter": [
                                    {"loadControlLimitListDataSelectors": [
                                      {"limitId": 113}
                                    ]}
                                  ]})"sv,
            .expected_data_txt = R"({"loadControlLimitListData": [
                                    {"loadControlLimitData": [
                                      [
                                        {"limitId": 25},
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
        },
        FunctionUpdateTestInput{
            .description       = "Test Load Control Limit delete elements from selected item"sv,
            .function_type     = kFunctionTypeLoadControlLimitListData,
            .data_txt          = R"({"loadControlLimitListData": [
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
            .filter_delete_txt = R"({"filter": [
                                    {"loadControlLimitListDataSelectors": [
                                      {"limitId": 10}
                                    ]},
                                    {"loadControlLimitDataElements": [
                                      {"value": [
                                        {"number": []}
                                      ]}
                                    ]}
                                  ]})"sv,
            .expected_data_txt = R"({"loadControlLimitListData": [
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
                                          {"scale": 10}
                                        ]}
                                      ]
                                    ]}
                                  ]})"sv,
        },
        FunctionUpdateTestInput{
            .description        = "Test Load Control Limit write data into selected item"sv,
            .function_type      = kFunctionTypeLoadControlLimitListData,
            .data_txt           = R"({"loadControlLimitListData": [
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
            .new_data_txt       = R"({"loadControlLimitListData": [
                                    {"loadControlLimitData": [
                                      [
                                        {"value": [
                                          {"number": 95},
                                          {"scale": 1}
                                        ]}
                                      ]
                                    ]}
                                  ]})"sv,
            .filter_partial_txt = R"({"filter": [
                                   {"loadControlLimitListDataSelectors": [
                                     {"limitId": 113}
                                   ]}
                                 ]})"sv,
            .expected_data_txt  = R"({"loadControlLimitListData": [
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
                                          {"number": 95},
                                          {"scale": 1}
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
        },
        FunctionUpdateTestInput{
            .description        = "Test Load Control Limit write data into all items"sv,
            .function_type      = kFunctionTypeLoadControlLimitListData,
            .data_txt           = R"({"loadControlLimitListData": [
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
            .new_data_txt       = R"({"loadControlLimitListData": [
                                    {"loadControlLimitData": [
                                      [
                                        {"value": [
                                          {"number": 33},
                                          {"scale": 10}
                                        ]}
                                      ]
                                    ]}
                                  ]})"sv,
            .filter_partial_txt = R"({"filter": [
                                 ]})"sv,
            .expected_data_txt  = R"({"loadControlLimitListData": [
                                    {"loadControlLimitData": [
                                      [
                                        {"limitId": 25},
                                        {"value": [
                                          {"number": 33},
                                          {"scale": 10}
                                        ]}
                                      ],
                                      [
                                        {"limitId": 113},
                                        {"value": [
                                          {"number": 33},
                                          {"scale": 10}
                                        ]}
                                      ],
                                      [
                                        {"limitId": 10},
                                        {"value": [
                                          {"number": 33},
                                          {"scale": 10}
                                        ]}
                                      ]
                                    ]}
                                  ]})"sv,
        }
    )
);
