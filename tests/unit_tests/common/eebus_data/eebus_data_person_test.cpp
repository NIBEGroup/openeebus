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
#include "tests/unit_tests/common/eebus_data/person.h"
#include "tests/unit_tests/json.h"
#include "tests/unit_tests/string_ptr.h"
#include "tests/unit_tests/value_ptr.h"

using namespace std::literals;

struct DataPersonTestInput {
  std::string_view description      = ""sv;
  std::string_view msg              = ""sv;
  StringPtr name                    = nullptr;
  ValuePtr<uint8_t> age             = nullptr;
  ValuePtr<AddressTestData> address = nullptr;
};

std::ostream& operator<<(std::ostream& os, DataPersonTestInput test_input) {
  return os << test_input.description;
}

class DataPersonTests : public ::testing::TestWithParam<DataPersonTestInput> {};

TEST_P(DataPersonTests, DataPersonTests) {
  // Arrange: Initialize the message buffer with parameters from test input
  std::unique_ptr<char[], decltype(&JsonFree)> s{JsonUnformat(GetParam().msg), JsonFree};
  ASSERT_NE(s, nullptr) << "Wrong test input!";

  // Act: Run the Json Parse
  std::unique_ptr<Person, decltype(&PersonDelete)> person{PersonParse(s.get()), PersonDelete};

  // Assert: Verify with expected datagram fields
  // then try to copy and serialize the obtained data
  ASSERT_NE(person, nullptr);

  EXPECT_EQ(GetParam().name, StringPtr(person->name));
  EXPECT_EQ(GetParam().age, ValuePtr<uint8_t>(person->age));
  EXPECT_EQ(GetParam().address, person->address);

  std::unique_ptr<Person, decltype(&PersonDelete)> person_copy{PersonCopy(person.get()), PersonDelete};
  person.reset();

  std::unique_ptr<char[], decltype(&JsonFree)> serialized{PersonPrintUnformatted(person_copy.get()), JsonFree};
  EXPECT_STREQ(serialized.get(), s.get());
}

INSTANTIATE_TEST_SUITE_P(
    DataPersonTests,
    DataPersonTests,
    ::testing::Values(
        DataPersonTestInput{
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
        },
        DataPersonTestInput{
            .description = "Test person Data Configuration"sv,
            .msg         = R"({"person": [
                             {"name": "John Doe"},
                             {"address": [
                               {"city": "London"}
                             ]}
                           ]})"sv,
            .name        = "John Doe",
            .address     = ValuePtrCreate<AddressTestData>(nullptr, "London"),
        },
        DataPersonTestInput{
            .description = "Test person Data Configuration"sv,
            .msg         = R"({"person": [
                             {"age": 45},
                             {"address": [
                               {"street": "10 Downing Street"}
                             ]}
                           ]})"sv,
            .age         = ValuePtrCreate<uint8_t>(45),
            .address     = ValuePtrCreate<AddressTestData>("10 Downing Street", nullptr),
        }
    )
);

struct DataPersonReadElementsTestInput {
  std::string_view description  = ""sv;
  std::string_view dst_msg_in   = ""sv;
  std::string_view src_msg      = ""sv;
  std::string_view elements_msg = ""sv;
  std::string_view dst_msg_out  = ""sv;
};

std::ostream& operator<<(std::ostream& os, DataPersonReadElementsTestInput test_input) {
  return os << test_input.description;
}

class DataPersonReadElementsTests : public ::testing::TestWithParam<DataPersonReadElementsTestInput> {};

