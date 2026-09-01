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
 * @brief Operations implementation declarations
 *
 * Transformed from spine-go/spine/operations.go
 */

#ifndef SRC_SPINE_FEATURE_OPERATIONS_H_
#define SRC_SPINE_FEATURE_OPERATIONS_H_

#include <stddef.h>

#include "src/common/eebus_malloc.h"
#include "src/spine/api/operations_interface.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

OperationsObject* OperationsCreate(bool read, bool read_partial, bool write, bool write_partial);

static inline void OperationsDelete(OperationsObject* operations) {
    if (operations != NULL) {
        OPERATIONS_DESTRUCT(operations);
        EEBUS_FREE(operations);
    }
}

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SPINE_FEATURE_OPERATIONS_H_
