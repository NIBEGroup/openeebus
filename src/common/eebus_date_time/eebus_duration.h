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
 * @brief Eebus Duration refer to ISO 8061
 */

#ifndef SRC_COMMON_EEBUS_DATE_TIME_DURATION_EEBUS_DURATION_H_
#define SRC_COMMON_EEBUS_DATE_TIME_DURATION_EEBUS_DURATION_H_

#include <stdbool.h>
#include <stdint.h>

#include "src/common/eebus_errors.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct EebusDuration EebusDuration;

/**
 * @brief Inverts the sign of the duration.
 *
 * This function takes a pointer to an EebusDuration structure and inverts its sign.
 * If the duration is positive, it becomes negative, and vice versa.
 *
 * @param[in,out] duration Pointer to the EebusDuration structure to be modified. Must not be null.
 */
void EebusDurationInvertSign(EebusDuration* duration);

/**
 * @struct EebusDuration
 * @brief Represents a duration of time with fields for years, months, days, hours, minutes, and seconds.
 *
 * This structure is used to define a time duration, including its sign (positive or negative).
 * Each field represents a specific component of the duration.
 */
struct EebusDuration {
    int32_t years;        /**< Number of years in the duration. */
    int32_t months;       /**< Number of months in the duration. */
    int32_t days;         /**< Number of days in the duration. */
    int32_t hours;        /**< Number of hours in the duration. */
    int32_t minutes;      /**< Number of minutes in the duration. */
    int32_t seconds;      /**< Number of seconds in the duration. */
    int32_t milliseconds; /**< Number of milliseconds in the duration. */
};

/**
 * @brief Checks if the duration is valid.
 *
 * The check is passed if any of the following conditions is met:
 * # All fields are zero;
 * # All fileds are positive or zero;
 * # All fields are negative or zero;
 *
 * @param[in] self Pointer to the EebusDuration structure to be checked. Must not be null.
 *
 * @return bool Returns true if the duration is valid, false otherwise.
 */
bool EebusDurationIsValid(const EebusDuration* self);

/**
 * @brief Parses a string representation of a duration into an EebusDuration structure.
 *
 * This function takes a string in the ISO 8061 format and converts it into an EebusDuration structure.
 * The input string must follow the format "PnYnMnDTnHnMnS", where:
 * - P is the duration designator (for period).
 * - nY is the number of years.
 * - nM is the number of months.
 * - nD is the number of days.
 * - T is the time designator (for time).
 * - nH is the number of hours.
 * - nM is the number of minutes.
 * - nS is the number of seconds.
 *
 * @param[in] s The input string to be parsed. Must not be null.
 * @param[out] duration Pointer to an EebusDuration structure where the parsed duration will be stored.
 *                      Must not be null.
 *
 * @return EebusError Returns kEebusErrorOk on success, error code otherwise.
 */
EebusError EebusDurationParse(const char* s, EebusDuration* duration);

/**
 * @brief Prints the string representation of an EebusDuration structure.
 *
 * This function converts an EebusDuration structure into a string format and prints it
 * using the specified format string.
 *
 * @param[in] fmt The format string used for printing.
 * Must not be null and shall contain a single %s format specifier
 * @param[in] duration Pointer to the EebusDuration structure to be printed. Must not be null.
 */
void EebusDurationPrint(const char* fmt, const EebusDuration* duration);

/**
 * @brief Converts an EebusDuration structure to a string representation.
 *
 * This function converts an EebusDuration structure into a string format.
 * The output string will be in the ISO 8061 format "PnYnMnDTnHnMnS".
 *
 * @param[in] self Pointer to the EebusDuration structure to be converted. Must not be null.
 *
 * @return char* Returns a pointer to the string representation of the duration.
 *               The caller is responsible for freeing the allocated memory
 *               by calling StringDelete().
 */
char* EebusDurationToString(const EebusDuration* self);

/**
 * @brief Converts an EebusDuration object to its equivalent duration in seconds.
 *
 * @param self A pointer to the EebusDuration object to be converted.
 * @return The duration represented by the EebusDuration object in seconds as an int64_t.
 */
int64_t EebusDurationToSeconds(const EebusDuration* self);

/**
 * @brief Compares two EebusDuration objects.
 *
 * This function compares two EebusDuration objects and determines their relative order.
 *
 * @param self A pointer to the first EebusDuration object to be compared.
 * @param other A pointer to the second EebusDuration object to be compared.
 * @return An integer indicating the result of the comparison:
 *         - A negative value if self is less than other.
 *         - Zero if self is equal to other.
 *         - A positive value if self is greater than other.
 */
int32_t EebusDurationCompare(const EebusDuration* self, const EebusDuration* other);

#ifdef __cplusplus
}
#endif

#endif  // SRC_COMMON_EEBUS_DATE_TIME_DURATION_EEBUS_DURATION_H_
