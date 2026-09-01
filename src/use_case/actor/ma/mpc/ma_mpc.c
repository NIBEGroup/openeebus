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
 * @brief Monitoring Appliance MPC use case implementation
 */

#include "src/use_case/actor/ma/mpc/ma_mpc.h"

#include <stddef.h>

#include "src/common/array_util.h"
#include "src/common/eebus_arguments.h"
#include "src/spine/entity/entity_local.h"
#include "src/spine/feature/feature_local.h"
#include "src/spine/model/usecase_information_types.h"
#include "src/use_case/actor/ma/mpc/ma_mpc_events.h"
#include "src/use_case/actor/ma/mpc/ma_mpc_internal.h"
#include "src/use_case/use_case.h"

static void MaMpcUseCaseDestruct(UseCaseObject* self);

static const UseCaseInterface mam_mpc_use_case_methods = {
    .destruct                       = MaMpcUseCaseDestruct,
    .is_entity_compatible           = UseCaseIsEntityCompatible,
    .is_use_case_compatible         = UseCaseIsUseCaseCompatible,
    .get_remote_entity_with_address = UseCaseGetRemoteEntityWithAddress,
};

static const UseCaseActorType valid_actor_types[] = {kUseCaseActorTypeMonitoredUnit};
static const EntityTypeType valid_entity_types[]  = {
    kEntityTypeTypeBattery,
    kEntityTypeTypeCompressor,
    kEntityTypeTypeElectricalImmersionHeater,
    kEntityTypeTypeEVSE,
    kEntityTypeTypeHeatPumpAppliance,
    kEntityTypeTypeInverter,
    kEntityTypeTypeSmartEnergyAppliance,
    kEntityTypeTypeSubMeterElectricity,
};

static const FeatureTypeType use_case_scenario_support_features[]
    = {kFeatureTypeTypeElectricalConnection, kFeatureTypeTypeMeasurement};

static const UseCaseScenario use_case_scenarios[] = {
    {
     .scenario             = (UseCaseScenarioSupportType)1,
     .mandatory            = true,
     .server_features      = use_case_scenario_support_features,
     .server_features_size = ARRAY_SIZE(use_case_scenario_support_features),
     },
    {
     .scenario             = (UseCaseScenarioSupportType)2,
     .mandatory            = false,
     .server_features      = use_case_scenario_support_features,
     .server_features_size = ARRAY_SIZE(use_case_scenario_support_features),
     },
    {
     .scenario             = (UseCaseScenarioSupportType)3,
     .mandatory            = false,
     .server_features      = use_case_scenario_support_features,
     .server_features_size = ARRAY_SIZE(use_case_scenario_support_features),
     },
    {
     .scenario             = (UseCaseScenarioSupportType)4,
     .mandatory            = false,
     .server_features      = use_case_scenario_support_features,
     .server_features_size = ARRAY_SIZE(use_case_scenario_support_features),
     },
    {
     .scenario             = (UseCaseScenarioSupportType)5,
     .mandatory            = false,
     .server_features      = use_case_scenario_support_features,
     .server_features_size = ARRAY_SIZE(use_case_scenario_support_features),
     },
};

static const UseCaseInfo ma_mpc_use_case_info = {
    .valid_actor_types       = valid_actor_types,
    .valid_actor_types_size  = ARRAY_SIZE(valid_actor_types),
    .valid_entity_types      = valid_entity_types,
    .valid_entity_types_size = ARRAY_SIZE(valid_entity_types),
    .use_case_scenarios      = use_case_scenarios,
    .use_case_scenarios_size = ARRAY_SIZE(use_case_scenarios),
    .actor                   = kUseCaseActorTypeMonitoringAppliance,
    .use_case_name_id        = kUseCaseNameTypeMonitoringOfPowerConsumption,
    .version                 = "1.0.0",
    .sub_revision            = "release",
    .available               = true,
};

static EebusError AddFeatures(UseCaseObject* self, EntityLocalObject* entity);
static EebusError
MaMpcUseCaseConstruct(MaMpcUseCase* self, EntityLocalObject* local_entity, MaMpcListenerObject* ma_mpc_listener);

EebusError AddFeatures(UseCaseObject* self, EntityLocalObject* entity) {
    UNUSED(self);
    // client features
    const FeatureTypeType client_features[] = {
        kFeatureTypeTypeElectricalConnection,
        kFeatureTypeTypeMeasurement,
    };

    for (size_t i = 0; i < ARRAY_SIZE(client_features); ++i) {
        if (ENTITY_LOCAL_ADD_FEATURE_WITH_TYPE_AND_ROLE(entity, client_features[i], kRoleTypeClient) == NULL) {
            return kEebusErrorInit;
        }
    }

    return kEebusErrorOk;
}

EebusError
MaMpcUseCaseConstruct(MaMpcUseCase* self, EntityLocalObject* local_entity, MaMpcListenerObject* ma_mpc_listener) {
    UseCaseConstruct(USE_CASE(self), &ma_mpc_use_case_info, local_entity, MaMpcHandleEvent);
    // Override "virtual functions table"
    USE_CASE_INTERFACE(self) = &mam_mpc_use_case_methods;

    self->ma_mpc_listener = ma_mpc_listener;
    return AddFeatures(USE_CASE_OBJECT(self), local_entity);
}

MaMpcUseCaseObject* MaMpcUseCaseCreate(EntityLocalObject* local_entity, MaMpcListenerObject* ma_mpc_listener) {
    MaMpcUseCase* ma_mpc_use_case = EEBUS_MALLOC(sizeof(*ma_mpc_use_case));
    if (ma_mpc_use_case == NULL) {
        return NULL;
    }

    if (MaMpcUseCaseConstruct(ma_mpc_use_case, local_entity, ma_mpc_listener) != kEebusErrorOk) {
        MaMpcUseCaseDelete(MA_MPC_USE_CASE_OBJECT(ma_mpc_use_case));
        return NULL;
    }

    return MA_MPC_USE_CASE_OBJECT(ma_mpc_use_case);
}

void MaMpcUseCaseDestruct(UseCaseObject* self) {
    UseCaseDestruct(self);
}
