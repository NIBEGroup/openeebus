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
 * @brief Controllable System LPC use case implementation
 */

#include "src/use_case/actor/cs/lpc/cs_lpc.h"

#include <stddef.h>

#include "src/common/array_util.h"
#include "src/use_case/actor/cs/cs_lp_internal.h"

static const UseCaseActorType valid_actor_types[] = {kUseCaseActorTypeEnergyGuard};
static const EntityTypeType valid_entity_types[]  = {
    kEntityTypeTypeGridGuard,
    kEntityTypeTypeCEM,  // KEO uses this entity type for an SMGW whysoever
};

static const FeatureTypeType use_case_scenario_support_3_features[] = {kFeatureTypeTypeDeviceDiagnosis};

static const UseCaseScenario use_case_scenarios[] = {
    {
     .scenario  = (UseCaseScenarioSupportType)1,
     .mandatory = true,
     },
    {
     .scenario  = (UseCaseScenarioSupportType)2,
     .mandatory = true,
     },
    {
     .scenario             = (UseCaseScenarioSupportType)3,
     .mandatory            = true,
     .server_features      = use_case_scenario_support_3_features,
     .server_features_size = ARRAY_SIZE(use_case_scenario_support_3_features),
     },
    {
     .scenario  = (UseCaseScenarioSupportType)4,
     .mandatory = true,
     },
};

static const UseCaseInfo cs_lpc_use_case_info = {
    .valid_actor_types       = valid_actor_types,
    .valid_actor_types_size  = ARRAY_SIZE(valid_actor_types),
    .valid_entity_types      = valid_entity_types,
    .valid_entity_types_size = ARRAY_SIZE(valid_entity_types),
    .use_case_scenarios      = use_case_scenarios,
    .use_case_scenarios_size = ARRAY_SIZE(use_case_scenarios),
    .actor                   = kUseCaseActorTypeControllableSystem,
    .use_case_name_id        = kUseCaseNameTypeLimitationOfPowerConsumption,
    .version                 = "1.0.0",
    .sub_revision            = "release",
    .available               = true,
};

CsLpUseCaseObject* CsLpcUseCaseCreate(
    EntityLocalObject* local_entity,
    ElectricalConnectionIdType ec_id,
    CsLpListenerObject* cs_lpc_listener
) {
    return CsLpUseCaseCreate(kEnergyDirectionTypeConsume, &cs_lpc_use_case_info, local_entity, ec_id, cs_lpc_listener);
}
