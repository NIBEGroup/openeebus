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
 * @brief Eebus Mutex implementation declarations
 */

#ifndef SRC_COMMON_EEBUS_MUTEX_EEBUS_MUTEX_H_
#define SRC_COMMON_EEBUS_MUTEX_EEBUS_MUTEX_H_

#include <stddef.h>

#include "src/common/api/eebus_mutex_interface.h"
#include "src/common/eebus_malloc.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

EebusMutexObject* EebusMutexCreate(void);

EebusMutexObject* EebusMutexCreateRecursive(void);

static inline void EebusMutexDelete(EebusMutexObject* eebus_mutex) {
    if (eebus_mutex != NULL) {
        EEBUS_MUTEX_DESTRUCT(eebus_mutex);
        EEBUS_FREE(eebus_mutex);
    }
}

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_COMMON_EEBUS_MUTEX_EEBUS_MUTEX_H_
