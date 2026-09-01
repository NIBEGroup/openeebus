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
/**
 * @file
 * @brief EEBUS Date & Time utility implementation
 */

#include <stdbool.h>
#include <stdio.h>
#include <time.h>

#include "src/common/array_util.h"
#include "src/common/eebus_malloc.h"

#include "src/common/eebus_date_time/eebus_date.h"
#include "src/common/eebus_date_time/eebus_date_time.h"
#include "src/common/eebus_date_time/eebus_duration.h"

#define DATE_BUF_SIZE (ARRAY_SIZE("YYYY-MM-DD"))

bool EebusDateTimeIsValid(const EebusDateTime* self) {
    // Check if the date and time components are valid
    return (EebusDateIsValid(&self->date) && EebusTimeIsValid(&self->time));
}

EebusError EebusDateTimeParse(const char* s, EebusDateTime* date_time) {
    if ((s == NULL) || (date_time == NULL)) {
        return kEebusErrorInputArgumentNull;
    }

    // Check for the presence of 'T' separator
    const char* const t_pos = strchr(s, 'T');
    if (t_pos == NULL) {
        return kEebusErrorParse;  // Invalid format, missing 'T'
    }

    const size_t date_len = t_pos - s;
    if (date_len >= DATE_BUF_SIZE) {
        return kEebusErrorParse;  // Invalid date format
    }

    char date_buf[DATE_BUF_SIZE];
    strncpy(date_buf, s, date_len);
    date_buf[date_len] = '\0';  // Null-terminate the date string

    const EebusError date_parse_err = EebusDateParse(date_buf, &date_time->date);
    if (date_parse_err != kEebusErrorOk) {
        return date_parse_err;
    }

    // Parse the time part (HH:MM:SS)
    return EebusTimeParse(t_pos + 1, &date_time->time);
}

char* EebusDateTimeToString(const EebusDateTime* self) {
    if ((self == NULL) || (!EebusDateTimeIsValid(self))) {
        return NULL;
    }

    static const size_t buffer_size = ARRAY_SIZE("YYYY-MM-DDTHH:MM:SSZ");

    // Allocate memory for the string representation
    char* buffer = (char*)EEBUS_MALLOC(buffer_size);
    if (buffer == NULL) {
        return NULL;  // Memory allocation failed
    }

    // Format the date and time into the string
    snprintf(
        buffer,
        buffer_size,
        "%04d-%02d-%02dT%02d:%02d:%02dZ",
        (int)self->date.year,
        (int)self->date.month,
        (int)self->date.day,
        (int)self->time.hour,
        (int)self->time.min,
        (int)self->time.sec
    );

    return buffer;
}

static void Normalize(int32_t* filed, int32_t* filed_next, int32_t max) {
    const int32_t rem = *filed % max;
    if (*filed >= max) {
        *filed_next += *filed / max;
        *filed = rem;
    } else if (*filed < 0) {
        if (rem == 0) {
            *filed_next -= (-*filed) / max;
            *filed = 0;
        } else {
            *filed_next -= 1 + (-*filed) / max;
            *filed = max + rem;
        }
    }
}

void NormalizeMonths(EebusDateTime* self) {
    self->date.month -= 1;
    Normalize(&self->date.month, &self->date.year, 12);
    self->date.month += 1;
}

void NormalizeDays(EebusDateTime* self) {
    self->date.day -= 1;
    int32_t days_in_month = EebusDateGetDaysInMonthNum(&self->date);
    if (self->date.day > 0) {
        while (self->date.day >= days_in_month) {
            self->date.day -= days_in_month;
            self->date.month += 1;
            NormalizeMonths(self);
            days_in_month = EebusDateGetDaysInMonthNum(&self->date);
        }
    } else {
        while (self->date.day < 0) {
            self->date.month -= 1;
            NormalizeMonths(self);
            days_in_month = EebusDateGetDaysInMonthNum(&self->date);
            self->date.day += days_in_month;
        }
    }

    self->date.day += 1;
}

EebusError EebusDateTimeAddDuration(EebusDateTime* self, const EebusDuration* duration) {
    if ((self == NULL) || (duration == NULL)) {
        return kEebusErrorInputArgumentNull;
    }

    if ((!EebusDateTimeIsValid(self)) || (!EebusDurationIsValid(duration))) {
        return kEebusErrorInputArgument;
    }

    self->time.sec += duration->seconds;
    Normalize(&self->time.sec, &self->time.min, 60);
    self->time.min += duration->minutes;
    Normalize(&self->time.min, &self->time.hour, 60);
    self->time.hour += duration->hours;
    Normalize(&self->time.hour, &self->date.day, 24);
    self->date.day += duration->days;
    NormalizeDays(self);
    self->date.month += duration->months;
    NormalizeMonths(self);

    return kEebusErrorOk;
}

void EebusDateTimeFromTm(EebusDateTime* self, const struct tm* tm) {
    if ((tm == NULL) || (self == NULL)) {
        return;
    }

    self->date.year  = tm->tm_year + 1900;
    self->date.month = tm->tm_mon + 1;
    self->date.day   = tm->tm_mday;
    self->time.hour  = tm->tm_hour;
    self->time.min   = tm->tm_min;
    self->time.sec   = tm->tm_sec;
}

EebusDateTime EebusDateTimeNow(void) {
    time_t now;
    struct tm tm;

    time(&now);

#ifdef _WIN32
    // Windows compatibility: localtime_s has different parameter order
    localtime_s(&tm, &now);
#else
    localtime_r(&now, &tm);
#endif  // _WIN32

    EebusDateTime date_time;
    EebusDateTimeFromTm(&date_time, &tm);
    return date_time;
}

int32_t EebusDateTimeCompare(const EebusDateTime* self, const EebusDateTime* other) {
    // Compare the dates
    const int32_t data_cmp_ret = EebusDateCompare(&self->date, &other->date);
    if (data_cmp_ret != 0) {
        return data_cmp_ret;
    }

    // Compare the time
    return EebusTimeCompare(&self->time, &other->time);
}
