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
 * @brief EEBUS Date utility implementation
 */

#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "src/common/array_util.h"
#include "src/common/eebus_malloc.h"

#include "src/common/eebus_date_time/eebus_date.h"

// 20 = max output of "%04d-%02d-%02d" with int32_t args (up to 19 chars) + null terminator
#define EEBUS_DATE_STRING_SIZE 20

static const int days_per_month[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

bool IsLeapYear(int32_t year) {
    return ((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0);
}

int32_t EebusDateGetDaysInMonthNum(const EebusDate* self) {
    if ((self->month < 1) || (self->month > 12)) {
        return 0;  // Invalid month
    }

    if (self->month == 2) {
        return IsLeapYear(self->year) ? 29 : 28;  // February
    }

    return days_per_month[self->month - 1];  // Return the number of days in the month
}

bool EebusDateIsValid(const EebusDate* self) {
    if (self == NULL) {
        return false;
    }

    // Check for valid ranges
    if ((self->year < 0) || (self->year > 2100)) {
        return false;  // Invalid year
    }

    if ((self->month < 1) || (self->month > 12) || (self->day < 1)) {
        return false;  // Invalid month
    }

    if ((self->day < 1) || (self->day > EebusDateGetDaysInMonthNum(self))) {
        return false;  // Invalid day
    }

    return true;
}

EebusError EebusDateParse(const char* s, EebusDate* date) {
    if ((s == NULL) || (date == NULL)) {
        return kEebusErrorInputArgumentNull;
    }

    memset(date, 0, sizeof(*date));  // Initialize date structure

    // Parse the date part (YYYY-MM-DD)
    char* endptr = NULL;

    // Parse year
    date->year = (int32_t)strtol(s, &endptr, 10);
    if ((endptr == s) || (*endptr != '-')) {
        return kEebusErrorParse;  // Invalid year format
    }
    s = endptr + 1;

    // Parse month
    date->month = (int32_t)strtol(s, &endptr, 10);
    if ((endptr == s) || (*endptr != '-')) {
        return kEebusErrorParse;  // Invalid month format
    }
    s = endptr + 1;

    // Parse day
    date->day = (int32_t)strtol(s, &endptr, 10);
    if ((endptr == s) || (*endptr != '\0')) {
        return kEebusErrorParse;  // Invalid day format
    }

    // Check for valid ranges
    if (!EebusDateIsValid(date)) {
        return kEebusErrorParse;  // Invalid date values
    }

    return kEebusErrorOk;
}

char* EebusDateToString(const EebusDate* self) {
    if ((self == NULL) || (!EebusDateIsValid(self))) {
        return NULL;
    }

    static const size_t buffer_size = EEBUS_DATE_STRING_SIZE;

    // Allocate memory for the string representation
    char* const buffer = (char*)EEBUS_MALLOC(buffer_size);
    if (buffer == NULL) {
        return NULL;  // Memory allocation failed
    }

    // Format the date into the string
    snprintf(buffer, buffer_size, "%04" PRId32 "-%02" PRId32 "-%02" PRId32, self->year, self->month, self->day);

    return buffer;
}

int32_t EebusDateCompare(const EebusDate* self, const EebusDate* other) {
    if (self->year != other->year) {
        return self->year - other->year;
    }

    if (self->month != other->month) {
        return self->month - other->month;
    }

    if (self->day != other->day) {
        return self->day - other->day;
    }

    return 0;
}
