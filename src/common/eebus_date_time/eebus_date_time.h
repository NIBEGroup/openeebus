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
 * @brief EEBUS Date & Time utility declarations
 */

#ifndef SRC_COMMON_DATE_TIME_EEBUS_DATE_TIME_H_
#define SRC_COMMON_DATE_TIME_EEBUS_DATE_TIME_H_

#include "src/common/eebus_date_time/eebus_date.h"
#include "src/common/eebus_date_time/eebus_duration.h"
#include "src/common/eebus_date_time/eebus_time.h"
#include "src/common/eebus_errors.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct EebusDateTime EebusDateTime;

struct EebusDateTime {
    EebusDate date; /**< Represents the date component in the EEBUS system */
    EebusTime time; /**< Represents the time component in the EEBUS system */
};

/**
 * @brief Checks if the given EebusDateTime structure has valid fields.
 *
 * This function checks whether the provided EebusDateTime object
 * contains valid date and time values. It ensures that the fields
 * of the structure adhere to the expected ranges and formats.
 *
 * @param[in] self A pointer to the EebusDateTime object to validate.
 *                 Must not be NULL.
 *
 * @return true if the EebusDateTime object is valid, false otherwise.
 */
bool EebusDateTimeIsValid(const EebusDateTime* self);

/**
 * @brief Parses a date string in the EEBUS format and converts it into a structured date representation.
 *
 * @param[in] s The input date string in the ISO 8061 format "YYYY-MM-DD".
 * @param[out] date Pointer to an EebusDate structure where the parsed date will be stored.
 *
 * @return EebusError Returns kEebusError on success, error code otherwise.
 */
EebusError EebusDateParse(const char* s, EebusDate* date);

/**
 * @brief Converts an EebusDate object to its ISO 8061 string representation.
 *
 * Returns a dynamically allocated string representing the date in a human-readable format.
 * The caller is responsible for freeing the returned string by calling StringDelete().
 *
 * @param self Pointer to the EebusDate object to be converted.
 * @return A pointer to a dynamically allocated string containing the
 *         string representation of the date, or NULL if the conversion fails.
 */
char* EebusDateToString(const EebusDate* self);

/**
 * @brief Parses a time string in the EEBUS format and converts it into a structured time representation.
 *
 * @param[in] s The input time string in the ISO 8061 format "HH:MM:SS".
 * @param[out] time Pointer to an EebusTime structure where the parsed time will be stored.
 *
 * @return EebusError Returns kEebusError on success, error code otherwise.
 */
EebusError EebusTimeParse(const char* s, EebusTime* time);

/**
 * @brief Converts an EebusTime object to its ISO 8061 string representation.
 *
 * Returns a dynamically allocated string representing the time in ISO 8061 format.
 * The caller is responsible for freeing the returned string by calling StringDelete().
 *
 * @param self Pointer to the EebusTime object to be converted.
 * @return A pointer to a dynamically allocated string containing the
 *         string representation of the time, or NULL if the conversion fails.
 */
char* EebusTimeToString(const EebusTime* self);

/**
 * @brief Parses a date-time string into EebusDateTime structure.
 *
 * This function takes a string representation of a date-time and converts it
 * into an EebusDateTime structure. The input string must follow one of the folllowing
 * ISO 8061 formats:
 *
 * # "2006-01-02T15:04:05"
 * # "2006-01-02T15:04:05Z"
 *
 * @param[in] s The input date-time string to be parsed. Must not be null.
 * @param[out] date_time Pointer to an EebusDateTime structure where the parsed
 *                       date-time will be stored. Must not be null.
 *
 * @return EebusError Returns kEebusError on success, error code otherwise.
 */
EebusError EebusDateTimeParse(const char* s, EebusDateTime* date_time);

/**
 * @brief Converts an EebusDateTime structure to a string representation.
 *
 * This function converts an EebusDateTime structure into a string format.
 * The output string will be in the ISO 8061 format "YYYY-MM-DDTHH:MM:SS".
 *
 * @param[in] self Pointer to the EebusDateTime structure to be converted.
 *                 Must not be null.
 *
 * @return char* Returns a pointer to the string representation of the date-time.
 *               The caller is responsible for freeing the allocated memory
 *               by calling StringDelete().
 */
char* EebusDateTimeToString(const EebusDateTime* self);

/**
 * @brief Adds EebusDuration to an EebusDateTime.
 *
 * @param[in,out] self Pointer to the EebusDateTime structure to which the
 *                     duration will be added. Must not be null.
 * @param[in] duration Pointer to the EebusDuration structure representing
 *                     the duration to be added. Must not be null.
 *
 * @return EebusError Returns kEebusErrorOk on success, error code otherwise.
 */
EebusError EebusDateTimeAddDuration(EebusDateTime* self, const EebusDuration* duration);

/**
 * @brief Retrieves the current date and time.
 *
 * This function provides the current date and time in the EebusDateTime format.
 * It is typically used for timestamping or scheduling purposes within the EEBUS framework.
 *
 * @return EebusDateTime The current date and time.
 */
EebusDateTime EebusDateTimeNow(void);

/**
 * @brief Compares two EebusDateTime objects.
 *
 * This function compares the provided EebusDateTime objects and determines
 * their relative order. It returns a value indicating whether the first
 * object is earlier than, equal to, or later than the second object.
 *
 * @param self A pointer to the first EebusDateTime object to compare.
 * @param other A pointer to the second EebusDateTime object to compare.
 * @return An integer indicating the comparison result:
 *         - A negative value if `self` is earlier than `other`.
 *         - Zero if `self` is equal to `other`.
 *         - A positive value if `self` is later than `other`.
 */
int32_t EebusDateTimeCompare(const EebusDateTime* self, const EebusDateTime* other);

#ifdef __cplusplus
}
#endif

#endif  // SRC_COMMON_DATE_TIME_EEBUS_DATE_TIME_H_
