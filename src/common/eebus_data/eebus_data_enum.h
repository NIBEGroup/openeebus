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
 * @brief EEBUS Data Enum declarations
 */

#ifndef SRC_COMMON_EEBUS_DATA_EEBUS_DATA_ENUM_H_
#define SRC_COMMON_EEBUS_DATA_EEBUS_DATA_ENUM_H_

#include <stddef.h>
#include <stdint.h>

#include "src/common/api/eebus_data_interface.h"
#include "src/common/struct_util.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @brief Enumeration name to identifier mapping type declaration
 * Used within EEBUS_JSON_ENUM()
 */
typedef struct EnumMapping EnumMapping;

/**
 * @brief Enumeration name to identifier mapping structure
 */
struct EnumMapping {
    /**
     * @brief Enumeration name (or text presentation)
     */
    const char* name;
    /**
     * @brief Related enumeration numeric value
     */
    int32_t value;
};

#define EEBUS_ENUM_MAPPING_END \
    {                          \
        .name  = NULL,         \
        .value = 0,            \
    }

extern const EebusDataInterface eebus_data_enum_methods;

/**
 * @brief EEBUS Data Enumeration type check
 */
#define EEBUS_DATA_IS_ENUM(cfg) (EEBUS_DATA_INTERFACE(cfg) == &eebus_data_enum_methods)

/**
 * @brief EEBUS Data Enumeration configuration
 * @param ej_name Expected Data record name
 * @param struct_name Structure name associated with Data record
 * @param struct_field Structure field name.
 * Type of structure field shall be int32_t*
 * @param ecfg Enumeration name to identifier table entry point
 * (see EnumMapping)
 */
#define EEBUS_DATA_ENUM(ej_name, struct_name, struct_field, ecfg)      \
    {                                                                  \
        .interface_ = &eebus_data_enum_methods,                        \
        .name       = ej_name,                                         \
        .offset     = STRUCT_MEMBER_OFFSET(struct_name, struct_field), \
        .size       = sizeof(int32_t),                                 \
        .metadata   = ecfg,                                            \
    }

/**
 * @brief EEBUS Data Enumeration configuration with specific flags
 * @param ed_name Expected Data record name
 * @param struct_name Structure name associated with Data record
 * @param struct_field Structure field name.
 * Type of structure field shall be int32_t*
 * @param ecfg Enumeration name to identifier table entry point
 * (see EnumMapping)
 * @param ed_flags EebusData flags to be applied (see @EebusDataFlag)
 */
#define EEBUS_DATA_ENUM_WITH_FLAGS(ed_name, struct_name, struct_field, ecfg, ed_flags) \
    {                                                                                  \
        .interface_ = &eebus_data_enum_methods,                                        \
        .name       = ed_name,                                                         \
        .offset     = STRUCT_MEMBER_OFFSET(struct_name, struct_field),                 \
        .size       = sizeof(int32_t),                                                 \
        .flags      = ed_flags,                                                        \
        .metadata   = ecfg,                                                            \
    }

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_COMMON_EEBUS_DATA_EEBUS_DATA_ENUM_H_
