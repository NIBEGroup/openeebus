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

#include <string_view>

#include <gtest/gtest.h>

#include "src/common/eebus_date_time/eebus_duration.h"
#include "src/common/string_util.h"

using std::literals::string_view_literals::operator""sv;

//-------------------------------------------------------------------------------------------//
//
// EebusDurationInvertSign() test
//
//-------------------------------------------------------------------------------------------//
struct EebusDurationInvertSignInput {
  std::string_view description    = ""sv;
  EebusDuration duration_input    = {0};
  EebusDuration duration_expected = {0};
};

std::ostream& operator<<(std::ostream& os, EebusDurationInvertSignInput test_input) {
  return os << test_input.description;
}

class EebusDurationInvertSignTests : public ::testing::TestWithParam<EebusDurationInvertSignInput> {};

TEST_P(EebusDurationInvertSignTests, EebusDurationInvertSignTests) {
  EebusDuration duration_input = GetParam().duration_input;
  EebusDurationInvertSign(&duration_input);
  EXPECT_EQ(duration_input.years, GetParam().duration_expected.years);
  EXPECT_EQ(duration_input.months, GetParam().duration_expected.months);
  EXPECT_EQ(duration_input.days, GetParam().duration_expected.days);
  EXPECT_EQ(duration_input.hours, GetParam().duration_expected.hours);
  EXPECT_EQ(duration_input.minutes, GetParam().duration_expected.minutes);
  EXPECT_EQ(duration_input.seconds, GetParam().duration_expected.seconds);
  EXPECT_EQ(duration_input.milliseconds, GetParam().duration_expected.milliseconds);
}

INSTANTIATE_TEST_SUITE_P(
    EebusDurationInvertSignTests,
    EebusDurationInvertSignTests,
    ::testing::Values(
        EebusDurationInvertSignInput{
            .description = "Positive to Negative",
            .duration_input
            = { .years = 1,  .months = 2,  .days = 3,  .hours = 4,  .minutes = 5,  .seconds = 6,  .milliseconds = 700},
            .duration_expected
            = {.years = -1, .months = -2, .days = -3, .hours = -4, .minutes = -5, .seconds = -6, .milliseconds = -700},
},
        EebusDurationInvertSignInput{
            .description = "Negative to Positive",
            .duration_input
            = {.years = -1, .months = -2, .days = -3, .hours = -4, .minutes = -5, .seconds = -6, .milliseconds = -700},
            .duration_expected
            = {.years = 1, .months = 2, .days = 3, .hours = 4, .minutes = 5, .seconds = 6, .milliseconds = 700},
        },
        EebusDurationInvertSignInput{
            .description = "Zero duration",
            .duration_input
            = {.years = 0, .months = 0, .days = 0, .hours = 0, .minutes = 0, .seconds = 0, .milliseconds = 0},
            .duration_expected
            = {.years = 0, .months = 0, .days = 0, .hours = 0, .minutes = 0, .seconds = 0, .milliseconds = 0},
        }
    )
);

//-------------------------------------------------------------------------------------------//
//
// EebusDurationIsValid() test
//
//-------------------------------------------------------------------------------------------//
struct EebusDurationIsValidInput {
  std::string_view description = ""sv;
  EebusDuration duration       = {0};
  bool is_valid                = false;
};

std::ostream& operator<<(std::ostream& os, EebusDurationIsValidInput test_input) {
  return os << test_input.description;
}

class EebusDurationIsValidTests : public ::testing::TestWithParam<EebusDurationIsValidInput> {};

TEST_P(EebusDurationIsValidTests, EebusDurationIsValidTests) {
  EXPECT_EQ(EebusDurationIsValid(&GetParam().duration), GetParam().is_valid);
}

