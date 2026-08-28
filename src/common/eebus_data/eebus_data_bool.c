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
 * @brief EEEBUS Data Bool implementation
 */

#include <stdbool.h>

#include "src/common/api/eebus_data_interface.h"
#include "src/common/eebus_assert.h"
#include "src/common/eebus_data/eebus_data_numeric.h"
#include "src/common/eebus_data/eebus_data_simple.h"
#include "src/common/eebus_errors.h"
#include "src/common/eebus_malloc.h"

static EebusError FromJsonObjectItem(const EebusDataCfg* cfg, void* base_addr, const JsonObject* json_obj);
static EebusError ToJsonObjectItem(const EebusDataCfg* cfg, const void* base_addr, JsonObject** json_obj);

const EebusDataInterface eebus_data_bool_methods = {
    .create_empty          = EebusDataBaseCreateEmpty,
    .parse                 = EebusDataBaseParse,
    .print_unformatted     = EebusDataBasePrintUnformatted,
    .from_json_object_item = FromJsonObjectItem,
    .from_json_object      = EebusDataBaseFromJsonObject,
    .to_json_object_item   = ToJsonObjectItem,
    .to_json_object        = EebusDataBaseToJsonObject,
    .copy                  = EebusDataBaseCopy,
    .copy_matching         = EebusDataBaseCopyMatching,
    .compare               = EebusDataSimpleCompare,
    .is_null               = EebusDataSimpleIsNull,
    .is_empty              = EebusDataSimpleIsEmpty,
    .has_identifiers       = EebusDataBaseHasIdentifiers,
    .selectors_match       = EebusDataBaseSelectorsMatch,
    .identifiers_match     = EebusDataBaseIdentifiersMatch,
    .read_elements         = EebusDataSimpleReadElements,
    .write                 = EebusDataSimpleWrite,
    .write_elements        = EebusDataBaseWriteElements,
    .write_partial         = EebusDataBaseWritePartial,
    .delete_elements       = EebusDataSimpleDeleteElements,
    .delete_               = EebusDataSimpleDelete,
    .delete_partial        = EebusDataBaseDeletePartial,
};

EebusError FromJsonObjectItem(const EebusDataCfg* cfg, void* base_addr, const JsonObject* json_obj) {
    if (!JsonIsBool(json_obj)) {
        return kEebusErrorParse;
    }

    bool* const buf = (bool*)EEBUS_DATA_CREATE_EMPTY(cfg, base_addr);
    if (buf == NULL) {
        return kEebusErrorMemoryAllocate;
    }

    *buf = JsonGetBool(json_obj);
    return kEebusErrorOk;
}

EebusError ToJsonObjectItem(const EebusDataCfg* cfg, const void* base_addr, JsonObject** json_obj) {
    const bool** const buf = (const bool**)((const uint8_t*)base_addr + cfg->offset);
    if (*buf == NULL) {
        *json_obj = NULL;
        return kEebusErrorOk;
    }

    *json_obj = JsonCreateBool(**buf);
    return (*json_obj != NULL) ? kEebusErrorOk : kEebusErrorMemoryAllocate;
}
