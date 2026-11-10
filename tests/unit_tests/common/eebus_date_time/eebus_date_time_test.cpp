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

#include "src/common/eebus_date_time/eebus_date_time.h"
#include "src/common/string_util.h"

//-------------------------------------------------------------------------------------------//
//
// EebusDateTimeParse() test
//
//-------------------------------------------------------------------------------------------//
TEST(EebusDateTimeParseTest, ValidDateTime) {
  EebusDateTime date_time;
  EXPECT_EQ(EebusDateTimeParse("2023-10-05T14:30:00Z", &date_time), kEebusErrorOk);
  EXPECT_EQ(date_time.date.year, 2023);
  EXPECT_EQ(date_time.date.month, 10);
  EXPECT_EQ(date_time.date.day, 5);
  EXPECT_EQ(date_time.time.hour, 14);
  EXPECT_EQ(date_time.time.min, 30);
  EXPECT_EQ(date_time.time.sec, 0);
}

TEST(EebusDateTimeParseTest, InvalidDateTimeFormat) {
  EebusDateTime date_time;
  EXPECT_EQ(EebusDateTimeParse("2023-10-05 14:30:00", &date_time), kEebusErrorParse);
}

TEST(EebusDateTimeParseTest, NullInputString) {
  EebusDateTime date_time;
  EXPECT_EQ(EebusDateTimeParse(nullptr, &date_time), kEebusErrorInputArgumentNull);
}

TEST(EebusDateTimeParseTest, NullOutputPointer) {
  EXPECT_EQ(EebusDateTimeParse("2023-10-05T14:30:00Z", nullptr), kEebusErrorInputArgumentNull);
}

TEST(EebusDateTimeParseTest, InvalidDateValues) {
  EebusDateTime date_time;
  EXPECT_EQ(EebusDateTimeParse("2023-13-32T25:61:61Z", &date_time), kEebusErrorParse);
}

//-------------------------------------------------------------------------------------------//
//
// EebusDateTimeToString() test
//
//-------------------------------------------------------------------------------------------//
TEST(EebusDateTimeToStringTest, ValidDateTime) {
  EebusDateTime date_time = {
      .date = {.year = 2023, .month = 10, .day = 5},
      .time = {  .hour = 14,   .min = 30, .sec = 0}
  };

  std::unique_ptr<char[], decltype(&StringDelete)> buffer_ptr(EebusDateTimeToString(&date_time), StringDelete);

  ASSERT_NE(buffer_ptr, nullptr);
  EXPECT_STREQ(buffer_ptr.get(), "2023-10-05T14:30:00Z");
}

TEST(EebusDateTimeToStringTest, NullInputPointer) {
  std::unique_ptr<char[], decltype(&StringDelete)> buffer_ptr(EebusDateTimeToString(nullptr), StringDelete);

  EXPECT_EQ(buffer_ptr, nullptr);
}

TEST(EebusDateTimeToStringTest, InvalidDateValues) {
  EebusDateTime date_time = {
      .date = {.year = 2023, .month = 13, .day = 32},
      .time = {  .hour = 14,   .min = 30,  .sec = 0}
  };

  std::unique_ptr<char[], decltype(&StringDelete)> buffer_ptr(EebusDateTimeToString(&date_time), StringDelete);

  EXPECT_EQ(buffer_ptr, nullptr);
}

TEST(EebusDateTimeToStringTest, InvalidTimeValues) {
  EebusDateTime date_time = {
      .date = {.year = 2023, .month = 10,  .day = 5},
      .time = {  .hour = 25,   .min = 61, .sec = 61}
  };

  std::unique_ptr<char[], decltype(&StringDelete)> buffer_ptr(EebusDateTimeToString(&date_time), StringDelete);

  EXPECT_EQ(buffer_ptr, nullptr);
}

