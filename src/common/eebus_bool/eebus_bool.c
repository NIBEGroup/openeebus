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
 * @brief Boolean utility functions implementation
 */

#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "src/common/eebus_errors.h"

const char* const kTrueString  = "true";
const char* const kFalseString = "false";

EebusError EebusBoolParse(const char* s, bool* value) {
    if ((s == NULL) || (value == NULL)) {
        return kEebusErrorInputArgumentNull;
    }

    if (strcmp(s, kTrueString) == 0) {
        *value = true;
    } else if (strcmp(s, kFalseString) == 0) {
        *value = false;
    } else {
        return kEebusErrorInputArgument;
    }

    return kEebusErrorOk;
}
