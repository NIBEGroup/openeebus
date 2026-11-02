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

#include <gtest/gtest.h>
#include "src/common/eebus_date_time/eebus_date_time.h"
#include "src/common/string_util.h"

//-------------------------------------------------------------------------------------------//
//
// EebusDateIsValid() test
//
//-------------------------------------------------------------------------------------------//
TEST(EebusDateIsValidTest, ValidDate) {
  static constexpr EebusDate date = {.year = 2023, .month = 10, .day = 5};
  EXPECT_TRUE(EebusDateIsValid(&date));
}

TEST(EebusDateIsValidTest, NullInput) {
  EXPECT_FALSE(EebusDateIsValid(nullptr));
}

TEST(EebusDateIsValidTest, InvalidYear) {
  static constexpr EebusDate date = {.year = 99999, .month = 10, .day = 5};
  EXPECT_FALSE(EebusDateIsValid(&date));
}

TEST(EebusDateIsValidTest, InvalidMonth) {
  static constexpr EebusDate date = {.year = 2023, .month = 13, .day = 5};
  EXPECT_FALSE(EebusDateIsValid(&date));
}

TEST(EebusDateIsValidTest, InvalidDay) {
  static constexpr EebusDate date = {.year = 2023, .month = 10, .day = 32};
  EXPECT_FALSE(EebusDateIsValid(&date));
}

TEST(EebusDateIsValidTest, LeapYearFebruary) {
  static constexpr EebusDate date = {.year = 2024, .month = 2, .day = 29};
  EXPECT_TRUE(EebusDateIsValid(&date));
}

TEST(EebusDateIsValidTest, NonLeapYearFebruary) {
  static constexpr EebusDate date = {.year = 2023, .month = 2, .day = 29};
  EXPECT_FALSE(EebusDateIsValid(&date));
}

TEST(EebusDateIsValidTest, February30th) {
  static constexpr EebusDate date = {.year = 2023, .month = 2, .day = 30};
  EXPECT_FALSE(EebusDateIsValid(&date));
}

TEST(EebusDateIsValidTest, MonthWith31Days) {
  static constexpr EebusDate date = {.year = 2023, .month = 1, .day = 31};
  EXPECT_TRUE(EebusDateIsValid(&date));
}

TEST(EebusDateIsValidTest, MonthWith30Days) {
  static constexpr EebusDate date = {.year = 2023, .month = 4, .day = 30};
  EXPECT_TRUE(EebusDateIsValid(&date));
}

TEST(EebusDateIsValidTest, MonthWith31DaysInvalidDay) {
  static constexpr EebusDate date = {.year = 2023, .month = 1, .day = 32};
  EXPECT_FALSE(EebusDateIsValid(&date));
}

TEST(EebusDateIsValidTest, MonthWith30DaysInvalidDay) {
  static constexpr EebusDate date = {.year = 2023, .month = 4, .day = 31};
  EXPECT_FALSE(EebusDateIsValid(&date));
}

//-------------------------------------------------------------------------------------------//
//
// EebusDateParse() test
//
//-------------------------------------------------------------------------------------------//
TEST(EebusDateParseTest, ValidDate) {
  EebusDate date;
  EXPECT_EQ(EebusDateParse("2023-10-05", &date), kEebusErrorOk);
  EXPECT_EQ(date.year, 2023);
  EXPECT_EQ(date.month, 10);
  EXPECT_EQ(date.day, 5);
}

TEST(EebusDateParseTest, InvalidFormat) {
  EebusDate date;
  EXPECT_EQ(EebusDateParse("10/05/2023", &date), kEebusErrorParse);
}

TEST(EebusDateParseTest, NullInput) {
  EebusDate date;
  EXPECT_EQ(EebusDateParse(nullptr, &date), kEebusErrorInputArgumentNull);
}

TEST(EebusDateParseTest, NullOutput) {
  EXPECT_EQ(EebusDateParse("2023-10-05", nullptr), kEebusErrorInputArgumentNull);
}

TEST(EebusDateParseTest, OutOfRangeYear) {
  EebusDate date;
  EXPECT_EQ(EebusDateParse("99999-10-05", &date), kEebusErrorParse);
}

TEST(EebusDateParseTest, OutOfRangeMonth) {
  EebusDate date;
  EXPECT_EQ(EebusDateParse("2023-13-05", &date), kEebusErrorParse);
}

TEST(EebusDateParseTest, OutOfRangeDay) {
  EebusDate date;
  EXPECT_EQ(EebusDateParse("2023-10-32", &date), kEebusErrorParse);
}

TEST(EebusDateParseTest, EmptyString) {
  EebusDate date;
  EXPECT_EQ(EebusDateParse("", &date), kEebusErrorParse);
}

