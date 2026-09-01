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
#include <stddef.h>
#include <string.h>

#include "src/common/api/eebus_data_interface.h"
#include "src/common/eebus_data/eebus_data_enum.h"

size_t EebusDataGetCfgSize(const EebusDataCfg* cfg_first) {
    if (cfg_first == NULL) {
        return 0;
    }

    size_t n = 0;
    while (cfg_first[n].name != NULL) {
        ++n;
    }

    return n;
}

size_t EebusDataGetEnumMappingSize(const EnumMapping* enum_mapping_first) {
    if (enum_mapping_first == NULL) {
        return 0;
    }

    size_t n = 0;
    while (enum_mapping_first[n].name != NULL) {
        ++n;
    }

    return n;
}

const EnumMapping* EebusDataGetEnumMappingWithValue(const EnumMapping* enum_mapping_first, int32_t value) {
    for (const EnumMapping* mapping_it = enum_mapping_first; mapping_it->name != NULL; ++mapping_it) {
        if (mapping_it->value == value) {
            return mapping_it;
        }
    }

    return NULL;
}

const EnumMapping* EebusDataGetEnumMappingWithName(const EnumMapping* enum_mapping_first, const char* name) {
    if (name == NULL) {
        return NULL;
    }

    for (const EnumMapping* mapping_it = enum_mapping_first; mapping_it->name != NULL; ++mapping_it) {
        if (strcmp(mapping_it->name, name) == 0) {
            return mapping_it;
        }
    }

    return NULL;
}
