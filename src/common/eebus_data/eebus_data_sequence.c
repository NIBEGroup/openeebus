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
 * @brief EEEBUS Data Sequence implementation
 */

#include "src/common/eebus_data/eebus_data_sequence.h"

#include <stdbool.h>
#include <string.h>

#include "src/common/api/eebus_data_interface.h"
#include "src/common/eebus_arguments.h"
#include "src/common/eebus_data/eebus_data_base.h"
#include "src/common/eebus_data/eebus_data_util.h"
#include "src/common/eebus_malloc.h"

static bool SelectorsMatch(
    const EebusDataCfg* cfg,
    const void* base_addr,
    const EebusDataCfg* selectors_cfg,
    const void* selectors_base_addr
);

const EebusDataInterface eebus_data_sequence_methods = {
    .create_empty          = EebusDataBaseCreateEmpty,
    .parse                 = EebusDataBaseParse,
    .print_unformatted     = EebusDataBasePrintUnformatted,
    .from_json_object_item = EebusDataSequenceFromJsonObjectItem,
    .from_json_object      = EebusDataBaseFromJsonObject,
    .to_json_object_item   = EebusDataSequenceToJsonObjectItem,
    .to_json_object        = EebusDataBaseToJsonObject,
    .copy                  = EebusDataBaseCopy,
    .compare               = EebusDataSequenceCompare,
    .is_null               = EebusDataSequenceIsNull,
    .is_empty              = EebusDataSequenceIsEmpty,
    .has_identifiers       = EebusDataSequenceHasIdentifiers,
    .selectors_match       = SelectorsMatch,
    .identifiers_match     = EebusDataSequenceIdentifiersMatch,
    .read_elements         = EebusDataSequenceReadElements,
    .write                 = EebusDataSequenceWrite,
    .write_elements        = EebusDataSequenceWriteElements,
    .write_partial         = EebusDataSequenceWritePartial,
    .delete_elements       = EebusDataSequenceDeleteElements,
    .delete_partial        = EebusDataBaseDeletePartial,
    .delete_               = EebusDataSequenceDelete,
};

EebusError EebusDataSequenceFromJsonObjectItem(const EebusDataCfg* cfg, void* base_addr, const JsonObject* json_obj) {
    if (!JsonIsArray(json_obj)) {
        return kEebusErrorParse;
    }

    void* const buf = EEBUS_DATA_CREATE_EMPTY(cfg, base_addr);
    if (buf == NULL) {
        return kEebusErrorMemoryAllocate;
    }

    const EebusDataCfg* cfg_first = (const EebusDataCfg*)cfg->metadata;
    for (const EebusDataCfg* cfg_it = cfg_first; cfg_it->name != NULL; ++cfg_it) {
        const EebusError ret = EEBUS_DATA_FROM_JSON_OBJECT(cfg_it, buf, json_obj, false);
        if (ret != kEebusErrorOk) {
            return ret;
        }
    }

    return kEebusErrorOk;
}

EebusError EebusDataSequenceToJsonObjectItem(const EebusDataCfg* cfg, const void* base_addr, JsonObject** json_obj) {
    const void** const buf = (const void**)((const uint8_t*)base_addr + cfg->offset);
    if (*buf == NULL) {
        *json_obj = NULL;
        return kEebusErrorOk;
    }

    *json_obj = JsonCreateArray();
    if (*json_obj == NULL) {
        return kEebusErrorMemoryAllocate;
    }

    const EebusDataCfg* const cfg_first = (const EebusDataCfg*)cfg->metadata;
    for (const EebusDataCfg* cfg_it = cfg_first; cfg_it->name != NULL; ++cfg_it) {
        EebusError ret = EEBUS_DATA_TO_JSON_OBJECT(cfg_it, *buf, *json_obj, false);
        if (ret != kEebusErrorOk) {
            JsonDelete(*json_obj);
            return ret;
        }
    }

    return kEebusErrorOk;
}

