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
 * @brief EEBUS Time utility implementation
 */

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>

#include "src/common/array_util.h"
#include "src/common/eebus_errors.h"
#include "src/common/eebus_malloc.h"

#include "src/common/eebus_date_time/eebus_time.h"

static const int32_t seconds_per_minute = 60;
static const int32_t minutes_per_hour   = 60;
static const int32_t hours_per_day      = 24;

bool EebusTimeIsValid(const EebusTime* self) {
    if (self == NULL) {
        return false;
    }

    // Check for valid ranges
    if ((self->hour < 0) || (self->hour >= hours_per_day)) {
        return false;  // Invalid hour
    }

    if ((self->min < 0) || (self->min >= minutes_per_hour)) {
        return false;  // Invalid minute
    }

    if ((self->sec < 0) || (self->sec >= seconds_per_minute)) {
        return false;  // Invalid second
    }

    return true;
}

EebusError EebusTimeParse(const char* s, EebusTime* time) {
    if ((s == NULL) || (time == NULL)) {
        return kEebusErrorInputArgumentNull;
    }

    memset(time, 0, sizeof(*time));  // Initialize time structure

    // Parse the time part (HH:MM:SS)
    char* endptr = NULL;

    // Parse hour
    time->hour = (int)strtol(s, &endptr, 10);
    if ((endptr == s) || (*endptr != ':')) {
        return kEebusErrorParse;  // Invalid hour format
    }

    s = endptr + 1;

    // Parse minute
    time->min = (int)strtol(s, &endptr, 10);
    if ((endptr == s) || (*endptr != ':')) {
        return kEebusErrorParse;  // Invalid minute format
    }

    s = endptr + 1;

    // Parse second
    time->sec = (int)strtol(s, &endptr, 10);
    if ((endptr == s) || ((*endptr != '\0') && (*endptr != 'Z') && (*endptr != '.'))) {
        return kEebusErrorParse;  // Invalid second format
    }

    // Handle fractional seconds (digits after dot)
    if (*endptr == '.') {
        endptr++;  // Skip the dot
        while (isdigit((int)*endptr)) {
            endptr++;  // Skip fractional digits
        }
    }

    if ((endptr == s) || ((*endptr != '\0') && (*endptr != 'Z'))) {
        return kEebusErrorParse;  // Invalid second format
    }

    if (EebusTimeIsValid(time) == false) {
        return kEebusErrorParse;  // Invalid time values
    }

    return kEebusErrorOk;
}

char* EebusTimeToString(const EebusTime* self) {
    if ((self == NULL) || (!EebusTimeIsValid(self))) {
        return NULL;
    }

    static const size_t buffer_size = ARRAY_SIZE("HH:MM:SSZ");

    // Allocate memory for the string representation
    char* const buffer = (char*)EEBUS_MALLOC(buffer_size);
    if (buffer == NULL) {
        return NULL;  // Memory allocation failed
    }

    // Format the time into the string
    snprintf(buffer, buffer_size, "%02d:%02d:%02dZ", (int)self->hour, (int)self->min, (int)self->sec);

    return buffer;
}

int32_t EebusTimeCompare(const EebusTime* self, const EebusTime* other) {
    if (self->hour != other->hour) {
        return self->hour - other->hour;
    }

    if (self->min != other->min) {
        return self->min - other->min;
    }

    return self->sec - other->sec;
}
