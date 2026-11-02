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

#include "src/common/eebus_data/eebus_data_tag.h"
#include "tests/unit_tests/common/eebus_data/employee.h"
#include "tests/unit_tests/json.h"
#include "tests/unit_tests/string_ptr.h"
#include "tests/unit_tests/value_ptr.h"

using namespace std::literals;

struct DataEmployeeTestInput {
  std::string_view description      = ""sv;
  std::string_view msg              = ""sv;
  StringPtr name                    = nullptr;
  StringPtr surname                 = nullptr;
  ValuePtr<uint32_t> id             = nullptr;
  ValuePtr<uint8_t> age             = nullptr;
  ValuePtr<Position> position       = nullptr;
  ValuePtr<uint32_t> salary         = nullptr;
  TagType is_married                = EEBUS_TAG_RESET;
  ValuePtr<bool> has_masters_degree = nullptr;
  std::vector<uint8_t> report       = {};
};

std::ostream& operator<<(std::ostream& os, DataEmployeeTestInput test_input) {
  return os << test_input.description;
}

class DataEmployeeTests : public ::testing::TestWithParam<DataEmployeeTestInput> {};

TEST_P(DataEmployeeTests, DataEmployeeTests) {
  // Arrange: Initialize the message buffer with parameters from test input
  std::unique_ptr<char[], decltype(&JsonFree)> s(JsonUnformat(GetParam().msg), JsonFree);
  ASSERT_NE(s, nullptr) << "Wrong test input!";

  // Act: Run the Json Parse
  std::unique_ptr<Employee, decltype(&EmployeeDelete)> employee{EmployeeParse(s.get()), EmployeeDelete};

  // Assert: Verify with expected datagram fields
  // then try to copy and serialize the obtained data
  ASSERT_NE(employee, nullptr);

  EXPECT_EQ(GetParam().name, StringPtr(employee->name));
  EXPECT_EQ(GetParam().surname, StringPtr(employee->surname));
  EXPECT_EQ(GetParam().id, ValuePtr<uint32_t>(employee->id));
  EXPECT_EQ(GetParam().age, ValuePtr<uint8_t>(employee->age));
  EXPECT_EQ(GetParam().position, ValuePtr<Position>(reinterpret_cast<Position*>(employee->position)));
  EXPECT_EQ(GetParam().salary, ValuePtr<uint32_t>(employee->salary));
  EXPECT_EQ(GetParam().is_married, employee->is_married);
  EXPECT_EQ(GetParam().has_masters_degree, ValuePtr<bool>(employee->has_masters_degree));
  ASSERT_EQ(GetParam().report.size(), employee->report_size);
  for (size_t i = 0; i < employee->report_size; ++i) {
    EXPECT_EQ(GetParam().report[i], *employee->report[i]) << "where i = " << i;
  }

  std::unique_ptr<Employee, decltype(&EmployeeDelete)> employee_copy{EmployeeCopy(employee.get()), EmployeeDelete};
  employee.reset();

  std::unique_ptr<char[], decltype(&JsonFree)> serialized{EmployeePrintUnformatted(employee_copy.get()), JsonFree};
  EXPECT_STREQ(serialized.get(), s.get());
}

