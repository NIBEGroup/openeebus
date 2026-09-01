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
 * @brief EEEBUS Data Sequence
 */

#ifndef SRC_COMMON_EEBUS_DATA_EEBUS_DATA_SEQUENCE_H_
#define SRC_COMMON_EEBUS_DATA_EEBUS_DATA_SEQUENCE_H_

#include "src/common/api/eebus_data_interface.h"
#include "src/common/struct_util.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @brief EEBUS Data Sequence Interface
 */
extern const EebusDataInterface eebus_data_sequence_methods;

/**
 * @brief EEBUS Data Sequence type check
 */
#define EEBUS_DATA_IS_SEQUENCE(cfg) (EEBUS_DATA_INTERFACE(cfg) == &eebus_data_sequence_methods)

/**
 * @brief End of EEBUS Data Sequence's items configuration container marker
 */
#define EEBUS_DATA_END \
    {                  \
        .name = NULL,  \
    }

/**
 * @brief EEBUS Data Sequence configuration. A sequence is a kind of collection of
 * different type elements wrapped into "[]". E.g.:
 *
 * {
 *   "person" : [{"name": "Homer"}, {"age: "36"}]
 * }
 *
 * @param ed_name Expected Data record name
 * @param struct_name Structure name associated with Data record
 * @param struct_field Structure field name. Together struct_name and struct_field
 * are used to calcualte the offset of filed within structure.
 * Witihn structure, struct_field shall be apointer to specific data to keep
 * sequence data in, e.g.:
 *
 * struct Person {
 *   char* name;
 *   uint8_t* age;
 * };
 *
 * @param se_cfg Sequence element Data configuration entry point (shall keep mappings
 * for sequence internal data, e.g for name and age in example above)
 */
#define EEBUS_DATA_SEQUENCE(ed_name, struct_name, struct_field, se_cfg)  \
    {                                                                    \
        .interface_ = &eebus_data_sequence_methods,                      \
        .name       = ed_name,                                           \
        .offset     = STRUCT_MEMBER_OFFSET(struct_name, struct_field),   \
        .size       = sizeof(*STRUCT_MEMBER(struct_name, struct_field)), \
        .metadata   = se_cfg,                                            \
    }

/**
 * @brief EEBUS Data Sequence configuration with specific flags
 * @param ed_name Expected Data record name
 * @param struct_name Structure name associated with Data record
 * @param struct_field Structure field name. Together struct_name and struct_field
 * @param se_cfg Sequence element Data configuration entry point (shall keep mappings
 * for sequence internal data, e.g for name and age in example above)
 * @param ed_flags EebusData flags to be applied (see @EebusDataFlag)
 */
#define EEBUS_DATA_SEQUENCE_WITH_FLAGS(ed_name, struct_name, struct_field, se_cfg, ed_flags) \
    {                                                                                        \
        .interface_ = &eebus_data_sequence_methods,                                          \
        .name       = ed_name,                                                               \
        .offset     = STRUCT_MEMBER_OFFSET(struct_name, struct_field),                       \
        .size       = sizeof(*STRUCT_MEMBER(struct_name, struct_field)),                     \
        .flags      = ed_flags,                                                              \
        .metadata   = se_cfg,                                                                \
    }

/**
 * @brief EEBUS Data Sequence configuration in order to simplify the
 * data structuring when sequence has to be handled at top level
 * (the entry structure data configuration)
 * @param ed_name Expected Data record name
 * @param struct_name Structure name associated with Data record
 * @param se_cfg Sequence element Data configuration entry point
 */
#define EEBUS_DATA_SEQUENCE_ROOT(ed_name, struct_name, se_cfg) \
    {                                                          \
        .interface_ = &eebus_data_sequence_methods,            \
        .name       = ed_name,                                 \
        .offset     = 0,                                       \
        .size       = sizeof(struct_name),                     \
        .metadata   = se_cfg,                                  \
    }

/**
 * @brief Temporary EEBUS Data Sequence configuration in order
 * to simplify the copy/delete of intermediate structures
 * @param struct_name Structure name associated with Data record
 * @param se_cfg Sequence element Data configuration entry point
 */
#define EEBUS_DATA_SEQUENCE_TMP(struct_name, se_cfg) \
    {                                                \
        .interface_ = &eebus_data_sequence_methods,  \
        .name       = "",                            \
        .offset     = 0,                             \
        .size       = sizeof(struct_name),           \
        .metadata   = se_cfg,                        \
    }

/**
 * @defgroup EebusDataSequenceMethods EEBUS Data Sequence implementation inherited by EEBUS Data Container
 * @{
 */
EebusError EebusDataSequenceFromJsonObjectItem(const EebusDataCfg* cfg, void* base_addr, const JsonObject* json_obj);
EebusError EebusDataSequenceToJsonObjectItem(const EebusDataCfg* cfg, const void* base_addr, JsonObject** json_obj);
EebusError EebusDataSequenceWrite(const EebusDataCfg* cfg, void* base_addr, const void* src_base_addr);
bool EebusDataSequenceCompare(
    const EebusDataCfg* a_cfg,
    const void* a_base_addr,
    const EebusDataCfg* b_cfg,
    const void* b_base_addr
);
bool EebusDataSequenceIsNull(const EebusDataCfg* cfg, const void* base_addr);
bool EebusDataSequenceIsEmpty(const EebusDataCfg* cfg, const void* base_addr);
bool EebusDataSequenceHasIdentifiers(const EebusDataCfg* cfg, const void* base_addr);
bool EebusDataSequenceIdentifiersMatch(const EebusDataCfg* cfg, const void* base_addr, const void* src_base_addr);
EebusError EebusDataSequenceReadElements(
    const EebusDataCfg* cfg,
    const void* base_addr,
    void* dst_base_addr,
    const EebusDataCfg* elements_cfg,
    const void* elements_base_addr
);
EebusError EebusDataSequenceWriteElements(const EebusDataCfg* cfg, void* base_addr, const void* src_base_addr);
EebusError EebusDataSequenceWritePartial(
    const EebusDataCfg* cfg,
    void* base_addr,
    const void* src_base_addr,
    const EebusDataCfg* selectors_cfg,
    const void* selectors_base_addr,
    SelectorsMatcher selectors_matcher
);
void EebusDataSequenceDeleteElements(
    const EebusDataCfg* cfg,
    void* base_addr,
    const EebusDataCfg* elements_cfg,
    const void* elements_base_addr
);
void EebusDataSequenceDelete(const EebusDataCfg* cfg, void* base_addr);
/** @} */

const EebusDataCfg* EebusDataSequenceGetFieldCfg(const EebusDataCfg* cfg, const char* name);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_COMMON_EEBUS_DATA_EEBUS_DATA_SEQUENCE_H_
