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

#include "tests/unit_tests/common/eebus_data/address_test_data.h"
#include "tests/unit_tests/common/eebus_data/employee.h"
#include "tests/unit_tests/common/eebus_data/info.h"
#include "tests/unit_tests/common/eebus_data/person.h"
#include "tests/unit_tests/common/eebus_data/somebody.h"
#include "tests/unit_tests/json.h"
#include "tests/unit_tests/string_ptr.h"
#include "tests/unit_tests/value_ptr.h"

using namespace std::literals;

struct DataChoiceFromObjectToObjectTestInput {
  std::string_view description = ""sv;
  std::string_view msg         = ""sv;
  StringPtr name               = nullptr;
  StringPtr surname            = nullptr;
  ValuePtr<uint32_t> id        = nullptr;
  ValuePtr<uint8_t> age        = nullptr;
  ValuePtr<Position> position  = nullptr;
  ValuePtr<uint32_t> salary    = nullptr;
  std::vector<uint8_t> report  = {};
};

std::ostream& operator<<(std::ostream& os, DataChoiceFromObjectToObjectTestInput test_input) {
  return os << test_input.description;
}

class DataChoiceFromObjectToObjectTests : public ::testing::TestWithParam<DataChoiceFromObjectToObjectTestInput> {};

TEST_P(DataChoiceFromObjectToObjectTests, DataChoiceFromObjectToObjectTests) {
  // Arrange: Initialize the message buffer with parameters from test input
  std::unique_ptr<char[], decltype(&JsonFree)> s{JsonUnformat(GetParam().msg), JsonFree};
  ASSERT_NE(s, nullptr) << "Wrong test input!";

  // Act: Run the Json Parse
  std::unique_ptr<Somebody, decltype(&SomebodyDelete)> somebody{SomebodyParse(s.get()), SomebodyDelete};

  // Assert: Verify with expected Employee fields
  // then try to copy and serialize the obtained data
  ASSERT_NE(somebody, nullptr);
  ASSERT_NE(somebody->data, nullptr);
  ASSERT_EQ(somebody->data_type_id, 0);

  const Employee* const employee = reinterpret_cast<Employee*>(somebody->data);
  EXPECT_EQ(GetParam().name, StringPtr(employee->name));
  EXPECT_EQ(GetParam().surname, StringPtr(employee->surname));
  EXPECT_EQ(GetParam().id, ValuePtr<uint32_t>(employee->id));
  EXPECT_EQ(GetParam().age, ValuePtr<uint8_t>(employee->age));
  EXPECT_EQ(GetParam().position, ValuePtr<Position>(reinterpret_cast<Position*>(employee->position)));
  EXPECT_EQ(GetParam().salary, ValuePtr<uint32_t>(employee->salary));
  ASSERT_EQ(GetParam().report.size(), employee->report_size);
  for (size_t i = 0; i < employee->report_size; ++i) {
    EXPECT_EQ(GetParam().report[i], *employee->report[i]) << "where i = " << i;
  }
}

INSTANTIATE_TEST_SUITE_P(
    DataChoiceFromObjectToObjectTests,
    DataChoiceFromObjectToObjectTests,
    ::testing::Values(DataChoiceFromObjectToObjectTestInput{
        .description = "Test employee choice Data Configuration"sv,
        .msg         = R"({
                         "employee": [
                           {"name": "John"},
                           {"surname": "Brown"},
                           {"id": 115200},
                           {"age": 31},
                           {"position": "manager"},
                           {"salary": 2000},
                           {"report": [8, 8, 0, 8, 6]}
                         ]
                       })"sv,
        .name        = "John",
        .surname     = "Brown",
        .id          = ValuePtrCreate<uint32_t>(115200),
        .age         = ValuePtrCreate<uint8_t>(31),
        .position    = ValuePtrCreate<Position>(kPositionManager),
        .salary      = ValuePtrCreate<uint32_t>(2000),
        .report      = {8, 8, 0, 8, 6},
})
);

struct DataChoiceFromObjectToSequenceTestInput {
  std::string_view description      = ""sv;
  std::string_view msg              = ""sv;
  StringPtr name                    = nullptr;
  ValuePtr<uint8_t> age             = nullptr;
  ValuePtr<AddressTestData> address = nullptr;
};

