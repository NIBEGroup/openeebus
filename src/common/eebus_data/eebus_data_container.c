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
 * @brief EEEBUS Data Container implementation
 */

#include "src/common/api/eebus_data_interface.h"
#include "src/common/eebus_arguments.h"
#include "src/common/eebus_assert.h"
#include "src/common/eebus_data/eebus_data_base.h"
#include "src/common/eebus_data/eebus_data_list.h"
#include "src/common/eebus_data/eebus_data_sequence.h"
#include "src/common/eebus_data/eebus_data_util.h"

static EebusError
CopyMatching(const EebusDataCfg* cfg, const void* base_addr, void* dst_base_addr, const void* data_to_match_base_addr);

static bool SelectorsMatch(
    const EebusDataCfg* cfg,
    const void* base_addr,
    const EebusDataCfg* selectors_cfg,
    const void* selectors_base_addr
);

static EebusError WritePartial(
    const EebusDataCfg* cfg,
    void* base_addr,
    const void* src_base_addr,
    const EebusDataCfg* selectors_cfg,
    const void* selectors_base_addr,
    SelectorsMatcher selectors_matcher
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

const EebusDataInterface eebus_data_container_methods = {
    .create_empty          = EebusDataBaseCreateEmpty,
    .parse                 = EebusDataBaseParse,
    .print_unformatted     = EebusDataBasePrintUnformatted,
    .from_json_object_item = EebusDataSequenceFromJsonObjectItem,
    .from_json_object      = EebusDataBaseFromJsonObject,
    .to_json_object_item   = EebusDataSequenceToJsonObjectItem,
    .to_json_object        = EebusDataBaseToJsonObject,
    .copy                  = EebusDataBaseCopy,
    .copy_matching         = CopyMatching,
    .compare               = EebusDataSequenceCompare,
    .is_null               = EebusDataSequenceIsNull,
    .is_empty              = EebusDataSequenceIsEmpty,
    .has_identifiers       = EebusDataSequenceHasIdentifiers,
    .selectors_match       = SelectorsMatch,
    .identifiers_match     = EebusDataSequenceIdentifiersMatch,
    .read_elements         = EebusDataSequenceReadElements,
    .write                 = EebusDataSequenceWrite,
    .write_elements        = EebusDataSequenceWriteElements,
    .write_partial         = WritePartial,
    .delete_elements       = EebusDataSequenceDeleteElements,
    .delete_partial        = DeletePartial,
    .delete_               = EebusDataSequenceDelete,
};

EebusError
CopyMatching(const EebusDataCfg* cfg, const void* base_addr, void* dst_base_addr, const void* data_to_match_base_addr) {
    const EebusDataCfg* const list_cfg = (const EebusDataCfg*)cfg->metadata;
    if (!EEBUS_DATA_IS_LIST(list_cfg)) {
        EEBUS_ASSERT_ALWAYS();
        return kEebusErrorInputArgument;
    }

    const void** const buf = (const void**)((const uint8_t*)base_addr + cfg->offset);
    if (*buf == NULL) {
        // Nothing to copied
        return kEebusErrorInputArgument;
    }

    void** dst_buf = (void**)((uint8_t*)dst_base_addr + cfg->offset);
    if (EEBUS_DATA_IS_NULL(cfg, dst_base_addr)) {
        *dst_buf = EEBUS_DATA_CREATE_EMPTY(cfg, dst_base_addr);
        if (*dst_buf == NULL) {
            return kEebusErrorMemoryAllocate;
        }
    }

    return EEBUS_DATA_COPY_MATCHING(list_cfg, *buf, *dst_buf, data_to_match_base_addr);
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

EebusError WritePartial(
    const EebusDataCfg* cfg,
    void* base_addr,
    const void* src_base_addr,
    const EebusDataCfg* selectors_cfg,
    const void* selectors_base_addr,
    SelectorsMatcher selectors_matcher
) {
    const EebusDataCfg* const list_cfg = (const EebusDataCfg*)cfg->metadata;
    if (!EEBUS_DATA_IS_LIST(list_cfg)) {
        EEBUS_ASSERT_ALWAYS();
        return kEebusErrorInputArgument;
    }

    const void** const src_buf = (const void**)((const uint8_t*)src_base_addr + cfg->offset);
    if (*src_buf == NULL) {
        // Nothing to written
        return kEebusErrorInputArgument;
    }

    void** buf = (void**)((uint8_t*)base_addr + cfg->offset);
    if (EEBUS_DATA_IS_NULL(cfg, base_addr)) {
        *buf = EEBUS_DATA_CREATE_EMPTY(cfg, base_addr);
        if (*buf == NULL) {
            return kEebusErrorMemoryAllocate;
        }
    }

    return EEBUS_DATA_WRITE_PARTIAL(list_cfg, *buf, *src_buf, selectors_cfg, selectors_base_addr, selectors_matcher);
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
    const EebusDataCfg* const list_cfg = (const EebusDataCfg*)cfg->metadata;
    if (!EEBUS_DATA_IS_LIST(list_cfg)) {
        EEBUS_ASSERT_ALWAYS();
        return;
    }

    void** const buf = (void**)((uint8_t*)base_addr + cfg->offset);
    if (*buf == NULL) {
        // Nothing to be deleted - ok
        return;
    }

    EEBUS_DATA_DELETE_PARTIAL(
        list_cfg,
        *buf,
        selectors_cfg,
        selectors_base_addr,
        selectors_matcher,
        elements_cfg,
        elements_base_addr
    );
}

void EbusDataContainerListMatchFirst(
    const EebusDataCfg* cfg,
    void* base_addr,
    EebusDataListMatchIterator* it,
    void* data_to_match_base_addr
) {
    const EebusDataCfg* const list_cfg = (const EebusDataCfg*)cfg->metadata;
    if (!EEBUS_DATA_IS_LIST(list_cfg)) {
        EEBUS_ASSERT_ALWAYS();
        EebusDataListMatchIteratorConstruct(it, NULL, NULL, NULL);
        return;
    }

    const void** const buf = (const void**)((const uint8_t*)base_addr + cfg->offset);
    EebusDataListMatchIteratorConstruct(it, list_cfg, *buf, data_to_match_base_addr);
}
