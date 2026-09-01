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
 * @file eebus_malloc.h
 * @brief Definitions of OS abstraction functions for malloc and free
 */

#ifndef SRC_COMMON_EEBUS_MALLOC_H_
#define SRC_COMMON_EEBUS_MALLOC_H_

#if defined(__freertos__)
#include <freertos/FreeRTOS.h>

#define EEBUS_MALLOC(size) pvPortMalloc((size))
#define EEBUS_FREE(size) vPortFree((size))
#elif defined(MEMORY_LEAKS_TEST)
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

void* test_malloc(size_t size, const char* file_name, int line);
void test_free(void* p);

#define EEBUS_MALLOC(size) test_malloc((size), __FILE__, __LINE__)
#define EEBUS_FREE(size) test_free((size))

#ifdef __cplusplus
}
#endif  // __cplusplus

#else
#include <stdlib.h>

#define EEBUS_MALLOC(size) malloc((size))
#define EEBUS_FREE(size) free((size))
#endif

#endif /* SRC_COMMON_EEBUS_MALLOC_H_ */
