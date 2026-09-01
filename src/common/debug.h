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
 * @brief Debug messaging header file
 */

#ifndef SRC_COMMON_DEBUG_H_
#define SRC_COMMON_DEBUG_H_

#include <stddef.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/** Print the debug message */
#define DEBUG_PRINTF(stream, fmt, ...)       \
    do {                                     \
        if (stream) {                        \
            DebugPrintf((fmt), __VA_ARGS__); \
        }                                    \
    } while (0)

/** Debug hexdump */
#define DEBUG_HEXDUMP(stream, in, len) \
    do {                               \
        if (stream) {                  \
            DebugHexdump((in), (len)); \
        }                              \
    } while (0)

/**
 * @brief Debug printf(), add a timestamp printing before
 * formatted arguments print
 * @param format Format same as printf() one
 */
void DebugPrintf(const char* format, ...);

/**
 * @brief Print the intput data in hex
 * @param data Input data
 * @param data_size Input data size
 */
void DebugHexdump(void* data, size_t data_size);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_COMMON_DEBUG_H_