std::ostream& operator<<(std::ostream& os, DataChoiceFromObjectToSequenceTestInput test_input) {
  return os << test_input.description;
}

class DataChoiceFromSomebodyToPersonTests : public ::testing::TestWithParam<DataChoiceFromObjectToSequenceTestInput> {};

TEST_P(DataChoiceFromSomebodyToPersonTests, DataChoiceFromSomebodyToPersonTests) {
  // Arrange: Initialize the message buffer with parameters from test input
  std::unique_ptr<char[], decltype(&JsonFree)> s{JsonUnformat(GetParam().msg), JsonFree};
  ASSERT_NE(s, nullptr) << "Wrong test input!";

  // Act: Run the Json Parse
  std::unique_ptr<Somebody, decltype(&SomebodyDelete)> somebody{SomebodyParse(s.get()), SomebodyDelete};

  // Assert: Verify with expected datagram fields
  // then try to copy and serialize the obtained data
  ASSERT_NE(somebody, nullptr);
  ASSERT_NE(somebody->data, nullptr);
  ASSERT_EQ(somebody->data_type_id, 1);

  const Person* const person = reinterpret_cast<Person*>(somebody->data);
  EXPECT_EQ(GetParam().name, StringPtr(person->name));
  EXPECT_EQ(GetParam().age, ValuePtr<uint8_t>(person->age));
  EXPECT_EQ(GetParam().address, person->address);

  std::unique_ptr<Somebody, decltype(&SomebodyDelete)> somebody_copy{SomebodyCopy(somebody.get()), SomebodyDelete};
  somebody.reset();

  std::unique_ptr<char[], decltype(&JsonFree)> serialized{SomebodyPrintUnformatted(somebody_copy.get()), JsonFree};
  EXPECT_STREQ(serialized.get(), s.get());
}

INSTANTIATE_TEST_SUITE_P(
    DataChoiceFromSomebodyToPersonTests,
    DataChoiceFromSomebodyToPersonTests,
    ::testing::Values(DataChoiceFromObjectToSequenceTestInput{
        .description = "Test person Data Configuration"sv,
        .msg         = R"({"person": [
                         {"name": "John Doe"},
                         {"age": 43},
                         {"address": [
                           {"street": "10 Downing Street"},
                           {"city": "London"}
                         ]}
                       ]})"sv,
        .name        = "John Doe",
        .age         = ValuePtrCreate<uint8_t>(43),
        .address     = ValuePtrCreate<AddressTestData>("10 Downing Street", "London"),
    })
);

struct DataChoiceFromSequenceToObjectTestInput {
  std::string_view description = ""sv;
  std::string_view msg         = ""sv;
  StringPtr name               = nullptr;
  StringPtr surname            = nullptr;
  ValuePtr<uint32_t> id        = nullptr;
  ValuePtr<uint8_t> age        = nullptr;
  ValuePtr<Position> position  = nullptr;
  ValuePtr<uint32_t> salary    = nullptr;
  std::vector<uint8_t> report  = {};
};

std::ostream& operator<<(std::ostream& os, DataChoiceFromSequenceToObjectTestInput test_input) {
  return os << test_input.description;
}

class DataChoiceFromInfoToEmployeeTests : public ::testing::TestWithParam<DataChoiceFromSequenceToObjectTestInput> {};

