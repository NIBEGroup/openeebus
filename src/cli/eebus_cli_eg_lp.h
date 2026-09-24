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
 * @brief EEBUS CLI EG Limitation of Power commands handling
 */

#ifndef SRC_CLI_EEBUS_CLI_EG_LP_H_
#define SRC_CLI_EEBUS_CLI_EG_LP_H_

#include <stddef.h>

#include "src/cli/eebus_cli_handler_interface.h"
#include "src/common/eebus_malloc.h"
#include "src/common/entity_address_list.h"
#include "src/spine/model/common_data_types.h"
#include "src/use_case/actor/eg/eg_lp.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

EebusCliHandlerObject*
EgLpCliCreate(EnergyDirectionType energy_direction, EgLpUseCaseObject* eg_lp, const EntityAddressList* addr_list);

static inline void EgLpCliDelete(EebusCliHandlerObject* eg_lp_cli) {
  if (eg_lp_cli != NULL) {
    EEBUS_CLI_HANDLER_DESTRUCT(eg_lp_cli);
    EEBUS_FREE(eg_lp_cli);
  }
}

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_CLI_EEBUS_CLI_EG_LP_H_
