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

#ifndef TESTS_UNIT_TESTS_COMMON_JSON_PERSON_H
#define TESTS_UNIT_TESTS_COMMON_JSON_PERSON_H

#include <stdbool.h>
#include <stdint.h>

#include "src/common/eebus_data/eebus_data_tag.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct Address Address;

struct Address {
  char* street;
  char* city;
};

typedef struct AddressElements AddressElements;

struct AddressElements {
  TagType street;
  TagType city;
};

typedef struct Person Person;

struct Person {
  char* name;
  uint8_t* age;
  Address* address;
};

typedef struct PersonElements PersonElements;

struct PersonElements {
  TagType name;
  TagType age;
  AddressElements* address;
};

Person* PersonParse(const char* s);
char* PersonPrintUnformatted(Person* person);
Person* PersonCopyElements(const Person* person_src, const PersonElements* elements_buf);
Person* PersonCopy(const Person* person);
bool PersonCompare(const Person* person_a, const Person* person_b);
Person* PersonReadElements(const Person* person_src, Person* person_dst, const PersonElements* elements_buf);
Person* PersonDeleteElements(Person* person, const PersonElements* person_elements);
void PersonDelete(Person* person);

PersonElements* PersonElementsParse(const char* s);
void PersonElementsDelete(PersonElements* person);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // TESTS_UNIT_TESTS_COMMON_JSON_PERSON_H