TEST_P(DataChoiceFromInfoToEmployeeTests, DataChoiceFromInfoToEmployeeTests) {
  // Arrange: Initialize the message buffer with parameters from test input
  std::unique_ptr<char[], decltype(&JsonFree)> s{JsonUnformat(GetParam().msg), JsonFree};
  ASSERT_NE(s, nullptr) << "Wrong test input!";

  // Act: Run the Json Parse
  std::unique_ptr<Info, decltype(&InfoDelete)> info{InfoParse(s.get()), InfoDelete};

  // Assert: Verify with expected Employee fields,
  // then try to copy and serialize the obtained data
  ASSERT_NE(info, nullptr);
  ASSERT_NE(info->data, nullptr);
  ASSERT_EQ(info->data_type_id, kInfoEmployee);

  const Employee* const employee = reinterpret_cast<Employee*>(info->data);
  EXPECT_EQ(GetParam().name, StringPtr(employee->name));
  EXPECT_EQ(GetParam().surname, StringPtr(employee->surname));
  EXPECT_EQ(GetParam().id, ValuePtr<uint32_t>(employee->id));
  EXPECT_EQ(GetParam().age, ValuePtr<uint8_t>(employee->age));
  EXPECT_EQ(GetParam().position, ValuePtr<Position>(reinterpret_cast<Position*>(employee->position)));
  EXPECT_EQ(GetParam().salary, ValuePtr<uint32_t>(employee->salary));
  ASSERT_EQ(GetParam().report.size(), employee->report_size);
  for (size_t i = 0; i < employee->report_size; ++i) {
    EXPECT_EQ(GetParam().report[i], *employee->report[i]) << "where i = " << i;
  }

  std::unique_ptr<Info, decltype(&InfoDelete)> info_copy{InfoCopy(info.get()), InfoDelete};
  info.reset();

  std::unique_ptr<char[], decltype(&JsonFree)> serialized{InfoPrintUnformatted(info_copy.get()), JsonFree};
  EXPECT_STREQ(serialized.get(), s.get());
}

INSTANTIATE_TEST_SUITE_P(
    DataChoiceFromInfoToEmployeeTests,
    DataChoiceFromInfoToEmployeeTests,
    ::testing::Values(DataChoiceFromSequenceToObjectTestInput{
        .description = "Test employee choice Data Configuration"sv,
        .msg         = R"({"info": [
                         {"employee": [
                           {"name": "John"},
                           {"surname": "Brown"},
                           {"id": 115200},
                           {"age": 31},
                           {"position": "manager"},
                           {"salary": 2000},
                           {"report": [8, 8, 0, 8, 6]}
                         ]}
                       ]})"sv,
        .name        = "John",
        .surname     = "Brown",
        .id          = ValuePtrCreate<uint32_t>(115200),
        .age         = ValuePtrCreate<uint8_t>(31),
        .position    = ValuePtrCreate<Position>(kPositionManager),
        .salary      = ValuePtrCreate<uint32_t>(2000),
        .report      = {8, 8, 0, 8, 6},
})
);

struct DataChoiceFromSequenceToSequenceTestInput {
  std::string_view description      = ""sv;
  std::string_view msg              = ""sv;
  StringPtr name                    = nullptr;
  ValuePtr<uint8_t> age             = nullptr;
  ValuePtr<AddressTestData> address = nullptr;
};

std::ostream& operator<<(std::ostream& os, DataChoiceFromSequenceToSequenceTestInput test_input) {
  return os << test_input.description;
}

class DataChoiceFromInfoToPersonTests : public ::testing::TestWithParam<DataChoiceFromSequenceToSequenceTestInput> {};

TEST_P(DataChoiceFromInfoToPersonTests, DataChoiceFromInfoToPersonTests) {
  // Arrange: Initialize the message buffer with parameters from test input
  std::unique_ptr<char[], decltype(&JsonFree)> s{JsonUnformat(GetParam().msg), JsonFree};
  ASSERT_NE(s, nullptr) << "Wrong test input!";

  // Act: Run the Json Parse
  std::unique_ptr<Info, decltype(&InfoDelete)> info{InfoParse(s.get()), InfoDelete};

  // Assert: Verify with expected datagram fields,
  // then try to copy and serialize the obtained data
  ASSERT_NE(info, nullptr);
  ASSERT_NE(info->data, nullptr);
  ASSERT_EQ(info->data_type_id, kInfoPerson);

  const Person* const person = reinterpret_cast<Person*>(info->data);
  EXPECT_EQ(GetParam().name, StringPtr(person->name));
  EXPECT_EQ(GetParam().age, ValuePtr<uint8_t>(person->age));
  EXPECT_EQ(GetParam().address, person->address);

  std::unique_ptr<Info, decltype(&InfoDelete)> info_copy{InfoCopy(info.get()), InfoDelete};
  info.reset();

  std::unique_ptr<char[], decltype(&JsonFree)> serialized{InfoPrintUnformatted(info_copy.get()), JsonFree};
  EXPECT_STREQ(serialized.get(), s.get());
}

