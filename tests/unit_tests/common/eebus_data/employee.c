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
#include "employee.h"

#include <string.h>

#include "employee_json.inc"
#include "src/common/eebus_data/eebus_data.h"
#include "src/common/eebus_malloc.h"
#include "src/common/string_util.h"

Employee* EmployeeParse(const char* s) {
  return (Employee*)EEBUS_DATA_PARSE(&employee_data_cfg, s);
}

char* EmployeePrintUnformatted(Employee* employee) {
  return EEBUS_DATA_PRINT_UNFORMATTED(&employee_data_cfg, &employee);
}

Employee* EmployeeCopyElements(const Employee* employee_src, const EmployeeElements* employee_elements) {
  Employee* employee_dst = NULL;
  EEBUS_DATA_READ_ELEMENTS(
      &employee_data_cfg,
      &employee_src,
      &employee_dst,
      &employee_elements_cfg,
      &employee_elements
  );
  return employee_dst;
}

Employee* EmployeeCopy(const Employee* employee) {
  Employee* employee_copy = NULL;
  EEBUS_DATA_COPY(&employee_data_cfg, &employee, &employee_copy);
  return employee_copy;
}

bool EmployeeCompare(const Employee* employee_a, const Employee* employee_b) {
  return EEBUS_DATA_COMPARE(&employee_data_cfg, &employee_a, &employee_data_cfg, &employee_b);
}

Employee*
EmployeeReadElements(const Employee* employee_src, Employee* employee_dst, const EmployeeElements* employee_elements) {
  EEBUS_DATA_READ_ELEMENTS(
      &employee_data_cfg,
      &employee_src,
      &employee_dst,
      &employee_elements_cfg,
      &employee_elements
  );
  return employee_dst;
}

Employee* EmployeeDeleteElements(Employee* employee, const EmployeeElements* employee_elements) {
  EEBUS_DATA_DELETE_ELEMENTS(&employee_data_cfg, &employee, &employee_elements_cfg, &employee_elements);
  return employee;
}

void EmployeeDelete(Employee* employee) {
  EEBUS_DATA_DELETE(&employee_data_cfg, &employee);
}

EmployeeElements* EmployeeElementsParse(const char* s) {
  return (EmployeeElements*)EEBUS_DATA_PARSE(&employee_elements_cfg, s);
}

void EmployeeElementsDelete(EmployeeElements* employee_elements) {
  EEBUS_DATA_DELETE(&employee_elements_cfg, &employee_elements);
}
