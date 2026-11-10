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
#include "tests/unit_tests/common/eebus_data/somebody.h"

#include <stddef.h>

#include "somebody_json.inc"
#include "src/common/eebus_data/eebus_data.h"
#include "src/common/eebus_malloc.h"
#include "tests/unit_tests/common/eebus_data/employee.h"
#include "tests/unit_tests/common/eebus_data/person.h"

Somebody* SomebodyParse(const char* s) {
  return (Somebody*)EEBUS_DATA_PARSE(&somebody_data_cfg, s);
}

char* SomebodyPrintUnformatted(Somebody* somebody) {
  return EEBUS_DATA_PRINT_UNFORMATTED(&somebody_data_cfg, &somebody);
}

Somebody* SomebodyCopy(const Somebody* somebody) {
  Somebody* somebody_copy = NULL;
  EEBUS_DATA_COPY(&somebody_data_cfg, &somebody, &somebody_copy);
  return somebody_copy;
}

bool SomebodyCompare(const Somebody* somebody_a, const Somebody* somebody_b) {
  return EEBUS_DATA_COMPARE(&somebody_data_cfg, &somebody_a, &somebody_data_cfg, &somebody_b);
}

void SomebodyDelete(Somebody* somebody) {
  EEBUS_DATA_DELETE(&somebody_data_cfg, &somebody);
}
