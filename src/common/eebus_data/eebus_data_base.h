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
 * @brief EEBUS Data Base methods
 */

#ifndef SRC_COMMON_EEBUS_DATA_EEBUS_DATA_BASE_H_
#define SRC_COMMON_EEBUS_DATA_EEBUS_DATA_BASE_H_

#include <stdbool.h>

#include "src/common/api/eebus_data_interface.h"
#include "src/common/eebus_errors.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

void* EebusDataBaseCreateEmpty(const EebusDataCfg* cfg, void* base_addr);

void* EebusDataBaseParse(const EebusDataCfg* cfg, const char* s);

char* EebusDataBasePrintUnformatted(const EebusDataCfg* cfg, const void* base_addr);

EebusError
EebusDataBaseFromJsonObject(const EebusDataCfg* cfg, void* base_addr, const JsonObject* json_obj, bool is_root);

EebusError
EebusDataBaseToJsonObject(const EebusDataCfg* cfg, const void* base_addr, JsonObject* json_obj, bool is_root);

EebusError EebusDataBaseCopy(const EebusDataCfg* cfg, const void* base_addr, void* dst_base_addr);

EebusError EebusDataBaseCopyMatching(
    const EebusDataCfg* cfg,
    const void* base_addr,
    void* dst_base_addr,
    const void* data_to_match_base_addr
);

bool EebusDataBaseHasIdentifiersFlags(const EebusDataCfg* cfg);

bool EebusDataBaseHasIdentifiers(const EebusDataCfg* cfg, const void* base_addr);

bool EebusDataBaseSelectorsMatch(
    const EebusDataCfg* cfg,
    const void* base_addr,
    const EebusDataCfg* selectors_cfg,
    const void* selectors_base_addr
);

bool EebusDataBaseIdentifiersMatch(const EebusDataCfg* cfg, const void* base_addr, const void* src_base_addr);

EebusError EebusDataBaseWriteElements(const EebusDataCfg* cfg, void* base_addr, const void* src_base_addr);

EebusError EebusDataBaseWritePartial(
    const EebusDataCfg* cfg,
    void* base_addr,
    const void* src_base_addr,
    const EebusDataCfg* selectors_cfg,
    const void* selectors_base_addr,
    SelectorsMatcher selectors_matcher
);

void EebusDataBaseDeletePartial(
    const EebusDataCfg* cfg,
    void* base_addr,
    const EebusDataCfg* selectors_cfg,
    const void* selectors_base_addr,
    SelectorsMatcher selectors_matcher,
    const EebusDataCfg* elements_cfg,
    const void* elements_base_addr
);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_COMMON_EEBUS_DATA_EEBUS_DATA_BASE_H_
