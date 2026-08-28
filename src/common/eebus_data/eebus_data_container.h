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
 * @brief EEEBUS Data Container is a EEBUS Data Sequence "subclass" whith the only difference in
 * partial data write/delete as it unlike EEBUS Data Sequence simply forwards the
 * partial data update requests directy to that internal EEBUS Data List keeping collection of items.
 */

#ifndef SRC_COMMON_EEBUS_DATA_EEBUS_DATA_CONTAINER_H_
#define SRC_COMMON_EEBUS_DATA_EEBUS_DATA_CONTAINER_H_

#include "src/common/api/eebus_data_interface.h"
#include "src/common/eebus_data/eebus_data_list.h"
#include "src/common/struct_util.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @brief EEBUS Data Container Interface
 */
extern const EebusDataInterface eebus_data_container_methods;

/**
 * @brief EEBUS Data Container type check
 */
#define EEBUS_DATA_IS_CONTAINER(cfg) (EEBUS_DATA_INTERFACE(cfg) == &eebus_data_container_methods)

/**
 * @brief EEBUS Data Container configuration. Comparing to EEBUS Data Sequence,
 * EEBUS Data Container shall wrap the only one element of EEBUS Data List type
 * keeping the collection of elements.
 *
 * @param ce_cfg Container element data configuration entry point. Shall point
 * to single instance of the EEBUS Data List
 */
#define EEBUS_DATA_CONTAINER(ed_name, struct_name, struct_field, ce_cfg) \
    {                                                                    \
        .interface_ = &eebus_data_container_methods,                     \
        .name       = ed_name,                                           \
        .offset     = STRUCT_MEMBER_OFFSET(struct_name, struct_field),   \
        .size       = sizeof(*STRUCT_MEMBER(struct_name, struct_field)), \
        .metadata   = ce_cfg,                                            \
    }

void EbusDataContainerListMatchFirst(
    const EebusDataCfg* cfg,
    const void* base_addr,
    EebusDataListMatchIterator* it,
    const void* data_to_match_base_addr
);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_COMMON_EEBUS_DATA_EEBUS_DATA_CONTAINER_H_
