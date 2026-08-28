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
 * @brief Simplfied C functionality with interface similar to C++ std::map<uint64_t, T>,
 * used for dynamically created look-up tables using uint64_t key (e.g. message counter)
 */

#ifndef SRC_COMMON_UINT64_LUT_H_
#define SRC_COMMON_UINT64_LUT_H_

#include "src/common/eebus_errors.h"
#include "src/common/vector.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @brief Function to delete Uint64 LUT value pointer type
 */
typedef void (*Uint64LutValueDeleter)(void* p);

/**
 * @brief Uint64 LUT type definition
 */
typedef struct Uint64Lut Uint64Lut;

/**
 * @brief Uint64 LUT structure
 */
struct Uint64Lut {
    /** LUT records container */
    Vector records;
};

/**
 * @brief Iniitialize the Uint64 LUT
 * @param lut Uint64 LUT instance to be initialized
 */
void Uint64LutConstruct(Uint64Lut* lut);

/**
 * @brief Release the Uint64 LUT resoruces
 * @param lut Uint64 LUT instance to be released
 */
void Uint64LutDestruct(Uint64Lut* lut);

/**
 * @brief Find a value in Uint64 LUT that corresponds to key specified
 * @param key Key to look for
 * @return Vlaue related to Key specified on success or NULL on fail
 */
void* Uint64LutFind(Uint64Lut* lut, uint64_t key);

/**
 * @brief Insert a new value into Uint64 LUT
 * @param lut Sting LUT instance to insert the value into
 * @param value Value tobe inserted
 * @return kEebusErrorOk on success, error code on fail
 */
EebusError Uint64LutInsert(Uint64Lut* lut, uint64_t key, void* value, Uint64LutValueDeleter deleter);

/**
 * @brief Remove a record from Uint64 LUT that corresponds to key
 * @param lut Sting LUT instance to remove the record from
 * @param key Key to look for
 * @return kEebusErrorOk on success, error code on fail
 */
EebusError Uint64LutRemove(Uint64Lut* lut, uint64_t key);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_COMMON_UINT64_LUT_H_