INSTANTIATE_TEST_SUITE_P(
    DataEmployeeTests,
    DataEmployeeTests,
    ::testing::Values(
        DataEmployeeTestInput{
            .description        = "Test employee Data Configuration"sv,
            .msg                = R"({"employee": [
                                    {"name": "John"},
                                    {"surname": "Brown"},
                                    {"id": 115200},
                                    {"age": 31},
                                    {"position": "manager"},
                                    {"salary": 2000},
                                    {"married": []},
                                    {"mastersDegree": true},
                                    {"report": [0, 8, 0]}
                                  ]})"sv,
            .name               = "John",
            .surname            = "Brown",
            .id                 = ValuePtrCreate<uint32_t>(115200),
            .age                = ValuePtrCreate<uint8_t>(31),
            .position           = ValuePtrCreate<Position>(kPositionManager),
            .salary             = ValuePtrCreate<uint32_t>(2000),
            .is_married         = EEBUS_TAG_SET,
            .has_masters_degree = ValuePtrCreate<bool>(true),
            .report             = {0, 8, 0},
},
        DataEmployeeTestInput{
            .description = "Test employee Data Configuration"sv,
            .msg         = R"({"employee": [
                             {"name": "John"},
                             {"id": 115200},
                             {"position": "manager"},
                             {"married": []}
                           ]})"sv,
            .name        = "John",
            .id          = ValuePtrCreate<uint32_t>(115200),
            .position    = ValuePtrCreate<Position>(kPositionManager),
            .is_married  = EEBUS_TAG_SET,
        },
        DataEmployeeTestInput{
            .description        = "Test employee Data Configuration"sv,
            .msg                = R"({"employee": [
                                    {"surname": "Brown"},
                                    {"age": 31},
                                    {"salary": 2000},
                                    {"mastersDegree": true},
                                    {"report": [0, 8, 8, 5]}
                                  ]})"sv,
            .surname            = "Brown",
            .age                = ValuePtrCreate<uint8_t>(31),
            .salary             = ValuePtrCreate<uint32_t>(2000),
            .has_masters_degree = ValuePtrCreate<bool>(true),
            .report             = {0, 8, 8, 5},
        },
        DataEmployeeTestInput{
            .description = "Test employee Data Configuration"sv,
            .msg         = R"({"employee": [
                             {"name": "John"},
                             {"surname": "Brown"},
                             {"id": 115200},
                             {"age": 31},
                             {"position": "manager"},
                             {"salary": 2000},
                             {"report": [8, 8, 0, 8, 6]}
                           ]})"sv,
            .name        = "John",
            .surname     = "Brown",
            .id          = ValuePtrCreate<uint32_t>(115200),
            .age         = ValuePtrCreate<uint8_t>(31),
            .position    = ValuePtrCreate<Position>(kPositionManager),
            .salary      = ValuePtrCreate<uint32_t>(2000),
            .report      = {8, 8, 0, 8, 6},
        }
    )
);

struct DataEmployeeCompareTestInput {
  std::string_view description = ""sv;
  std::string_view msg_a       = ""sv;
  std::string_view msg_b       = ""sv;
  bool are_equal               = false;
};

std::ostream& operator<<(std::ostream& os, DataEmployeeCompareTestInput test_input) {
  return os << test_input.description;
}

class DataEmployeeCompareTests : public ::testing::TestWithParam<DataEmployeeCompareTestInput> {};

TEST_P(DataEmployeeCompareTests, DataEmployeeCompareTests) {
  // Arrange: Initialize the message buffers with parameters from test input
  std::unique_ptr<char[], decltype(&JsonFree)> s_a(JsonUnformat(GetParam().msg_a), JsonFree);
  std::unique_ptr<char[], decltype(&JsonFree)> s_b(JsonUnformat(GetParam().msg_b), JsonFree);
  ASSERT_NE(s_a, nullptr) << "Wrong test input!";
  ASSERT_NE(s_b, nullptr) << "Wrong test input!";
  std::unique_ptr<Employee, decltype(&EmployeeDelete)> employee_a{EmployeeParse(s_a.get()), EmployeeDelete};
  std::unique_ptr<Employee, decltype(&EmployeeDelete)> employee_b{EmployeeParse(s_b.get()), EmployeeDelete};
  ASSERT_NE(employee_a, nullptr);
  ASSERT_NE(employee_b, nullptr);

  // Act: compare data
  const bool ret = EmployeeCompare(employee_a.get(), employee_b.get());

  // Assert: Verify with expected equality status
  EXPECT_EQ(GetParam().are_equal, ret);
}

