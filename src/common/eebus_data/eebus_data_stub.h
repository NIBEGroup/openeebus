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
 * @brief EEEBUS Data Stub
 */

#ifndef SRC_COMMON_EEBUS_DATA_EEBUS_DATA_STUB_H_
#define SRC_COMMON_EEBUS_DATA_EEBUS_DATA_STUB_H_

#include "src/common/api/eebus_data_interface.h"
#include "src/common/struct_util.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @brief EEBUS Data Stub Interface
 */
extern const EebusDataInterface eebus_data_stub_methods;

/**
 * @brief EEBUS Data Stub type check
 */
#define EEBUS_DATA_IS_STUB(cfg) (EEBUS_DATA_INTERFACE(cfg) == &eebus_data_stub_methods)

/**
 * @brief EEBUS Data Stub configuration
 * @param ed_name Expected Data record name
 */
#define EEBUS_DATA_STUB(ed_name)                \
    {                                           \
        .interface_ = &eebus_data_stub_methods, \
        .name       = ed_name,                  \
        .offset     = 0,                        \
        .size       = 0,                        \
        .metadata   = NULL,                     \
    }

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_COMMON_EEBUS_DATA_EEBUS_DATA_STUB_H_