bool EebusDataSequenceCompare(
    const EebusDataCfg* a_cfg,
    const void* a_base_addr,
    const EebusDataCfg* b_cfg,
    const void* b_base_addr
) {
    if (!EEBUS_DATA_TYPE_EQ(a_cfg, b_cfg)) {
        return false;
    }

    const void** const a_buf = (const void**)((const uint8_t*)a_base_addr + a_cfg->offset);
    const void** const b_buf = (const void**)((const uint8_t*)b_base_addr + b_cfg->offset);

    if ((*a_buf == NULL) || (*b_buf == NULL)) {
        return *a_buf == *b_buf;
    }

    const EebusDataCfg* const a_cfg_first = (const EebusDataCfg*)a_cfg->metadata;
    const EebusDataCfg* const b_cfg_first = (const EebusDataCfg*)b_cfg->metadata;

    const size_t cfg_size = EebusDataGetCfgSize(a_cfg_first);
    if (cfg_size != EebusDataGetCfgSize(b_cfg_first)) {
        return false;
    }

    const EebusDataCfg* a_cfg_it = a_cfg_first;
    const EebusDataCfg* b_cfg_it = b_cfg_first;
    for (; (a_cfg_it->name != NULL) && (b_cfg_it->name != NULL); ++a_cfg_it, ++b_cfg_it) {
        if (!EEBUS_DATA_COMPARE(a_cfg_it, *a_buf, b_cfg_it, *b_buf)) {
            return false;
        }
    }

    return true;
}

bool EebusDataSequenceIsNull(const EebusDataCfg* cfg, const void* base_addr) {
    const void** const buf = (const void**)((const uint8_t*)base_addr + cfg->offset);
    return *buf == NULL;
}

bool EebusDataSequenceIsEmpty(const EebusDataCfg* cfg, const void* base_addr) {
    const void** const buf = (const void**)((const uint8_t*)base_addr + cfg->offset);
    if (*buf == NULL) {
        return false;
    }

    const EebusDataCfg* const cfg_first = (const EebusDataCfg*)cfg->metadata;
    for (const EebusDataCfg* cfg_it = cfg_first; cfg_it->name != NULL; ++cfg_it) {
        if (!EEBUS_DATA_IS_NULL(cfg_it, *buf)) {
            return false;
        }
    }

    return true;
}

bool EebusDataSequenceHasIdentifiers(const EebusDataCfg* cfg, const void* base_addr) {
    const void** const buf = (const void**)((const uint8_t*)base_addr + cfg->offset);
    if (*buf == NULL) {
        return false;
    }

    if (EebusDataBaseHasIdentifiersFlags(cfg)) {
        return true;
    }

    const EebusDataCfg* const cfg_first = (const EebusDataCfg*)cfg->metadata;
    for (const EebusDataCfg* cfg_it = cfg_first; cfg_it->name != NULL; ++cfg_it) {
        if (EEBUS_DATA_HAS_IDENTIFIERS(cfg_it, *buf)) {
            return true;
        }
    }

    return false;
}

const EebusDataCfg* EebusDataSequenceGetFieldCfg(const EebusDataCfg* cfg, const char* name) {
    if ((cfg == NULL) || (name == NULL)) {
        return NULL;
    }

    const EebusDataCfg* cfg_first = (const EebusDataCfg*)cfg->metadata;
    if (cfg_first == NULL) {
        return NULL;
    }

    for (const EebusDataCfg* cfg_it = cfg_first; cfg_it->name != NULL; ++cfg_it) {
        if (!strcmp(cfg_it->name, name)) {
            return cfg_it;
        }
    }

    return NULL;
}

bool SelectorsMatch(
    const EebusDataCfg* cfg,
    const void* base_addr,
    const EebusDataCfg* selectors_cfg,
    const void* selectors_base_addr
) {
    if (!EEBUS_DATA_IS_SEQUENCE(selectors_cfg)) {
        return false;
    }

    void** const buf       = (void**)((uint8_t*)base_addr + cfg->offset);
    void** const selectors = (void**)((uint8_t*)selectors_base_addr + selectors_cfg->offset);

    if (*selectors == NULL) {
        return true;
    }

    if (*buf == NULL) {
        return false;
    }

    const EebusDataCfg* selectors_cfg_it = (const EebusDataCfg*)selectors_cfg->metadata;
    for (; selectors_cfg_it->name != NULL; ++selectors_cfg_it) {
        if (EEBUS_DATA_IS_NULL(selectors_cfg_it, *selectors)) {
            continue;
        }

        const EebusDataCfg* data_cfg_it = EebusDataSequenceGetFieldCfg(cfg, selectors_cfg_it->name);
        if (data_cfg_it == NULL) {
            // TODO: Handle each specific selectors case
            continue;
        }

        if (!EEBUS_DATA_COMPARE(data_cfg_it, *buf, selectors_cfg_it, *selectors)) {
            return false;
        }
    }

    return true;
}