//-------------------------------------------------------------------------------------------//
//
// EebusDateTimeAddDuration() test
//
//-------------------------------------------------------------------------------------------//
TEST(EebusDateTimeAddDurationTest, AddValidDuration) {
  EebusDateTime date_time = {
      .date = {.year = 2023, .month = 10, .day = 5},
      .time = {  .hour = 14,   .min = 30, .sec = 0}
  };

  EebusDuration duration = {.days = 1, .hours = 2, .minutes = 15, .seconds = 30};

  EXPECT_EQ(EebusDateTimeAddDuration(&date_time, &duration), kEebusErrorOk);
  EXPECT_EQ(date_time.date.year, 2023);
  EXPECT_EQ(date_time.date.month, 10);
  EXPECT_EQ(date_time.date.day, 6);
  EXPECT_EQ(date_time.time.hour, 16);
  EXPECT_EQ(date_time.time.min, 45);
  EXPECT_EQ(date_time.time.sec, 30);
}

TEST(EebusDateTimeAddDurationTest, NullDateTimePointer) {
  EebusDuration duration = {.days = 1, .hours = 2, .minutes = 15, .seconds = 30};

  EXPECT_EQ(EebusDateTimeAddDuration(nullptr, &duration), kEebusErrorInputArgumentNull);
}

TEST(EebusDateTimeAddDurationTest, NullDurationPointer) {
  EebusDateTime date_time = {
      .date = {.year = 2023, .month = 10, .day = 5},
      .time = {  .hour = 14,   .min = 30, .sec = 0}
  };

  EXPECT_EQ(EebusDateTimeAddDuration(&date_time, nullptr), kEebusErrorInputArgumentNull);
}

TEST(EebusDateTimeAddDurationTest, OverflowDateTime) {
  EebusDateTime date_time = {
      .date = {.year = 2023, .month = 12, .day = 31},
      .time = {  .hour = 23,   .min = 59, .sec = 59}
  };

  EebusDuration duration = {.days = 1, .hours = 0, .minutes = 0, .seconds = 1};

  EXPECT_EQ(EebusDateTimeAddDuration(&date_time, &duration), kEebusErrorOk);
  EXPECT_EQ(date_time.date.year, 2024);
  EXPECT_EQ(date_time.date.month, 1);
  EXPECT_EQ(date_time.date.day, 2);
  EXPECT_EQ(date_time.time.hour, 0);
  EXPECT_EQ(date_time.time.min, 0);
  EXPECT_EQ(date_time.time.sec, 0);
}

TEST(EebusDateTimeAddDurationTest, NegativeDuration) {
  EebusDateTime date_time = {
      .date = {.year = 2023, .month = 10, .day = 5},
      .time = {  .hour = 14,   .min = 30, .sec = 2}
  };

  EebusDuration duration = {.days = -1, .hours = -2, .minutes = -15, .seconds = -30};

  EXPECT_EQ(EebusDateTimeAddDuration(&date_time, &duration), kEebusErrorOk);
  EXPECT_EQ(date_time.date.year, 2023);
  EXPECT_EQ(date_time.date.month, 10);
  EXPECT_EQ(date_time.date.day, 4);
  EXPECT_EQ(date_time.time.hour, 12);
  EXPECT_EQ(date_time.time.min, 14);
  EXPECT_EQ(date_time.time.sec, 32);
}

TEST(EebusDateTimeAddDurationTest, LargeDurationValuesPoistive) {
  EebusDateTime date_time = {
      .date = {.year = 2023, .month = 1, .day = 1},
      .time = {   .hour = 0,   .min = 0, .sec = 0}
  };

  EebusDuration duration = {.days = 0, .hours = 50000, .minutes = 300000, .seconds = 10000000};

  EXPECT_EQ(EebusDateTimeAddDuration(&date_time, &duration), kEebusErrorOk);
  // Validate the resulting date and time after adding the large duration
  EXPECT_EQ(date_time.date.year, 2029);
  EXPECT_EQ(date_time.date.month, 8);
  EXPECT_EQ(date_time.date.day, 4);
  EXPECT_EQ(date_time.time.hour, 9);
  EXPECT_EQ(date_time.time.min, 46);
  EXPECT_EQ(date_time.time.sec, 40);
}