INSTANTIATE_TEST_SUITE_P(
    EebusDurationIsValidTests,
    EebusDurationIsValidTests,
    ::testing::Values(
        EebusDurationIsValidInput{
            .description = "All fields zero",
            .duration = {.years = 0, .months = 0, .days = 0, .hours = 0, .minutes = 0, .seconds = 0, .milliseconds = 0},
            .is_valid = true,
},
        EebusDurationIsValidInput{
            .description = "All fields positive",
            .duration
            = {.years = 1, .months = 2, .days = 3, .hours = 4, .minutes = 5, .seconds = 6, .milliseconds = 700},
            .is_valid = true,
        },
        EebusDurationIsValidInput{
            .description = "All fields negative",
            .duration
            = {.years = -1, .months = -2, .days = -3, .hours = -4, .minutes = -5, .seconds = -6, .milliseconds = -700},
            .is_valid = true,
        },
        EebusDurationIsValidInput{
            .description = "Negative year, others positive",
            .duration
            = {.years = -1, .months = 2, .days = 3, .hours = 4, .minutes = 5, .seconds = 6, .milliseconds = 700},
            .is_valid = false,
        },
        EebusDurationIsValidInput{
            .description = "Negative month, others positive",
            .duration
            = {.years = 1, .months = -2, .days = 3, .hours = 4, .minutes = 5, .seconds = 6, .milliseconds = 700},
            .is_valid = false,
        },
        EebusDurationIsValidInput{
            .description = "Negative day, others positive",
            .duration
            = {.years = 1, .months = 2, .days = -3, .hours = 4, .minutes = 5, .seconds = 6, .milliseconds = 700},
            .is_valid = false,
        },
        EebusDurationIsValidInput{
            .description = "Positive hours, others positive",
            .duration
            = {.years = -1, .months = -2, .days = -3, .hours = 4, .minutes = -5, .seconds = -6, .milliseconds = -700},
            .is_valid = false,
        },
        EebusDurationIsValidInput{
            .description = "Positive minutes, others positive",
            .duration
            = {.years = -1, .months = -2, .days = -3, .hours = -4, .minutes = 5, .seconds = -6, .milliseconds = -700},
            .is_valid = false,
        },
        EebusDurationIsValidInput{
            .description = "Positive seconds, others positive",
            .duration
            = {.years = -1, .months = -2, .days = -3, .hours = -4, .minutes = -5, .seconds = 6, .milliseconds = -700},
            .is_valid = false,
        },
        EebusDurationIsValidInput{
            .description = "Positive milliseconds, others positive",
            .duration
            = {.years = -1, .months = -2, .days = -3, .hours = -4, .minutes = -5, .seconds = -6, .milliseconds = 700},
            .is_valid = false,
        }
    )
);

//-------------------------------------------------------------------------------------------//
//
// EebusDurationParse() test
//
//-------------------------------------------------------------------------------------------//
struct EebusDurationParseInput {
  std::string_view description    = ""sv;
  const char* duration_string     = nullptr;
  EebusError expected_ret         = kEebusErrorOk;
  EebusDuration duration_expected = {0};
};

std::ostream& operator<<(std::ostream& os, EebusDurationParseInput test_input) {
  return os << test_input.description;
}

class EebusDurationParseTests : public ::testing::TestWithParam<EebusDurationParseInput> {};

TEST_P(EebusDurationParseTests, EebusDurationParseTests) {
  EebusDuration duration{0};
  const EebusError ret = EebusDurationParse(GetParam().duration_string, &duration);
  ASSERT_EQ(GetParam().expected_ret, ret);
  if (ret == kEebusErrorOk) {
    EXPECT_EQ(GetParam().duration_expected.years, duration.years);
    EXPECT_EQ(GetParam().duration_expected.months, duration.months);
    EXPECT_EQ(GetParam().duration_expected.days, duration.days);
    EXPECT_EQ(GetParam().duration_expected.hours, duration.hours);
    EXPECT_EQ(GetParam().duration_expected.minutes, duration.minutes);
    EXPECT_EQ(GetParam().duration_expected.seconds, duration.seconds);
    EXPECT_EQ(GetParam().duration_expected.milliseconds, duration.milliseconds);
  }
}

