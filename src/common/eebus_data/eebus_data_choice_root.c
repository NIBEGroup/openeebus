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
 * @brief EEEBUS Data Choice Root implementation
 */

#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "src/common/api/eebus_data_interface.h"
#include "src/common/eebus_arguments.h"
#include "src/common/eebus_assert.h"
#include "src/common/eebus_data/eebus_data_base.h"
#include "src/common/eebus_malloc.h"

static void* Parse(const EebusDataCfg* cfg, const char* s);
static char* PrintUnformatted(const EebusDataCfg* cfg, const void* base_addr);
static EebusError FromJsonObjectItem(const EebusDataCfg* cfg, void* base_addr, const JsonObject* json_obj);
static EebusError FromJsonObject(const EebusDataCfg* cfg, void* base_addr, const JsonObject* json_obj, bool is_root);
static EebusError ToJsonObjectItem(const EebusDataCfg* cfg, const void* base_addr, JsonObject** json_obj);
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
static EebusError WritePartial(
    const EebusDataCfg* cfg,
    void* base_addr,
    const void* src_base_addr,
    const EebusDataCfg* selectors_cfg,
    const void* selectors_base_addr,
    SelectorsMatcher selectors_matcher
);
static void DeleteElements(
    const EebusDataCfg* cfg,
    void* base_addr,
    const EebusDataCfg* elements_cfg,
    const void* elements_base_addr
);
static void DeletePartial(
    const EebusDataCfg* cfg,
    void* base_addr,
    const EebusDataCfg* selectors_cfg,
    const void* selectors_base_addr,
    SelectorsMatcher selectors_matcher,
    const EebusDataCfg* elements_cfg,
    const void* elements_base_addr
);
static void Delete(const EebusDataCfg* cfg, void* base_addr);

const EebusDataInterface eebus_data_choice_root_methods = {
    .create_empty          = EebusDataBaseCreateEmpty,
    .parse                 = Parse,
    .print_unformatted     = PrintUnformatted,
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
    .write_partial         = WritePartial,
    .delete_elements       = DeleteElements,
    .delete_partial        = DeletePartial,
    .delete_               = Delete,
};

void* Parse(const EebusDataCfg* cfg, const char* s) {
    const JsonObject* const json_root = JsonParse(s);
    if (json_root == NULL) {
        return NULL;
    }

    void* const buf = EEBUS_DATA_CREATE_EMPTY(cfg, (void*)&buf);
    if (buf == NULL) {
        return NULL;
    }

    const EebusDataCfg* const choice_cfg = (const EebusDataCfg*)cfg->metadata;

    const EebusError ret = EEBUS_DATA_FROM_JSON_OBJECT(choice_cfg, buf, json_root, true);
    JsonDelete((JsonObject*)json_root);
    if (ret != kEebusErrorOk) {
        EEBUS_DATA_DELETE(cfg, (void*)&buf);
        return NULL;
    }

    return buf;
}

char* PrintUnformatted(const EebusDataCfg* cfg, const void* base_addr) {
    if (base_addr == NULL) {
        return NULL;
    }

    JsonObject* const json_root = JsonCreateObject();
    if (json_root == NULL) {
        return NULL;
    }

    char* s = NULL;

    const EebusDataCfg* const choice_cfg = (const EebusDataCfg*)cfg->metadata;

    void** const buf     = (void**)base_addr;
    const EebusError ret = EEBUS_DATA_TO_JSON_OBJECT(choice_cfg, *buf, json_root, true);
    if (ret == kEebusErrorOk) {
        s = JsonPrintUnformatted(json_root);
    }

    JsonDelete(json_root);
    return s;
}

EebusError FromJsonObjectItem(const EebusDataCfg* cfg, void* base_addr, const JsonObject* json_obj) {
    UNUSED(cfg);
    UNUSED(base_addr);
    UNUSED(json_obj);

    EEBUS_ASSERT_ALWAYS();
    return kEebusErrorOther;
}

