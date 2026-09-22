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
 * @brief Monitoring Appliance MGCP use case implementation
 */

#include "src/use_case/actor/ma/mgcp/ma_mgcp.h"

#include <stddef.h>

#include "src/common/array_util.h"
#include "src/common/eebus_arguments.h"
#include "src/spine/entity/entity_local.h"
#include "src/spine/feature/feature_local.h"
#include "src/spine/model/usecase_information_types.h"
#include "src/use_case/actor/ma/mgcp/ma_mgcp_events.h"
#include "src/use_case/actor/ma/mgcp/ma_mgcp_internal.h"
#include "src/use_case/use_case.h"

static void MaMgcpUseCaseDestruct(UseCaseObject* self);

static const UseCaseInterface ma_mgcp_use_case_methods = {
    .destruct                       = MaMgcpUseCaseDestruct,
    .is_entity_compatible           = UseCaseIsEntityCompatible,
    .is_use_case_compatible         = UseCaseIsUseCaseCompatible,
    .get_remote_entity_with_address = UseCaseGetRemoteEntityWithAddress,
};

/* MA accepts remote entities that advertise as GridConnectionPoint actor */
static const UseCaseActorType valid_actor_types[] = {kUseCaseActorTypeGridConnectionPoint};

/* Valid remote entity types for the Grid Connection Point */
static const EntityTypeType valid_entity_types[] = {
    kEntityTypeTypeGridConnectionPointOfPremises,
    kEntityTypeTypeCEM,
};

static const FeatureTypeType use_case_scenario_features[] = {
    kFeatureTypeTypeElectricalConnection,
    kFeatureTypeTypeMeasurement,
};

static const FeatureTypeType use_case_scenario1_features[] = {
    kFeatureTypeTypeDeviceConfiguration,
};

static const UseCaseScenario use_case_scenarios[] = {
    {
     .scenario             = (UseCaseScenarioSupportType)1,
     .mandatory            = false,
     .server_features      = use_case_scenario1_features,
     .server_features_size = ARRAY_SIZE(use_case_scenario1_features),
     },
    {
     .scenario             = (UseCaseScenarioSupportType)2,
     .mandatory            = true,
     .server_features      = use_case_scenario_features,
     .server_features_size = ARRAY_SIZE(use_case_scenario_features),
     },
    {
     .scenario             = (UseCaseScenarioSupportType)3,
     .mandatory            = false,
     .server_features      = use_case_scenario_features,
     .server_features_size = ARRAY_SIZE(use_case_scenario_features),
     },
    {
     .scenario             = (UseCaseScenarioSupportType)4,
     .mandatory            = false,
     .server_features      = use_case_scenario_features,
     .server_features_size = ARRAY_SIZE(use_case_scenario_features),
     },
    {
     .scenario             = (UseCaseScenarioSupportType)5,
     .mandatory            = false,
     .server_features      = use_case_scenario_features,
     .server_features_size = ARRAY_SIZE(use_case_scenario_features),
     },
    {
     .scenario             = (UseCaseScenarioSupportType)6,
     .mandatory            = false,
     .server_features      = use_case_scenario_features,
     .server_features_size = ARRAY_SIZE(use_case_scenario_features),
     },
    {
     .scenario             = (UseCaseScenarioSupportType)7,
     .mandatory            = false,
     .server_features      = use_case_scenario_features,
     .server_features_size = ARRAY_SIZE(use_case_scenario_features),
     },
};

static const UseCaseInfo ma_mgcp_use_case_info = {
    .valid_actor_types       = valid_actor_types,
    .valid_actor_types_size  = ARRAY_SIZE(valid_actor_types),
    .valid_entity_types      = valid_entity_types,
    .valid_entity_types_size = ARRAY_SIZE(valid_entity_types),
    .use_case_scenarios      = use_case_scenarios,
    .use_case_scenarios_size = ARRAY_SIZE(use_case_scenarios),
    .actor                   = kUseCaseActorTypeMonitoringAppliance,
    .use_case_name_id        = kUseCaseNameTypeMonitoringOfGridConnectionPoint,
    .version                 = "1.0.0",
    .sub_revision            = "release",
    .available               = true,
};

static EebusError AddFeatures(UseCaseObject* self, EntityLocalObject* entity);
static EebusError
MaMgcpUseCaseConstruct(MaMgcpUseCase* self, EntityLocalObject* local_entity, MaMgcpListenerObject* ma_mgcp_listener);

static EebusError AddFeatures(UseCaseObject* self, EntityLocalObject* entity) {
  UNUSED(self);

  const FeatureTypeType client_features[] = {
      kFeatureTypeTypeElectricalConnection,
      kFeatureTypeTypeMeasurement,
      kFeatureTypeTypeDeviceConfiguration,
  };

  for (size_t i = 0; i < ARRAY_SIZE(client_features); ++i) {
    if (ENTITY_LOCAL_ADD_FEATURE_WITH_TYPE_AND_ROLE(entity, client_features[i], kRoleTypeClient) == NULL) {
      return kEebusErrorInit;
    }
  }

  return kEebusErrorOk;
}

static EebusError
MaMgcpUseCaseConstruct(MaMgcpUseCase* self, EntityLocalObject* local_entity, MaMgcpListenerObject* ma_mgcp_listener) {
  UseCaseConstruct(USE_CASE(self), &ma_mgcp_use_case_info, local_entity, MaMgcpHandleEvent);
  USE_CASE_INTERFACE(self) = &ma_mgcp_use_case_methods;

  self->ma_mgcp_listener = ma_mgcp_listener;
  return AddFeatures(USE_CASE_OBJECT(self), local_entity);
}

MaMgcpUseCaseObject* MaMgcpUseCaseCreate(EntityLocalObject* local_entity, MaMgcpListenerObject* ma_mgcp_listener) {
  MaMgcpUseCase* const ma_mgcp_use_case = EEBUS_MALLOC(sizeof(*ma_mgcp_use_case));
  if (ma_mgcp_use_case == NULL) {
    return NULL;
  }

  if (MaMgcpUseCaseConstruct(ma_mgcp_use_case, local_entity, ma_mgcp_listener) != kEebusErrorOk) {
    MaMgcpUseCaseDelete(MA_MGCP_USE_CASE_OBJECT(ma_mgcp_use_case));
    return NULL;
  }

  return MA_MGCP_USE_CASE_OBJECT(ma_mgcp_use_case);
}

static void MaMgcpUseCaseDestruct(UseCaseObject* self) {
  UseCaseDestruct(self);
}
