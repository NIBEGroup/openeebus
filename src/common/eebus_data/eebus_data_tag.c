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
 * @brief EEEBUS Data Tag implementation
 */

#include "src/common/eebus_data/eebus_data_tag.h"

#include <stdbool.h>

#include "src/common/api/eebus_data_interface.h"
#include "src/common/eebus_arguments.h"
#include "src/common/eebus_assert.h"
#include "src/common/eebus_data/eebus_data_base.h"
#include "src/common/eebus_errors.h"

static void* CreateEmpty(const EebusDataCfg* cfg, void* base_addr);
static EebusError FromJsonObjectItem(const EebusDataCfg* cfg, void* base_addr, const JsonObject* json_obj);
static EebusError ToJsonObjectItem(const EebusDataCfg* cfg, const void* base_addr, JsonObject** json_obj);
bool Compare(const EebusDataCfg* a_cfg, const void* a_base_addr, const EebusDataCfg* b_cfg, const void* b_base_addr);
static bool IsNull(const EebusDataCfg* cfg, const void* base_addr);
static bool IsEmpty(const EebusDataCfg* cfg, const void* base_addr);
static EebusError ReadElements(
    const EebusDataCfg* cfg,
    const void* base_addr,
    void* dst_base_addr,
    const EebusDataCfg* elements_cfg,
    const void* elements_base_addr
);
static EebusError Write(const EebusDataCfg* cfg, void* base_addr, const void* src_base_addr);
void DeleteElements(
    const EebusDataCfg* cfg,
    void* base_addr,
    const EebusDataCfg* elements_cfg,
    const void* elements_base_addr
);
void Delete(const EebusDataCfg* cfg, void* base_addr);

const EebusDataInterface eebus_data_tag_methods = {
    .create_empty          = CreateEmpty,
    .parse                 = EebusDataBaseParse,
    .print_unformatted     = EebusDataBasePrintUnformatted,
    .from_json_object_item = FromJsonObjectItem,
    .from_json_object      = EebusDataBaseFromJsonObject,
    .to_json_object_item   = ToJsonObjectItem,
    .to_json_object        = EebusDataBaseToJsonObject,
    .copy                  = EebusDataBaseCopy,
    .compare               = Compare,
    .is_null               = IsNull,
    .is_empty              = IsEmpty,
    .has_identifiers       = EebusDataBaseHasIdentifiers,
    .selectors_match       = EebusDataBaseSelectorsMatch,
    .identifiers_match     = EebusDataBaseIdentifiersMatch,
    .read_elements         = ReadElements,
    .write                 = Write,
    .write_elements        = EebusDataBaseWriteElements,
    .write_partial         = EebusDataBaseWritePartial,
    .delete_elements       = DeleteElements,
    .delete_partial        = EebusDataBaseDeletePartial,
    .delete_               = Delete,
};

void* CreateEmpty(const EebusDataCfg* cfg, void* base_addr) {
    TagType* const buf = (TagType*)((uint8_t*)base_addr + cfg->offset);

    return *buf = EEBUS_TAG_RESET;
}

EebusError FromJsonObjectItem(const EebusDataCfg* cfg, void* base_addr, const JsonObject* json_obj) {
    TagType* const buf = (TagType*)((uint8_t*)base_addr + cfg->offset);

    if (json_obj == NULL) {
        *buf = EEBUS_TAG_RESET;
        return kEebusErrorOk;
    }

    if (JsonIsArray(json_obj) && (JsonGetArraySize(json_obj) == 0)) {
        *buf = EEBUS_TAG_SET;
        return kEebusErrorOk;
    }

    return kEebusErrorParse;
}

EebusError ToJsonObjectItem(const EebusDataCfg* cfg, const void* base_addr, JsonObject** json_obj) {
    const TagType* const buf = (const TagType*)((const uint8_t*)base_addr + cfg->offset);

    if (*buf == EEBUS_TAG_RESET) {
        *json_obj = NULL;
        return kEebusErrorOk;
    }

    *json_obj = JsonCreateArray();
    return (*json_obj != NULL) ? kEebusErrorOk : kEebusErrorMemoryAllocate;
}

bool Compare(const EebusDataCfg* a_cfg, const void* a_base_addr, const EebusDataCfg* b_cfg, const void* b_base_addr) {
    if (!EEBUS_DATA_TYPE_EQ(a_cfg, b_cfg)) {
        return false;
    }

    return EEBUS_DATA_IS_NULL(a_cfg, a_base_addr) == EEBUS_DATA_IS_NULL(b_cfg, b_base_addr);
}

bool IsNull(const EebusDataCfg* cfg, const void* base_addr) {
    const TagType* const buf = (const TagType*)((const uint8_t*)base_addr + cfg->offset);
    // Consider tag set to false is an equivalent to NULL
    return *buf == EEBUS_TAG_RESET;
}

bool IsEmpty(const EebusDataCfg* cfg, const void* base_addr) {
    UNUSED(cfg);
    UNUSED(base_addr);

    return false;
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

    return EEBUS_DATA_COPY(cfg, base_addr, dst_base_addr);
}

EebusError Write(const EebusDataCfg* cfg, void* base_addr, const void* src_base_addr) {
    const TagType* const src_buf = (const TagType*)((const uint8_t*)src_base_addr + cfg->offset);
    if (*src_buf == NULL) {
        EEBUS_DATA_DELETE(cfg, base_addr);
        return kEebusErrorOk;
    }

    TagType* const buf = (TagType*)((uint8_t*)base_addr + cfg->offset);

    *buf = *src_buf;
    return kEebusErrorOk;
}

void DeleteElements(
    const EebusDataCfg* cfg,
    void* base_addr,
    const EebusDataCfg* elements_cfg,
    const void* elements_base_addr
) {
    if (EEBUS_DATA_IS_NULL(elements_cfg, elements_base_addr)) {
        // Should not be deleted - ok
        return;
    }

    EEBUS_DATA_DELETE(cfg, base_addr);
}

void Delete(const EebusDataCfg* cfg, void* base_addr) {
    TagType* const buf = (TagType*)((uint8_t*)base_addr + cfg->offset);

    *buf = EEBUS_TAG_RESET;
}
