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
 * @brief EG LPP Listener implementation declarations
 */

#ifndef EXAMPLES_HEMS_EG_LPP_LISTENER_H_
#define EXAMPLES_HEMS_EG_LPP_LISTENER_H_

#include <stddef.h>

#include "examples/hems/hems.h"
#include "src/common/eebus_malloc.h"
#include "src/use_case/api/eg_lp_listener_interface.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

EgLpListenerObject* EgLppListenerCreate(HemsObject* hems);

static inline void EgLppListenerDelete(EgLpListenerObject* eg_lpp_listener) {
    if (eg_lpp_listener != NULL) {
        EG_LP_LISTENER_DESTRUCT(eg_lpp_listener);
        EEBUS_FREE(eg_lpp_listener);
    }
}

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // EXAMPLES_HEMS_EG_LPP_LISTENER_H_
