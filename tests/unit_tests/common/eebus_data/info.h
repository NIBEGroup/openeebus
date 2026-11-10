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

#ifndef TESTS_UNIT_TESTS_COMMON_JSON_INFO_H
#define TESTS_UNIT_TESTS_COMMON_JSON_INFO_H

#include <stdbool.h>
#include <stdint.h>

#include "tests/unit_tests/common/eebus_data/employee.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

enum {
  kInfoPadding,
  kInfoEmployee,
  kInfoTag,
  kInfoPerson,
};

typedef TagType InfoTag;

typedef struct Info Info;

struct Info {
  int8_t padding[7];
  void* data;
  int32_t data_type_id;
};

Info* InfoParse(const char* s);
char* InfoPrintUnformatted(Info* info);
Info* InfoCopy(const Info* info);
bool InfoCompare(const Info* info_a, const Info* info_b);
void InfoDelete(Info* info);

// EEBUS Data Choice Element relate unit test functions
Employee* InfoEmployeeParse(const char* s);
char* InfoEmployeePrintUnformatted(Employee* employee);
Employee* InfoEmployeeCopy(const Employee* employee);
void InfoEmployeeDelete(Employee* employee);
InfoTag InfoTagCopy(InfoTag tag);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // TESTS_UNIT_TESTS_COMMON_JSON_INFO_H
