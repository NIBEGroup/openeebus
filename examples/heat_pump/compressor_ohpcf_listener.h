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
 * @brief Compressor OHPCF Listener implementation declarations
 */

#ifndef EXAMPLES_HEAT_PUMP_COMPRESSOR_OHPCF_LISTENER_H_
#define EXAMPLES_HEAT_PUMP_COMPRESSOR_OHPCF_LISTENER_H_

#include <stddef.h>

#include "src/common/eebus_malloc.h"
#include "src/use_case/api/compressor_ohpcf_listener_interface.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

CompressorOhpcfListenerObject* CompressorOhpcfListenerCreate(void);

static inline void CompressorOhpcfListenerDelete(CompressorOhpcfListenerObject* compressor_ohpcf_listener) {
  if (compressor_ohpcf_listener != NULL) {
    COMPRESSOR_OHPCF_LISTENER_DESTRUCT(compressor_ohpcf_listener);
    EEBUS_FREE(compressor_ohpcf_listener);
  }
}

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // EXAMPLES_HEAT_PUMP_COMPRESSOR_OHPCF_LISTENER_H_