TEST(EebusDateParseTest, LeapYearFebruary) {
  EebusDate date;
  EXPECT_EQ(EebusDateParse("2024-02-29", &date), kEebusErrorOk);
  EXPECT_EQ(date.year, 2024);
  EXPECT_EQ(date.month, 2);
  EXPECT_EQ(date.day, 29);
}

TEST(EebusDateParseTest, NonLeapYearFebruary) {
  EebusDate date;
  EXPECT_EQ(EebusDateParse("2023-02-29", &date), kEebusErrorParse);
}

TEST(EebusDateParseTest, ExtraNumbersInInput) {
  EebusDate date;
  EXPECT_EQ(EebusDateParse("2023-10-05-123", &date), kEebusErrorParse);
}

//-------------------------------------------------------------------------------------------//
//
// EebusDateToString() test
//
//-------------------------------------------------------------------------------------------//
TEST(EebusDateToStringTest, ValidDate) {
  static constexpr EebusDate date = {.year = 2023, .month = 10, .day = 5};
  std::unique_ptr<char[], decltype(&StringDelete)> s{EebusDateToString(&date), StringDelete};
  EXPECT_STREQ(s.get(), "2023-10-05");
}

TEST(EebusDateToStringTest, NullInput) {
  std::unique_ptr<char[], decltype(&StringDelete)> s{EebusDateToString(nullptr), StringDelete};
  EXPECT_EQ(s, nullptr);
}

TEST(EebusDateToStringTest, InvalidYear) {
  static constexpr EebusDate date = {.year = 99999, .month = 10, .day = 5};
  std::unique_ptr<char[], decltype(&StringDelete)> s{EebusDateToString(&date), StringDelete};
  EXPECT_EQ(s, nullptr);
}

TEST(EebusDateToStringTest, InvalidMonth) {
  static constexpr EebusDate date = {.year = 2023, .month = 13, .day = 5};
  std::unique_ptr<char[], decltype(&StringDelete)> s{EebusDateToString(&date), StringDelete};
  EXPECT_EQ(s, nullptr);
}

TEST(EebusDateToStringTest, InvalidDay) {
  static constexpr EebusDate date = {.year = 2023, .month = 10, .day = 32};
  std::unique_ptr<char[], decltype(&StringDelete)> s{EebusDateToString(&date), StringDelete};
  EXPECT_EQ(s, nullptr);
}

TEST(EebusDateToStringTest, LeapYearFebruary) {
  static constexpr EebusDate date = {.year = 2024, .month = 2, .day = 29};
  std::unique_ptr<char[], decltype(&StringDelete)> s{EebusDateToString(&date), StringDelete};
  EXPECT_STREQ(s.get(), "2024-02-29");
}

TEST(EebusDateToStringTest, NonLeapYearFebruary) {
  static constexpr EebusDate date = {.year = 2023, .month = 2, .day = 29};
  std::unique_ptr<char[], decltype(&StringDelete)> s{EebusDateToString(&date), StringDelete};
  EXPECT_EQ(s, nullptr);
}

//-------------------------------------------------------------------------------------------//
//
// EebusDateCompare() test
//
//-------------------------------------------------------------------------------------------//
TEST(EebusDateCompareTest, EqualDates) {
  static constexpr EebusDate date1 = {.year = 2023, .month = 10, .day = 5};
  static constexpr EebusDate date2 = {.year = 2023, .month = 10, .day = 5};
  EXPECT_EQ(EebusDateCompare(&date1, &date2), 0);
}

TEST(EebusDateCompareTest, FirstDateEarlier) {
  static constexpr EebusDate date1 = {.year = 2023, .month = 10, .day = 4};
  static constexpr EebusDate date2 = {.year = 2023, .month = 10, .day = 5};
  EXPECT_LT(EebusDateCompare(&date1, &date2), 0);
}

TEST(EebusDateCompareTest, SecondDateEarlier) {
  static constexpr EebusDate date1 = {.year = 2023, .month = 10, .day = 6};
  static constexpr EebusDate date2 = {.year = 2023, .month = 10, .day = 5};
  EXPECT_GT(EebusDateCompare(&date1, &date2), 0);
}

TEST(EebusDateCompareTest, DifferentYears) {
  static constexpr EebusDate date1 = {.year = 2022, .month = 10, .day = 5};
  static constexpr EebusDate date2 = {.year = 2023, .month = 10, .day = 5};
  EXPECT_LT(EebusDateCompare(&date1, &date2), 0);
}

TEST(EebusDateCompareTest, DifferentMonths) {
  static constexpr EebusDate date1 = {.year = 2023, .month = 9, .day = 5};
  static constexpr EebusDate date2 = {.year = 2023, .month = 10, .day = 5};
  EXPECT_LT(EebusDateCompare(&date1, &date2), 0);
}
