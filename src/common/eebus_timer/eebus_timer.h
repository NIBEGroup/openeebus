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
 * @brief Eebus Timer implementation declarations
 */

#ifndef SRC_COMMON_EEBUS_TIMER_EEBUS_TIMER_H_
#define SRC_COMMON_EEBUS_TIMER_EEBUS_TIMER_H_

#include <stddef.h>

#include "src/common/api/eebus_timer_interface.h"
#include "src/common/eebus_malloc.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

EebusTimerObject* EebusTimerCreate(EebusTimerTimeoutCallback cb, void* ctx);

static inline void EebusTimerDelete(EebusTimerObject* eebus_timer) {
    if (eebus_timer != NULL) {
        EEBUS_TIMER_DESTRUCT(eebus_timer);
        EEBUS_FREE(eebus_timer);
    }
}

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_COMMON_EEBUS_TIMER_EEBUS_TIMER_H_
