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

#include <limits>
#include <memory>
#include <string_view>

#include "src/common/string_util.h"
#include "src/use_case/model/scaled_value.h"

using std::literals::string_view_literals::operator""sv;

//-------------------------------------------------------------------------------------------//
//
// ScaledValueToString() test
//
//-------------------------------------------------------------------------------------------//
struct ScaledValueToStringTestInput {
  std::string_view description{""sv};
  ScaledValue scaled_value{0, 0};
  const char* s{nullptr};
};

std::ostream& operator<<(std::ostream& os, ScaledValueToStringTestInput test_input) {
  return os << test_input.description;
}

class ScaledValueToStringTests : public ::testing::TestWithParam<ScaledValueToStringTestInput> {};

TEST_P(ScaledValueToStringTests, ScaledValueToStringTests) {
  auto const_string_deleter = [](const char* p) { StringDelete(const_cast<char*>(p)); };
  // Act: Run the scaled value to string conversion
  std::unique_ptr<const char[], decltype(const_string_deleter)> s{
      ScaledValueToString(&GetParam().scaled_value),
      const_string_deleter
  };

  // Assert: Verify with expected string
  EXPECT_STREQ(s.get(), GetParam().s);
}

INSTANTIATE_TEST_SUITE_P(
    ScaledValueToStringTests,
    ScaledValueToStringTests,
    ::testing::Values(
        // Simple integer tests
        ScaledValueToStringTestInput{
            .description  = "Simple positive integer"sv,
            .scaled_value = ScaledValue{123, 0},
            .s            = "123",
},
        ScaledValueToStringTestInput{
            .description  = "Simple negative integer"sv,
            .scaled_value = ScaledValue{-456, 0},
            .s            = "-456",
        },
        ScaledValueToStringTestInput{
            .description  = "Single digit"sv,
            .scaled_value = ScaledValue{7, 0},
            .s            = "7",
        },
        ScaledValueToStringTestInput{
            .description  = "Large positive integer"sv,
            .scaled_value = ScaledValue{999999, 0},
            .s            = "999999",
        },
        ScaledValueToStringTestInput{
            .description  = "Large negative integer"sv,
            .scaled_value = ScaledValue{-888888, 0},
            .s            = "-888888",
        },
        // Decimal tests
        ScaledValueToStringTestInput{
            .description  = "Negative decimal with two places"sv,
            .scaled_value = ScaledValue{-12345, -2},
            .s            = "-123.45",
        },
        ScaledValueToStringTestInput{
            .description  = "Positive decimal with two places"sv,
            .scaled_value = ScaledValue{67890, -2},
            .s            = "678.90",
        },
        ScaledValueToStringTestInput{
            .description  = "Decimal with one place"sv,
            .scaled_value = ScaledValue{125, -1},
            .s            = "12.5",
        },
        ScaledValueToStringTestInput{
            .description  = "Decimal with three places"sv,
            .scaled_value = ScaledValue{1234, -3},
            .s            = "1.234",
        },
        ScaledValueToStringTestInput{
            .description  = "Decimal with four places"sv,
            .scaled_value = ScaledValue{98765, -4},
            .s            = "9.8765",
        },
        ScaledValueToStringTestInput{
            .description  = "Decimal with five places"sv,
            .scaled_value = ScaledValue{12345, -5},
            .s            = "0.12345",
        },
        ScaledValueToStringTestInput{
            .description  = "Very small positive decimal"sv,
            .scaled_value = ScaledValue{1, -3},
            .s            = "0.001",
        },
        ScaledValueToStringTestInput{
            .description  = "Very small negative decimal"sv,
            .scaled_value = ScaledValue{-7, -3},
            .s            = "-0.007",
        },
        // Leading digit variations
        ScaledValueToStringTestInput{
            .description  = "Single digit before decimal"sv,
            .scaled_value = ScaledValue{199, -2},
            .s            = "1.99",
        },
        ScaledValueToStringTestInput{
            .description  = "Multiple digits before decimal"sv,
            .scaled_value = ScaledValue{987654, -2},
            .s            = "9876.54",
        },
        // Trailing zeros in fraction
        ScaledValueToStringTestInput{
            .description  = "Trailing zero in fraction"sv,
            .scaled_value = ScaledValue{4560, -2},
            .s            = "45.60",
        },
        ScaledValueToStringTestInput{
            .description  = "Multiple trailing zeros"sv,
            .scaled_value = ScaledValue{123000, -4},
            .s            = "12.3000",
        },
        // Edge cases with signs
        ScaledValueToStringTestInput{
            .description  = "Negative small decimal"sv,
            .scaled_value = ScaledValue{-5, -1},
            .s            = "-0.5",
        },
        ScaledValueToStringTestInput{
            .description  = "Positive integer (no explicit sign)"sv,
            .scaled_value = ScaledValue{123, 0},
            .s            = "123",
        },
        ScaledValueToStringTestInput{
            .description  = "Positive decimal (no explicit sign)"sv,
            .scaled_value = ScaledValue{4567, -2},
            .s            = "45.67",
        },
        // Valid edge cases with single zero
        ScaledValueToStringTestInput{
            .description  = "Single zero"sv,
            .scaled_value = ScaledValue{0, 0},
            .s            = "0",
        },
        ScaledValueToStringTestInput{
            .description  = "Zero with negative scale"sv,
            .scaled_value = ScaledValue{0, -1},
            .s            = "0",
        },
        ScaledValueToStringTestInput{
            .description  = "Zero with positive scale"sv,
            .scaled_value = ScaledValue{0, 1},
            .s            = "0",
        },
        // Leading zeros handled as normalized values
        ScaledValueToStringTestInput{
            .description  = "Value that would have leading zero"sv,
            .scaled_value = ScaledValue{123, 0},
            .s            = "123",
        },
        ScaledValueToStringTestInput{
            .description  = "Decimal value normalized"sv,
            .scaled_value = ScaledValue{123, -2},
            .s            = "1.23",
        },
        ScaledValueToStringTestInput{
            .description  = "Negative value normalized"sv,
            .scaled_value = ScaledValue{-123, 0},
            .s            = "-123",
        },
        // Large numbers
        ScaledValueToStringTestInput{
            .description  = "Very large number"sv,
            .scaled_value = ScaledValue{99999999999999, 0},
            .s            = "99999999999999",
        },
        ScaledValueToStringTestInput{
            .description  = "Very large negative"sv,
            .scaled_value = ScaledValue{-99999999999999, 0},
            .s            = "-99999999999999",
        },
        ScaledValueToStringTestInput{
            .description  = "Many decimal places"sv,
            .scaled_value = ScaledValue{1123456789, -9},
            .s            = "1.123456789",
        },
        // Positive scale tests
        ScaledValueToStringTestInput{
            .description  = "Positive scale by 10"sv,
            .scaled_value = ScaledValue{7, 1},
            .s            = "70",
        },
        ScaledValueToStringTestInput{
            .description  = "Positive scale by 100"sv,
            .scaled_value = ScaledValue{5, 2},
            .s            = "500",
        },
        ScaledValueToStringTestInput{
            .description  = "Positive scale by 1000"sv,
            .scaled_value = ScaledValue{1, 3},
            .s            = "1000",
        },
        ScaledValueToStringTestInput{
            .description  = "Negative value positive scale"sv,
            .scaled_value = ScaledValue{-3, 1},
            .s            = "-30",
        },
        // Negative scale with zero fraction (evenly divisible)
        ScaledValueToStringTestInput{
            .description  = "Negative scale zero fraction"sv,
            .scaled_value = ScaledValue{10, -1},
            .s            = "1",
        },
        ScaledValueToStringTestInput{
            .description  = "Negative scale zero fraction two places"sv,
            .scaled_value = ScaledValue{200, -2},
            .s            = "2",
        },
        // Out-of-range scale returns NULL
        ScaledValueToStringTestInput{
            .description  = "Scale 19 exceeds kMaxSafeScale"sv,
            .scaled_value = ScaledValue{1, 19},
            .s            = nullptr,
        },
        ScaledValueToStringTestInput{
            .description  = "INT8_MAX scale"sv,
            .scaled_value = ScaledValue{1, INT8_MAX},
            .s            = nullptr,
        },
        ScaledValueToStringTestInput{
            .description  = "Scale -19 exceeds kMaxSafeScale"sv,
            .scaled_value = ScaledValue{1, -19},
            .s            = nullptr,
        },
        ScaledValueToStringTestInput{
            .description  = "INT8_MIN scale"sv,
            .scaled_value = ScaledValue{1, INT8_MIN},
            .s            = nullptr,
        }
    )
);

