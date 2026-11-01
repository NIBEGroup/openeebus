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
#include "person.h"

#include <stddef.h>

#include "person_json.inc"
#include "src/common/eebus_data/eebus_data.h"
#include "src/common/eebus_malloc.h"
#include "src/common/string_util.h"

Person* PersonParse(const char* s) {
  return (Person*)EEBUS_DATA_PARSE(&person_data_cfg, s);
}

char* PersonPrintUnformatted(Person* person) {
  return EEBUS_DATA_PRINT_UNFORMATTED(&person_data_cfg, &person);
}

Person* PersonCopyElements(const Person* person_src, const PersonElements* person_elements) {
  Person* person_dst = NULL;
  EEBUS_DATA_READ_ELEMENTS(&person_data_cfg, &person_src, &person_dst, &person_elements_cfg, &person_elements);
  return person_dst;
}

Person* PersonCopy(const Person* person) {
  Person* person_copy = NULL;
  EEBUS_DATA_COPY(&person_data_cfg, &person, &person_copy);
  return person_copy;
}

bool PersonCompare(const Person* person_a, const Person* person_b) {
  return EEBUS_DATA_COMPARE(&person_data_cfg, &person_a, &person_data_cfg, &person_b);
}

Person* PersonReadElements(const Person* person_src, Person* person_dst, const PersonElements* person_elements) {
  EEBUS_DATA_READ_ELEMENTS(&person_data_cfg, &person_src, &person_dst, &person_elements_cfg, &person_elements);
  return person_dst;
}

Person* PersonDeleteElements(Person* person, const PersonElements* person_elements) {
  EEBUS_DATA_DELETE_ELEMENTS(&person_data_cfg, &person, &person_elements_cfg, &person_elements);
  return person;
}

void PersonDelete(Person* person) {
  EEBUS_DATA_DELETE(&person_data_cfg, &person);
}

PersonElements* PersonElementsParse(const char* s) {
  return (PersonElements*)EEBUS_DATA_PARSE(&person_elements_cfg, s);
}

void PersonElementsDelete(PersonElements* person_elements) {
  EEBUS_DATA_DELETE(&person_elements_cfg, &person_elements);
}
