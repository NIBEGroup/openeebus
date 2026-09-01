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
 * @brief Eebus Queue implementation declarations
 */

#ifndef SRC_COMMON_EEBUS_QUEUE_EEBUS_QUEUE_H_
#define SRC_COMMON_EEBUS_QUEUE_EEBUS_QUEUE_H_

#include <stddef.h>

#include "src/common/api/eebus_queue_interface.h"
#include "src/common/eebus_malloc.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

static const uint32_t kTimeoutInfinite = 0xFFFFFFFF;

/** Queue message deallocator type definition */
typedef void (*QueueMsgDeallocator)(void*);

EebusQueueObject* EebusQueueCreate(size_t max_msg, size_t msg_size, QueueMsgDeallocator msg_deallocator);

static inline void EebusQueueDelete(EebusQueueObject* eebus_queue) {
    if (eebus_queue != NULL) {
        EEBUS_QUEUE_DESTRUCT(eebus_queue);
        EEBUS_FREE(eebus_queue);
    }
}

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_COMMON_EEBUS_QUEUE_EEBUS_QUEUE_H_
