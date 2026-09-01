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
 * @brief EEBUS CLI MA MPC commands handling
 */

#ifndef SRC_CLI_EEBUS_CLI_MA_MPC_H_
#define SRC_CLI_EEBUS_CLI_MA_MPC_H_

#include <stddef.h>

#include "src/cli/eebus_cli_handler_interface.h"
#include "src/common/eebus_malloc.h"
#include "src/spine/model/common_data_types.h"
#include "src/spine/model/entity_types.h"
#include "src/use_case/actor/ma/mpc/ma_mpc.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

EebusCliHandlerObject* MaMpcCliCreate(MaMpcUseCaseObject* ma_mpc, const EntityAddressType* entity_addr);

static inline void MaMpcCliDelete(EebusCliHandlerObject* ma_mpc_cli) {
    if (ma_mpc_cli != NULL) {
        EEBUS_CLI_HANDLER_DESTRUCT(ma_mpc_cli);
        EEBUS_FREE(ma_mpc_cli);
    }
}

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_CLI_EEBUS_CLI_MA_MPC_H_
