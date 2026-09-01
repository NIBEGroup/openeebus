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
 * @brief Boolean utility functions
 */

#ifndef SRC_COMMON_EEBUS_BOOL_EEBUS_BOOL_H_
#define SRC_COMMON_EEBUS_BOOL_EEBUS_BOOL_H_

#include <stdbool.h>

#include "src/common/eebus_errors.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

extern const char* const kTrueString;
extern const char* const kFalseString;

/**
 * @brief Parse a string representation of a boolean value.
 *
 * @param s The string to parse.
 * @param value Pointer to a boolean variable to store the result.
 * @return An EebusError indicating success or failure.
 */
EebusError EebusBoolParse(const char* s, bool* value);

/**
 * @brief Convert a boolean value to its string representation.
 *
 * @param value The boolean value to convert.
 * @return A string representation of the boolean value ("true" or "false").
 */
static inline const char* EebusBoolToString(bool value) {
    return value ? kTrueString : kFalseString;
}

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_COMMON_EEBUS_BOOL_EEBUS_BOOL_H_
