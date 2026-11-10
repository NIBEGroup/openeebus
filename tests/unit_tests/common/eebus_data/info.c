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
#include "tests/unit_tests/common/eebus_data/info.h"

#include "info_json.inc"
#include "src/common/eebus_data/eebus_data.h"
#include "src/common/eebus_malloc.h"

Info* InfoParse(const char* s) {
  return (Info*)EEBUS_DATA_PARSE(&info_data_cfg, s);
}

char* InfoPrintUnformatted(Info* info) {
  return EEBUS_DATA_PRINT_UNFORMATTED(&info_data_cfg, &info);
}

Info* InfoCopy(const Info* info) {
  Info* info_copy = NULL;
  EEBUS_DATA_COPY(&info_data_cfg, &info, &info_copy);
  return info_copy;
}

bool InfoCompare(const Info* info_a, const Info* info_b) {
  return EEBUS_DATA_COMPARE(&info_data_cfg, &info_a, &info_data_cfg, &info_b);
}

void InfoDelete(Info* info) {
  EEBUS_DATA_DELETE(&info_data_cfg, &info);
}

Employee* InfoEmployeeParse(const char* s) {
  return (Employee*)EEBUS_DATA_PARSE(&info_choice_data_cfg[kInfoEmployee], s);
}

char* InfoEmployeePrintUnformatted(Employee* employee) {
  return EEBUS_DATA_PRINT_UNFORMATTED(&info_choice_data_cfg[kInfoEmployee], &employee);
}

Employee* InfoEmployeeCopy(const Employee* employee) {
  Employee* employee_copy = NULL;
  EEBUS_DATA_COPY(&info_choice_data_cfg[kInfoEmployee], &employee, &employee_copy);
  return employee_copy;
}

void InfoEmployeeDelete(Employee* employee) {
  EEBUS_DATA_DELETE(&info_choice_data_cfg[kInfoEmployee], &employee);
}

InfoTag InfoTagCopy(InfoTag tag) {
  InfoTag tag_copy = NULL;
  EEBUS_DATA_COPY(&info_choice_data_cfg[kInfoTag], &tag, &tag_copy);
  return tag_copy;
}
