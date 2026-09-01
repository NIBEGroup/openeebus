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
 * @brief MA MPC use case internal declarations
 */

#ifndef SRC_USE_CASE_MA_MPC_MA_MPC_INTERNAL_H_
#define SRC_USE_CASE_MA_MPC_MA_MPC_INTERNAL_H_

#include <stdbool.h>

#include "src/use_case/api/ma_mpc_listener_interface.h"
#include "src/use_case/model/scaled_value.h"
#include "src/use_case/specialization/device_diagnosis/device_diagnosis_client.h"
#include "src/use_case/use_case.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct MaMpcUseCase MaMpcUseCase;
struct MaMpcUseCase {
    /** Inherits the Use Case */
    UseCase obj;

    MaMpcListenerObject* ma_mpc_listener;
};

#define MA_MPC_USE_CASE(obj) ((MaMpcUseCase*)(obj))

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_USE_CASE_MA_MPC_MA_MPC_INTERNAL_H_