INSTANTIATE_TEST_SUITE_P(
    DataEmployeeCompareTests,
    DataEmployeeCompareTests,
    ::testing::Values(
        DataEmployeeCompareTestInput{
            .description = "Test employee data match 1"sv,
            .msg_a       = R"({"employee": [
                             {"name": "John"},
                             {"surname": "Brown"},
                             {"id": 115200},
                             {"age": 31},
                             {"position": "manager"},
                             {"salary": 2000},
                             {"married": []},
                             {"mastersDegree": true},
                             {"report": [0, 8, 0]}
                           ]})"sv,
            .msg_b       = R"({"employee": [
                             {"name": "John"},
                             {"surname": "Brown"},
                             {"id": 115200},
                             {"age": 31},
                             {"position": "manager"},
                             {"salary": 2000},
                             {"married": []},
                             {"mastersDegree": true},
                             {"report": [0, 8, 0]}
                           ]})"sv,
            .are_equal   = true,
        },
        DataEmployeeCompareTestInput{
            .description = "Test employee data match 2"sv,
            .msg_a       = R"({"employee": [
                             {"name": "John"},
                             {"id": 115200},
                             {"position": "manager"},
                             {"married": []}
                           ]})"sv,
            .msg_b       = R"({"employee": [
                             {"name": "John"},
                             {"id": 115200},
                             {"position": "manager"},
                             {"married": []}
                           ]})"sv,
            .are_equal   = true,
        },
        DataEmployeeCompareTestInput{
            .description = "Test employee data match 3"sv,
            .msg_a       = R"({"employee": [
                             {"surname": "Brown"},
                             {"age": 31},
                             {"salary": 2000},
                             {"mastersDegree": true},
                             {"report": [0, 8, 8, 5]}
                           ]})"sv,
            .msg_b       = R"({"employee": [
                             {"surname": "Brown"},
                             {"age": 31},
                             {"salary": 2000},
                             {"mastersDegree": true},
                             {"report": [0, 8, 8, 5]}
                           ]})"sv,
            .are_equal   = true,
        },
        DataEmployeeCompareTestInput{
            .description = "Test employee data match 4"sv,
            .msg_a       = R"({"employee": [
                             {"name": "John"},
                             {"surname": "Brown"},
                             {"id": 115200},
                             {"age": 31},
                             {"position": "manager"},
                             {"salary": 2000},
                             {"report": [8, 8, 0, 8, 6]}
                           ]})"sv,
            .msg_b       = R"({"employee": [
                             {"name": "John"},
                             {"surname": "Brown"},
                             {"id": 115200},
                             {"age": 31},
                             {"position": "manager"},
                             {"salary": 2000},
                             {"report": [8, 8, 0, 8, 6]}
                           ]})"sv,
            .are_equal   = true,
        },
        DataEmployeeCompareTestInput{
            .description = "Test employee data mismatch 1 (missing surname field in b)"sv,
            .msg_a       = R"({"employee": [
                             {"name": "John"},
                             {"surname": "Brown"},
                             {"id": 115200},
                             {"age": 31},
                             {"position": "manager"},
                             {"salary": 2000},
                             {"married": []},
                             {"mastersDegree": true},
                             {"report": [0, 8, 0]}
                           ]})"sv,
            .msg_b       = R"({"employee": [
                             {"name": "John"},
                             {"id": 115200},
                             {"age": 31},
                             {"position": "manager"},
                             {"salary": 2000},
                             {"married": []},
                             {"mastersDegree": true},
                             {"report": [0, 8, 0]}
                           ]})"sv,
            .are_equal   = false,
        },
        DataEmployeeCompareTestInput{
            .description = "Test employee data mismatch 2 (extra field in b)"sv,
            .msg_a       = R"({"employee": [
                             {"name": "John"},
                             {"id": 115200},
                             {"position": "manager"},
                             {"married": []}
                           ]})"sv,
            .msg_b       = R"({"employee": [
                             {"name": "John"},
                             {"id": 115200},
                             {"position": "manager"},
                             {"married": []},
                             {"report": [0]}
                           ]})"sv,
            .are_equal   = false,
        },
        DataEmployeeCompareTestInput{
            .description = "Test employee data mismatch 3 (report vries)"sv,
            .msg_a       = R"({"employee": [
                             {"surname": "Brown"},
                             {"age": 31},
                             {"salary": 2000},
                             {"mastersDegree": true},
                             {"report": [0, 8, 8, 5]}
                           ]})"sv,
            .msg_b       = R"({"employee": [
                             {"surname": "Brown"},
                             {"age": 31},
                             {"salary": 2000},
                             {"mastersDegree": true},
                             {"report": [0, 8, 7, 5]}
                           ]})"sv,
            .are_equal   = false,
        },
        DataEmployeeCompareTestInput{
            .description = "Test employee data mismatch 4 (typo in surname)"sv,
            .msg_a       = R"({"employee": [
                             {"name": "John"},
                             {"surname": "Brown"},
                             {"id": 115200},
                             {"age": 31},
                             {"position": "manager"},
                             {"salary": 2000},
                             {"report": [8, 8, 0, 8, 6]}
                           ]})"sv,
            .msg_b       = R"({"employee": [
                             {"name": "John"},
                             {"surname": "Brow"},
                             {"id": 115200},
                             {"age": 31},
                             {"position": "manager"},
                             {"salary": 2000},
                             {"report": [8, 8, 0, 8, 6]}
                           ]})"sv,
            .are_equal   = false,
        },
        DataEmployeeCompareTestInput{
            .description = "Test employee data mismatch 5 (age mismatch)"sv,
            .msg_a       = R"({"employee": [
                             {"name": "John"},
                             {"surname": "Brown"},
                             {"id": 115200},
                             {"age": 31},
                             {"position": "manager"},
                             {"salary": 2000},
                             {"report": [8, 8, 0, 8, 6]}
                           ]})"sv,
            .msg_b       = R"({"employee": [
                             {"name": "John"},
                             {"surname": "Brown"},
                             {"id": 115200},
                             {"age": 30},
                             {"position": "manager"},
                             {"salary": 2000},
                             {"report": [8, 8, 0, 8, 6]}
                           ]})"sv,
            .are_equal   = false,
        }
    )
);