INSTANTIATE_TEST_SUITE_P(
    DataChoiceFromInfoToPersonTests,
    DataChoiceFromInfoToPersonTests,
    ::testing::Values(DataChoiceFromSequenceToSequenceTestInput{
        .description = "Test person Data Configuration"sv,
        .msg         = R"({"info": [
                         {"person": [
                           {"name": "John Doe"},
                           {"age": 43},
                           {"address": [
                             {"street": "10 Downing Street"},
                             {"city": "London"}
                           ]}
                         ]}
                       ]})"sv,
        .name        = "John Doe",
        .age         = ValuePtrCreate<uint8_t>(43),
        .address     = ValuePtrCreate<AddressTestData>("10 Downing Street", "London"),
    })
);

struct DataChoiceCompareTestInput {
  std::string_view description = ""sv;
  std::string_view msg_a       = ""sv;
  std::string_view msg_b       = ""sv;
  bool are_equal               = false;
};

std::ostream& operator<<(std::ostream& os, DataChoiceCompareTestInput test_input) {
  return os << test_input.description;
}

class DataChoiceCompareTests : public ::testing::TestWithParam<DataChoiceCompareTestInput> {};

TEST_P(DataChoiceCompareTests, DataChoiceCompareTests) {
  // Arrange: Initialize the message buffers with parameters from test input
  std::unique_ptr<char[], decltype(&JsonFree)> s_a(JsonUnformat(GetParam().msg_a), JsonFree);
  std::unique_ptr<char[], decltype(&JsonFree)> s_b(JsonUnformat(GetParam().msg_b), JsonFree);
  ASSERT_NE(s_a, nullptr) << "Wrong test input!";
  ASSERT_NE(s_b, nullptr) << "Wrong test input!";
  std::unique_ptr<Info, decltype(&InfoDelete)> info_a{InfoParse(s_a.get()), InfoDelete};
  std::unique_ptr<Info, decltype(&InfoDelete)> info_b{InfoParse(s_b.get()), InfoDelete};
  ASSERT_NE(info_a, nullptr);
  ASSERT_NE(info_b, nullptr);

  // Act: compare data
  const bool ret = InfoCompare(info_a.get(), info_b.get());

  // Assert: Verify with expected equality status
  EXPECT_EQ(GetParam().are_equal, ret);
}

INSTANTIATE_TEST_SUITE_P(
    DataChoiceCompareTests,
    DataChoiceCompareTests,
    ::testing::Values(
        DataChoiceCompareTestInput{
            .description = "Test choice data match"sv,
            .msg_a       = R"({"info": [
                             {"person": [
                               {"name": "John Doe"},
                               {"age": 43},
                               {"address": [
                                 {"street": "10 Downing Street"},
                                 {"city": "London"}
                               ]}
                             ]}
                           ]})"sv,
            .msg_b       = R"({"info": [
                             {"person": [
                               {"name": "John Doe"},
                               {"age": 43},
                               {"address": [
                                 {"street": "10 Downing Street"},
                                 {"city": "London"}
                               ]}
                             ]}
                           ]})"sv,
            .are_equal   = true,
        },
        DataChoiceCompareTestInput{
            .description = "Test choice data mismatch (different choice)"sv,
            .msg_a       = R"({"info": [
                             {"person": [
                               {"name": "John Doe"},
                               {"age": 43},
                               {"address": [
                                 {"street": "10 Downing Street"},
                                 {"city": "London"}
                               ]}
                             ]}
                           ]})"sv,
            .msg_b       = R"({"info": [
                             {"employee": [
                               {"name": "John"},
                               {"surname": "Brown"},
                               {"id": 115200},
                               {"age": 31},
                               {"position": "manager"},
                               {"salary": 2000},
                               {"report": [8, 8, 0, 8, 6]}
                             ]}
                           ]})"sv,
            .are_equal   = false,
        },
        DataChoiceCompareTestInput{
            .description = "Test choice data mismatch (different address)"sv,
            .msg_a       = R"({"info": [
                             {"person": [
                               {"name": "John Doe"},
                               {"age": 43},
                               {"address": [
                                 {"street": "10 Downing Street"},
                                 {"city": "London"}
                               ]}
                             ]}
                           ]})"sv,
            .msg_b       = R"({"info": [
                             {"person": [
                               {"name": "John Doe"},
                               {"age": 43},
                               {"address": [
                                 {"street": "11 Downing Street"},
                                 {"city": "London"}
                               ]}
                             ]}
                           ]})"sv,
            .are_equal   = false,
        }
    )
);