//-------------------------------------------------------------------------------------------//
//
// ScaledValueParse() test
//
//-------------------------------------------------------------------------------------------//
struct ScaledValueParseTestInput {
  std::string_view description{""sv};
  const char* s{nullptr};
  EebusError ret{kEebusErrorOk};
  ScaledValue scaled_value{0, 0};
};

std::ostream& operator<<(std::ostream& os, ScaledValueParseTestInput test_input) {
  return os << test_input.description;
}

class ScaledValueParseTests : public ::testing::TestWithParam<ScaledValueParseTestInput> {};

TEST_P(ScaledValueParseTests, ScaledValueParseTests) {
  // Arrange: Initialize the message buffer with parameters from test input
  ScaledValue scaled_value{0, 0};

  // Act: Run the scaled value parsing
  const EebusError ret = ScaledValueParse(GetParam().s, &scaled_value);

  // Assert: Verify with expected datagram fields,
  EXPECT_EQ(ret, GetParam().ret);
  if (ret == kEebusErrorOk) {
    EXPECT_EQ(scaled_value.value, GetParam().scaled_value.value);
    EXPECT_EQ(scaled_value.scale, GetParam().scaled_value.scale);
  }
}

INSTANTIATE_TEST_SUITE_P(
    ScaledValueParseTests,
    ScaledValueParseTests,
    ::testing::Values(
        // Simple integer tests
        ScaledValueParseTestInput{
            .description  = "Simple positive integer"sv,
            .s            = "123",
            .scaled_value = ScaledValue{123, 0},
},
        ScaledValueParseTestInput{
            .description  = "Simple negative integer"sv,
            .s            = "-456",
            .scaled_value = ScaledValue{-456, 0},
        },
        ScaledValueParseTestInput{
            .description  = "Single digit"sv,
            .s            = "7",
            .scaled_value = ScaledValue{7, 0},
        },
        ScaledValueParseTestInput{
            .description  = "Large positive integer"sv,
            .s            = "999999",
            .scaled_value = ScaledValue{999999, 0},
        },
        ScaledValueParseTestInput{
            .description  = "Large negative integer"sv,
            .s            = "-888888",
            .scaled_value = ScaledValue{-888888, 0},
        },
        // Decimal tests
        ScaledValueParseTestInput{
            .description  = "Negative decimal with two places"sv,
            .s            = "-123.45",
            .scaled_value = ScaledValue{-12345, -2},
        },
        ScaledValueParseTestInput{
            .description  = "Positive decimal with two places"sv,
            .s            = "678.90",
            .scaled_value = ScaledValue{67890, -2},
        },
        ScaledValueParseTestInput{
            .description  = "Decimal with one place"sv,
            .s            = "12.5",
            .scaled_value = ScaledValue{125, -1},
        },
        ScaledValueParseTestInput{
            .description  = "Decimal with three places"sv,
            .s            = "1.234",
            .scaled_value = ScaledValue{1234, -3},
        },
        ScaledValueParseTestInput{
            .description  = "Decimal with four places"sv,
            .s            = "9.8765",
            .scaled_value = ScaledValue{98765, -4},
        },
        ScaledValueParseTestInput{
            .description  = "Decimal with five places"sv,
            .s            = "0.12345",
            .scaled_value = ScaledValue{12345, -5},
        },
        ScaledValueParseTestInput{
            .description  = "Very small positive decimal"sv,
            .s            = "0.001",
            .scaled_value = ScaledValue{1, -3},
        },
        ScaledValueParseTestInput{
            .description  = "Very small negative decimal"sv,
            .s            = "-0.007",
            .scaled_value = ScaledValue{-7, -3},
        },
        // Leading digit variations
        ScaledValueParseTestInput{
            .description  = "Single digit before decimal"sv,
            .s            = "1.99",
            .scaled_value = ScaledValue{199, -2},
        },
        ScaledValueParseTestInput{
            .description  = "Multiple digits before decimal"sv,
            .s            = "9876.54",
            .scaled_value = ScaledValue{987654, -2},
        },
        // Trailing zeros in fraction
        ScaledValueParseTestInput{
            .description  = "Trailing zero in fraction"sv,
            .s            = "45.60",
            .scaled_value = ScaledValue{4560, -2},
        },
        ScaledValueParseTestInput{
            .description  = "Multiple trailing zeros"sv,
            .s            = "12.3000",
            .scaled_value = ScaledValue{123000, -4},
        },
        // Edge cases with signs
        ScaledValueParseTestInput{
            .description  = "Negative small decimal"sv,
            .s            = "-0.5",
            .scaled_value = ScaledValue{-5, -1},
        },
        ScaledValueParseTestInput{
            .description  = "Positive explicit sign"sv,
            .s            = "+123",
            .scaled_value = ScaledValue{123, 0},
        },
        ScaledValueParseTestInput{
            .description  = "Positive explicit sign with decimal"sv,
            .s            = "+45.67",
            .scaled_value = ScaledValue{4567, -2},
        },
        // Error cases - empty and null
        ScaledValueParseTestInput{
            .description  = "Empty string"sv,
            .s            = "",
            .ret          = kEebusErrorParse,
            .scaled_value = ScaledValue{0, 0},
        },
        ScaledValueParseTestInput{
            .description  = "Null pointer"sv,
            .s            = nullptr,
            .ret          = kEebusErrorInputArgumentNull,
            .scaled_value = ScaledValue{0, 0},
        },
        // Error cases - invalid characters
        ScaledValueParseTestInput{
            .description  = "Alphabetic characters"sv,
            .s            = "abc",
            .ret          = kEebusErrorParse,
            .scaled_value = ScaledValue{0, 0},
        },
        ScaledValueParseTestInput{
            .description  = "Mixed alphanumeric"sv,
            .s            = "12a34",
            .ret          = kEebusErrorParse,
            .scaled_value = ScaledValue{0, 0},
        },
        ScaledValueParseTestInput{
            .description  = "Special characters"sv,
            .s            = "12#34",
            .ret          = kEebusErrorParse,
            .scaled_value = ScaledValue{0, 0},
        },
        // Error cases - multiple decimals
        ScaledValueParseTestInput{
            .description  = "Multiple decimal points"sv,
            .s            = "12.34.56",
            .ret          = kEebusErrorParse,
            .scaled_value = ScaledValue{0, 0},
        },
        ScaledValueParseTestInput{
            .description  = "Two decimal points"sv,
            .s            = "1..2",
            .ret          = kEebusErrorParse,
            .scaled_value = ScaledValue{0, 0},
        },
        // Error cases - multiple signs
        ScaledValueParseTestInput{
            .description  = "Double negative"sv,
            .s            = "--123",
            .ret          = kEebusErrorParse,
            .scaled_value = ScaledValue{0, 0},
        },
        ScaledValueParseTestInput{
            .description  = "Multiple signs"sv,
            .s            = "+-123",
            .ret          = kEebusErrorParse,
            .scaled_value = ScaledValue{0, 0},
        },
        ScaledValueParseTestInput{
            .description  = "Sign in middle"sv,
            .s            = "12-34",
            .ret          = kEebusErrorParse,
            .scaled_value = ScaledValue{0, 0},
        },
        // Error cases - only special chars
        ScaledValueParseTestInput{
            .description  = "Only decimal point"sv,
            .s            = ".",
            .ret          = kEebusErrorParse,
            .scaled_value = ScaledValue{0, 0},
        },
        ScaledValueParseTestInput{
            .description  = "Only minus sign"sv,
            .s            = "-",
            .ret          = kEebusErrorParse,
            .scaled_value = ScaledValue{0, 0},
        },
        ScaledValueParseTestInput{
            .description  = "Only plus sign"sv,
            .s            = "+",
            .ret          = kEebusErrorParse,
            .scaled_value = ScaledValue{0, 0},
        },
        ScaledValueParseTestInput{
            .description  = "Sign with decimal only"sv,
            .s            = "-.",
            .ret          = kEebusErrorParse,
            .scaled_value = ScaledValue{0, 0},
        },
        // Error cases - whitespace
        ScaledValueParseTestInput{
            .description  = "Leading whitespace"sv,
            .s            = " 123",
            .ret          = kEebusErrorParse,
            .scaled_value = ScaledValue{0, 0},
        },
        ScaledValueParseTestInput{
            .description  = "Trailing whitespace"sv,
            .s            = "123 ",
            .ret          = kEebusErrorParse,
            .scaled_value = ScaledValue{0, 0},
        },
        ScaledValueParseTestInput{
            .description  = "Whitespace in middle"sv,
            .s            = "12 34",
            .ret          = kEebusErrorParse,
            .scaled_value = ScaledValue{0, 0},
        },
        // Edge cases - decimal at start or end
        ScaledValueParseTestInput{
            .description  = "Decimal at start"sv,
            .s            = ".123",
            .ret          = kEebusErrorParse,
            .scaled_value = ScaledValue{0, 0},
        },
        ScaledValueParseTestInput{
            .description  = "Decimal at end"sv,
            .s            = "123.",
            .ret          = kEebusErrorParse,
            .scaled_value = ScaledValue{0, 0},
        },
        // Valid edge cases with single zero
        ScaledValueParseTestInput{
            .description  = "Single zero"sv,
            .s            = "0",
            .scaled_value = ScaledValue{0, 0},
        },
        ScaledValueParseTestInput{
            .description  = "Negative zero"sv,
            .s            = "-0",
            .scaled_value = ScaledValue{0, 0},
        },
        ScaledValueParseTestInput{
            .description  = "Zero with decimal"sv,
            .s            = "0.0",
            .scaled_value = ScaledValue{0, 0},
        },
        // Leading zeros - potentially invalid depending on implementation
        ScaledValueParseTestInput{
            .description  = "Leading zero before integer"sv,
            .s            = "0123",
            .scaled_value = ScaledValue{123, 0},
        },
        ScaledValueParseTestInput{
            .description  = "Multiple leading zeros"sv,
            .s            = "00123",
            .scaled_value = ScaledValue{123, 0},
        },
        ScaledValueParseTestInput{
            .description  = "Leading zero before decimal"sv,
            .s            = "01.23",
            .scaled_value = ScaledValue{123, -2},
        },
        ScaledValueParseTestInput{
            .description  = "Negative with leading zeros"sv,
            .s            = "-00123",
            .scaled_value = ScaledValue{-123, 0},
        },
        // Overflow/underflow cases
        ScaledValueParseTestInput{
            .description  = "Very large number"sv,
            .s            = "99999999999999",
            .scaled_value = ScaledValue{99999999999999, 0},
        },
        ScaledValueParseTestInput{
            .description  = "Very large negative"sv,
            .s            = "-99999999999999",
            .scaled_value = ScaledValue{-99999999999999, 0},
        },
        ScaledValueParseTestInput{
            .description  = "Many decimal places"sv,
            .s            = "1.123456789",
            .scaled_value = ScaledValue{1123456789, -9},
        }
    )
);

