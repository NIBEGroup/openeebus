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
 * @brief Simplfied C functionality with interface similar to C++ std::map<std::string, T>,
 * used for dynamically created look-up tables using text key (e.g. SKI)
 */

#ifndef SRC_COMMON_STRING_LUT_H_
#define SRC_COMMON_STRING_LUT_H_

#include "src/common/eebus_errors.h"
#include "src/common/vector.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @brief Function to delete String LUT value pointer type
 */
typedef void (*StringLutValueDeleter)(void* p);

/**
 * @brief String LUT type definition
 */
typedef struct StringLut StringLut;

/**
 * @brief String LUT structure
 */
struct StringLut {
    /** LUT records container */
    Vector records;
};

/**
 * @brief Iniitialize the String LUT
 * @param lut String LUT instance to be initialized
 */
void StringLutInit(StringLut* lut);

/**
 * @brief Release the String LUT
 * @param lut String LUT instance to be released
 */
void StringLutRelease(StringLut* lut);

/**
 * @brief Find a value in String LUT that corresponds to key specified
 * @param key Key to look for
 * @return Vlaue related to Key specified on success or NULL on fail
 */
void* StringLutFind(const StringLut* lut, const char* key);

/**
 * @brief Insert a new value into String LUT
 * @param lut Sting LUT instance to insert the value into
 * @param value Value tobe inserted
 * @return kEebusErrorOk on success, error code on fail
 */
EebusError StringLutInsert(StringLut* lut, const char* key, void* value, StringLutValueDeleter deleter);

/**
 * @brief Remove a record from String LUT that corresponds to key
 * @param lut Sting LUT instance to remove the record from
 * @param key Key to look for
 * @return kEebusErrorOk on success, error code on fail
 */
EebusError StringLutRemove(StringLut* lut, const char* key);

size_t StringLutGetSize(const StringLut* lut);

void* StringLutGetElementValue(const StringLut* lut, size_t idx);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_COMMON_STRING_LUT_H_
