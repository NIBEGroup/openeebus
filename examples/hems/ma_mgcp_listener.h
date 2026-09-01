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
 * @brief Ma Mgcp Listener implementation declarations
 */

#ifndef EXAMPLES_HEMS_MA_MGCP_LISTENER_H_
#define EXAMPLES_HEMS_MA_MGCP_LISTENER_H_

#include <stddef.h>

#include "examples/hems/hems.h"
#include "src/common/eebus_malloc.h"
#include "src/use_case/api/ma_mgcp_listener_interface.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

MaMgcpListenerObject* MaMgcpListenerCreate(HemsObject* hems);

static inline void MaMgcpListenerDelete(MaMgcpListenerObject* ma_mgcp_listener) {
    if (ma_mgcp_listener != NULL) {
        MA_MGCP_LISTENER_DESTRUCT(ma_mgcp_listener);
        EEBUS_FREE(ma_mgcp_listener);
    }
}

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // EXAMPLES_HEMS_MA_MGCP_LISTENER_H_