struct DataChoiceEmployeeCopyTestInput {
  std::string_view description = ""sv;
  std::string_view src_msg     = ""sv;
};

std::ostream& operator<<(std::ostream& os, DataChoiceEmployeeCopyTestInput test_input) {
  return os << test_input.description;
}

class DataChoiceEmployeeCopyTests : public ::testing::TestWithParam<DataChoiceEmployeeCopyTestInput> {};

TEST_P(DataChoiceEmployeeCopyTests, DataChoiceEmployeeCopyTests) {
  // Arrange: Initialize the message buffers with parameters from test input
  std::unique_ptr<char[], decltype(&JsonFree)> s_src(JsonUnformat(GetParam().src_msg), JsonFree);
  ASSERT_NE(s_src, nullptr) << "Wrong test input!";
  std::unique_ptr<Employee, decltype(&InfoEmployeeDelete)> employee_src{
      InfoEmployeeParse(s_src.get()),
      InfoEmployeeDelete
  };
  ASSERT_NE(employee_src, nullptr);

  // Act: copy
  std::unique_ptr<Employee, decltype(&InfoEmployeeDelete)> employee_copy{
      InfoEmployeeCopy(employee_src.get()),
      InfoEmployeeDelete
  };
  std::unique_ptr<char[], decltype(&JsonFree)> s_ret{InfoEmployeePrintUnformatted(employee_copy.get()), JsonFree};

  // Assert: Verify with expected output data
  EXPECT_STREQ(s_ret.get(), s_src.get());
}

INSTANTIATE_TEST_SUITE_P(
    DataChoiceEmployeeCopyTests,
    DataChoiceEmployeeCopyTests,
    ::testing::Values(
        DataChoiceEmployeeCopyTestInput{
            .description = "Test employee data copy surname"sv,
            .src_msg     = R"({"employee": [
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
        },
        DataChoiceEmployeeCopyTestInput{
            .description = "Test employee data copy report"sv,
            .src_msg     = R"({"employee": [
                              {"name": "John"},
                              {"surname": "Doe"},
                              {"id": 4639},
                              {"position": "tester"},
                              {"salary": 2500},
                              {"mastersDegree": true},
                              {"report": [8, 8, 8, 8, 7]}
                            ]})"sv,
        },
        DataChoiceEmployeeCopyTestInput{
            .description = "Test employee data copy id (null)"sv,
            .src_msg     = R"({"employee": [
                              {"age": 21},
                              {"position": "manager"},
                              {"salary": 1000},
                              {"married": []},
                              {"mastersDegree": true},
                              {"report": [6, 8, 5]}
                            ]})"sv,
        },
        DataChoiceEmployeeCopyTestInput{
            .description = "Test employee data copy id, age, position, married, masters degree"sv,
            .src_msg     = R"({"employee": [
                              {"id": 4639},
                              {"age": 37},
                              {"position": "developer"},
                              {"married": []},
                              {"mastersDegree": true},
                              {"report": [8, 8, 8, 8, 8]}
                            ]})"sv,
        }
    )
);

TEST(DataChoiceEmployeeCopyTag, DataChoiceEmployeeCopyTag) {
  InfoTag tag = EEBUS_TAG_SET;
  EXPECT_EQ(tag, InfoTagCopy(tag));

  tag = EEBUS_TAG_RESET;
  EXPECT_EQ(tag, InfoTagCopy(tag));

  tag = EEBUS_TAG_SET;
  EXPECT_EQ(tag, InfoTagCopy(tag));
}