INSTANTIATE_TEST_SUITE_P(
    EebusDurationParseTests,
    EebusDurationParseTests,
    ::testing::Values(
        EebusDurationParseInput{
            .description       = "3 milliseconds",
            .duration_string   = "PT0.003S",
            .duration_expected = {.hours = 0, .minutes = 0, .seconds = 0, .milliseconds = 3},
},
        EebusDurationParseInput{
            .description       = "45 seconds",
            .duration_string   = "PT45S",
            .duration_expected = {.hours = 0, .minutes = 0, .seconds = 45, .milliseconds = 0},
        },
        EebusDurationParseInput{
            .description       = "20 minutes",
            .duration_string   = "PT20M",
            .duration_expected = {.hours = 0, .minutes = 20, .seconds = 0, .milliseconds = 0},
        },
        EebusDurationParseInput{
            .description       = "2 hours",
            .duration_string   = "PT2H",
            .duration_expected = {.hours = 2, .minutes = 0, .seconds = 0, .milliseconds = 0},
        },
        EebusDurationParseInput{
            .description       = "5 days",
            .duration_string   = "P5D",
            .duration_expected = {.years = 0, .months = 0, .days = 5},
        },
        EebusDurationParseInput{
            .description       = "2 months",
            .duration_string   = "P2M",
            .duration_expected = {.years = 0, .months = 2, .days = 0},
        },
        EebusDurationParseInput{
            .description       = "1 year",
            .duration_string   = "P1Y",
            .duration_expected = {.years = 1, .months = 0, .days = 0},
        },
        EebusDurationParseInput{
            .description       = "Valid duration 0 seconds",
            .duration_string   = "PT0S",
            .duration_expected = {.hours = 0, .minutes = 0, .seconds = 0, .milliseconds = 0},
        },
        EebusDurationParseInput{
            .description       = "Valid duration 1 hour 30 minutes 15 seconds",
            .duration_string   = "PT1H30M15S",
            .duration_expected = {.hours = 1, .minutes = 30, .seconds = 15, .milliseconds = 0},
        },
        EebusDurationParseInput{
            .description       = "Valid duration -2 hours 45 minutes",
            .duration_string   = "-PT2H45M",
            .duration_expected = {.hours = -2, .minutes = -45, .seconds = 0, .milliseconds = 0},
        },
        EebusDurationParseInput{
            .description       = "1 hour, 30 minutes, 15 seconds, 500 milliseconds",
            .duration_string   = "PT1H30M15.500S",
            .duration_expected = {.hours = 1, .minutes = 30, .seconds = 15, .milliseconds = 500},
        },
        EebusDurationParseInput{
            .description     = "1 year, 2 months, 3 days, 4 hours, 5 minutes, 6 seconds",
            .duration_string = "P1Y2M3DT4H5M6S",
            .duration_expected
            = {.years = 1, .months = 2, .days = 3, .hours = 4, .minutes = 5, .seconds = 6, .milliseconds = 0},
        },
        EebusDurationParseInput{
            .description     = "Negative 1 year, 2 months, 3 days, 4 hours, 5 minutes, 6, seconds, 25 milliseconds",
            .duration_string = "-P1Y2M3DT4H5M6.025S",
            .duration_expected
            = {.years = -1, .months = -2, .days = -3, .hours = -4, .minutes = -5, .seconds = -6, .milliseconds = -25},
        },
        EebusDurationParseInput{
            .description     = "6 years, 1 month, 4 days, 11 hours, 6 seconds, 3 milliseconds",
            .duration_string = "P6Y1M4DT11H6.003S",
            .duration_expected
            = {.years = 6, .months = 1, .days = 4, .hours = 11, .minutes = 0, .seconds = 6, .milliseconds = 3},
        },
        EebusDurationParseInput{
            .description     = "Null input",
            .duration_string = nullptr,
            .expected_ret    = kEebusErrorInputArgumentNull,
        },
        EebusDurationParseInput{
            .description     = "Invalid input",
            .duration_string = "InvalidDuration",
            .expected_ret    = kEebusErrorParse,
        },
        EebusDurationParseInput{
            .description     = "Empty input",
            .duration_string = "",
            .expected_ret    = kEebusErrorParse,
        },
        EebusDurationParseInput{
            .description     = "Missing 'P' designator",
            .duration_string = "1H30M",
            .expected_ret    = kEebusErrorParse,
        }
    )
);

