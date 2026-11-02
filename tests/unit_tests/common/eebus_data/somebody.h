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
 * @brief Simple data structure to be tested within Json serialization/deserialization
 */

#ifndef TESTS_UNIT_TESTS_COMMON_JSON_SOMEBODY_H
#define TESTS_UNIT_TESTS_COMMON_JSON_SOMEBODY_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct Somebody Somebody;

struct Somebody {
  int8_t pading[5];
  void* data;
  int32_t data_type_id;
};

Somebody* SomebodyParse(const char* s);
char* SomebodyPrintUnformatted(Somebody* somebody);
Somebody* SomebodyCopy(const Somebody* somebody);
bool SomebodyCompare(const Somebody* somebody_a, const Somebody* somebody_b);
void SomebodyDelete(Somebody* somebody);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // TESTS_UNIT_TESTS_COMMON_JSON_SOMEBODY_H
