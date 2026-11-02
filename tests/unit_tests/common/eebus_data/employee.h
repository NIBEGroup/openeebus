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

#ifndef TESTS_UNIT_TESTS_COMMON_JSON_EMPLOYEE_H
#define TESTS_UNIT_TESTS_COMMON_JSON_EMPLOYEE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "src/common/eebus_data/eebus_data_tag.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

enum Position {
  kPositionDeveloper,
  kPositionManager,
  kPositionTester,
};

typedef enum Position Position;

typedef struct Employee Employee;

struct Employee {
  char* name;
  char* surname;
  uint32_t* id;
  uint8_t* age;
  int32_t* position;
  uint32_t* salary;
  TagType is_married;
  bool* has_masters_degree;
  uint8_t** report;
  size_t report_size;
};

typedef struct EmployeeElements EmployeeElements;

struct EmployeeElements {
  TagType name;
  TagType surname;
  TagType id;
  TagType age;
  TagType position;
  TagType salary;
  TagType is_married;
  TagType has_masters_degree;
  TagType report;
};

Employee* EmployeeParse(const char* s);
char* EmployeePrintUnformatted(Employee* employee);
Employee* EmployeeCopyElements(const Employee* employee_src, const EmployeeElements* employee_elements);
Employee* EmployeeCopy(const Employee* employee);
bool EmployeeCompare(const Employee* employee_a, const Employee* employee_b);
Employee*
EmployeeReadElements(const Employee* employee_src, Employee* employee_dst, const EmployeeElements* employee_elements);
Employee* EmployeeDeleteElements(Employee* employee, const EmployeeElements* employee_elements);
void EmployeeDelete(Employee* employee);

EmployeeElements* EmployeeElementsParse(const char* s);
void EmployeeElementsDelete(EmployeeElements* employee_elements);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // TESTS_UNIT_TESTS_COMMON_JSON_EMPLOYEE_H