struct DataEmployeeReadElementsTestInput {
  std::string_view description  = ""sv;
  std::string_view dst_msg_in   = ""sv;
  std::string_view src_msg      = ""sv;
  std::string_view elements_msg = ""sv;
  std::string_view dst_msg_out  = ""sv;
};

std::ostream& operator<<(std::ostream& os, DataEmployeeReadElementsTestInput test_input) {
  return os << test_input.description;
}

class DataEmployeeReadElementsTests : public ::testing::TestWithParam<DataEmployeeReadElementsTestInput> {};

TEST_P(DataEmployeeReadElementsTests, DataEmployeeReadElementsTests) {
  // Arrange: Initialize the message buffers with parameters from test input
  std::unique_ptr<char[], decltype(&JsonFree)> s_dst_in(JsonUnformat(GetParam().dst_msg_in), JsonFree);
  std::unique_ptr<char[], decltype(&JsonFree)> s_src(JsonUnformat(GetParam().src_msg), JsonFree);
  std::unique_ptr<char[], decltype(&JsonFree)> s_elements(JsonUnformat(GetParam().elements_msg), JsonFree);
  std::unique_ptr<char[], decltype(&JsonFree)> s_dst_out(JsonUnformat(GetParam().dst_msg_out), JsonFree);
  ASSERT_NE(s_dst_in, nullptr) << "Wrong test input!";
  ASSERT_NE(s_src, nullptr) << "Wrong test input!";
  ASSERT_NE(s_elements, nullptr) << "Wrong test input!";
  ASSERT_NE(s_dst_out, nullptr) << "Wrong test input!";
  std::unique_ptr<Employee, decltype(&EmployeeDelete)> employee_dst_in{EmployeeParse(s_dst_in.get()), EmployeeDelete};
  std::unique_ptr<Employee, decltype(&EmployeeDelete)> employee_src{EmployeeParse(s_src.get()), EmployeeDelete};
  std::unique_ptr<EmployeeElements, decltype(&EmployeeElementsDelete)> elements{
      EmployeeElementsParse(s_elements.get()),
      EmployeeElementsDelete
  };
  std::unique_ptr<Employee, decltype(&EmployeeDelete)> employee_dst_out_expected{
      EmployeeParse(s_dst_out.get()),
      EmployeeDelete
  };
  ASSERT_NE(employee_dst_in, nullptr);
  ASSERT_NE(employee_src, nullptr);
  ASSERT_NE(elements, nullptr);
  ASSERT_NE(employee_dst_out_expected, nullptr);

  // Act: write elements
  std::unique_ptr<Employee, decltype(&EmployeeDelete)> employee_dst_out_obtained{
      EmployeeReadElements(employee_src.get(), employee_dst_in.release(), elements.get()),
      EmployeeDelete
  };
  std::unique_ptr<char[], decltype(&JsonFree)> s_dst_out_obtained{
      EmployeePrintUnformatted(employee_dst_out_obtained.get()),
      JsonFree
  };

  // Assert: Verify with expected output data
  EXPECT_STREQ(s_dst_out_obtained.get(), s_dst_out.get());
}