//-------------------------------------------------------------------------------------------//
//
// ScaledValueToDouble() test
//
//-------------------------------------------------------------------------------------------//
struct ScaledValueToDoubleTestInput {
  std::string_view description{""sv};
  ScaledValue scaled_value{0, 0};
  bool null_scaled_value{false};
  bool null_output{false};
  EebusError expected_ret{kEebusErrorOk};
  double expected{0.0};
};

std::ostream& operator<<(std::ostream& os, ScaledValueToDoubleTestInput test_input) {
  return os << test_input.description;
}

class ScaledValueToDoubleTests : public ::testing::TestWithParam<ScaledValueToDoubleTestInput> {};

TEST_P(ScaledValueToDoubleTests, ScaledValueToDoubleTests) {
  const ScaledValue* const input = GetParam().null_scaled_value ? nullptr : &GetParam().scaled_value;
  double value{0.0};
  double* const output = GetParam().null_output ? nullptr : &value;
  EXPECT_EQ(ScaledValueToDouble(input, output), GetParam().expected_ret);
  if (GetParam().expected_ret == kEebusErrorOk) {
    EXPECT_DOUBLE_EQ(value, GetParam().expected);
  }
}

INSTANTIATE_TEST_SUITE_P(
    ScaledValueToDoubleTests,
    ScaledValueToDoubleTests,
    ::testing::Values(
        ScaledValueToDoubleTestInput{
            .description  = "Zero"sv,
            .scaled_value = ScaledValue{0, 0},
            .expected     = 0.0,
},
        ScaledValueToDoubleTestInput{
            .description  = "Positive integer"sv,
            .scaled_value = ScaledValue{123, 0},
            .expected     = 123.0,
        },
        ScaledValueToDoubleTestInput{
            .description  = "Negative integer"sv,
            .scaled_value = ScaledValue{-456, 0},
            .expected     = -456.0,
        },
        ScaledValueToDoubleTestInput{
            .description  = "Positive scale multiplies"sv,
            .scaled_value = ScaledValue{5, 2},
            .expected     = 500.0,
        },
        ScaledValueToDoubleTestInput{
            .description  = "Negative scale one decimal"sv,
            .scaled_value = ScaledValue{125, -1},
            .expected     = 12.5,
        },
        ScaledValueToDoubleTestInput{
            .description  = "Negative scale two decimals"sv,
            .scaled_value = ScaledValue{12345, -2},
            .expected     = 123.45,
        },
        ScaledValueToDoubleTestInput{
            .description  = "Negative decimal two places"sv,
            .scaled_value = ScaledValue{-12345, -2},
            .expected     = -123.45,
        },
        ScaledValueToDoubleTestInput{
            .description  = "Very small positive"sv,
            .scaled_value = ScaledValue{1, -3},
            .expected     = 0.001,
        },
        ScaledValueToDoubleTestInput{
            .description  = "Very small negative"sv,
            .scaled_value = ScaledValue{-7, -3},
            .expected     = -0.007,
        },
        ScaledValueToDoubleTestInput{
            .description  = "Four decimal places"sv,
            .scaled_value = ScaledValue{98765, -4},
            .expected     = 9.8765,
        },
        ScaledValueToDoubleTestInput{
            .description  = "Value between -1 and 0"sv,
            .scaled_value = ScaledValue{-5, -1},
            .expected     = -0.5,
        },
        ScaledValueToDoubleTestInput{
            .description  = "Large positive"sv,
            .scaled_value = ScaledValue{99999999999999, 0},
            .expected     = 99999999999999.0,
        },
        ScaledValueToDoubleTestInput{
            .description       = "Null scaled value"sv,
            .null_scaled_value = true,
            .expected_ret      = kEebusErrorInputArgumentNull,
        },
        ScaledValueToDoubleTestInput{
            .description  = "Null output"sv,
            .null_output  = true,
            .expected_ret = kEebusErrorInputArgumentNull,
        },
        ScaledValueToDoubleTestInput{
            .description  = "INT8_MIN scale"sv,
            .scaled_value = ScaledValue{1, INT8_MIN},
            .expected_ret = kEebusErrorInputArgumentOutOfRange,
        },
        ScaledValueToDoubleTestInput{
            .description  = "Scale 19 overflows PowerOfTen"sv,
            .scaled_value = ScaledValue{1, 19},
            .expected_ret = kEebusErrorInputArgumentOutOfRange,
        },
        ScaledValueToDoubleTestInput{
            .description  = "Scale -19 overflows PowerOfTen"sv,
            .scaled_value = ScaledValue{1, -19},
            .expected_ret = kEebusErrorInputArgumentOutOfRange,
        },
        ScaledValueToDoubleTestInput{
            .description  = "Scale 18 is within safe range"sv,
            .scaled_value = ScaledValue{1, 18},
            .expected     = 1e18,
        },
        ScaledValueToDoubleTestInput{
            .description  = "Scale -18 is within safe range"sv,
            .scaled_value = ScaledValue{1, -18},
            .expected     = 1e-18,
        },
        ScaledValueToDoubleTestInput{
            .description  = "Zero value with out-of-range scale returns 0.0"sv,
            .scaled_value = ScaledValue{0, 19},
            .expected     = 0.0,
        }
    )
);