//-------------------------------------------------------------------------------------------//
//
// EebusDurationToString() test
//
//-------------------------------------------------------------------------------------------//
struct EebusDurationToStringInput {
  std::string_view description = ""sv;
  EebusDuration duration       = {0};
  const char* duration_string  = nullptr;
};

std::ostream& operator<<(std::ostream& os, EebusDurationToStringInput test_input) {
  return os << test_input.description;
}

class EebusDurationToStringTests : public ::testing::TestWithParam<EebusDurationToStringInput> {};

TEST_P(EebusDurationToStringTests, EebusDurationToStringTests) {
  std::unique_ptr<char[], decltype(&StringDelete)> s(EebusDurationToString(&GetParam().duration), &StringDelete);
  EXPECT_STREQ(GetParam().duration_string, s.get());
}

INSTANTIATE_TEST_SUITE_P(
    EebusDurationToStringTests,
    EebusDurationToStringTests,
    ::testing::Values(
        EebusDurationToStringInput{
            .description     = "Zero duration",
            .duration        = {.hours = 0, .minutes = 0, .seconds = 0, .milliseconds = 0},
            .duration_string = "PT0S",
},
        EebusDurationToStringInput{
            .description     = "1 hours, 30 minutes and 15 seconds",
            .duration        = {.hours = 1, .minutes = 30, .seconds = 15, .milliseconds = 0},
            .duration_string = "PT1H30M15S",
        },
        EebusDurationToStringInput{
            .description     = "45 seconds",
            .duration        = {.hours = 0, .minutes = 0, .seconds = 45, .milliseconds = 0},
            .duration_string = "PT45S",
        },
        EebusDurationToStringInput{
            .description     = "37 seconds and 500 milliseconds",
            .duration        = {.hours = 0, .minutes = 0, .seconds = 37, .milliseconds = 500},
            .duration_string = "PT37.500S",
        },
        EebusDurationToStringInput{
            .description     = "11 seconds and 33 milliseconds",
            .duration        = {.hours = 0, .minutes = 0, .seconds = 11, .milliseconds = 33},
            .duration_string = "PT11.033S",
        },
        EebusDurationToStringInput{
            .description     = "8 seconds and 2 milliseconds",
            .duration        = {.hours = 0, .minutes = 0, .seconds = 8, .milliseconds = 2},
            .duration_string = "PT8.002S",
        },
        EebusDurationToStringInput{
            .description     = "Negative 1 seconds and 7 milliseconds",
            .duration        = {.hours = 0, .minutes = 0, .seconds = -1, .milliseconds = -7},
            .duration_string = "-PT1.007S",
        },
        EebusDurationToStringInput{
            .description     = "20 minutes",
            .duration        = {.hours = 0, .minutes = 20, .seconds = 0, .milliseconds = 0},
            .duration_string = "PT20M",
        },
        EebusDurationToStringInput{
            .description     = "2 hours",
            .duration        = {.hours = 2, .minutes = 0, .seconds = 0, .milliseconds = 0},
            .duration_string = "PT2H",
        }
    )
);

//-------------------------------------------------------------------------------------------//
//
// EebusDurationToSeconds() test
//
//-------------------------------------------------------------------------------------------//
struct EebusDurationToSecondsInput {
  std::string_view description = ""sv;
  EebusDuration duration       = {0};
  int64_t expected_seconds     = 0;
};

std::ostream& operator<<(std::ostream& os, EebusDurationToSecondsInput test_input) {
  return os << test_input.description;
}

class EebusDurationToSecondsTests : public ::testing::TestWithParam<EebusDurationToSecondsInput> {};

TEST_P(EebusDurationToSecondsTests, EebusDurationToSecondsTests) {
  EXPECT_EQ(EebusDurationToSeconds(&GetParam().duration), GetParam().expected_seconds);
}

