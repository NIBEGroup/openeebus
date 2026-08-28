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
 * @brief Pointer to simple data helper definitions
 */

#ifndef SRC_EEBUS_SRC_COMMON_SIMPLE_PTR_H_
#define SRC_EEBUS_SRC_COMMON_SIMPLE_PTR_H_

#include "src/common/eebus_malloc.h"

#define SIMPLE_PTR_CREATE_IMPL(name, type)                 \
    type* name(type val) {                                 \
        type* const p = (type*)EEBUS_MALLOC(sizeof(type)); \
        if (p != NULL) {                                   \
            *p = val;                                      \
        }                                                  \
                                                           \
        return p;                                          \
    }

#define SIMPLE_PTR_DELETE_IMPL(name, type) \
    void name(type* p) {                   \
        EEBUS_FREE(p);                     \
    }

#endif  // SRC_EEBUS_SRC_COMMON_SIMPLE_PTR_H_
