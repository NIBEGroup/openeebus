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
 * @brief EEBUS Data Simple group basic methods
 */

#ifndef SRC_COMMON_EEBUS_DATA_EEBUS_DATA_SIMPLE_H_
#define SRC_COMMON_EEBUS_DATA_EEBUS_DATA_SIMPLE_H_

#include <stdbool.h>

#include "src/common/api/eebus_data_interface.h"
#include "src/common/eebus_data/eebus_data_base.h"
#include "src/common/eebus_errors.h"
#include "src/common/struct_util.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @brief Simple data Data configuration
 * @param ed_interface EEBUS Data Configuration of assigned strcuture field
 * @param ed_name Expected Data record name
 * @param struct_name Structure name associated with Data record
 * @param struct_field Structure field name. Together struct_name and struct_field
 * are used to calcualte the offset of filed within structure
 */
#define EEBUS_DATA_SIMPLE(ed_interface, ed_name, struct_name, struct_field, ed_size) \
    {                                                                                \
        .interface_ = ed_interface,                                                  \
        .name       = ed_name,                                                       \
        .offset     = STRUCT_MEMBER_OFFSET(struct_name, struct_field),               \
        .size       = ed_size,                                                       \
        .metadata   = NULL,                                                          \
    }

bool EebusDataSimpleCompare(
    const EebusDataCfg* a_cfg,
    const void* a_base_addr,
    const EebusDataCfg* b_cfg,
    const void* b_base_addr
);
bool EebusDataSimpleIsNull(const EebusDataCfg* cfg, const void* base_addr);
bool EebusDataSimpleIsEmpty(const EebusDataCfg* cfg, const void* base_addr);
EebusError EebusDataSimpleReadElements(
    const EebusDataCfg* cfg,
    const void* base_addr,
    void* dst_base_addr,
    const EebusDataCfg* elements_cfg,
    const void* elements_base_addr
);
EebusError EebusDataSimpleWrite(const EebusDataCfg* cfg, void* src_base_addr, const void* dst_base_addr);
void EebusDataSimpleDeleteElements(
    const EebusDataCfg* cfg,
    void* base_addr,
    const EebusDataCfg* elements_cfg,
    const void* elements_base_addr
);
void EebusDataSimpleDelete(const EebusDataCfg* cfg, void* base_addr);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_COMMON_EEBUS_DATA_EEBUS_DATA_SIMPLE_H_