INSTANTIATE_TEST_SUITE_P(
    DataEmployeeReadElementsTests,
    DataEmployeeReadElementsTests,
    ::testing::Values(
        DataEmployeeReadElementsTestInput{
            .description  = "Test employee data write surname"sv,
            .dst_msg_in   = R"({"employee": [
                              {"name": "John"},
                              {"surname": "Brown"},
                              {"id": 115200},
                              {"age": 31},
                              {"position": "manager"},
                              {"salary": 2000},
                              {"married": []},
                              {"mastersDegree": true},
                              {"report": [0, 8, 0]}
                            ]})"sv,
            .src_msg      = R"({"employee": [
                              {"surname":"Doe"}
                            ]})"sv,
            .elements_msg = R"({"employeeElements": [
                              {"surname":[]}
                            ]})"sv,
            .dst_msg_out  = R"({"employee": [
                              {"name": "John"},
                              {"surname": "Doe"},
                              {"id": 115200},
                              {"age": 31},
                              {"position": "manager"},
                              {"salary": 2000},
                              {"married": []},
                              {"mastersDegree": true},
                              {"report": [0, 8, 0]}
                            ]})"sv,
        },
        DataEmployeeReadElementsTestInput{
            .description  = "Test employee data write report"sv,
            .dst_msg_in   = R"({"employee": [
                              {"name": "John"},
                              {"surname": "Brown"},
                              {"id": 115200},
                              {"age": 31},
                              {"position": "manager"},
                              {"salary": 2000},
                              {"married": []},
                              {"mastersDegree": true},
                              {"report": [0, 8, 0]}
                            ]})"sv,
            .src_msg      = R"({"employee": [
                              {"report": [3, 4, 5, 6]}
                            ]})"sv,
            .elements_msg = R"({"employeeElements": [
                              {"report":[]}
                            ]})"sv,
            .dst_msg_out  = R"({"employee": [
                              {"name": "John"},
                              {"surname": "Brown"},
                              {"id": 115200},
                              {"age": 31},
                              {"position": "manager"},
                              {"salary": 2000},
                              {"married": []},
                              {"mastersDegree": true},
                              {"report": [3, 4, 5, 6]}
                            ]})"sv,
        },
        DataEmployeeReadElementsTestInput{
            .description  = "Test employee data write all fields from source"sv,
            .dst_msg_in   = R"({"employee": [
                              {"name": "John"},
                              {"surname": "Brown"},
                              {"id": 115200},
                              {"age": 31},
                              {"position": "manager"},
                              {"salary": 2000},
                              {"married": []},
                              {"mastersDegree": true},
                              {"report": [0, 8, 0]}
                            ]})"sv,
            .src_msg      = R"({"employee": [
                              {"surname": "Doe"},
                              {"report": [3, 4, 5]}
                            ]})"sv,
            .elements_msg = R"({"employeeElements": [
                            ]})"sv,
            .dst_msg_out  = R"({"employee": [
                              {"surname": "Doe"},
                              {"report": [3, 4, 5]}
                            ]})"sv,
        },
        DataEmployeeReadElementsTestInput{
            .description  = "Test employee data write id, age, position, married, masters degree"sv,
            .dst_msg_in   = R"({"employee": [
                              {"name": "John"},
                              {"surname": "Brown"},
                              {"id": 115200},
                              {"age": 31},
                              {"position": "manager"},
                              {"salary": 2000},
                              {"married": []},
                              {"mastersDegree": true},
                              {"report": [0, 8, 0]}
                            ]})"sv,
            .src_msg      = R"({"employee": [
                              {"id": 101},
                              {"age": 32},
                              {"position": "tester"},
                              {"mastersDegree": false}
                            ]})"sv,
            .elements_msg = R"({"employeeElements": [
                              {"id": []},
                              {"age": []},
                              {"position": []},
                              {"married": []},
                              {"mastersDegree": []}
                            ]})"sv,
            .dst_msg_out  = R"({"employee": [
                              {"name": "John"},
                              {"surname": "Brown"},
                              {"id": 101},
                              {"age": 32},
                              {"position": "tester"},
                              {"salary": 2000},
                              {"mastersDegree": false},
                              {"report": [0, 8, 0]}
                            ]})"sv,
        }
    )
);