bool EebusDataSequenceIdentifiersMatch(const EebusDataCfg* cfg, const void* base_addr, const void* src_base_addr) {
    if (EebusDataBaseHasIdentifiersFlags(cfg)) {
        return EEBUS_DATA_COMPARE(cfg, base_addr, cfg, src_base_addr);
    }

    if (!EEBUS_DATA_HAS_IDENTIFIERS(cfg, src_base_addr)) {
        return true;
    }

    const void** const buf     = (const void**)((const uint8_t*)base_addr + cfg->offset);
    const void** const src_buf = (const void**)((const uint8_t*)src_base_addr + cfg->offset);

    if (*buf == NULL) {
        return false;
    }

    for (const EebusDataCfg* cfg_it = cfg->metadata; cfg_it->name != NULL; ++cfg_it) {
        if (!EEBUS_DATA_IDENTIFIERS_MATCH(cfg_it, *buf, *src_buf)) {
            return false;
        }
    }

    return true;
}

EebusError EebusDataSequenceReadElements(
    const EebusDataCfg* cfg,
    const void* base_addr,
    void* dst_base_addr,
    const EebusDataCfg* elements_cfg,
    const void* elements_base_addr
) {
    if (!EEBUS_DATA_IS_SEQUENCE(elements_cfg)) {
        return kEebusErrorInputArgument;
    }

    const void** const buf      = (const void**)((const uint8_t*)base_addr + cfg->offset);
    const void** const elements = (const void**)((const uint8_t*)elements_base_addr + elements_cfg->offset);
    if ((*buf == NULL) || (*elements == NULL)) {
        // Nothing to be written — ok
        return kEebusErrorOk;
    }

    if (EEBUS_DATA_IS_EMPTY(elements_cfg, elements_base_addr)) {
        EEBUS_DATA_DELETE(cfg, dst_base_addr);
        return EEBUS_DATA_COPY(cfg, base_addr, dst_base_addr);
    };

    void** const dst = (void**)((uint8_t*)dst_base_addr + cfg->offset);
    if (*dst == NULL) {
        *dst = EEBUS_DATA_CREATE_EMPTY(cfg, dst_base_addr);
        if (*dst == NULL) {
            return kEebusErrorMemoryAllocate;
        }
    }

    EebusError ret = kEebusErrorOk;

    const EebusDataCfg* cfg_it          = (const EebusDataCfg*)cfg->metadata;
    const EebusDataCfg* elements_cfg_it = (const EebusDataCfg*)elements_cfg->metadata;
    for (; (cfg_it->name != NULL) && (elements_cfg_it->name != NULL); ++cfg_it, ++elements_cfg_it) {
        if (!EEBUS_DATA_IS_NULL(elements_cfg_it, *elements)) {
            if (EEBUS_DATA_IS_EMPTY(elements_cfg_it, *elements)) {
                EEBUS_DATA_DELETE(cfg_it, *dst);
                ret = EEBUS_DATA_COPY(cfg_it, *buf, *dst);
            } else {
                ret = EEBUS_DATA_READ_ELEMENTS(cfg_it, *buf, *dst, elements_cfg_it, *elements);
            }
        }

        if (ret != kEebusErrorOk) {
            return ret;
        }
    }

    return kEebusErrorOk;
}

EebusError EebusDataSequenceWrite(const EebusDataCfg* cfg, void* base_addr, const void* src_base_addr) {
    const void** const src_buf = (const void**)((const uint8_t*)src_base_addr + cfg->offset);
    if (*src_buf == NULL) {
        EEBUS_DATA_DELETE(cfg, base_addr);
        return kEebusErrorOk;
    }

    void** const buf = (void**)((uint8_t*)base_addr + cfg->offset);
    if (*buf == NULL) {
        *buf = EEBUS_DATA_CREATE_EMPTY(cfg, base_addr);
        if (*buf == NULL) {
            return kEebusErrorMemoryAllocate;
        }
    }

    const EebusDataCfg* const cfg_first = (const EebusDataCfg*)cfg->metadata;
    for (const EebusDataCfg* cfg_it = cfg_first; cfg_it->name != NULL; ++cfg_it) {
        const EebusError ret = EEBUS_DATA_WRITE(cfg_it, *buf, *src_buf);
        if (ret != kEebusErrorOk) {
            return ret;
        }
    }

    return kEebusErrorOk;
}

