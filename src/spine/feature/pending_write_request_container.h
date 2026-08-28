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
 * @brief Pending Write Request Container implementation declarations
 */

#ifndef SRC_SPINE_FEATURE_PENDING_WRITE_REQUEST_CONTAINER_H_
#define SRC_SPINE_FEATURE_PENDING_WRITE_REQUEST_CONTAINER_H_

#include "src/common/eebus_malloc.h"
#include "src/spine/api/pending_write_request_container_interface.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

PendingWriteRequestContainerObject* PendingWriteRequestContainerCreate(void);

static inline void PendingWriteRequestContainerDelete(PendingWriteRequestContainerObject* self) {
    if (self != NULL) {
        PENDING_WRITE_REQUEST_CONTAINER_DESTRUCT(self);
        EEBUS_FREE(self);
    }
}

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SPINE_FEATURE_PENDING_WRITE_REQUEST_CONTAINER_H_
