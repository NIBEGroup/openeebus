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
/**
 * @file
 * @brief Test Smart Energy Management PS write partial with alternative id only
 */

#include <gtest/gtest.h>

#include <string_view>

#include "src/spine/model/function_types.h"
#include "tests/src/spine/function/function_update_test.h"

using std::literals::string_view_literals::operator""sv;

static constexpr char data_init[] = R"(
{"smartEnergyManagementPsData": [
  {"alternatives": [
    [
      {"relation": [{"alternativesId": 0}]},
      {"powerSequence": [
        [
          {"description": [{"sequenceId": 0}, {"powerUnit": "W"}]},
          {"state": [{"state": "inactive"}]}
        ]
      ]}
    ],
    [
      {"relation": [{"alternativesId": 1}]},
      {"powerSequence": [
        [
          {"description": [{"sequenceId": 3}, {"powerUnit": "W"}]},
          {"state": [{"state": "inactive"}]}
        ]
      ]}
    ],
    [
      {"relation": [{"alternativesId": 2}]},
      {"powerSequence": [
        [
          {"description": [{"sequenceId": 5}, {"powerUnit": "W"}]},
          {"state": [{"state": "inactive"}]}
        ]
      ]}
    ]
  ]}
]})";

static constexpr char new_data[] = R"(
{"smartEnergyManagementPsData": [
  {"alternatives": [
    [
      {"relation": [{"alternativesId": 1}]},
      {"powerSequence": [
        [
          {"description": [{"sequenceId": 3}, {"powerUnit": "W"}, {"valueSource": "calculatedValue"}]},
          {"state": [{"state": "inactive"}, {"sequenceRemoteControllable": true}]},
          {"scheduleConstraints": [{"earliestStartTime": "PT0S"}, {"latestEndTime": "P1D"}]}
        ]
      ]}
    ]
  ]}
]})";

static constexpr char expected_data[] = R"(
{"smartEnergyManagementPsData": [
  {"alternatives": [
    [
      {"relation": [{"alternativesId": 0}]},
      {"powerSequence": [
        [
          {"description": [{"sequenceId": 0}, {"powerUnit": "W"}]},
          {"state": [{"state": "inactive"}]}
        ]
      ]}
    ],
    [
      {"relation": [{"alternativesId": 1}]},
      {"powerSequence": [
        [
          {"description": [{"sequenceId": 3}, {"powerUnit": "W"}, {"valueSource": "calculatedValue"}]},
          {"state": [{"state": "inactive"}, {"sequenceRemoteControllable": true}]},
          {"scheduleConstraints": [{"earliestStartTime": "PT0S"}, {"latestEndTime": "P1D"}]}
        ]
      ]}
    ],
    [
      {"relation": [{"alternativesId": 2}]},
      {"powerSequence": [
        [
          {"description": [{"sequenceId": 5}, {"powerUnit": "W"}]},
          {"state": [{"state": "inactive"}]}
        ]
      ]}
    ]
  ]}
]})";

INSTANTIATE_TEST_SUITE_P(
    SmartEnergyManagementPsWritePartialTests,
    FunctionUpdateTests,
    ::testing::Values(FunctionUpdateTestInput{
        .description        = "Test Smart Energy Management PS write partial with alternative id only"sv,
        .function_type      = kFunctionTypeSmartEnergyManagementPsData,
        .data_txt           = data_init,
        .new_data_txt       = new_data,
        .filter_partial_txt = R"({"filter": []})"sv,
        .expected_data_txt  = expected_data,
    })
);