//-------------------------------------------------------------------------------------------//
//
// ScaledValueWithDouble() test
//
//-------------------------------------------------------------------------------------------//
struct ScaledValueWithDoubleTestInput {
  std::string_view description{""sv};
  double value{0.0};
  EebusError expected_ret{kEebusErrorOk};
  ScaledValue expected{0, 0};
};

std::ostream& operator<<(std::ostream& os, ScaledValueWithDoubleTestInput test_input) {
  return os << test_input.description;
}

class ScaledValueWithDoubleTests : public ::testing::TestWithParam<ScaledValueWithDoubleTestInput> {};

TEST_P(ScaledValueWithDoubleTests, ScaledValueWithDoubleTests) {
  ScaledValue sv{0, 0};
  EXPECT_EQ(ScaledValueWithDouble(&sv, GetParam().value), GetParam().expected_ret);
  if (GetParam().expected_ret == kEebusErrorOk) {
    EXPECT_EQ(sv.value, GetParam().expected.value);
    EXPECT_EQ(sv.scale, GetParam().expected.scale);
  }
}

TEST(ScaledValueWithDoubleTest, NullScaledValue) {
  EXPECT_EQ(ScaledValueWithDouble(nullptr, 1.0), kEebusErrorInputArgumentNull);
}

INSTANTIATE_TEST_SUITE_P(
    ScaledValueWithDoubleTests,
    ScaledValueWithDoubleTests,
    ::testing::Values(
        ScaledValueWithDoubleTestInput{
            .description = "Zero"sv,
            .value       = 0.0,
            .expected    = ScaledValue{0, 0},
},
        ScaledValueWithDoubleTestInput{
            .description = "Positive integer uses scale 0"sv,
            .value       = 123.0,
            .expected    = ScaledValue{123, 0},
        },
        ScaledValueWithDoubleTestInput{
            .description = "Negative integer uses scale 0"sv,
            .value       = -456.0,
            .expected    = ScaledValue{-456, 0},
        },
        ScaledValueWithDoubleTestInput{
            .description = "One decimal place uses scale -1"sv,
            .value       = 12.5,
            .expected    = ScaledValue{125, -1},
        },
        ScaledValueWithDoubleTestInput{
            .description = "Negative one decimal uses scale -1"sv,
            .value       = -0.5,
            .expected    = ScaledValue{-5, -1},
        },
        ScaledValueWithDoubleTestInput{
            .description = "Two decimal places uses scale -2"sv,
            .value       = 123.45,
            .expected    = ScaledValue{12345, -2},
        },
        ScaledValueWithDoubleTestInput{
            .description = "Negative two decimals uses scale -2"sv,
            .value       = -123.45,
            .expected    = ScaledValue{-12345, -2},
        },
        ScaledValueWithDoubleTestInput{
            .description = "Three decimal places uses scale -3"sv,
            .value       = 1.234,
            .expected    = ScaledValue{1234, -3},
        },
        ScaledValueWithDoubleTestInput{
            .description = "Very small uses scale -3"sv,
            .value       = 0.001,
            .expected    = ScaledValue{1, -3},
        },
        ScaledValueWithDoubleTestInput{
            .description = "Four decimal places uses scale -4"sv,
            .value       = 9.8765,
            .expected    = ScaledValue{98765, -4},
        },
        ScaledValueWithDoubleTestInput{
            .description = "Four decimal minimum uses scale -4"sv,
            .value       = 0.0001,
            .expected    = ScaledValue{1, -4},
        },
        ScaledValueWithDoubleTestInput{
            .description = "More than four decimals falls back to scale -4"sv,
            .value       = 1.23456,
            .expected    = ScaledValue{12346, -4},
        },
        ScaledValueWithDoubleTestInput{
            .description = "Large integer uses scale 0"sv,
            .value       = 999999.0,
            .expected    = ScaledValue{999999, 0},
        },
        ScaledValueWithDoubleTestInput{
            .description  = "Positive infinity"sv,
            .value        = std::numeric_limits<double>::infinity(),
            .expected_ret = kEebusErrorInputArgumentOutOfRange,
        },
        ScaledValueWithDoubleTestInput{
            .description  = "Negative infinity"sv,
            .value        = -std::numeric_limits<double>::infinity(),
            .expected_ret = kEebusErrorInputArgumentOutOfRange,
        },
        ScaledValueWithDoubleTestInput{
            .description  = "NaN"sv,
            .value        = std::numeric_limits<double>::quiet_NaN(),
            .expected_ret = kEebusErrorInputArgumentOutOfRange,
        },
        ScaledValueWithDoubleTestInput{
            .description  = "Too large positive"sv,
            .value        = 1e15,
            .expected_ret = kEebusErrorInputArgumentOutOfRange,
        },
        ScaledValueWithDoubleTestInput{
            .description  = "Too large negative"sv,
            .value        = -1e15,
            .expected_ret = kEebusErrorInputArgumentOutOfRange,
        }
    )
);
