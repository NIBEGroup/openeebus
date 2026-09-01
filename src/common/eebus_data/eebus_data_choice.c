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
 * @brief EEEBUS Data Choice implementation
 */

#include <stdbool.h>

#include "src/common/api/eebus_data_interface.h"
#include "src/common/eebus_arguments.h"
#include "src/common/eebus_assert.h"
#include "src/common/eebus_data/eebus_data_base.h"
#include "src/common/eebus_data/eebus_data_util.h"
#include "src/common/eebus_errors.h"

static void* CreateEmpty(const EebusDataCfg* cfg, void* base_addr);
static EebusError FromJsonObjectItem(const EebusDataCfg* cfg, void* base_addr, const JsonObject* json_item);
static EebusError FromJsonObject(const EebusDataCfg* cfg, void* base_addr, const JsonObject* json_obj, bool is_root);
static EebusError ToJsonObjectItem(const EebusDataCfg* cfg, const void* base_addr, JsonObject** json_item);
static EebusError ToJsonObject(const EebusDataCfg* cfg, const void* base_addr, JsonObject* json_obj, bool is_root);
static bool
Compare(const EebusDataCfg* a_cfg, const void* a_base_addr, const EebusDataCfg* b_cfg, const void* b_base_addr);
static bool IsNull(const EebusDataCfg* cfg, const void* base_addr);
static bool IsEmpty(const EebusDataCfg* cfg, const void* base_addr);
static bool HasIdentifiers(const EebusDataCfg* cfg, const void* base_addr);
static bool SelectorsMatch(
    const EebusDataCfg* cfg,
    const void* base_addr,
    const EebusDataCfg* selectors_cfg,
    const void* selectors_base_addr
);
static bool IdentifiersMatch(const EebusDataCfg* cfg, const void* base_addr, const void* src_base_addr);
static EebusError ReadElements(
    const EebusDataCfg* cfg,
    const void* base_addr,
    void* dst_base_addr,
    const EebusDataCfg* elements_cfg,
    const void* elements_base_addr
);
static EebusError Write(const EebusDataCfg* cfg, void* base_addr, const void* src_base_addr);
static void DeleteElements(
    const EebusDataCfg* cfg,
    void* base_addr,
    const EebusDataCfg* elements_cfg,
    const void* elements_base_addr
);
static void Delete(const EebusDataCfg* cfg, void* base_addr);

const EebusDataInterface eebus_data_choice_methods = {
    .create_empty          = CreateEmpty,
    .parse                 = EebusDataBaseParse,
    .print_unformatted     = EebusDataBasePrintUnformatted,
    .from_json_object_item = FromJsonObjectItem,
    .from_json_object      = FromJsonObject,
    .to_json_object_item   = ToJsonObjectItem,
    .to_json_object        = ToJsonObject,
    .copy                  = EebusDataBaseCopy,
    .copy_matching         = EebusDataBaseCopyMatching,
    .compare               = Compare,
    .is_null               = IsNull,
    .is_empty              = IsEmpty,
    .has_identifiers       = HasIdentifiers,
    .selectors_match       = SelectorsMatch,
    .identifiers_match     = IdentifiersMatch,
    .read_elements         = ReadElements,
    .write                 = Write,
    .write_elements        = EebusDataBaseWriteElements,
    .write_partial         = EebusDataBaseWritePartial,
    .delete_elements       = DeleteElements,
    .delete_partial        = EebusDataBaseDeletePartial,
    .delete_               = Delete,
};

void* CreateEmpty(const EebusDataCfg* cfg, void* base_addr) {
    UNUSED(cfg);
    UNUSED(base_addr);

    EEBUS_ASSERT_ALWAYS();
    return NULL;
}

EebusError FromJsonObjectItem(const EebusDataCfg* cfg, void* base_addr, const JsonObject* json_item) {
    UNUSED(cfg);
    UNUSED(base_addr);
    UNUSED(json_item);

    EEBUS_ASSERT_ALWAYS();
    return kEebusErrorOther;
}