struct DataEmployeeDeleteElementsTestInput {
  std::string_view description  = ""sv;
  std::string_view dst_msg_in   = ""sv;
  std::string_view elements_msg = ""sv;
  std::string_view dst_msg_out  = ""sv;
};

std::ostream& operator<<(std::ostream& os, DataEmployeeDeleteElementsTestInput test_input) {
  return os << test_input.description;
}

class DataEmployeeDeleteElementsTests : public ::testing::TestWithParam<DataEmployeeDeleteElementsTestInput> {};

TEST_P(DataEmployeeDeleteElementsTests, DataEmployeeDeleteElementsTests) {
  // Arrange: Initialize the message buffers with parameters from test input
  std::unique_ptr<char[], decltype(&JsonFree)> s_dst_in(JsonUnformat(GetParam().dst_msg_in), JsonFree);
  std::unique_ptr<char[], decltype(&JsonFree)> s_elements(JsonUnformat(GetParam().elements_msg), JsonFree);
  std::unique_ptr<char[], decltype(&JsonFree)> s_dst_out(JsonUnformat(GetParam().dst_msg_out), JsonFree);
  ASSERT_NE(s_dst_in, nullptr) << "Wrong test input!";
  ASSERT_NE(s_elements, nullptr) << "Wrong test input!";
  ASSERT_NE(s_dst_out, nullptr) << "Wrong test input!";
  std::unique_ptr<Employee, decltype(&EmployeeDelete)> employee_dst_in{EmployeeParse(s_dst_in.get()), EmployeeDelete};
  std::unique_ptr<EmployeeElements, decltype(&EmployeeElementsDelete)> elements{
      EmployeeElementsParse(s_elements.get()),
      EmployeeElementsDelete
  };
  std::unique_ptr<Employee, decltype(&EmployeeDelete)> employee_dst_out_expected{
      EmployeeParse(s_dst_out.get()),
      EmployeeDelete
  };
  ASSERT_NE(employee_dst_in, nullptr);
  ASSERT_NE(elements, nullptr);
  ASSERT_NE(employee_dst_out_expected, nullptr);

  // Act: write elements
  std::unique_ptr<Employee, decltype(&EmployeeDelete)> employee_dst_out_obtained{
      EmployeeDeleteElements(employee_dst_in.release(), elements.get()),
      EmployeeDelete
  };
  std::unique_ptr<char[], decltype(&JsonFree)> s_dst_out_obtained{
      EmployeePrintUnformatted(employee_dst_out_obtained.get()),
      JsonFree
  };

  // Assert: Verify with expected output data
  EXPECT_STREQ(s_dst_out_obtained.get(), s_dst_out.get());
}