INSTANTIATE_TEST_SUITE_P(
    EebusDurationToSecondsTests,
    EebusDurationToSecondsTests,
    ::testing::Values(
        EebusDurationToSecondsInput{
            .description      = "1 hour, 30 minutes, 15 seconds (1*3600 + 30*60 + 15 = 5415 seconds)",
            .duration         = {.hours = 1, .minutes = 30, .seconds = 15, .milliseconds = 0},
            .expected_seconds = 5415,
},
        EebusDurationToSecondsInput{
            .description      = "-1 hour, -30 minutes, -15 seconds (-(1*3600 + 30*60 + 15) = -5415 seconds)",
            .duration         = {.hours = -1, .minutes = -30, .seconds = -15, .milliseconds = 0},
            .expected_seconds = -5415,
        },
        EebusDurationToSecondsInput{
            .description      = "45 seconds",
            .duration         = {.hours = 0, .minutes = 0, .seconds = 45, .milliseconds = 0},
            .expected_seconds = 45,
        },
        EebusDurationToSecondsInput{
            .description      = "-45 seconds",
            .duration         = {.hours = 0, .minutes = 0, .seconds = -45, .milliseconds = 0},
            .expected_seconds = -45,
        },
        EebusDurationToSecondsInput{
            .description      = "20 minutes (20*60 = 1200 seconds)",
            .duration         = {.hours = 0, .minutes = 20, .seconds = 0, .milliseconds = 0},
            .expected_seconds = 1200,
        },
        EebusDurationToSecondsInput{
            .description      = "-20 minutes (-(20*60) = -1200 seconds)",
            .duration         = {.hours = 0, .minutes = -20, .seconds = 0, .milliseconds = 0},
            .expected_seconds = -1200,
        },
        EebusDurationToSecondsInput{
            .description      = "2 hours (2*3600 = 7200 seconds)",
            .duration         = {.hours = 2, .minutes = 0, .seconds = 0, .milliseconds = 0},
            .expected_seconds = 7200,
        },
        EebusDurationToSecondsInput{
            .description      = "-2 hours (-(2*3600) = -7200 seconds)",
            .duration         = {.hours = -2, .minutes = 0, .seconds = 0, .milliseconds = 0},
            .expected_seconds = -7200,
        },
        EebusDurationToSecondsInput{
            .description      = "Zero duration",
            .duration         = {0},
            .expected_seconds = 0,
        }
    )
);

//-------------------------------------------------------------------------------------------//
//
// EebusDurationCompare() test
//
//-------------------------------------------------------------------------------------------//
struct EebusDurationCompareInput {
  std::string_view description = ""sv;
  EebusDuration duration1      = {0};
  EebusDuration duration2      = {0};
  int32_t expected_result      = 0;
};

std::ostream& operator<<(std::ostream& os, EebusDurationCompareInput test_input) {
  return os << test_input.description;
}

class EebusDurationCompareTests : public ::testing::TestWithParam<EebusDurationCompareInput> {};

TEST_P(EebusDurationCompareTests, EebusDurationCompareTests) {
  const int32_t cmp_result = EebusDurationCompare(&GetParam().duration1, &GetParam().duration2);
  if (GetParam().expected_result > 0) {
    EXPECT_GT(cmp_result, 0);
  } else if (GetParam().expected_result < 0) {
    EXPECT_LT(cmp_result, 0);
  } else {
    EXPECT_EQ(cmp_result, 0);
  }
}

