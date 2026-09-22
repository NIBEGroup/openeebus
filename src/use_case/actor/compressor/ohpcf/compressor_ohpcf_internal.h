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
 * @brief Compressor OHPCF use case internal declarations
 */

#ifndef SRC_USE_CASE_ACTOR_COMPRESSOR_OHPCF_COMPRESSOR_OHPCF_INTERNAL_H_
#define SRC_USE_CASE_ACTOR_COMPRESSOR_OHPCF_COMPRESSOR_OHPCF_INTERNAL_H_

#include <stdbool.h>

#include "src/use_case/api/compressor_ohpcf_listener_interface.h"
#include "src/use_case/model/ohpcf_types.h"
#include "src/use_case/use_case.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct CompressorOhpcfUseCase CompressorOhpcfUseCase;

struct CompressorOhpcfUseCase {
  /** Inherits the Use Case */
  UseCase obj;

  CompressorOhpcfListenerObject* cp_ohpcf_listener;

  CompressorOhpcfState state;
};

#define COMPRESSOR_OHPCF_USE_CASE(obj) ((CompressorOhpcfUseCase*)(obj))

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_USE_CASE_ACTOR_COMPRESSOR_OHPCF_COMPRESSOR_OHPCF_INTERNAL_H_
