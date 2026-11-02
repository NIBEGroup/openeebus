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

#include "src/spine/model/function_types.h"
#include "src/spine/model/loadcontrol_types.h"
#include "src/use_case/specialization/helper.h"
#include "tests/unit_tests/memory_leak.inc"
#include "tests/unit_tests/spine/function/function_data_test_data.h"

static constexpr FunctionType function_type = kFunctionTypeLoadControlLimitListData;

static constexpr char data_txt[] = R"({"loadControlLimitListData": [
  {"loadControlLimitData": [
    [
      {"limitId": 1},
      {"value": [
        {"number": 121},
        {"scale": -1}
      ]}
    ],
    [
      {"limitId": 2},
      {"value": [
        {"number": 121},
        {"scale": 10}
      ]}
    ],
    [
      {"limitId": 3},
      {"value": [
        {"number": 121},
        {"scale": 0}
      ]}
    ],
    [
      {"limitId": 4},
      {"value": [
        {"number": 121},
        {"scale": 2}
      ]}
    ],
    [
      {"limitId": 5},
      {"value": [
        {"number": 121},
        {"scale": 0}
      ]}
    ]
  ]}
]})";

TEST(FunctionUpdateTests, FunctionUpdateTests) {
  std::unique_ptr<FunctionData, decltype(&FunctionDataDelete)> function_data
      = FunctionDataTestDataParse(function_type, data_txt);

  ASSERT_NE(function_data, nullptr) << "Wrong Function Data input!";

  static NumberType number = 121;
  static ScaleType scale   = 0;

  static ScaledNumberType scaled_number = {
      .number = &number,
      .scale  = &scale,
  };

  static LoadControlLimitDataType filter = {
      .value = &scaled_number,
  };

  size_t num_matches = 0;

  EebusDataListMatchIterator it = {0};
  HelperListMatchFirst(function_type, function_data->data, &filter, &it);

  for (; !EebusDataListMatchIteratorIsDone(&it); EebusDataListMatchIteratorNext(&it)) {
    LoadControlLimitDataType* item = (LoadControlLimitDataType*)EebusDataListMatchIteratorGet(&it);
    ASSERT_NE(item, nullptr);
    ASSERT_NE(item->value, nullptr);
    ASSERT_NE(item->value->number, nullptr);
    ASSERT_NE(item->value->scale, nullptr);
    ASSERT_NE(item->limit_id, nullptr);

    EXPECT_EQ(*item->value->number, 121);
    EXPECT_EQ(*item->value->scale, 0);
    if (num_matches == 0) {
      EXPECT_EQ(*item->limit_id, 3);
    } else if (num_matches == 1) {
      EXPECT_EQ(*item->limit_id, 5);
    }
    ++num_matches;
  }

  EXPECT_EQ(num_matches, 2);
}
