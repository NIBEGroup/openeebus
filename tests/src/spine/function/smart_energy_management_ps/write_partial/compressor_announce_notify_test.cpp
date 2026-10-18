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
 * @brief Test Smart Energy Management PS write Compressor Announce Notify message
 */

#include <gtest/gtest.h>

#include <string_view>

#include "src/spine/model/function_types.h"
#include "tests/src/spine/function/function_update_test.h"

using std::literals::string_view_literals::operator""sv;

static constexpr char compressor_announce_notify[] = R"(
{"smartEnergyManagementPsData": [
  {"nodeScheduleInformation": [
    {"nodeRemoteControllable": true},
    {"supportsSingleSlotSchedulingOnly": true},
    {"alternativesCount": 1},
    {"totalSequencesCountMax": 1},
    {"supportsReselection": false}
  ]},
  {"alternatives": [
    [
      {"relation": [{"alternativesId": 0}]},
      {"powerSequence": [
        [
          {"description": [{"sequenceId": 0}, {"powerUnit": "W"}, {"valueSource": "calculatedValue"}]},
          {"state": [{"state": "inactive"}, {"sequenceRemoteControllable": true}]},
          {"scheduleConstraints": [{"earliestStartTime": "PT0S"}, {"latestEndTime": "P1D"}]},
          {"operatingConstraintsInterrupt": [{"isPausable": false}, {"isStoppable": true}]},
          {"operatingConstraintsDuration": [{"activeDurationMin": "PT3M"}]},
          {"powerTimeSlot": [
            [
              {"schedule": [{"slotNumber": 0}]},
              {"valueList": [
                {"value": [
                  [
                    {"valueType": "powerMax"},
                    {"value": [{"number": 2000}, {"scale": 0}]}
                  ]
                ]}
              ]}
            ]
          ]}
        ]
      ]}
    ]
  ]}
]})";

INSTANTIATE_TEST_SUITE_P(
    SmartEnergyManagementPsWritePartialTests,
    FunctionUpdateTests,
    ::testing::Values(FunctionUpdateTestInput{
        .description        = "Test Smart Energy Management PS merge notify ready message"sv,
        .function_type      = kFunctionTypeSmartEnergyManagementPsData,
        .data_txt           = R"({"smartEnergyManagementPsData":[]})"sv,
        .new_data_txt       = compressor_announce_notify,
        .filter_partial_txt = R"({"filter": []})"sv,
        .expected_data_txt  = compressor_announce_notify,
    })
);