EebusError FromJsonObject(const EebusDataCfg* cfg, void* base_addr, const JsonObject* json_obj, bool is_root) {
    const EebusDataCfg* const choice_cfg = (const EebusDataCfg*)cfg->metadata;

    void** const data      = (void**)((uint8_t*)base_addr + cfg->offset);
    int32_t* const type_id = (int32_t*)((uint8_t*)base_addr + cfg->type_id_offset);

    for (size_t i = 0; choice_cfg[i].name != NULL; ++i) {
        const JsonObject* const json_item = JsonGetItem(json_obj, choice_cfg[i].name, is_root);
        if (json_item != NULL) {
            *type_id = (int32_t)i;
            return EEBUS_DATA_FROM_JSON_OBJECT_ITEM(&choice_cfg[i], (void*)data, json_item);
        }
    }

    return kEebusErrorOk;
}

EebusError ToJsonObjectItem(const EebusDataCfg* cfg, const void* base_addr, JsonObject** json_item) {
    UNUSED(cfg);
    UNUSED(base_addr);
    UNUSED(json_item);

    EEBUS_ASSERT_ALWAYS();
    return kEebusErrorOther;
}

static EebusError ToJsonObject(const EebusDataCfg* cfg, const void* base_addr, JsonObject* json_obj, bool is_root) {
    const void** const data = (const void**)((const uint8_t*)base_addr + cfg->offset);
    if (*data == NULL) {
        return kEebusErrorOk;
    }

    const int32_t* const type_id         = (const int32_t*)((const uint8_t*)base_addr + cfg->type_id_offset);
    const EebusDataCfg* const choice_cfg = (const EebusDataCfg*)cfg->metadata;
    if (*type_id >= (int32_t)EebusDataGetCfgSize(choice_cfg)) {
        return kEebusErrorInputArgumentOutOfRange;
    }

    return EEBUS_DATA_TO_JSON_OBJECT(&choice_cfg[*type_id], (void*)data, json_obj, is_root);
}

bool Compare(const EebusDataCfg* a_cfg, const void* a_base_addr, const EebusDataCfg* b_cfg, const void* b_base_addr) {
    if (!EEBUS_DATA_TYPE_EQ(a_cfg, b_cfg)) {
        return false;
    }

    const EebusDataCfg* const a_choice_cfg = (const EebusDataCfg*)a_cfg->metadata;
    const EebusDataCfg* const b_choice_cfg = (const EebusDataCfg*)b_cfg->metadata;

    const int32_t* const a_type_id = (const int32_t*)((const uint8_t*)a_base_addr + a_cfg->type_id_offset);
    const int32_t* const b_type_id = (const int32_t*)((const uint8_t*)b_base_addr + b_cfg->type_id_offset);

    if ((*a_type_id >= (int32_t)EebusDataGetCfgSize(a_choice_cfg))
        || (*b_type_id >= (int32_t)EebusDataGetCfgSize(b_choice_cfg))) {
        EEBUS_ASSERT_ALWAYS();
        return false;
    }

    const void* const a_buf = (const uint8_t*)a_base_addr + a_cfg->offset;
    const void* const b_buf = (const uint8_t*)b_base_addr + b_cfg->offset;
    return EEBUS_DATA_COMPARE(&a_choice_cfg[*a_type_id], a_buf, &b_choice_cfg[*b_type_id], b_buf);
}

bool IsNull(const EebusDataCfg* cfg, const void* base_addr) {
    const EebusDataCfg* const choice_cfg = (const EebusDataCfg*)cfg->metadata;

    const int32_t* const type_id = (const int32_t*)((const uint8_t*)base_addr + cfg->type_id_offset);

    if (*type_id >= (int32_t)EebusDataGetCfgSize(choice_cfg)) {
        EEBUS_ASSERT_ALWAYS();
        return false;
    }

    return EEBUS_DATA_IS_NULL(&choice_cfg[*type_id], (const uint8_t*)base_addr + cfg->offset);
}

