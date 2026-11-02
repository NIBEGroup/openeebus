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

#include "src/common/eebus_date_time/eebus_time.h"
#include "src/common/string_util.h"

//-------------------------------------------------------------------------------------------//
//
// EebusTimeIsValid() test
//
//-------------------------------------------------------------------------------------------//
TEST(EebusTimeTest, ValidTime) {
  EebusTime valid_time = {.hour = 12, .min = 30, .sec = 45};  // 12:30:45
  EXPECT_TRUE(EebusTimeIsValid(&valid_time));
}

TEST(EebusTimeTest, InvalidHour) {
  EebusTime invalid_hour_time = {.hour = 25, .min = 30, .sec = 45};  // Invalid hour
  EXPECT_FALSE(EebusTimeIsValid(&invalid_hour_time));
}

TEST(EebusTimeTest, InvalidMinute) {
  EebusTime invalid_minute_time = {.hour = 12, .min = 60, .sec = 45};  // Invalid minute
  EXPECT_FALSE(EebusTimeIsValid(&invalid_minute_time));
}

TEST(EebusTimeTest, InvalidSecond) {
  EebusTime invalid_second_time = {.hour = 12, .min = 30, .sec = 60};  // Invalid second
  EXPECT_FALSE(EebusTimeIsValid(&invalid_second_time));
}

TEST(EebusTimeTest, NegativeHour) {
  EebusTime negative_hour_time = {.hour = -1, .min = 30, .sec = 45};  // Negative hour
  EXPECT_FALSE(EebusTimeIsValid(&negative_hour_time));
}

TEST(EebusTimeTest, NegativeMinute) {
  EebusTime negative_minute_time = {.hour = 12, .min = -1, .sec = 45};  // Negative minute
  EXPECT_FALSE(EebusTimeIsValid(&negative_minute_time));
}

TEST(EebusTimeTest, NegativeSecond) {
  EebusTime negative_second_time = {.hour = 12, .min = 30, .sec = -1};  // Negative second
  EXPECT_FALSE(EebusTimeIsValid(&negative_second_time));
}

TEST(EebusTimeTest, EdgeCaseValidTime) {
  EebusTime edge_case_time = {.hour = 23, .min = 59, .sec = 59};  // 23:59:59
  EXPECT_TRUE(EebusTimeIsValid(&edge_case_time));
}

TEST(EebusTimeTest, EdgeCaseInvalidTime) {
  EebusTime edge_case_invalid_time = {.hour = 24, .min = 0, .sec = 0};  // Invalid 24:00:00
  EXPECT_FALSE(EebusTimeIsValid(&edge_case_invalid_time));
}

//-------------------------------------------------------------------------------------------//
//
// EebusTimeParse() test
//
//-------------------------------------------------------------------------------------------//
TEST(EebusTimeParseTest, ValidTime) {
  EebusTime parsed_time;
  EXPECT_EQ(EebusTimeParse("12:30:45", &parsed_time), kEebusErrorOk);
  EXPECT_EQ(parsed_time.hour, 12);
  EXPECT_EQ(parsed_time.min, 30);
  EXPECT_EQ(parsed_time.sec, 45);
}

TEST(EebusTimeParseTest, InvalidFormat) {
  EebusTime parsed_time;
  EXPECT_EQ(EebusTimeParse("12-30-45", &parsed_time), kEebusErrorParse);
}

TEST(EebusTimeParseTest, MissingFields) {
  EebusTime parsed_time;
  EXPECT_EQ(EebusTimeParse("12:30", &parsed_time), kEebusErrorParse);
}

TEST(EebusTimeParseTest, ExtraFields) {
  EebusTime parsed_time;
  EXPECT_EQ(EebusTimeParse("12:30:45:67", &parsed_time), kEebusErrorParse);
}

TEST(EebusTimeParseTest, NonNumericCharacters) {
  EebusTime parsed_time;
  EXPECT_EQ(EebusTimeParse("12:ab:45", &parsed_time), kEebusErrorParse);
}

TEST(EebusTimeParseTest, OutOfRangeHour) {
  EebusTime parsed_time;
  EXPECT_EQ(EebusTimeParse("25:30:45", &parsed_time), kEebusErrorParse);
}

TEST(EebusTimeParseTest, OutOfRangeMinute) {
  EebusTime parsed_time;
  EXPECT_EQ(EebusTimeParse("12:60:45", &parsed_time), kEebusErrorParse);
}

TEST(EebusTimeParseTest, OutOfRangeSecond) {
  EebusTime parsed_time;
  EXPECT_EQ(EebusTimeParse("12:30:60", &parsed_time), kEebusErrorParse);
}

TEST(EebusTimeParseTest, NegativeHour) {
  EebusTime parsed_time;
  EXPECT_EQ(EebusTimeParse("-1:30:45", &parsed_time), kEebusErrorParse);
}

TEST(EebusTimeParseTest, NegativeMinute) {
  EebusTime parsed_time;
  EXPECT_EQ(EebusTimeParse("12:-1:45", &parsed_time), kEebusErrorParse);
}

TEST(EebusTimeParseTest, NegativeSecond) {
  EebusTime parsed_time;
  EXPECT_EQ(EebusTimeParse("12:30:-1", &parsed_time), kEebusErrorParse);
}