TEST(EebusDateTimeAddDurationTest, LargeDurationValuesNegative) {
  EebusDateTime date_time = {
      .date = {.year = 2023, .month = 1, .day = 1},
      .time = {   .hour = 0,   .min = 0, .sec = 0}
  };

  EebusDuration duration = {.days = -5, .hours = -50000, .minutes = -300000, .seconds = -10000000};

  EXPECT_EQ(EebusDateTimeAddDuration(&date_time, &duration), kEebusErrorOk);
  // Validate the resulting date and time after adding the large duration
  EXPECT_EQ(date_time.date.year, 2016);
  EXPECT_EQ(date_time.date.month, 5);
  EXPECT_EQ(date_time.date.day, 24);
  EXPECT_EQ(date_time.time.hour, 14);
  EXPECT_EQ(date_time.time.min, 13);
  EXPECT_EQ(date_time.time.sec, 20);
}

//-------------------------------------------------------------------------------------------//
//
// EebusDateTimeCompare() test
//
//-------------------------------------------------------------------------------------------//
TEST(EebusDateTimeCompareTest, SameDateTime) {
  static constexpr EebusDateTime dt1 = {
      .date = {.year = 2023, .month = 10,  .day = 5},
      .time = {  .hour = 12,   .min = 30, .sec = 45}
  };
  static constexpr EebusDateTime dt2 = {
      .date = {.year = 2023, .month = 10,  .day = 5},
      .time = {  .hour = 12,   .min = 30, .sec = 45}
  };
  EXPECT_EQ(EebusDateTimeCompare(&dt1, &dt2), 0);
}

TEST(EebusDateTimeCompareTest, EarlierDate) {
  static constexpr EebusDateTime dt1 = {
      .date = {.year = 2023, .month = 10,  .day = 4},
      .time = {  .hour = 12,   .min = 30, .sec = 45}
  };
  static constexpr EebusDateTime dt2 = {
      .date = {.year = 2023, .month = 10,  .day = 5},
      .time = {  .hour = 12,   .min = 30, .sec = 45}
  };
  EXPECT_LT(EebusDateTimeCompare(&dt1, &dt2), 0);
}

TEST(EebusDateTimeCompareTest, LaterDate) {
  static constexpr EebusDateTime dt1 = {
      .date = {.year = 2023, .month = 10,  .day = 6},
      .time = {  .hour = 12,   .min = 30, .sec = 45}
  };
  static constexpr EebusDateTime dt2 = {
      .date = {.year = 2023, .month = 10,  .day = 5},
      .time = {  .hour = 12,   .min = 30, .sec = 45}
  };
  EXPECT_GT(EebusDateTimeCompare(&dt1, &dt2), 0);
}

TEST(EebusDateTimeCompareTest, EarlierTime) {
  static constexpr EebusDateTime dt1 = {
      .date = {.year = 2023, .month = 10,  .day = 5},
      .time = {  .hour = 11,   .min = 30, .sec = 45}
  };
  static constexpr EebusDateTime dt2 = {
      .date = {.year = 2023, .month = 10,  .day = 5},
      .time = {  .hour = 12,   .min = 30, .sec = 45}
  };
  EXPECT_LT(EebusDateTimeCompare(&dt1, &dt2), 0);
}

TEST(EebusDateTimeCompareTest, LaterTime) {
  static constexpr EebusDateTime dt1 = {
      .date = {.year = 2023, .month = 10,  .day = 5},
      .time = {  .hour = 13,   .min = 30, .sec = 45}
  };
  static constexpr EebusDateTime dt2 = {
      .date = {.year = 2023, .month = 10,  .day = 5},
      .time = {  .hour = 12,   .min = 30, .sec = 45}
  };
  EXPECT_GT(EebusDateTimeCompare(&dt1, &dt2), 0);
}
