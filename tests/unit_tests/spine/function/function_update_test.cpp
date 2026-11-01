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

#include "tests/unit_tests/spine/function/function_update_test.h"

#include <memory>

#include "src/common/json.h"
#include "src/spine/function/function.h"
#include "tests/unit_tests/json.h"
#include "tests/unit_tests/spine/function/filter_test_data.h"
#include "tests/unit_tests/spine/function/function_data_test_data.h"

std::ostream& operator<<(std::ostream& os, FunctionUpdateTestInput test_input) {
  return os << test_input.description;
}

TEST_P(FunctionUpdateTests, FunctionUpdateTests) {
  // Arrange: Initialize the Function with parameters from test input
  std::unique_ptr<FunctionData, decltype(&FunctionDataDelete)> function_data
      = FunctionDataTestDataParse(GetParam().function_type, GetParam().data_txt);
  ASSERT_NE(GetParam().data_txt.size() != 0, function_data == nullptr) << "Wrong Function Data input!";
  std::unique_ptr<FunctionObject, decltype(&FunctionDelete)> fcn{
      FunctionCreate(GetParam().function_type),
      FunctionDelete
  };
  ASSERT_NE(fcn, nullptr);
  // Write the initial data to function
  ASSERT_EQ(FUNCTION_UPDATE_DATA(fcn.get(), function_data->data, nullptr, nullptr, false, true), kEebusErrorOk);

  std::unique_ptr<FilterType, decltype(&FilterDelete)> filter_partial
      = FilterTestDataParse(GetParam().filter_partial_txt);
  ASSERT_NE(GetParam().filter_partial_txt.size() != 0, filter_partial == nullptr) << "Wrong Filter Partial input!";

  std::unique_ptr<FilterType, decltype(&FilterDelete)> filter_delete
      = FilterTestDataParse(GetParam().filter_delete_txt);
  ASSERT_NE(GetParam().filter_delete_txt.size() != 0, filter_delete == nullptr) << "Wrong Filter Delete input!";

  std::unique_ptr<FunctionData, decltype(&FunctionDataDelete)> new_function_data
      = FunctionDataTestDataParse(GetParam().function_type, GetParam().new_data_txt);
  ASSERT_NE(GetParam().new_data_txt.size() != 0, new_function_data == nullptr) << "Wrong New Data input!";

  const void* const new_data = (new_function_data != nullptr) ? new_function_data->data : nullptr;

  // Act: Run the data update
  FUNCTION_UPDATE_DATA(
      fcn.get(),
      new_data,
      filter_partial.get(),
      filter_delete.get(),
      GetParam().wr_remote,
      GetParam().persist
  );
  std::unique_ptr<FunctionData, decltype(&FunctionDataDelete)> function_data_obtained{
      FunctionDataCreate(FUNCTION_GET_FUNCTION_TYPE(fcn.get()), FUNCTION_DATA_COPY(fcn.get())),
      FunctionDataDelete
  };
  std::unique_ptr<char[], decltype(&JsonFree)> s_obtained_data
      = FunctionDataTestDataPrint(function_data_obtained.get());

  // Assert: Verify with expected data
  std::unique_ptr<char[], decltype(&JsonFree)> s_expected_data{JsonUnformat(GetParam().expected_data_txt), JsonFree};
  ASSERT_NE(GetParam().expected_data_txt.size() != 0, s_expected_data == nullptr) << "Wrong Expected Data input!";

  EXPECT_STREQ(s_expected_data.get(), s_obtained_data.get());
}
