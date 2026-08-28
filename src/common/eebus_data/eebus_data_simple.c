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

bool EebusDataSimpleCompare(
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

    return memcmp(*a_buf, *b_buf, a_cfg->size) == 0;
}

bool EebusDataSimpleIsNull(const EebusDataCfg* cfg, const void* base_addr) {
    const void** const buf = (const void**)((const uint8_t*)base_addr + cfg->offset);

    return (*buf == NULL);
}

bool EebusDataSimpleIsEmpty(const EebusDataCfg* cfg, const void* base_addr) {
    UNUSED(cfg);
    UNUSED(base_addr);
    // There is no specific value for simple data to be trated as "empty"
    return false;
}

EebusError EebusDataSimpleReadElements(
    const EebusDataCfg* cfg,
    const void* base_addr,
    void* dst_base_addr,
    const EebusDataCfg* elements_cfg,
    const void* elements_base_addr
) {
    if (EEBUS_DATA_IS_NULL(elements_cfg, elements_base_addr)) {
        // Should not be written - ok
        return kEebusErrorOk;
    }

    return EEBUS_DATA_COPY(cfg, base_addr, dst_base_addr);
}

EebusError EebusDataSimpleWrite(const EebusDataCfg* cfg, void* base_addr, const void* src_base_addr) {
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

    memcpy(*buf, *src_buf, cfg->size);
    return kEebusErrorOk;
}

void EebusDataSimpleDeleteElements(
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

void EebusDataSimpleDelete(const EebusDataCfg* cfg, void* base_addr) {
    void** const buf = (void**)((uint8_t*)base_addr + cfg->offset);
    EEBUS_FREE(*buf);
    *buf = NULL;
}
