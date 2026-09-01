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
 * @brief EEEBUS Data String
 */

#ifndef SRC_COMMON_EEBUS_DATA_EEBUS_DATA_STRING_H_
#define SRC_COMMON_EEBUS_DATA_EEBUS_DATA_STRING_H_

#include "src/common/api/eebus_data_interface.h"
#include "src/common/eebus_data/eebus_data_simple.h"
#include "src/common/struct_util.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @brief EEBUS Data String Interface
 */
extern const EebusDataInterface eebus_data_string_methods;

/**
 * @brief EEBUS Data String type check
 */
#define EEBUS_DATA_IS_STRING(cfg) (EEBUS_DATA_INTERFACE(cfg) == &eebus_data_string_methods)

/**
 * @brief EEBUS Data String configuration
 * @param ed_name Expected Data record name
 * @param struct_name Structure name associated with Data record
 * @param struct_field Structure field name.
 * Type of structure field shall be char*
 */
#define EEBUS_DATA_STRING(ed_name, struct_name, struct_field) \
    EEBUS_DATA_SIMPLE(&eebus_data_string_methods, ed_name, struct_name, struct_field, 0)

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_COMMON_EEBUS_DATA_EEBUS_DATA_STRING_H_
