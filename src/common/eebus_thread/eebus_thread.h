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
 * @brief Eebus Thread implementation declarations
 */

#ifndef SRC_COMMON_EEBUS_THREAD_EEBUS_THREAD_H_
#define SRC_COMMON_EEBUS_THREAD_EEBUS_THREAD_H_

#include <stddef.h>
#include <stdint.h>

#include "src/common/api/eebus_thread_interface.h"
#include "src/common/eebus_malloc.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/** Thread starting routine type definition */
typedef void* (*EebusThreadRoutine)(void*);

EebusThreadObject* EebusThreadCreate(EebusThreadRoutine routine, void* parameters, size_t stack_size);

static inline void EebusThreadDelete(EebusThreadObject* eebus_thread) {
    if (eebus_thread != NULL) {
        EEBUS_THREAD_DESTRUCT(eebus_thread);
        EEBUS_FREE(eebus_thread);
    }
}

/**
 * @brief Suspends the execution of the calling thread for a specified number of seconds
 * @param seconds Number of seconds to sleep
 */
void EebusThreadSleep(int32_t seconds);

/**
 * @brief Suspends the execution of the calling thread for a specified number of microseconds
 * @param useconds Number of microseconds to sleep
 */
void EebusThreadUsleep(int32_t useconds);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_COMMON_EEBUS_THREAD_EEBUS_THREAD_H_
