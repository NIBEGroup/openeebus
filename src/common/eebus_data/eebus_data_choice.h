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
 * @brief EEEBUS Data Choice
 */

#ifndef SRC_COMMON_EEBUS_DATA_EEBUS_DATA_CHOICE_H_
#define SRC_COMMON_EEBUS_DATA_EEBUS_DATA_CHOICE_H_

#include "src/common/api/eebus_data_interface.h"
#include "src/common/eebus_data/eebus_data_simple.h"
#include "src/common/eebus_data/eebus_data_stub.h"
#include "src/common/struct_util.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @brief EEBUS Data Choice Interface
 */
extern const EebusDataInterface eebus_data_choice_methods;

/**
 * @brief EEBUS Data Choice type check
 */
#define EEBUS_DATA_IS_CHOICE(cfg) (EEBUS_DATA_INTERFACE(cfg) == &eebus_data_choice_methods)

/**
 * @brief EEBUS Data Choice configuration, contines pointer to data that can vary and
 * type identifier.
 * A "Choice" itself means data can be any of several options, the entry Json text can be any of:
 * {
 *   "choiceA": [
 *     {"dataA" : 1}
 *   ]
 * }
 *
 * or
 *
 * {
 *   "choiceB": [
 *     {"dataB" : "Saple text"}
 *   ]
 * }
 *
 * @param struct_name Structure name associated with Json record
 * @param struct_field Structure field name.
 * Type of structure field shall be EebusJsonChoice
 * @param ce_cfg Choice Element configuration entry point
 */
#define EEBUS_DATA_CHOICE(struct_name, struct_field, ce_cfg)                         \
    {                                                                                \
        .interface_     = &eebus_data_choice_methods,                                \
        .name           = "",                                                        \
        .offset         = STRUCT_MEMBER_OFFSET(struct_name, struct_field),           \
        .type_id_offset = STRUCT_MEMBER_OFFSET(struct_name, struct_field##_type_id), \
        .metadata       = ce_cfg,                                                    \
    }

/**
 * @brief An element of Data Choiceconfiguration
 * @param ed_type EEBUS Data Configuration type name (tag, sequence, list, etc.)
 * @param ed_name Expected Data record name
 * @param struct_name Structure name associated with Data Choice element
 * @param ce_cfg Choice Element Data configuration entry point
 */
#define EEBUS_DATA_CHOICE_ELEMENT(ed_type, ed_name, struct_name, ce_cfg) \
    {                                                                    \
        .interface_ = &eebus_data_##ed_type##_methods,                   \
        .name       = ed_name,                                           \
        .offset     = 0,                                                 \
        .size       = sizeof(struct_name),                               \
        .metadata   = ce_cfg,                                            \
    }

/**
 * @brief An empty element of EEBUS Data Choice
 * @param ed_name Expected Data record name
 */
#define EEBUS_DATA_CHOICE_ELEMENT_EMPTY(ed_name) EEBUS_DATA_STUB(ed_name)

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_COMMON_EEBUS_DATA_EEBUS_DATA_CHOICE_H_
