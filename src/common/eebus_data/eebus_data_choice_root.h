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
 * @brief EEEBUS Data Choice Root is a kind of wrapper that does
 * reuquired adjustments when Data Choice is added directly to the
 * root structure (e.g. SHIP messages)
 */

#ifndef SRC_COMMON_EEBUS_DATA_EEBUS_DATA_CHOICE_ROOT_H_
#define SRC_COMMON_EEBUS_DATA_EEBUS_DATA_CHOICE_ROOT_H_

#include "src/common/api/eebus_data_interface.h"
#include "src/common/struct_util.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @brief EEBUS Data Choice Root Interface
 */
extern const EebusDataInterface eebus_data_choice_root_methods;

/**
 * @brief EEBUS Data Choice Root type check
 */
#define EEBUS_DATA_IS_CHOICE_ROOT(cfg) (EEBUS_DATA_INTERFACE(cfg) == &eebus_data_choice_root_methods)

/**
 * @brief EEBUS Data Choice Root is a kind of wrapper that does
 * reuquired adjustments when Data Choice is added directly to the
 * root structure (e.g. SHIP messages)
 * @param struct_name Structure name associated with Data record
 * @param c_cfg Choice Data configuration entry point
 */
#define EEBUS_DATA_CHOICE_ROOT(struct_name, c_cfg)     \
    {                                                  \
        .interface_ = &eebus_data_choice_root_methods, \
        .name       = "",                              \
        .offset     = 0,                               \
        .size       = sizeof(struct_name),             \
        .metadata   = c_cfg,                           \
    }

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_COMMON_EEBUS_DATA_EEBUS_DATA_CHOICE_ROOT_H_