EebusError EebusDataSequenceWriteElements(const EebusDataCfg* cfg, void* base_addr, const void* src_base_addr) {
    const void** const src_buf = (const void**)((const uint8_t*)src_base_addr + cfg->offset);
    if (*src_buf == NULL) {
        // Nothing to be copied — ok
        return kEebusErrorOk;
    }

    void** buf = (void**)((uint8_t*)base_addr + cfg->offset);
    if (*buf == NULL) {
        *buf = EEBUS_DATA_CREATE_EMPTY(cfg, base_addr);
        if (*buf == NULL) {
            return kEebusErrorMemoryAllocate;
        }
    }

    const EebusDataCfg* const cfg_first = (const EebusDataCfg*)cfg->metadata;
    for (const EebusDataCfg* cfg_it = cfg_first; cfg_it->name != NULL; ++cfg_it) {
        const EebusError ret = EEBUS_DATA_WRITE_ELEMENTS(cfg_it, *buf, *src_buf);
        if (ret != kEebusErrorOk) {
            return ret;
        }
    }

    return kEebusErrorOk;
}

EebusError EebusDataSequenceWritePartial(
    const EebusDataCfg* cfg,
    void* base_addr,
    const void* src_base_addr,
    const EebusDataCfg* selectors_cfg,
    const void* selectors_base_addr,
    SelectorsMatcher selectors_matcher
) {
    UNUSED(selectors_cfg);
    UNUSED(selectors_base_addr);
    UNUSED(selectors_matcher);

    const void** const src_buf = (const void**)((const uint8_t*)src_base_addr + cfg->offset);
    if (*src_buf == NULL) {
        // Nothing to be copied — ok
        return kEebusErrorOk;
    }

    void** buf = (void**)((uint8_t*)base_addr + cfg->offset);
    if (*buf == NULL) {
        *buf = EEBUS_DATA_CREATE_EMPTY(cfg, base_addr);
        if (*buf == NULL) {
            return kEebusErrorMemoryAllocate;
        }
    }

    const EebusDataCfg* const cfg_first = (const EebusDataCfg*)cfg->metadata;
    for (const EebusDataCfg* cfg_it = cfg_first; cfg_it->name != NULL; ++cfg_it) {
        // Selectors are used on container level and specific custom data types
        const EebusError ret = EEBUS_DATA_WRITE_PARTIAL(cfg_it, *buf, *src_buf, NULL, NULL, NULL);
        if (ret != kEebusErrorOk) {
            return ret;
        }
    }

    return kEebusErrorOk;
}

void EebusDataSequenceDeleteElements(
    const EebusDataCfg* cfg,
    void* base_addr,
    const EebusDataCfg* elements_cfg,
    const void* elements_base_addr
) {
    if (!EEBUS_DATA_IS_SEQUENCE(elements_cfg)) {
        return;
    }

    void** const buf            = (void**)((uint8_t*)base_addr + cfg->offset);
    const void** const elements = (const void**)((const uint8_t*)elements_base_addr + elements_cfg->offset);
    if ((*buf == NULL) || (*elements == NULL)) {
        // Nothing to be deleted — ok
        return;
    }

    if (EEBUS_DATA_IS_EMPTY(elements_cfg, elements_base_addr)) {
        EEBUS_DATA_DELETE(cfg, base_addr);
        return;
    }

    const EebusDataCfg* cfg_it          = (const EebusDataCfg*)cfg->metadata;
    const EebusDataCfg* elements_cfg_it = (const EebusDataCfg*)elements_cfg->metadata;
    for (; (cfg_it->name != NULL) && (elements_cfg_it->name != NULL); ++cfg_it, ++elements_cfg_it) {
        if (!EEBUS_DATA_IS_NULL(elements_cfg_it, *elements)) {
            if (EEBUS_DATA_IS_EMPTY(elements_cfg_it, *elements)) {
                EEBUS_DATA_DELETE(cfg_it, *buf);
            } else {
                EEBUS_DATA_DELETE_ELEMENTS(cfg_it, *buf, elements_cfg_it, *elements);
            }
        }
    }
}

void EebusDataSequenceDelete(const EebusDataCfg* cfg, void* base_addr) {
    void** const buf = (void**)((uint8_t*)base_addr + cfg->offset);
    if (*buf == NULL) {
        return;
    }

    const EebusDataCfg* const cfg_first = (const EebusDataCfg*)cfg->metadata;
    for (const EebusDataCfg* cfg_it = cfg_first; cfg_it->name != NULL; ++cfg_it) {
        EEBUS_DATA_DELETE(cfg_it, *buf);
    }

    EEBUS_FREE(*buf);
    *buf = NULL;
}
