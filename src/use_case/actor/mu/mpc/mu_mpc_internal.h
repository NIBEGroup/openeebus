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

#ifndef SRC_USE_CASE_ACTOR_MU_MPC_MU_MPC_INTERNAL_H_
#define SRC_USE_CASE_ACTOR_MU_MPC_MU_MPC_INTERNAL_H_

#include <stddef.h>

#include "src/spine/model/measurement_types.h"
#include "src/use_case/actor/common/eebus_monitor_container.h"
#include "src/use_case/actor/mu/mpc/mu_mpc_measurement.h"
#include "src/use_case/actor/mu/mpc/mu_mpc_monitor.h"
#include "src/use_case/api/mu_mpc_listener_interface.h"
#include "src/use_case/model/load_limit_types.h"
#include "src/use_case/use_case.h"

typedef struct MuMpcUseCase MuMpcUseCase;

struct MuMpcUseCase {
  /** Inherits the Use Case */
  UseCase obj;

  ElectricalConnectionIdType electrical_connection_id;

  EebusMonitorContainer monitor_container;

  MuMpcListenerObject* mu_mpc_listener;

  UseCaseScenario mu_mpc_scenarios[5];

  size_t mu_mpc_scenarios_size;
  UseCaseInfo mu_mpc_use_case_info;
};

#define MU_MPC_USE_CASE(self) ((MuMpcUseCase*)(self))

#endif  // SRC_USE_CASE_ACTOR_MU_MPC_MU_MPC_INTERNAL_H_
