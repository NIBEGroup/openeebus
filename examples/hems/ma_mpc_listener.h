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
 * @brief Ma Mpc Listener implementation declarations
 */

#ifndef SRC_USE_CASE_ACTOR_MA_MPC_MA_MPC_LISTENER_H_
#define SRC_USE_CASE_ACTOR_MA_MPC_MA_MPC_LISTENER_H_

#include <stddef.h>

#include "examples/hems/hems.h"
#include "src/common/eebus_malloc.h"
#include "src/use_case/api/ma_mpc_listener_interface.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

MaMpcListenerObject* MaMpcListenerCreate(HemsObject* hems);

static inline void MaMpcListenerDelete(MaMpcListenerObject* ma_mpc_listener) {
    if (ma_mpc_listener != NULL) {
        MA_MPC_LISTENER_DESTRUCT(ma_mpc_listener);
        EEBUS_FREE(ma_mpc_listener);
    }
}

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_USE_CASE_ACTOR_MA_MPC_MA_MPC_LISTENER_H_
