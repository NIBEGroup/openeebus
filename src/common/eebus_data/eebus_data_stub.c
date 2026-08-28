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
 * @brief EEEBUS Data Stub implementation
 */

#include <stdbool.h>
#include <string.h>

#include "src/common/api/eebus_data_interface.h"
#include "src/common/eebus_arguments.h"
#include "src/common/eebus_assert.h"
#include "src/common/eebus_data/eebus_data_base.h"
#include "src/common/eebus_malloc.h"

static void* CreateEmpty(const EebusDataCfg* cfg, void* base_addr);
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
static EebusError Write(const EebusDataCfg* cfg, void* base_addr, const void* src_base);
static EebusError WriteElements(const EebusDataCfg* cfg, void* base_addr, const void* src_base);
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

const EebusDataInterface eebus_data_stub_methods = {
    .create_empty          = CreateEmpty,
    .parse                 = Parse,
    .print_unformatted     = PrintUnformatted,
    .from_json_object_item = FromJsonObjectItem,
    .from_json_object      = FromJsonObject,
    .to_json_object_item   = ToJsonObjectItem,
    .to_json_object        = ToJsonObject,
    .copy                  = EebusDataBaseCopy,
    .compare               = Compare,
    .is_null               = IsNull,
    .is_empty              = IsEmpty,
    .has_identifiers       = HasIdentifiers,
    .selectors_match       = SelectorsMatch,
    .identifiers_match     = IdentifiersMatch,
    .read_elements         = ReadElements,
    .write                 = Write,
    .write_elements        = WriteElements,
    .write_partial         = WritePartial,
    .delete_elements       = DeleteElements,
    .delete_partial        = DeletePartial,
    .delete_               = Delete,
};

void* CreateEmpty(const EebusDataCfg* cfg, void* base_addr) {
    UNUSED(cfg);
    UNUSED(base_addr);

    return NULL;
}

void* Parse(const EebusDataCfg* cfg, const char* s) {
    UNUSED(cfg);
    UNUSED(s);

    return NULL;
}

char* PrintUnformatted(const EebusDataCfg* cfg, const void* base_addr) {
    UNUSED(cfg);
    UNUSED(base_addr);

    return NULL;
}

EebusError FromJsonObjectItem(const EebusDataCfg* cfg, void* base_addr, const JsonObject* json_obj) {
    UNUSED(cfg);
    UNUSED(base_addr);
    UNUSED(json_obj);

    return kEebusErrorOk;
}

EebusError FromJsonObject(const EebusDataCfg* cfg, void* base_addr, const JsonObject* json_obj, bool is_root) {
    UNUSED(cfg);
    UNUSED(base_addr);
    UNUSED(json_obj);
    UNUSED(is_root);

    return kEebusErrorOk;
}

EebusError ToJsonObjectItem(const EebusDataCfg* cfg, const void* base_addr, JsonObject** json_obj) {
    UNUSED(cfg);
    UNUSED(base_addr);

    *json_obj = NULL;
    return kEebusErrorOk;
}

EebusError ToJsonObject(const EebusDataCfg* cfg, const void* base_addr, JsonObject* json_obj, bool is_root) {
    UNUSED(cfg);
    UNUSED(base_addr);
    UNUSED(json_obj);
    UNUSED(is_root);

    return kEebusErrorOk;
}

bool Compare(const EebusDataCfg* a_cfg, const void* a_base_addr, const EebusDataCfg* b_cfg, const void* b_base_addr) {
    UNUSED(a_cfg);
    UNUSED(a_base_addr);
    UNUSED(b_cfg);
    UNUSED(b_base_addr);

    return true;
}

bool IsNull(const EebusDataCfg* cfg, const void* base_addr) {
    UNUSED(cfg);
    UNUSED(base_addr);

    return true;
}

bool IsEmpty(const EebusDataCfg* cfg, const void* base_addr) {
    UNUSED(cfg);
    UNUSED(base_addr);

    return false;
}

bool HasIdentifiers(const EebusDataCfg* cfg, const void* base_addr) {
    UNUSED(cfg);
    UNUSED(base_addr);

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

    return false;
}

bool IdentifiersMatch(const EebusDataCfg* cfg, const void* base_addr, const void* src_base_addr) {
    UNUSED(cfg);
    UNUSED(base_addr);
    UNUSED(src_base_addr);

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

    return kEebusErrorOk;
}

EebusError Write(const EebusDataCfg* cfg, void* base_addr, const void* src_base) {
    UNUSED(cfg);
    UNUSED(base_addr);
    UNUSED(src_base);

    return kEebusErrorOk;
}

EebusError WriteElements(const EebusDataCfg* cfg, void* base_addr, const void* src_base) {
    UNUSED(cfg);
    UNUSED(base_addr);
    UNUSED(src_base);

    return kEebusErrorOk;
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
}

void Delete(const EebusDataCfg* cfg, void* base_addr) {
    UNUSED(cfg);
    UNUSED(base_addr);
}