EebusError ToJsonObject(const EebusDataCfg* cfg, const void* base_addr, JsonObject* json_obj, bool is_root) {
    UNUSED(cfg);
    UNUSED(base_addr);
    UNUSED(json_obj);
    UNUSED(is_root);

    EEBUS_ASSERT_ALWAYS();
    return kEebusErrorOther;
}

EebusError ToJsonObjectItem(const EebusDataCfg* cfg, const void* base_addr, JsonObject** json_obj) {
    UNUSED(cfg);
    UNUSED(base_addr);
    UNUSED(json_obj);

    EEBUS_ASSERT_ALWAYS();
    return kEebusErrorOther;
}

EebusError FromJsonObject(const EebusDataCfg* cfg, void* base_addr, const JsonObject* json_obj, bool is_root) {
    UNUSED(cfg);
    UNUSED(base_addr);
    UNUSED(json_obj);
    UNUSED(is_root);

    EEBUS_ASSERT_ALWAYS();
    return kEebusErrorOther;
}

bool Compare(const EebusDataCfg* a_cfg, const void* a_base_addr, const EebusDataCfg* b_cfg, const void* b_base_addr) {
    UNUSED(a_cfg);
    UNUSED(a_base_addr);
    UNUSED(b_cfg);
    UNUSED(b_base_addr);

    EEBUS_ASSERT_ALWAYS();
    return false;
}

bool IsNull(const EebusDataCfg* cfg, const void* base_addr) {
    UNUSED(cfg);
    UNUSED(base_addr);

    EEBUS_ASSERT_ALWAYS();
    return false;
}

bool IsEmpty(const EebusDataCfg* cfg, const void* base_addr) {
    UNUSED(cfg);
    UNUSED(base_addr);

    EEBUS_ASSERT_ALWAYS();
    return false;
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
    if ((base_addr == NULL) || (src_base_addr == NULL)) {
        return kEebusErrorInputArgument;
    }

    const void** const src_buf = (const void**)src_base_addr;
    void** const buf           = (void**)base_addr;

    if (*src_buf == NULL) {
        EEBUS_DATA_DELETE(cfg, base_addr);
        return kEebusErrorOk;
    }

    if (*buf == NULL) {
        *buf = EEBUS_DATA_CREATE_EMPTY(cfg, base_addr);
        if (*buf == NULL) {
            return kEebusErrorMemoryAllocate;
        }
    }

    const EebusDataCfg* const choice_cfg = (const EebusDataCfg*)cfg->metadata;
    return EEBUS_DATA_WRITE(choice_cfg, *buf, *src_buf);
}

EebusError WritePartial(
    const EebusDataCfg* cfg,
    void* base_addr,
    const void* src_base_addr,
    const EebusDataCfg* selectors_cfg,
    const void* selectors_base_addr,
    SelectorsMatcher selectors_matcher
) {
    UNUSED(cfg);
    UNUSED(base_addr);
    UNUSED(src_base_addr);
    UNUSED(selectors_cfg);
    UNUSED(selectors_base_addr);
    UNUSED(selectors_matcher);

    EEBUS_ASSERT_ALWAYS();
    return kEebusErrorOther;
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

void DeletePartial(
    const EebusDataCfg* cfg,
    void* base_addr,
    const EebusDataCfg* selectors_cfg,
    const void* selectors_base_addr,
    SelectorsMatcher selectors_matcher,
    const EebusDataCfg* elements_cfg,
    const void* elements_base_addr
) {
    UNUSED(cfg);
    UNUSED(base_addr);
    UNUSED(selectors_cfg);
    UNUSED(selectors_base_addr);
    UNUSED(selectors_matcher);
    UNUSED(elements_cfg);
    UNUSED(elements_base_addr);

    EEBUS_ASSERT_ALWAYS();
}

void Delete(const EebusDataCfg* cfg, void* base_addr) {
    void** buf = (void**)base_addr;
    if (buf == NULL) {
        return;
    }

    const EebusDataCfg* const choice_cfg = (const EebusDataCfg*)cfg->metadata;
    EEBUS_DATA_DELETE(choice_cfg, *buf);
    EEBUS_FREE(*buf);
    *buf = NULL;
}
