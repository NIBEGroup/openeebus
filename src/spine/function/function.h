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
 * @brief Function implementation declarations
 *
 * Transformed from spine-go/spine/function_data.go
 */

#ifndef SRC_SPINE_FUNCTION_FUNCTION_H_
#define SRC_SPINE_FUNCTION_FUNCTION_H_

#include <stddef.h>

#include "src/common/eebus_malloc.h"
#include "src/spine/api/function_interface.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

FunctionObject* FunctionCreate(FunctionType type);

static inline void FunctionDelete(FunctionObject* function) {
    if (function != NULL) {
        FUNCTION_DESTRUCT(function);
        EEBUS_FREE(function);
    }
}

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SPINE_FUNCTION_FUNCTION_H_