TEST_P(DataPersonReadElementsTests, DataPersonReadElementsTests) {
  // Arrange: Initialize the message buffers with parameters from test input
  std::unique_ptr<char[], decltype(&JsonFree)> s_dst_in(JsonUnformat(GetParam().dst_msg_in), JsonFree);
  std::unique_ptr<char[], decltype(&JsonFree)> s_src(JsonUnformat(GetParam().src_msg), JsonFree);
  std::unique_ptr<char[], decltype(&JsonFree)> s_elements(JsonUnformat(GetParam().elements_msg), JsonFree);
  std::unique_ptr<char[], decltype(&JsonFree)> s_dst_out(JsonUnformat(GetParam().dst_msg_out), JsonFree);
  ASSERT_NE(s_dst_in, nullptr) << "Wrong test input!";
  ASSERT_NE(s_src, nullptr) << "Wrong test input!";
  ASSERT_NE(s_elements, nullptr) << "Wrong test input!";
  ASSERT_NE(s_dst_out, nullptr) << "Wrong test input!";
  std::unique_ptr<Person, decltype(&PersonDelete)> person_dst_in{PersonParse(s_dst_in.get()), PersonDelete};
  std::unique_ptr<Person, decltype(&PersonDelete)> person_src{PersonParse(s_src.get()), PersonDelete};
  std::unique_ptr<PersonElements, decltype(&PersonElementsDelete)> elements{
      PersonElementsParse(s_elements.get()),
      PersonElementsDelete
  };
  std::unique_ptr<Person, decltype(&PersonDelete)> person_dst_out_expected{PersonParse(s_dst_out.get()), PersonDelete};
  ASSERT_NE(person_dst_in, nullptr);
  ASSERT_NE(person_src, nullptr);
  ASSERT_NE(elements, nullptr);
  ASSERT_NE(person_dst_out_expected, nullptr);

  // Act: write elements
  std::unique_ptr<Person, decltype(&PersonDelete)> person_dst_out_obtained{
      PersonReadElements(person_src.get(), person_dst_in.release(), elements.get()),
      PersonDelete
  };
  std::unique_ptr<char[], decltype(&JsonFree)> s_dst_out_obtained{
      PersonPrintUnformatted(person_dst_out_obtained.get()),
      JsonFree
  };

  // Assert: Verify with expected output data
  EXPECT_STREQ(s_dst_out_obtained.get(), s_dst_out.get());
}

INSTANTIATE_TEST_SUITE_P(
    DataPersonReadElementsTests,
    DataPersonReadElementsTests,
    ::testing::Values(
        DataPersonReadElementsTestInput{
            .description  = "Test person write city"sv,
            .dst_msg_in   = R"({"person": [
                              {"name": "John Doe"},
                              {"age": 43},
                              {"address": [
                                {"street": "10 Downing Street"},
                                {"city": "London"}
                              ]}
                            ]})"sv,
            .src_msg      = R"({"person": [
                              {"address": [
                                {"city": "Springfield"}
                              ]}
                            ]})"sv,
            .elements_msg = R"({"personElements": [
                              {"address": [
                                {"city": []}
                              ]}
                            ]})"sv,
            .dst_msg_out  = R"({"person": [
                              {"name": "John Doe"},
                              {"age": 43},
                              {"address": [
                                {"street": "10 Downing Street"},
                                {"city": "Springfield"}
                              ]}
                            ]})"sv,
        },
        DataPersonReadElementsTestInput{
            .description  = "Test person write address"sv,
            .dst_msg_in   = R"({"person": [
                              {"name": "John Doe"},
                              {"age": 43},
                              {"address": [
                                {"street": "10 Downing Street"},
                                {"city": "London"}
                              ]}
                            ]})"sv,
            .src_msg      = R"({"person": [
                              {"address": [
                                {"street": "742 Evergreen Terrace"},
                                {"city": "Springfield"}
                              ]}
                            ]})"sv,
            .elements_msg = R"({"personElements": [
                              {"address": []}
                              ]}
                            ]})"sv,
            .dst_msg_out  = R"({"person": [
                              {"name": "John Doe"},
                              {"age": 43},
                              {"address": [
                                {"street": "742 Evergreen Terrace"},
                                {"city": "Springfield"}
                              ]}
                            ]})"sv,
        }
    )
);

struct DataPersonDeleteElementsTestInput {
  std::string_view description  = ""sv;
  std::string_view dst_msg_in   = ""sv;
  std::string_view src_msg      = ""sv;
  std::string_view elements_msg = ""sv;
  std::string_view dst_msg_out  = ""sv;
};

std::ostream& operator<<(std::ostream& os, DataPersonDeleteElementsTestInput test_input) {
  return os << test_input.description;
}