bool IsEmpty(const EebusDataCfg* cfg, const void* base_addr) {
    const EebusDataCfg* const choice_cfg = (const EebusDataCfg*)cfg->metadata;

    const int32_t* const type_id = (const int32_t*)((const uint8_t*)base_addr + cfg->type_id_offset);

    if (*type_id >= (int32_t)EebusDataGetCfgSize(choice_cfg)) {
        EEBUS_ASSERT_ALWAYS();
        return false;
    }

    return EEBUS_DATA_IS_EMPTY(&choice_cfg[*type_id], (const uint8_t*)base_addr + cfg->offset);
}

bool HasIdentifiers(const EebusDataCfg* cfg, const void* base_addr) {
    UNUSED(cfg);
    UNUSED(base_addr);

    EEBUS_ASSERT_ALWAYS();
    return false;
}

bool SelectorsMatch(
    const EebusDataCfg* cfg,
    const void* base_addr,
    const EebusDataCfg* selectors_cfg,
    const void* selectors_base_addr
) {
    UNUSED(cfg);
    UNUSED(base_addr);
    UNUSED(selectors_cfg);
    UNUSED(selectors_base_addr);

    EEBUS_ASSERT_ALWAYS();
    return false;
}

bool IdentifiersMatch(const EebusDataCfg* cfg, const void* base_addr, const void* src_base_addr) {
    UNUSED(cfg);
    UNUSED(base_addr);
    UNUSED(src_base_addr);

    EEBUS_ASSERT_ALWAYS();
    return false;
}

EebusError ReadElements(
    const EebusDataCfg* cfg,
    const void* base_addr,
    void* dst_base_addr,
    const EebusDataCfg* elements_cfg,
    const void* elements_base_addr
) {
    UNUSED(cfg);
    UNUSED(base_addr);
    UNUSED(dst_base_addr);
    UNUSED(elements_cfg);
    UNUSED(elements_base_addr);

    EEBUS_ASSERT_ALWAYS();
    return kEebusErrorOther;
}

EebusError Write(const EebusDataCfg* cfg, void* base_addr, const void* src_base_addr) {
    const EebusDataCfg* const choice_cfg = (const EebusDataCfg*)cfg->metadata;

    const int32_t* const src_type_id = (const int32_t*)((const uint8_t*)src_base_addr + cfg->type_id_offset);
    int32_t* const type_id           = (int32_t*)((uint8_t*)base_addr + cfg->type_id_offset);

    if (*src_type_id >= (int32_t)EebusDataGetCfgSize(choice_cfg)) {
        EEBUS_ASSERT_ALWAYS();
        return kEebusErrorInputArgument;
    }

    *type_id = *src_type_id;
    EEBUS_DATA_WRITE(
        &choice_cfg[*src_type_id],
        (uint8_t*)base_addr + cfg->offset,
        (const uint8_t*)src_base_addr + cfg->offset
    );
    return kEebusErrorOk;
}

void DeleteElements(
    const EebusDataCfg* cfg,
    void* base_addr,
    const EebusDataCfg* elements_cfg,
    const void* elements_base_addr
) {
    UNUSED(cfg);
    UNUSED(base_addr);
    UNUSED(elements_cfg);
    UNUSED(elements_base_addr);

    EEBUS_ASSERT_ALWAYS();
}

void Delete(const EebusDataCfg* cfg, void* base_addr) {
    const EebusDataCfg* const choice_cfg = (const EebusDataCfg*)cfg->metadata;
    const int32_t* const choice_type_id  = (const int32_t*)((const uint8_t*)base_addr + cfg->type_id_offset);

    if (*choice_type_id >= (int32_t)EebusDataGetCfgSize(choice_cfg)) {
        EEBUS_ASSERT_ALWAYS();
        return;
    }

    EEBUS_DATA_DELETE(&choice_cfg[*choice_type_id], (uint8_t*)base_addr + cfg->offset);
}
