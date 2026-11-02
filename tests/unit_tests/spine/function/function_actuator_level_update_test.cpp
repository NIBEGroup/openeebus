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
    ActuatorLevelUpdatelTests,
    FunctionUpdateTests,
    ::testing::Values(
        FunctionUpdateTestInput{
            .description       = "Test Actuator Level delete number"sv,
            .function_type     = kFunctionTypeActuatorLevelData,
            .data_txt          = R"({"actuatorLevelData": [
                                   {"function": "absolut"},
                                   {"value": [
                                     {"number": 121},
                                     {"scale": 10}
                                   ]}
                                 ]})"sv,
            .filter_delete_txt = R"({"filter": [
                                   {"actuatorLevelDataElements": [
                                     {"value": [
                                       {"number": []}
                                     ]}
                                   ]}
                                 ]})"sv,
            .expected_data_txt = R"({"actuatorLevelData": [
                                   {"function": "absolut"},
                                   {"value": [
                                     {"scale": 10}
                                   ]}
                                 ]})"sv,
        },
        FunctionUpdateTestInput{
            .description       = "Test Actuator Level delete value"sv,
            .function_type     = kFunctionTypeActuatorLevelData,
            .data_txt          = R"({"actuatorLevelData": [
                                   {"function": "absolut"},
                                   {"value": [
                                     {"number": 121},
                                     {"scale": 10}
                                   ]}
                                 ]})"sv,
            .filter_delete_txt = R"({"filter": [
                                   {"actuatorLevelDataElements": [
                                     {"value": []
                                     }
                                   ]}
                                 ]})"sv,
            .expected_data_txt = R"({"actuatorLevelData": [
                                   {"function": "absolut"}
                                   ]}
                                 ]})"sv,
        },
        FunctionUpdateTestInput{
            .description        = "Test Actuator Level write scale"sv,
            .function_type      = kFunctionTypeActuatorLevelData,
            .data_txt           = R"({"actuatorLevelData": [
                                    {"function": "absolut"},
                                    {"value": [
                                      {"number": 121},
                                      {"scale": 10}
                                    ]}
                                  ]})"sv,
            .new_data_txt       = R"({"actuatorLevelData": [
                                    {"value": [
                                      {"scale": 20}
                                    ]}
                                  ]})"sv,
            .filter_partial_txt = R"({"filter": [
                                  ]})"sv,
            .expected_data_txt  = R"({"actuatorLevelData": [
                                    {"function": "absolut"},
                                    {"value": [
                                      {"number": 121},
                                      {"scale": 20}
                                    ]}
                                  ]})"sv,
        },
        FunctionUpdateTestInput{
            .description        = "Test Actuator Level write value"sv,
            .function_type      = kFunctionTypeActuatorLevelData,
            .data_txt           = R"({"actuatorLevelData": [
                                    {"function": "absolut"},
                                    {"value": [
                                      {"number": 1045},
                                      {"scale": 100}
                                    ]}
                                  ]})"sv,
            .new_data_txt       = R"({"actuatorLevelData": [
                                    {"value": [
                                      {"number": 27},
                                      {"scale": 1}
                                    ]}
                                  ]})"sv,
            .filter_partial_txt = R"({"filter": [
                                  ]})"sv,
            .expected_data_txt  = R"({"actuatorLevelData": [
                                    {"function": "absolut"},
                                    {"value": [
                                      {"number": 27},
                                      {"scale": 1}
                                    ]}
                                  ]})"sv,
        }
    )
);