class DataPersonDeleteElementsTests : public ::testing::TestWithParam<DataPersonDeleteElementsTestInput> {};

TEST_P(DataPersonDeleteElementsTests, DataPersonDeleteElementsTests) {
  // Arrange: Initialize the message buffers with parameters from test input
  std::unique_ptr<char[], decltype(&JsonFree)> s_dst_in(JsonUnformat(GetParam().dst_msg_in), JsonFree);
  std::unique_ptr<char[], decltype(&JsonFree)> s_elements(JsonUnformat(GetParam().elements_msg), JsonFree);
  std::unique_ptr<char[], decltype(&JsonFree)> s_dst_out(JsonUnformat(GetParam().dst_msg_out), JsonFree);
  ASSERT_NE(s_dst_in, nullptr) << "Wrong test input!";
  ASSERT_NE(s_elements, nullptr) << "Wrong test input!";
  ASSERT_NE(s_dst_out, nullptr) << "Wrong test input!";
  std::unique_ptr<Person, decltype(&PersonDelete)> person_dst_in{PersonParse(s_dst_in.get()), PersonDelete};
  std::unique_ptr<PersonElements, decltype(&PersonElementsDelete)> elements{
      PersonElementsParse(s_elements.get()),
      PersonElementsDelete
  };
  std::unique_ptr<Person, decltype(&PersonDelete)> person_dst_out_expected{PersonParse(s_dst_out.get()), PersonDelete};
  ASSERT_NE(person_dst_in, nullptr);
  ASSERT_NE(elements, nullptr);
  ASSERT_NE(person_dst_out_expected, nullptr);

  // Act: write elements
  std::unique_ptr<Person, decltype(&PersonDelete)> person_dst_out_obtained{
      PersonDeleteElements(person_dst_in.release(), elements.get()),
      PersonDelete
  };
  std::unique_ptr<char[], decltype(&JsonFree)> s_dst_out_obtained{
      PersonPrintUnformatted(person_dst_out_obtained.get()),
      JsonFree
  };

  // Assert: Verify with expected output data
  EXPECT_STREQ(s_dst_out_obtained.get(), s_dst_out.get());
}

INSTANTIATE_TEST_SUITE_P(
    DataPersonDeleteElementsTests,
    DataPersonDeleteElementsTests,
    ::testing::Values(
        DataPersonDeleteElementsTestInput{
            .description  = "Test person delete city"sv,
            .dst_msg_in   = R"({"person": [
                              {"name": "John Doe"},
                              {"age": 43},
                              {"address": [
                                {"street": "10 Downing Street"},
                                {"city": "London"}
                              ]}
                            ]})"sv,
            .elements_msg = R"({"personElements": [
                              {"address": [
                                {"city": []}
                              ]}
                            ]})"sv,
            .dst_msg_out  = R"({"person": [
                              {"name": "John Doe"},
                              {"age": 43},
                              {"address": [
                                {"street": "10 Downing Street"}
                              ]}
                            ]})"sv,
        },
        DataPersonDeleteElementsTestInput{
            .description  = "Test person delete address"sv,
            .dst_msg_in   = R"({"person": [
                              {"name": "John Doe"},
                              {"age": 43},
                              {"address": [
                                {"street": "10 Downing Street"},
                                {"city": "London"}
                              ]}
                            ]})"sv,
            .elements_msg = R"({"personElements": [
                              {"address": []}
                              ]}
                            ]})"sv,
            .dst_msg_out  = R"({"person": [
                              {"name": "John Doe"},
                              {"age": 43}
                            ]})"sv,
        },
        DataPersonDeleteElementsTestInput{
            .description  = "Test person delete name (null)"sv,
            .dst_msg_in   = R"({"person": [
                              {"age": 43},
                              {"address": [
                                {"street": "10 Downing Street"},
                                {"city": "London"}
                              ]}
                            ]})"sv,
            .elements_msg = R"({"personElements": [
                              {"name": []}
                              ]}
                            ]})"sv,
            .dst_msg_out  = R"({"person": [
                              {"age": 43},
                              {"address": [
                                {"street": "10 Downing Street"},
                                {"city": "London"}
                              ]}
                            ]})"sv,
        }
    )
);

