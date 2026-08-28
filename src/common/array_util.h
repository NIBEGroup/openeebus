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
 * @brief Array utility
 */
#ifndef SRC_COMMON_ARRAY_UTIL_H_
#define SRC_COMMON_ARRAY_UTIL_H_

#include <stdint.h>
#include <string.h>

#include "src/common/eebus_malloc.h"

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))
#endif  // ARRAY_SIZE

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

static inline void* ArrayCopy(const void* src, size_t src_size, size_t element_size) {
    if ((src == NULL) || (src_size == 0) || (element_size == 0)) {
        return NULL;
    }

    if (src_size > SIZE_MAX / element_size) {
        return NULL;  // overflow
    }

    const size_t bytes_num = src_size * element_size;
    void* const dst        = EEBUS_MALLOC(bytes_num);
    if (dst == NULL) {
        return NULL;
    }

    memcpy(dst, src, bytes_num);
    return dst;
}

static inline void ArrayRelease(void* a) {
    if (a == NULL) {
        return;
    }

    EEBUS_FREE(a);
}

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_COMMON_ARRAY_UTIL_H_
