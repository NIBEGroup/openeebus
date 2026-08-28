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
 * @brief GCP MGCP use case internal declarations
 */

#ifndef SRC_USE_CASE_ACTOR_GCP_MGCP_GCP_MGCP_INTERNAL_H_
#define SRC_USE_CASE_ACTOR_GCP_MGCP_GCP_MGCP_INTERNAL_H_

#include <stdbool.h>
#include <stddef.h>

#include "src/use_case/actor/common/eebus_monitor_container.h"
#include "src/use_case/actor/gcp/mgcp/gcp_mgcp_monitor.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct GcpMgcpUseCase GcpMgcpUseCase;

struct GcpMgcpUseCase {
    /** Inherits the Use Case */
    UseCase obj;

    ElectricalConnectionIdType electrical_connection_id;

    EebusMonitorContainer monitor_container;

    /** Dynamically built scenario array (scenarios 1–7, up to 7 entries) */
    UseCaseScenario gcp_mgcp_scenarios[7];
    size_t gcp_mgcp_scenarios_size;

    bool has_scenario1;

    UseCaseInfo gcp_mgcp_use_case_info;
};

#define GCP_MGCP_USE_CASE(self) ((GcpMgcpUseCase*)(self))

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_USE_CASE_ACTOR_GCP_MGCP_GCP_MGCP_INTERNAL_H_