INSTANTIATE_TEST_SUITE_P(
    DataEmployeeDeleteElementsTests,
    DataEmployeeDeleteElementsTests,
    ::testing::Values(
        DataEmployeeDeleteElementsTestInput{
            .description  = "Test employee data delete surname"sv,
            .dst_msg_in   = R"({"employee": [
                              {"name": "John"},
                              {"surname": "Brown"},
                              {"id": 115200},
                              {"age": 31},
                              {"position": "manager"},
                              {"salary": 2000},
                              {"married": []},
                              {"mastersDegree": true},
                              {"report": [0, 8, 0]}
                            ]})"sv,
            .elements_msg = R"({"employeeElements": [
                              {"surname":[]}
                            ]})"sv,
            .dst_msg_out  = R"({"employee": [
                              {"name": "John"},
                              {"id": 115200},
                              {"age": 31},
                              {"position": "manager"},
                              {"salary": 2000},
                              {"married": []},
                              {"mastersDegree": true},
                              {"report": [0, 8, 0]}
                            ]})"sv,
        },
        DataEmployeeDeleteElementsTestInput{
            .description  = "Test employee data delete report"sv,
            .dst_msg_in   = R"({"employee": [
                              {"name": "John"},
                              {"surname": "Brown"},
                              {"id": 115200},
                              {"age": 31},
                              {"position": "manager"},
                              {"salary": 2000},
                              {"married": []},
                              {"mastersDegree": true},
                              {"report": [0, 8, 0]}
                            ]})"sv,
            .elements_msg = R"({"employeeElements": [
                              {"report":[]}
                            ]})"sv,
            .dst_msg_out  = R"({"employee": [
                              {"name": "John"},
                              {"surname": "Brown"},
                              {"id": 115200},
                              {"age": 31},
                              {"position": "manager"},
                              {"salary": 2000},
                              {"married": []},
                              {"mastersDegree": true}
                            ]})"sv,
        },
        DataEmployeeDeleteElementsTestInput{
            .description  = "Test employee data delete id (null)"sv,
            .dst_msg_in   = R"({"employee": [
                              {"name": "John"},
                              {"surname": "Brown"},
                              {"age": 31},
                              {"position": "manager"},
                              {"salary": 2000},
                              {"married": []},
                              {"mastersDegree": true},
                              {"report": [0, 8, 0]}
                            ]})"sv,
            .elements_msg = R"({"employeeElements": [
                              {"id": []}
                            ]})"sv,
            .dst_msg_out  = R"({"employee": [
                              {"name": "John"},
                              {"surname": "Brown"},
                              {"age": 31},
                              {"position": "manager"},
                              {"salary": 2000},
                              {"married": []},
                              {"mastersDegree": true},
                              {"report": [0, 8, 0]}
                            ]})"sv,
        },
        DataEmployeeDeleteElementsTestInput{
            .description  = "Test employee data delete id, age, position, married, masters degree"sv,
            .dst_msg_in   = R"({"employee": [
                              {"name": "John"},
                              {"surname": "Brown"},
                              {"id": 115200},
                              {"age": 31},
                              {"position": "manager"},
                              {"salary": 2000},
                              {"married": []},
                              {"mastersDegree": true},
                              {"report": [0, 8, 0]}
                            ]})"sv,
            .elements_msg = R"({"employeeElements": [
                              {"id": []},
                              {"age": []},
                              {"position": []},
                              {"married": []},
                              {"mastersDegree": []}
                            ]})"sv,
            .dst_msg_out  = R"({"employee": [
                              {"name": "John"},
                              {"surname": "Brown"},
                              {"salary": 2000},
                              {"report": [0, 8, 0]}
                            ]})"sv,
        }
    )
);

struct DataEmployeeCopyElementsTestInput {
  std::string_view description  = ""sv;
  std::string_view src_msg      = ""sv;
  std::string_view elements_msg = ""sv;
  std::string_view ret_msg      = ""sv;
};

std::ostream& operator<<(std::ostream& os, DataEmployeeCopyElementsTestInput test_input) {
  return os << test_input.description;
}

class DataEmployeeCopyElementsTests : public ::testing::TestWithParam<DataEmployeeCopyElementsTestInput> {};