INSTANTIATE_TEST_SUITE_P(
    EebusDurationCompareTests,
    EebusDurationCompareTests,
    ::testing::Values(
        EebusDurationCompareInput{
            .description     = "1 hour 30 minutes 15 seconds equal",
            .duration1       = {.hours = 1, .minutes = 30, .seconds = 15, .milliseconds = 0},
            .duration2       = {.hours = 1, .minutes = 30, .seconds = 15, .milliseconds = 0},
            .expected_result = 0,
},
        EebusDurationCompareInput{
            .description     = "Greater year",
            .duration1       = {.years = 1, .hours = 1, .minutes = 30, .seconds = 15, .milliseconds = 0},
            .duration2       = {.hours = 1, .minutes = 30, .seconds = 15, .milliseconds = 0},
            .expected_result = 1,
        },
        EebusDurationCompareInput{
            .description     = "Less year",
            .duration1       = {.years = 0, .hours = 1, .minutes = 30, .seconds = 15, .milliseconds = 0},
            .duration2       = {.years = 1, .hours = 1, .minutes = 30, .seconds = 15, .milliseconds = 0},
            .expected_result = -1,
        },
        EebusDurationCompareInput{
            .description     = "Greater months",
            .duration1       = {.months = 1, .hours = 0, .minutes = 11, .seconds = 4, .milliseconds = 0},
            .duration2       = {.months = 0, .hours = 1, .minutes = 30, .seconds = 15, .milliseconds = 0},
            .expected_result = 1,
        },
        EebusDurationCompareInput{
            .description     = "Less months",
            .duration1       = {.months = 0, .hours = 2, .minutes = 31, .seconds = 16, .milliseconds = 0},
            .duration2       = {.months = 1, .hours = 1, .minutes = 30, .seconds = 15, .milliseconds = 0},
            .expected_result = -1,
        },
        EebusDurationCompareInput{
            .description     = "Greater days",
            .duration1       = {.days = 2, .hours = 5, .minutes = 20, .seconds = 1, .milliseconds = 0},
            .duration2       = {.days = 1, .hours = 11, .minutes = 30, .seconds = 15, .milliseconds = 0},
            .expected_result = 1,
        },
        EebusDurationCompareInput{
            .description     = "Less days",
            .duration1       = {.days = 1, .hours = 9, .minutes = 41, .seconds = 25, .milliseconds = 11},
            .duration2       = {.days = 2, .hours = 1, .minutes = 30, .seconds = 15, .milliseconds = 0},
            .expected_result = -1,
        },
        EebusDurationCompareInput{
            .description     = "Greater hours",
            .duration1       = {.hours = 3, .minutes = 0, .seconds = 0, .milliseconds = 0},
            .duration2       = {.hours = 2, .minutes = 59, .seconds = 59, .milliseconds = 999},
            .expected_result = 1,
        },
        EebusDurationCompareInput{
            .description     = "Less hours",
            .duration1       = {.hours = 10, .minutes = 59, .seconds = 59, .milliseconds = 999},
            .duration2       = {.hours = 11, .minutes = 0, .seconds = 0, .milliseconds = 0},
            .expected_result = -1,
        },
        EebusDurationCompareInput{
            .description     = "Greater minutes",
            .duration1       = {.hours = 1, .minutes = 31, .seconds = 0, .milliseconds = 0},
            .duration2       = {.hours = 1, .minutes = 30, .seconds = 59, .milliseconds = 999},
            .expected_result = 1,
        },
        EebusDurationCompareInput{
            .description     = "Less minutes",
            .duration1       = {.hours = 5, .minutes = 29, .seconds = 59, .milliseconds = 999},
            .duration2       = {.hours = 5, .minutes = 30, .seconds = 0, .milliseconds = 0},
            .expected_result = -1,
        },
        EebusDurationCompareInput{
            .description     = "Greater seconds",
            .duration1       = {.hours = 0, .minutes = 0, .seconds = 46, .milliseconds = 0},
            .duration2       = {.hours = 0, .minutes = 0, .seconds = 45, .milliseconds = 999},
            .expected_result = 1,
        },
        EebusDurationCompareInput{
            .description     = "Less seconds",
            .duration1       = {.hours = 0, .minutes = 0, .seconds = 29, .milliseconds = 999},
            .duration2       = {.hours = 0, .minutes = 0, .seconds = 30, .milliseconds = 0},
            .expected_result = -1,
        },
        EebusDurationCompareInput{
            .description     = "Greater milliseconds",
            .duration1       = {.hours = 0, .minutes = 0, .seconds = 0, .milliseconds = 501},
            .duration2       = {.hours = 0, .minutes = 0, .seconds = 0, .milliseconds = 500},
            .expected_result = 1,
        },
        EebusDurationCompareInput{
            .description     = "Less milliseconds",
            .duration1       = {.hours = 0, .minutes = 0, .seconds = 0, .milliseconds = 499},
            .duration2       = {.hours = 0, .minutes = 0, .seconds = 0, .milliseconds = 500},
            .expected_result = -1,
        }
    )
);
