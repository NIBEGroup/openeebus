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
 * @brief Smart Energy Management PS is a EEBUS Data Sequence "subclass" whith the only difference in
 * partial data write/delete as it unlike EEBUS Data Sequence simply forwards the
 * partial data update requests directy to that internal EEBUS Data List keeping collection of items.
 */

#ifndef SRC_SPINE_MODEL_SMART_ENERGY_MANAGEMENT_PS_CFG_H_
#define SRC_SPINE_MODEL_SMART_ENERGY_MANAGEMENT_PS_CFG_H_

#include "src/common/api/eebus_data_interface.h"
#include "src/common/struct_util.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @brief Smart Energy Management PS Interface
 */
extern const EebusDataInterface eebus_data_smart_energy_management_ps_methods;

/**
 * @brief Smart Energy Management PS configuration. Comparing to EEBUS Data Sequence,
 * Smart Energy Management PS shall wrap the only one element of EEBUS Data List type
 * keeping the collection of elements.
 *
 * @param ce_cfg Container element data configuration entry point. Shall point
 * to single instance of the EEBUS Data List
 */
#define SMART_ENERGY_MANAGEMENT_PS_CFG(ed_name, struct_name, struct_field, ce_cfg) \
  {                                                                                \
      .interface_ = &eebus_data_smart_energy_management_ps_methods,                \
      .name       = ed_name,                                                       \
      .offset     = STRUCT_MEMBER_OFFSET(struct_name, struct_field),               \
      .size       = sizeof(*STRUCT_MEMBER(struct_name, struct_field)),             \
      .metadata   = ce_cfg,                                                        \
  }

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SPINE_MODEL_SMART_ENERGY_MANAGEMENT_PS_CFG_H_