TEST(EebusTimeParseTest, EdgeCaseValidTime) {
  EebusTime parsed_time;
  EXPECT_EQ(EebusTimeParse("23:59:59", &parsed_time), kEebusErrorOk);
  EXPECT_EQ(parsed_time.hour, 23);
  EXPECT_EQ(parsed_time.min, 59);
  EXPECT_EQ(parsed_time.sec, 59);
}

TEST(EebusTimeParseTest, EdgeCaseInvalidTime) {
  EebusTime parsed_time;
  EXPECT_EQ(EebusTimeParse("24:00:00", &parsed_time), kEebusErrorParse);
}

TEST(EebusTimeParseTest, NullPointerInput) {
  EXPECT_EQ(EebusTimeParse(nullptr, nullptr), kEebusErrorInputArgumentNull);
}

TEST(EebusTimeParseTest, NullPointerOutput) {
  EXPECT_EQ(EebusTimeParse("12:30:45", nullptr), kEebusErrorInputArgumentNull);
}

TEST(EebusTimeParseTest, NullPointerInputValidOutput) {
  EebusTime parsed_time;
  EXPECT_EQ(EebusTimeParse(nullptr, &parsed_time), kEebusErrorInputArgumentNull);
}

//-------------------------------------------------------------------------------------------//
//
// EebusTimeToString() test
//
//-------------------------------------------------------------------------------------------//
TEST(EebusTimeToStringTest, SingleDigitTime) {
  static constexpr EebusTime single_digit_time = {.hour = 1, .min = 2, .sec = 3};  // 01:02:03
  std::unique_ptr<char[], decltype(&StringDelete)> s(EebusTimeToString(&single_digit_time), &StringDelete);
  ASSERT_NE(s, nullptr);
  EXPECT_STREQ(s.get(), "01:02:03Z");
}

TEST(EebusTimeToStringTest, NoonTime) {
  static constexpr EebusTime noon_time = {.hour = 12, .min = 0, .sec = 0};  // 12:00:00
  std::unique_ptr<char[], decltype(&StringDelete)> s(EebusTimeToString(&noon_time), &StringDelete);
  ASSERT_NE(s, nullptr);
  EXPECT_STREQ(s.get(), "12:00:00Z");
}

TEST(EebusTimeToStringTest, InvalidNegativeTime) {
  static constexpr EebusTime negative_time = {.hour = -1, .min = -1, .sec = -1};  // Invalid negative time
  std::unique_ptr<char[], decltype(&StringDelete)> s(EebusTimeToString(&negative_time), &StringDelete);
  EXPECT_EQ(s, nullptr);
}

TEST(EebusTimeToStringTest, NullPointerInput) {
  std::unique_ptr<char[], decltype(&StringDelete)> s(EebusTimeToString(nullptr), &StringDelete);
  EXPECT_EQ(s, nullptr);
}

TEST(EebusTimeToStringTest, LargeInvalidTime) {
  static constexpr EebusTime large_time = {.hour = 100, .min = 100, .sec = 100};  // Invalid large time
  std::unique_ptr<char[], decltype(&StringDelete)> s(EebusTimeToString(&large_time), &StringDelete);
  EXPECT_EQ(s, nullptr);
}

//-------------------------------------------------------------------------------------------//
//
// EebusTimeCompare() test
//
//-------------------------------------------------------------------------------------------//
TEST(EebusTimeCompareTest, EqualTimes) {
  EebusTime time1 = {.hour = 12, .min = 30, .sec = 45};
  EebusTime time2 = {.hour = 12, .min = 30, .sec = 45};
  EXPECT_EQ(EebusTimeCompare(&time1, &time2), 0);
}

TEST(EebusTimeCompareTest, Time1EarlierThanTime2) {
  EebusTime time1 = {.hour = 11, .min = 30, .sec = 45};
  EebusTime time2 = {.hour = 12, .min = 30, .sec = 45};
  EXPECT_LT(EebusTimeCompare(&time1, &time2), 0);
}

TEST(EebusTimeCompareTest, Time1LaterThanTime2) {
  EebusTime time1 = {.hour = 13, .min = 30, .sec = 45};
  EebusTime time2 = {.hour = 12, .min = 30, .sec = 45};
  EXPECT_GT(EebusTimeCompare(&time1, &time2), 0);
}

TEST(EebusTimeCompareTest, CompareMinutes) {
  EebusTime time1 = {.hour = 12, .min = 29, .sec = 45};
  EebusTime time2 = {.hour = 12, .min = 30, .sec = 45};
  EXPECT_LT(EebusTimeCompare(&time1, &time2), 0);
}

TEST(EebusTimeCompareTest, CompareSeconds) {
  EebusTime time1 = {.hour = 12, .min = 30, .sec = 44};
  EebusTime time2 = {.hour = 12, .min = 30, .sec = 45};
  EXPECT_LT(EebusTimeCompare(&time1, &time2), 0);
}

TEST(EebusTimeCompareTest, EdgeCaseMidnightComparison) {
  EebusTime time1 = {.hour = 0, .min = 0, .sec = 0};     // Midnight
  EebusTime time2 = {.hour = 23, .min = 59, .sec = 59};  // One second before midnight
  EXPECT_LT(EebusTimeCompare(&time1, &time2), 0);
}
