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
 * @brief Pending Write Request implementation declarations
 */

#ifndef SRC_SPINE_FEATURE_PENDING_WRITE_REQUEST_H_
#define SRC_SPINE_FEATURE_PENDING_WRITE_REQUEST_H_

#include "src/common/eebus_malloc.h"
#include "src/spine/api/pending_write_request_interface.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

PendingWriteRequestObject* PendingWriteRequestCreate(const Message* msg);

static inline void PendingWriteRequestDelete(PendingWriteRequestObject* pending_write_request) {
    if (pending_write_request != NULL) {
        PENDING_WRITE_REQUEST_DESTRUCT(pending_write_request);
        EEBUS_FREE(pending_write_request);
    }
}

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SPINE_FEATURE_PENDING_WRITE_REQUEST_H_
