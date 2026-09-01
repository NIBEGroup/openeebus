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
 * @brief Eebus Data implementation
 */

#include <stdbool.h>

#include "src/common/api/eebus_data_interface.h"
#include "src/common/eebus_arguments.h"
#include "src/common/eebus_assert.h"
#include "src/common/eebus_data/eebus_data_simple.h"
#include "src/common/eebus_errors.h"
#include "src/common/string_util.h"

static void* CreateEmpty(const EebusDataCfg* cfg, void* base_addr);
static EebusError FromJsonObjectItems(const EebusDataCfg* cfg, void* base_addr, const JsonObject* json_obj);
static EebusError ToJsonObjectItems(const EebusDataCfg* cfg, const void* base_addr, JsonObject** json_obj);
static bool
Compare(const EebusDataCfg* cfg, const void* a_base_addr, const EebusDataCfg* b_cfg, const void* b_base_addr);
static EebusError ReadElements(
    const EebusDataCfg* cfg,
    const void* base_addr,
    void* dst_base_addr,
    const EebusDataCfg* elements_cfg,
    const void* elements_base_addr
);
static EebusError Write(const EebusDataCfg* cfg, void* base_addr, const void* src_base_addr);

const EebusDataInterface eebus_data_string_methods = {
    .create_empty          = CreateEmpty,
    .parse                 = EebusDataBaseParse,
    .print_unformatted     = EebusDataBasePrintUnformatted,
    .from_json_object_item = FromJsonObjectItems,
    .from_json_object      = EebusDataBaseFromJsonObject,
    .to_json_object_item   = ToJsonObjectItems,
    .to_json_object        = EebusDataBaseToJsonObject,
    .copy                  = EebusDataBaseCopy,
    .compare               = Compare,
    .is_null               = EebusDataSimpleIsNull,
    .is_empty              = EebusDataSimpleIsEmpty,
    .has_identifiers       = EebusDataBaseHasIdentifiers,
    .selectors_match       = EebusDataBaseSelectorsMatch,
    .identifiers_match     = EebusDataBaseIdentifiersMatch,
    .read_elements         = ReadElements,
    .write                 = Write,
    .write_elements        = EebusDataBaseWriteElements,
    .write_partial         = EebusDataBaseWritePartial,
    .delete_elements       = EebusDataSimpleDeleteElements,
    .delete_partial        = EebusDataBaseDeletePartial,
    .delete_               = EebusDataSimpleDelete,
};

void* CreateEmpty(const EebusDataCfg* cfg, void* base_addr) {
    UNUSED(cfg);
    UNUSED(base_addr);

    EEBUS_ASSERT_ALWAYS();
    return NULL;
}

EebusError FromJsonObjectItems(const EebusDataCfg* cfg, void* base_addr, const JsonObject* json_obj) {
    if (!JsonIsString(json_obj)) {
        return kEebusErrorParse;
    }

    char** const ps = (char**)((uint8_t*)base_addr + cfg->offset);

    *ps = StringCopy(JsonGetString(json_obj));

    return (*ps != NULL) ? kEebusErrorOk : kEebusErrorParse;
}

EebusError ToJsonObjectItems(const EebusDataCfg* cfg, const void* base_addr, JsonObject** json_obj) {
    const char** const ps = (const char**)((const uint8_t*)base_addr + cfg->offset);
    if (*ps == NULL) {
        *json_obj = NULL;
        return kEebusErrorOk;
    }

    *json_obj = JsonCreateString(*ps);
    return (*json_obj != NULL) ? kEebusErrorOk : kEebusErrorMemoryAllocate;
}

bool Compare(const EebusDataCfg* a_cfg, const void* a_base_addr, const EebusDataCfg* b_cfg, const void* b_base_addr) {
    if (!EEBUS_DATA_TYPE_EQ(a_cfg, b_cfg)) {
        return false;
    }

    const char** const a_buf = (const char**)((const uint8_t*)a_base_addr + a_cfg->offset);
    const char** const b_buf = (const char**)((const uint8_t*)b_base_addr + b_cfg->offset);

    if ((*a_buf == NULL) || (*b_buf == NULL)) {
        return *a_buf == *b_buf;
    }

    return strcmp(*a_buf, *b_buf) == 0;
}

EebusError ReadElements(
    const EebusDataCfg* cfg,
    const void* base_addr,
    void* dst_base_addr,
    const EebusDataCfg* elements_cfg,
    const void* elements_base_addr
) {
    if (EEBUS_DATA_IS_NULL(elements_cfg, elements_base_addr)) {
        // Should not be written - ok
        return kEebusErrorOk;
    };

    EEBUS_DATA_DELETE(cfg, dst_base_addr);
    return EEBUS_DATA_COPY(cfg, base_addr, dst_base_addr);
}

EebusError Write(const EebusDataCfg* cfg, void* base_addr, const void* src_base_addr) {
    const char** const src_buf = (const char**)((const uint8_t*)src_base_addr + cfg->offset);
    if (*src_buf == NULL) {
        EEBUS_DATA_DELETE(cfg, base_addr);
        return kEebusErrorOk;
    }

    const size_t src_buf_size = strlen((char*)*src_buf) + 1;

    char** buf      = (char**)((uint8_t*)base_addr + cfg->offset);
    size_t buf_size = 0;
    if (*buf != NULL) {
        buf_size = strlen((char*)*buf) + 1;
        if (buf_size != src_buf_size) {
            EEBUS_DATA_DELETE(cfg, base_addr);
            *buf = NULL;
        }
    }

    if (*buf == NULL) {
        *buf = EEBUS_MALLOC(src_buf_size);
        if (*buf == NULL) {
            return kEebusErrorMemoryAllocate;
        }
    }

    strcpy(*buf, *src_buf);
    return kEebusErrorOk;
}
