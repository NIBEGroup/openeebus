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
 * @brief Customer Energy Manager OHPCF use case implementation
 */

#include "src/use_case/actor/cem/ohpcf/cem_ohpcf.h"

#include <stddef.h>

#include "src/common/array_util.h"
#include "src/spine/entity/entity_local.h"
#include "src/spine/feature/feature_local.h"
#include "src/spine/model/usecase_information_types.h"
#include "src/use_case/actor/cem/ohpcf/cem_ohpcf.h"
#include "src/use_case/actor/cem/ohpcf/cem_ohpcf_events.h"
#include "src/use_case/actor/cem/ohpcf/cem_ohpcf_internal.h"
#include "src/use_case/use_case.h"

static const UseCaseInterface cem_ohpcf_use_case_methods = {
    .destruct                       = UseCaseDestruct,
    .is_entity_compatible           = UseCaseIsEntityCompatible,
    .is_use_case_compatible         = UseCaseIsUseCaseCompatible,
    .get_remote_entity_with_address = UseCaseGetRemoteEntityWithAddress,
};

static const UseCaseActorType valid_actor_types[] = {kUseCaseActorTypeCompressor};
static const EntityTypeType valid_entity_types[]  = {kEntityTypeTypeCompressor};

static const FeatureTypeType use_case_scenario_support_1_features[] = {kFeatureTypeTypeSmartEnergyManagementPs};
static const FeatureTypeType use_case_scenario_support_2_features[] = {kFeatureTypeTypeSmartEnergyManagementPs};

static const UseCaseScenario use_case_scenarios[] = {
    {
     .scenario             = (UseCaseScenarioSupportType)1,
     .mandatory            = true,
     .server_features      = use_case_scenario_support_1_features,
     .server_features_size = ARRAY_SIZE(use_case_scenario_support_1_features),
     },
    {
     .scenario             = (UseCaseScenarioSupportType)2,
     .mandatory            = true,
     .server_features      = use_case_scenario_support_2_features,
     .server_features_size = ARRAY_SIZE(use_case_scenario_support_2_features),
     },
};

static const UseCaseInfo cem_ohpcf_use_case_info = {
    .valid_actor_types       = valid_actor_types,
    .valid_actor_types_size  = ARRAY_SIZE(valid_actor_types),
    .valid_entity_types      = valid_entity_types,
    .valid_entity_types_size = ARRAY_SIZE(valid_entity_types),
    .use_case_scenarios      = use_case_scenarios,
    .use_case_scenarios_size = ARRAY_SIZE(use_case_scenarios),
    .actor                   = kUseCaseActorTypeCEM,
    .use_case_name_id        = kUseCaseNameTypeOptimizationOfSelfConsumptionByHeatPumpCompressorFlexibility,
    .version                 = "1.0.0",
    .sub_revision            = "release",
    .available               = true,
};

static EebusError AddFeatures(EntityLocalObject* entity);
static EebusError CemOhpcfUseCaseConstruct(
    CemOhpcfUseCase* self,
    EntityLocalObject* local_entity,
    CemOhpcfListenerObject* cem_ohpcf_listener
);

EebusError AddFeatures(EntityLocalObject* entity) {
  // CEM OHPCF client features
  FeatureLocalObject* const fl
      = ENTITY_LOCAL_ADD_FEATURE_WITH_TYPE_AND_ROLE(entity, kFeatureTypeTypeSmartEnergyManagementPs, kRoleTypeClient);
  if (fl == NULL) {
    return kEebusErrorMemoryAllocate;
  }

  FEATURE_LOCAL_SET_FUNCTION_OPERATIONS(fl, kFunctionTypeSmartEnergyManagementPsData, true, false);

  return kEebusErrorOk;
}

EebusError CemOhpcfUseCaseConstruct(
    CemOhpcfUseCase* self,
    EntityLocalObject* local_entity,
    CemOhpcfListenerObject* cem_ohpcf_listener
) {
  UseCaseConstruct(USE_CASE(self), &cem_ohpcf_use_case_info, local_entity, CemOhpcfHandleEvent);
  // Override "virtual functions table"
  USE_CASE_INTERFACE(self) = &cem_ohpcf_use_case_methods;

  self->cem_ohpcf_listener = cem_ohpcf_listener;
  self->state              = kCompressorOhpcfStateUndefined;

  return AddFeatures(local_entity);
}

CemOhpcfUseCaseObject*
CemOhpcfUseCaseCreate(EntityLocalObject* local_entity, CemOhpcfListenerObject* cem_ohpcf_listener) {
  CemOhpcfUseCase* cem_ohpcf_use_case = EEBUS_MALLOC(sizeof(*cem_ohpcf_use_case));
  if (cem_ohpcf_use_case == NULL) {
    return NULL;
  }

  if (CemOhpcfUseCaseConstruct(cem_ohpcf_use_case, local_entity, cem_ohpcf_listener) != kEebusErrorOk) {
    UseCaseDelete(USE_CASE_OBJECT(cem_ohpcf_use_case));
    return NULL;
  }

  return CEM_OHPCF_USE_CASE_OBJECT(cem_ohpcf_use_case);
}
