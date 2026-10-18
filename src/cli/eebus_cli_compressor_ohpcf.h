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
 * @brief EEBUS CLI Compressor OHPCF commands handling
 */

#ifndef SRC_CLI_EEBUS_CLI_COMPRESSOR_OHPCF_H_
#define SRC_CLI_EEBUS_CLI_COMPRESSOR_OHPCF_H_

#include <stddef.h>

#include "src/cli/eebus_cli_handler_interface.h"
#include "src/common/eebus_malloc.h"
#include "src/use_case/actor/compressor/ohpcf/compressor_ohpcf.h"
#include "src/use_case/actor/mu/mpc/mu_mpc.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

EebusCliHandlerObject*
CompressorOhpcfCliCreate(CompressorOhpcfUseCaseObject* compressor_ohpcf, MuMpcUseCaseObject* mu_mpc);

static inline void CompressorOhpcfCliDelete(EebusCliHandlerObject* compressor_ohpcf_cli) {
  if (compressor_ohpcf_cli != NULL) {
    EEBUS_CLI_HANDLER_DESTRUCT(compressor_ohpcf_cli);
    EEBUS_FREE(compressor_ohpcf_cli);
  }
}

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  // SRC_CLI_EEBUS_CLI_COMPRESSOR_OHPCF_H_
