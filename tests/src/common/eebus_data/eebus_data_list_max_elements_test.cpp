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
#include "tests/src/common/eebus_data/eebus_list_wrapper.h"
#include "tests/src/common/eebus_data/employee.h"

#include <stdint.h>

#include <gtest/gtest.h>

#include <memory>
#include <string>
#include <vector>

// Pre-fill the list to `count` elements using a single AppendList call.
// All slots point to the same dummy variable — only the count matters here.
static void PrefillList(EebusListWrapper<int32_t>& list, int32_t& dummy, size_t count) {
  std::vector<int32_t*> ptrs(count, &dummy);
  ASSERT_EQ(list.AppendList(ptrs.data(), count), kEebusErrorOk);
}

TEST(EebusDataListMaxElementsTests, DataAppendAtMaxLimitSucceedsTest) {
  // Arrange: bring the list to exactly one element below the cap
  EebusListWrapper<int32_t> list;
  int32_t dummy{0};
  ASSERT_NO_FATAL_FAILURE(PrefillList(list, dummy, EEBUS_DATA_LIST_MAX_ELEMENTS_NUM - 1));

  // Act: append the limit-th element
  const EebusError result = list.Append(&dummy);

  // Assert: the element at the cap boundary is accepted
  EXPECT_EQ(result, kEebusErrorOk);
  EXPECT_EQ(list.Size(), static_cast<size_t>(EEBUS_DATA_LIST_MAX_ELEMENTS_NUM));
}

TEST(EebusDataListMaxElementsTests, DataAppendOverMaxLimitFailsTest) {
  // Arrange: fill the list to exactly the cap
  EebusListWrapper<int32_t> list;
  int32_t dummy{0};
  ASSERT_NO_FATAL_FAILURE(PrefillList(list, dummy, EEBUS_DATA_LIST_MAX_ELEMENTS_NUM));

  // Act: attempt to append one element beyond the cap
  const EebusError result = list.Append(&dummy);

  // Assert: call is rejected and the list is left unchanged
  EXPECT_EQ(result, kEebusErrorMemoryAllocate);
  EXPECT_EQ(list.Size(), static_cast<size_t>(EEBUS_DATA_LIST_MAX_ELEMENTS_NUM));
}

TEST(EebusDataListMaxElementsTests, DataAppendListAtMaxLimitSucceedsTest) {
  // Arrange: start with one element already in the list
  EebusListWrapper<int32_t> list;
  int32_t dummy{0};
  ASSERT_NO_FATAL_FAILURE(PrefillList(list, dummy, 1));

  // Act: append a batch that brings the total to exactly the cap
  std::vector<int32_t*> batch(EEBUS_DATA_LIST_MAX_ELEMENTS_NUM - 1, &dummy);
  const EebusError result = list.AppendList(batch.data(), batch.size());

  // Assert: the batch landing exactly at the cap is accepted
  EXPECT_EQ(result, kEebusErrorOk);
  EXPECT_EQ(list.Size(), static_cast<size_t>(EEBUS_DATA_LIST_MAX_ELEMENTS_NUM));
}

TEST(EebusDataListMaxElementsTests, DataAppendListOverMaxLimitFailsTest) {
  // Arrange: start with one element already in the list
  EebusListWrapper<int32_t> list;
  int32_t dummy{0};
  ASSERT_NO_FATAL_FAILURE(PrefillList(list, dummy, 1));

  // Act: attempt to append a batch that would exceed the cap by one
  std::vector<int32_t*> batch(EEBUS_DATA_LIST_MAX_ELEMENTS_NUM, &dummy);
  const EebusError result = list.AppendList(batch.data(), batch.size());

  // Assert: the batch is rejected and the list is left unchanged
  EXPECT_EQ(result, kEebusErrorMemoryAllocate);
  EXPECT_EQ(list.Size(), 1u);
}

// Build a minimal Employee JSON whose "report" list has exactly `report_size`
// uint8_t entries.
static std::string BuildEmployeeWithReport(size_t report_size) {
  std::string json;
  json.reserve(report_size * 3 + 64);
  json = R"({"employee": [{"report": [)";
  for (size_t i = 0; i < report_size; ++i) {
    if (i > 0) {
      json += ',';
    }

    json += '1';
  }

  json += R"(]}]})";
  return json;
}

TEST(EebusDataListMaxElementsTests, DataFromJsonAtMaxLimitSucceedsTest) {
  // Arrange: build a JSON with a report list of exactly the cap size
  const std::string json = BuildEmployeeWithReport(EEBUS_DATA_LIST_MAX_ELEMENTS_NUM);

  // Act
  std::unique_ptr<Employee, decltype(&EmployeeDelete)> employee{EmployeeParse(json.c_str()), EmployeeDelete};

  // Assert: parsing succeeds and all elements are present
  ASSERT_NE(employee, nullptr);
  EXPECT_EQ(employee->report_size, static_cast<size_t>(EEBUS_DATA_LIST_MAX_ELEMENTS_NUM));
}

TEST(EebusDataListMaxElementsTests, DataFromJsonOverMaxLimitFailsTest) {
  // Arrange: build a JSON with one more entry than the cap allows
  const std::string json = BuildEmployeeWithReport(EEBUS_DATA_LIST_MAX_ELEMENTS_NUM + 1);

  // Act
  std::unique_ptr<Employee, decltype(&EmployeeDelete)> employee{EmployeeParse(json.c_str()), EmployeeDelete};

  // Assert: parsing is rejected
  EXPECT_EQ(employee, nullptr);
}
