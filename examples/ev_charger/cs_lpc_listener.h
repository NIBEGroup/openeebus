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
 * @brief CS LPC Listener declarations for EV Charger
 */

#ifndef EXAMPLES_EV_CHARGER_CS_LPC_LISTENER_H_
#define EXAMPLES_EV_CHARGER_CS_LPC_LISTENER_H_

#include "src/common/eebus_malloc.h"
#include "src/use_case/actor/cs/cs_lp.h"
#include "src/use_case/api/cs_lp_listener_interface.h"

#ifdef __cplusplus
extern "C" {
#endif

CsLpListenerObject* CsLpcListenerCreate(void);

static inline void CsLpcListenerDelete(CsLpListenerObject* cs_lpc_listener) {
  if (cs_lpc_listener != NULL) {
    CS_LP_LISTENER_DESTRUCT(cs_lpc_listener);
    EEBUS_FREE(cs_lpc_listener);
  }
}

#ifdef __cplusplus
}
#endif

#endif  // EXAMPLES_EV_CHARGER_CS_LPC_LISTENER_H_
