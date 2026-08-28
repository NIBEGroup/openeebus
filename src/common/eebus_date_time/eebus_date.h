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
 * @brief EEBUS Date utility declarations
 */

#ifndef SRC_COMMON_DATE_EEBUS_DATE_TIME_H_
#define SRC_COMMON_DATE_EEBUS_DATE_TIME_H_

#include <stdbool.h>
#include <stdint.h>

#include "src/common/eebus_errors.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct EebusDate EebusDate;

struct EebusDate {
    int32_t year;  /**< Year (e.g. 1800, 2023 etc.) */
    int32_t month; /**< Month of the year (1-12) */
    int32_t day;   /**< Day of the month (1-31) */
};

int32_t EebusDateGetDaysInMonthNum(const EebusDate* self);

/**
 * @brief Checks if the given EebusDate structure has valid fields.
 *
 * This function checks whether the provided EebusDate structure contains
 * valid date values. It ensures that the year, month, and day fields
 * conform to the expected ranges for a valid date.
 *
 * @param[in] self Pointer to the EebusDate structure to be checked.
 * @return true if the date is valid, false otherwise.
 */
bool EebusDateIsValid(const EebusDate* self);

/**
 * @brief Parses a ISO 8061 string and converts it into an EebusDate structure.
 *
 * @param s The input ISO 8061 date string.
 * @param date A pointer to an EebusDate structure where the parsed date will
 *             be stored.
 *
 * @return kEebusErrorOk on success, error code otherwise.
 */
EebusError EebusDateParse(const char* s, EebusDate* date);

/**
 * @brief Converts an EebusDate object to ISO 8061 date string representation.
 *
 * @param self Pointer to the EebusDate object to be converted.
 * @return A pointer to a dynamically allocated string containing the
 *         string representation of the EebusDate. The caller is
 *         responsible for freeing the allocated memory with StringDelete().
 */
char* EebusDateToString(const EebusDate* self);

/**
 * @brief Compares two EebusDate objects.
 *
 * This function compares the provided EebusDate objects and determines their
 * relative ordering. It returns an integer value indicating whether the first
 * date is earlier, later, or the same as the second date.
 *
 * @param self A pointer to the first EebusDate object to compare.
 * @param other A pointer to the second EebusDate object to compare.
 * @return An int32_t value indicating the comparison result:
 *         - A negative value if `self` is earlier than `other`.
 *         - Zero if `self` is equal to `other`.
 *         - A positive value if `self` is later than `other`.
 */
int32_t EebusDateCompare(const EebusDate* self, const EebusDate* other);

#ifdef __cplusplus
}
#endif

#endif  // SRC_COMMON_DATE_EEBUS_DATE_TIME_H_
