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
 * @brief EEBUS Data Simple group basic methods implementation
 */

#include <stdbool.h>
#include <string.h>

#include "src/common/api/eebus_data_interface.h"
#include "src/common/eebus_arguments.h"
#include "src/common/eebus_assert.h"
#include "src/common/eebus_errors.h"
#include "src/common/eebus_malloc.h"

void* EebusDataBaseCreateEmpty(const EebusDataCfg* cfg, void* base_addr) {
    if (base_addr == NULL) {
        EEBUS_ASSERT_ALWAYS();
        return NULL;
    }

    void** const buf = (void**)((uint8_t*)base_addr + cfg->offset);
    if (cfg->size == 0) {
        return *buf = NULL;
    }

    *buf = EEBUS_MALLOC(cfg->size);
    if (*buf != NULL) {
        memset(*buf, 0, cfg->size);
    }

    return *buf;
}

void* EebusDataBaseParse(const EebusDataCfg* cfg, const char* s) {
    const JsonObject* const json_root = JsonParse(s);
    if (json_root == NULL) {
        return NULL;
    }

    void* buf = NULL;

    const EebusError ret = EEBUS_DATA_FROM_JSON_OBJECT(cfg, &buf, json_root, true);
    JsonDelete((JsonObject*)json_root);
    if (ret != kEebusErrorOk) {
        EEBUS_DATA_DELETE(cfg, &buf);
        return NULL;
    }

    return buf;
}

char* EebusDataBasePrintUnformatted(const EebusDataCfg* cfg, const void* base_addr) {
    JsonObject* const json_root = JsonCreateObject();
    if (json_root == NULL) {
        return NULL;
    }

    char* s = NULL;
    if (EEBUS_DATA_TO_JSON_OBJECT(cfg, base_addr, json_root, true) == kEebusErrorOk) {
        s = JsonPrintUnformatted(json_root);
    }

    JsonDelete(json_root);
    return s;
}

EebusError
EebusDataBaseFromJsonObject(const EebusDataCfg* cfg, void* base_addr, const JsonObject* json_obj, bool is_root) {
    const JsonObject* const json_item = JsonGetItem(json_obj, cfg->name, is_root);
    if (json_item == NULL) {
        // Ok — no item in message
        return kEebusErrorOk;
    }

    return EEBUS_DATA_FROM_JSON_OBJECT_ITEM(cfg, base_addr, json_item);
}

EebusError
EebusDataBaseToJsonObject(const EebusDataCfg* cfg, const void* base_addr, JsonObject* json_obj, bool is_root) {
    JsonObject* json_item = NULL;
    const EebusError ret  = EEBUS_DATA_TO_JSON_OBJECT_ITEM(cfg, base_addr, &json_item);
    if (ret != kEebusErrorOk) {
        return ret;
    }

    if (json_item != NULL) {
        if (!JsonAddItem(json_obj, cfg->name, json_item, is_root)) {
            JsonDelete(json_item);
            return kEebusErrorMemoryAllocate;
        }
    }

    return kEebusErrorOk;
}

EebusError EebusDataBaseCopy(const EebusDataCfg* cfg, const void* base_addr, void* dst_base_addr) {
    return EEBUS_DATA_WRITE(cfg, dst_base_addr, base_addr);
}

EebusError EebusDataBaseCopyMatching(
    const EebusDataCfg* cfg,
    const void* base_addr,
    void* dst_base_addr,
    const void* data_to_match_base_addr
) {
    UNUSED(data_to_match_base_addr);
    // No matching check is done by default
    return EEBUS_DATA_WRITE(cfg, dst_base_addr, base_addr);
}

bool EebusDataBaseHasIdentifiersFlags(const EebusDataCfg* cfg) {
    const bool has_identifiers     = !!(cfg->flags & kEebusDataFlagIsIdentifier);
    const bool has_sub_identifiers = !!(cfg->flags & kEebusDataFlagIsSubIdentifier);

    return has_identifiers || has_sub_identifiers;
}

bool EebusDataBaseHasIdentifiers(const EebusDataCfg* cfg, const void* base_addr) {
    if (EEBUS_DATA_IS_NULL(cfg, base_addr)) {
        return false;
    }

    return EebusDataBaseHasIdentifiersFlags(cfg);
}

bool EebusDataBaseSelectorsMatch(
    const EebusDataCfg* cfg,
    const void* base_addr,
    const EebusDataCfg* selectors_cfg,
    const void* selectors_base_addr
) {
    UNUSED(cfg);
    UNUSED(base_addr);
    UNUSED(selectors_cfg);
    UNUSED(selectors_base_addr);
    // SelectorsMatch() currently is not supported by default
    EEBUS_ASSERT_ALWAYS();
    return false;
}

bool EebusDataBaseIdentifiersMatch(const EebusDataCfg* cfg, const void* base_addr, const void* src_base_addr) {
    if (!EebusDataBaseHasIdentifiersFlags(cfg)) {
        // Return true to skip the comparing of non-identifier data
        return true;
    }

    if (EEBUS_DATA_IS_NULL(cfg, src_base_addr)) {
        return true;
    }

    return EEBUS_DATA_COMPARE(cfg, base_addr, cfg, src_base_addr);
}

EebusError EebusDataBaseWriteElements(const EebusDataCfg* cfg, void* base_addr, const void* src_base_addr) {
    if (EEBUS_DATA_IS_NULL(cfg, src_base_addr)) {
        return kEebusErrorOk;
    }

    return EEBUS_DATA_WRITE(cfg, base_addr, src_base_addr);
}

EebusError EebusDataBaseWritePartial(
    const EebusDataCfg* cfg,
    void* base_addr,
    void* src_base_addr,
    const EebusDataCfg* selectors_cfg,
    const void* selectors_base_addr,
    SelectorsMatcher selectors_matcher
) {
    UNUSED(selectors_cfg);
    UNUSED(selectors_base_addr);
    UNUSED(selectors_matcher);

    return EEBUS_DATA_WRITE_ELEMENTS(cfg, base_addr, src_base_addr);
}

void EebusDataBaseDeletePartial(
    const EebusDataCfg* cfg,
    void* base_addr,
    const EebusDataCfg* selectors_cfg,
    const void* selectors_base_addr,
    SelectorsMatcher selectors_matcher,
    const EebusDataCfg* elements_cfg,
    const void* elements_base_addr
) {
    UNUSED(selectors_cfg);
    UNUSED(selectors_base_addr);
    UNUSED(selectors_matcher);

    EEBUS_DATA_DELETE_ELEMENTS(cfg, base_addr, elements_cfg, elements_base_addr);
}