TEST_P(DataEmployeeCopyElementsTests, DataEmployeeCopyElementsTests) {
  // Arrange: Initialize the message buffers with parameters from test input
  std::unique_ptr<char[], decltype(&JsonFree)> s_src(JsonUnformat(GetParam().src_msg), JsonFree);
  std::unique_ptr<char[], decltype(&JsonFree)> s_elements(JsonUnformat(GetParam().elements_msg), JsonFree);
  std::unique_ptr<char[], decltype(&JsonFree)> s_ret(JsonUnformat(GetParam().ret_msg), JsonFree);
  ASSERT_NE(s_src, nullptr) << "Wrong test input!";
  ASSERT_NE(s_elements, nullptr) << "Wrong test input!";
  ASSERT_NE(s_ret, nullptr) << "Wrong test input!";
  std::unique_ptr<Employee, decltype(&EmployeeDelete)> employee_src{EmployeeParse(s_src.get()), EmployeeDelete};
  std::unique_ptr<EmployeeElements, decltype(&EmployeeElementsDelete)> elements{
      EmployeeElementsParse(s_elements.get()),
      EmployeeElementsDelete
  };
  ASSERT_NE(employee_src, nullptr);
  ASSERT_NE(elements, nullptr);

  // Act: copy elements
  std::unique_ptr<Employee, decltype(&EmployeeDelete)> employee_ret_obtained{
      EmployeeCopyElements(employee_src.get(), elements.get()),
      EmployeeDelete
  };
  std::unique_ptr<char[], decltype(&JsonFree)> s_ret_obtained{
      EmployeePrintUnformatted(employee_ret_obtained.get()),
      JsonFree
  };

  // Assert: Verify with expected output data
  EXPECT_STREQ(s_ret_obtained.get(), s_ret.get());
}

INSTANTIATE_TEST_SUITE_P(
    DataEmployeeCopyElementsTests,
    DataEmployeeCopyElementsTests,
    ::testing::Values(
        DataEmployeeCopyElementsTestInput{
            .description  = "Test employee data copy surname"sv,
            .src_msg      = R"({"employee": [
                              {"name": "John"},
                              {"surname": "Brown"},
                              {"id": 115200},
                              {"age": 31},
                              {"position": "manager"},
                              {"salary": 2000},
                              {"married": []},
                              {"mastersDegree": true},
                              {"report": [0, 8, 0]}
                            ]})"sv,
            .elements_msg = R"({"employeeElements": [
                              {"surname":[]}
                            ]})"sv,
            .ret_msg      = R"({"employee": [
                              {"surname": "Brown"}
                            ]})"sv,
        },
        DataEmployeeCopyElementsTestInput{
            .description  = "Test employee data copy report"sv,
            .src_msg      = R"({"employee": [
                              {"name": "John"},
                              {"surname": "Brown"},
                              {"id": 115200},
                              {"age": 31},
                              {"position": "manager"},
                              {"salary": 2000},
                              {"married": []},
                              {"mastersDegree": true},
                              {"report": [0, 8, 0]}
                            ]})"sv,
            .elements_msg = R"({"employeeElements": [
                              {"report":[]}
                            ]})"sv,
            .ret_msg      = R"({"employee": [
                              {"report": [0, 8, 0]}
                            ]})"sv,
        },
        DataEmployeeCopyElementsTestInput{
            .description  = "Test employee data copy id (null)"sv,
            .src_msg      = R"({"employee": [
                              {"name": "John"},
                              {"surname": "Brown"},
                              {"age": 31},
                              {"position": "manager"},
                              {"salary": 2000},
                              {"married": []},
                              {"mastersDegree": true},
                              {"report": [0, 8, 0]}
                            ]})"sv,
            .elements_msg = R"({"employeeElements": [
                              {"id": []}
                            ]})"sv,
            .ret_msg      = R"({"employee": [
                            ]})"sv,
        },
        DataEmployeeCopyElementsTestInput{
            .description  = "Test employee data copy id, age, position, married, masters degree"sv,
            .src_msg      = R"({"employee": [
                              {"name": "John"},
                              {"surname": "Brown"},
                              {"id": 115200},
                              {"age": 31},
                              {"position": "manager"},
                              {"salary": 2000},
                              {"married": []},
                              {"mastersDegree": true},
                              {"report": [0, 8, 0]}
                            ]})"sv,
            .elements_msg = R"({"employeeElements": [
                              {"id": []},
                              {"age": []},
                              {"position": []},
                              {"married": []},
                              {"mastersDegree": []}
                            ]})"sv,
            .ret_msg      = R"({"employee": [
                              {"id": 115200},
                              {"age": 31},
                              {"position": "manager"},
                              {"married": []},
                              {"mastersDegree": true}
                            ]})"sv,
        }
    )
);