struct DataPersonCopyElementsTestInput {
  std::string_view description  = ""sv;
  std::string_view src_msg      = ""sv;
  std::string_view elements_msg = ""sv;
  std::string_view ret_msg      = ""sv;
};

std::ostream& operator<<(std::ostream& os, DataPersonCopyElementsTestInput test_input) {
  return os << test_input.description;
}

class DataPersonCopyElementsTests : public ::testing::TestWithParam<DataPersonCopyElementsTestInput> {};

TEST_P(DataPersonCopyElementsTests, DataPersonCopyElementsTests) {
  // Arrange: Initialize the message buffers with parameters from test input
  std::unique_ptr<char[], decltype(&JsonFree)> s_src(JsonUnformat(GetParam().src_msg), JsonFree);
  std::unique_ptr<char[], decltype(&JsonFree)> s_elements(JsonUnformat(GetParam().elements_msg), JsonFree);
  std::unique_ptr<char[], decltype(&JsonFree)> s_ret(JsonUnformat(GetParam().ret_msg), JsonFree);
  ASSERT_NE(s_src, nullptr) << "Wrong test input!";
  ASSERT_NE(s_elements, nullptr) << "Wrong test input!";
  ASSERT_NE(s_ret, nullptr) << "Wrong test input!";
  std::unique_ptr<Person, decltype(&PersonDelete)> person_src{PersonParse(s_src.get()), PersonDelete};
  std::unique_ptr<PersonElements, decltype(&PersonElementsDelete)> elements{
      PersonElementsParse(s_elements.get()),
      PersonElementsDelete
  };
  ASSERT_NE(person_src, nullptr);
  ASSERT_NE(elements, nullptr);

  // Act: copy elements
  std::unique_ptr<Person, decltype(&PersonDelete)> person_ret_obtained{
      PersonCopyElements(person_src.get(), elements.get()),
      PersonDelete
  };
  std::unique_ptr<char[], decltype(&JsonFree)> s_ret_obtained{
      PersonPrintUnformatted(person_ret_obtained.get()),
      JsonFree
  };

  // Assert: Verify with expected output data
  EXPECT_STREQ(s_ret_obtained.get(), s_ret.get());
}

INSTANTIATE_TEST_SUITE_P(
    DataPersonCopyElementsTests,
    DataPersonCopyElementsTests,
    ::testing::Values(
        DataPersonCopyElementsTestInput{
            .description  = "Test person copy city"sv,
            .src_msg      = R"({"person": [
                              {"name": "John Doe"},
                              {"age": 43},
                              {"address": [
                                {"street": "10 Downing Street"},
                                {"city": "London"}
                              ]}
                            ]})"sv,
            .elements_msg = R"({"personElements": [
                              {"address": [
                                {"city": []}
                              ]}
                            ]})"sv,
            .ret_msg      = R"({"person": [
                              {"address": [
                                {"city": "London"}
                              ]}
                            ]})"sv,
        },
        DataPersonCopyElementsTestInput{
            .description  = "Test person copy address"sv,
            .src_msg      = R"({"person": [
                              {"name": "John Doe"},
                              {"age": 43},
                              {"address": [
                                {"street": "10 Downing Street"},
                                {"city": "London"}
                              ]}
                            ]})"sv,
            .elements_msg = R"({"personElements": [
                              {"address": []}
                              ]}
                            ]})"sv,
            .ret_msg      = R"({"person": [
                              {"address": [
                                {"street": "10 Downing Street"},
                                {"city": "London"}
                              ]}
                            ]})"sv,
        },
        DataPersonCopyElementsTestInput{
            .description  = "Test person copy name (null)"sv,
            .src_msg      = R"({"person": [
                              {"age": 43},
                              {"address": [
                                {"street": "10 Downing Street"},
                                {"city": "London"}
                              ]}
                            ]})"sv,
            .elements_msg = R"({"personElements": [
                              {"name": []}
                              ]}
                            ]})"sv,
            .ret_msg      = R"({"person": [
                            ]})"sv,
        }
    )
);
