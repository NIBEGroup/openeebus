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
 * @brief EEBUS Time utility declarations
 */

#ifndef SRC_COMMON_DATE_TIME_EEBUS_TIME_H_
#define SRC_COMMON_DATE_TIME_EEBUS_TIME_H_

#include <stdbool.h>
#include <stdint.h>

#include "src/common/eebus_date_time/eebus_duration.h"
#include "src/common/eebus_errors.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct EebusTime EebusTime;

struct EebusTime {
    int32_t hour; /**< Hour of the day (0-23) */
    int32_t min;  /**< Minute of the hour (0-59) */
    int32_t sec;  /**< Second of the minute (0-59) */
};

/**
 * @brief Checks if the given EebusTime object represents a valid time.
 *
 * This function validates the EebusTime object to ensure that it contains
 * a properly formatted and logically correct time value.
 *
 * @param self A pointer to the EebusTime object to validate.
 * @return true if the EebusTime object is valid, false otherwise.
 */
bool EebusTimeIsValid(const EebusTime* self);

/**
 * @brief Parses the ISO 8061 time string and converts it into an EebusTime structure.
 *
 * @param s The input ISO 8061 string representing the time.
 * @param time A pointer to an EebusTime structure where the parsed time will
 *             be stored. The structure must be allocated by the caller.
 * @return kEebusErrorOk on success, error code otherwise.
 */
EebusError EebusTimeParse(const char* s, EebusTime* time);

/**
 * @brief Converts an EebusTime object to ISO 8061 time string representation.
 *
 * @param self Pointer to the EebusTime object to be converted.
 * @return A pointer to a null-terminated string representing the ISO 8061 time.
 *         The caller is responsible for freeing the allocated memory with StringDelete()
 */
char* EebusTimeToString(const EebusTime* self);

/**
 * @brief Compares two EebusTime objects.
 *
 * This function compares the time values of two EebusTime objects and determines
 * their relative order.
 *
 * @param self A pointer to the first EebusTime object.
 * @param other A pointer to the second EebusTime object.
 * @return An integer indicating the comparison result:
 *         - A negative value if `self` is earlier than `other`.
 *         - Zero if `self` is equal to `other`.
 *         - A positive value if `self` is later than `other`.
 */
int32_t EebusTimeCompare(const EebusTime* self, const EebusTime* other);

#ifdef __cplusplus
}
#endif

#endif  // SRC_COMMON_DATE_TIME_EEBUS_TIME_H_
