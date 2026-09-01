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

#include "src/use_case/actor/mu/mpc/mu_mpc.h"
#include "src/use_case/actor/mu/mpc/mu_mpc_events.h"

#include "src/common/array_util.h"
#include "src/common/eebus_arguments.h"
#include "src/use_case/actor/common/eebus_monitor_features.h"
#include "src/use_case/actor/mu/mpc/mu_mpc_internal.h"
#include "src/use_case/actor/mu/mpc/mu_mpc_measurement.h"
#include "src/use_case/actor/mu/mpc/mu_mpc_monitor.h"
#include "src/use_case/model/load_limit_types.h"
#include "src/use_case/use_case.h"

static const UseCaseActorType valid_actor_types[] = {kUseCaseActorTypeMonitoringAppliance};

static void Destruct(UseCaseObject* self);
static bool IsEntityCompatible(const UseCaseObject* self, const EntityRemoteObject* remote_entity);

static const UseCaseInterface mu_mpc_use_case_methods = {
    .destruct                       = Destruct,
    .is_entity_compatible           = IsEntityCompatible,
    .is_use_case_compatible         = UseCaseIsUseCaseCompatible,
    .get_remote_entity_with_address = UseCaseGetRemoteEntityWithAddress,
};

static EebusError MuMpcUseCaseConstruct(
    MuMpcUseCase* self,
    EntityLocalObject* local_entity,
    ElectricalConnectionIdType ec_id,
    const MuMpcConfig* cfg,
    MuMpcListenerObject* listener
);

EebusError AddMuMpcScenario1(MuMpcUseCase* self, const MuMpcMonitorPowerConfig* power_cfg) {
    EebusMonitorObject* const power_monitor = MuMpcMonitorPowerCreate(power_cfg);
    if (power_monitor == NULL) {
        return kEebusErrorInit;
    }

    EebusMonitorContainerAdd(&self->monitor_container, power_monitor);

    static const FeatureTypeType use_case_scenario_support_1_features[] = {
        kFeatureTypeTypeElectricalConnection,
        kFeatureTypeTypeMeasurement,
    };

    self->mu_mpc_scenarios[self->mu_mpc_scenarios_size++] = (UseCaseScenario){
        .scenario             = (UseCaseScenarioSupportType)1,
        .mandatory            = true,
        .server_features      = use_case_scenario_support_1_features,
        .server_features_size = ARRAY_SIZE(use_case_scenario_support_1_features),
    };

    return kEebusErrorOk;
}

EebusError AddMuMpcScenario2(MuMpcUseCase* self, const MuMpcMonitorEnergyConfig* energy_cfg) {
    EebusMonitorObject* const energy_monitor = MuMpcMonitorEnergyCreate(energy_cfg);
    if (energy_monitor == NULL) {
        return kEebusErrorInit;
    }

    EebusMonitorContainerAdd(&self->monitor_container, energy_monitor);

    static const FeatureTypeType use_case_scenario_support_2_features[] = {
        kFeatureTypeTypeElectricalConnection,
        kFeatureTypeTypeMeasurement,
    };

    self->mu_mpc_scenarios[self->mu_mpc_scenarios_size++] = (UseCaseScenario){
        .scenario             = (UseCaseScenarioSupportType)2,
        .mandatory            = false,
        .server_features      = use_case_scenario_support_2_features,
        .server_features_size = ARRAY_SIZE(use_case_scenario_support_2_features),
    };

    return kEebusErrorOk;
}

EebusError AddMuMpcScenario3(MuMpcUseCase* self, const MuMpcMonitorCurrentConfig* current_cfg) {
    EebusMonitorObject* const current_monitor = MuMpcMonitorCurrentCreate(current_cfg);
    if (current_monitor == NULL) {
        return kEebusErrorInit;
    }

    EebusMonitorContainerAdd(&self->monitor_container, current_monitor);

    static const FeatureTypeType use_case_scenario_support_3_features[] = {
        kFeatureTypeTypeElectricalConnection,
        kFeatureTypeTypeMeasurement,
    };

    self->mu_mpc_scenarios[self->mu_mpc_scenarios_size++] = (UseCaseScenario){
        .scenario             = (UseCaseScenarioSupportType)3,
        .mandatory            = false,
        .server_features      = use_case_scenario_support_3_features,
        .server_features_size = ARRAY_SIZE(use_case_scenario_support_3_features),
    };

    return kEebusErrorOk;
}

EebusError AddMuMpcScenario4(MuMpcUseCase* self, const MuMpcMonitorVoltageConfig* voltage_cfg) {
    EebusMonitorObject* const voltage_monitor = MuMpcMonitorVoltageCreate(voltage_cfg);
    if (voltage_monitor == NULL) {
        return kEebusErrorInit;
    }

    EebusMonitorContainerAdd(&self->monitor_container, voltage_monitor);

    static const FeatureTypeType use_case_scenario_support_4_features[] = {
        kFeatureTypeTypeElectricalConnection,
        kFeatureTypeTypeMeasurement,
    };

    self->mu_mpc_scenarios[self->mu_mpc_scenarios_size++] = (UseCaseScenario){
        .scenario             = (UseCaseScenarioSupportType)4,
        .mandatory            = false,
        .server_features      = use_case_scenario_support_4_features,
        .server_features_size = ARRAY_SIZE(use_case_scenario_support_4_features),
    };

    return kEebusErrorOk;
}

EebusError AddMuMpcScenario5(MuMpcUseCase* self, const MuMpcMonitorFrequencyConfig* frequency_cfg) {
    EebusMonitorObject* const frequency_monitor = MuMpcMonitorFrequencyCreate(frequency_cfg);
    if (frequency_monitor == NULL) {
        return kEebusErrorInit;
    }

    EebusMonitorContainerAdd(&self->monitor_container, frequency_monitor);

    static const FeatureTypeType use_case_scenario_support_5_features[] = {
        kFeatureTypeTypeElectricalConnection,
        kFeatureTypeTypeMeasurement,
    };

    self->mu_mpc_scenarios[self->mu_mpc_scenarios_size++] = (UseCaseScenario){
        .scenario             = (UseCaseScenarioSupportType)5,
        .mandatory            = false,
        .server_features      = use_case_scenario_support_5_features,
        .server_features_size = ARRAY_SIZE(use_case_scenario_support_5_features),
    };

    return kEebusErrorOk;
}

EebusError MuMpcUseCaseConstruct(
    MuMpcUseCase* self,
    EntityLocalObject* local_entity,
    ElectricalConnectionIdType ec_id,
    const MuMpcConfig* cfg,
    MuMpcListenerObject* listener
) {
    // Override "virtual functions table"
    USE_CASE_INTERFACE(self) = &mu_mpc_use_case_methods;

    self->electrical_connection_id = ec_id;
    self->mu_mpc_listener          = listener;
    self->mu_mpc_scenarios_size    = 0;

    const EebusError container_err = EebusMonitorContainerConstruct(&self->monitor_container);
    if (container_err != kEebusErrorOk) {
        return container_err;
    }

    const EebusError add_scenario1_err = AddMuMpcScenario1(self, &cfg->power_cfg);
    if (add_scenario1_err != kEebusErrorOk) {
        return add_scenario1_err;
    }

    if (cfg->energy_cfg != NULL) {
        const EebusError add_scenario2_err = AddMuMpcScenario2(self, cfg->energy_cfg);
        if (add_scenario2_err != kEebusErrorOk) {
            return add_scenario2_err;
        }
    }

    if (cfg->current_cfg != NULL) {
        const EebusError add_scenario3_err = AddMuMpcScenario3(self, cfg->current_cfg);
        if (add_scenario3_err != kEebusErrorOk) {
            return add_scenario3_err;
        }
    }

    if (cfg->voltage_cfg != NULL) {
        const EebusError add_scenario4_err = AddMuMpcScenario4(self, cfg->voltage_cfg);
        if (add_scenario4_err != kEebusErrorOk) {
            return add_scenario4_err;
        }
    }

    if (cfg->frequency_cfg != NULL) {
        const EebusError add_scenario5_err = AddMuMpcScenario5(self, cfg->frequency_cfg);
        if (add_scenario5_err != kEebusErrorOk) {
            return add_scenario5_err;
        }
    }

    self->mu_mpc_use_case_info = (UseCaseInfo){
        .valid_actor_types       = valid_actor_types,
        .valid_actor_types_size  = ARRAY_SIZE(valid_actor_types),
        .valid_entity_types      = NULL,
        .valid_entity_types_size = 0,
        .use_case_scenarios      = self->mu_mpc_scenarios,
        .use_case_scenarios_size = self->mu_mpc_scenarios_size,
        .actor                   = kUseCaseActorTypeMonitoredUnit,
        .use_case_name_id        = kUseCaseNameTypeMonitoringOfPowerConsumption,
        .version                 = "1.0.0",
        .sub_revision            = "release",
        .available               = true,
    };

    UseCaseConstruct(USE_CASE(self), &self->mu_mpc_use_case_info, local_entity, MuMpcHandleEvent);

    return EebusMonitorFeaturesSetup(&self->monitor_container, local_entity, self->electrical_connection_id);
}

MuMpcUseCaseObject* MuMpcUseCaseCreate(
    EntityLocalObject* local_entity,
    ElectricalConnectionIdType ec_id,
    const MuMpcConfig* cfg,
    MuMpcListenerObject* listener
) {
    if (cfg == NULL) {
        return NULL;
    }

    MuMpcUseCase* const mu_mpc_use_case = EEBUS_MALLOC(sizeof(MuMpcUseCase));
    if (mu_mpc_use_case == NULL) {
        return NULL;
    }

    if (MuMpcUseCaseConstruct(mu_mpc_use_case, local_entity, ec_id, cfg, listener) != kEebusErrorOk) {
        MuMpcUseCaseDelete(MU_MPC_USE_CASE_OBJECT(mu_mpc_use_case));
        return NULL;
    }

    return MU_MPC_USE_CASE_OBJECT(mu_mpc_use_case);
}

void Destruct(UseCaseObject* self) {
    MuMpcUseCase* const mu_mpc_use_case = MU_MPC_USE_CASE(self);

    EebusMonitorContainerDestruct(&mu_mpc_use_case->monitor_container);

    UseCaseDestruct(self);
}

bool IsEntityCompatible(const UseCaseObject* self, const EntityRemoteObject* remote_entity) {
    UNUSED(self);
    UNUSED(remote_entity);
    return true;  // For now, all entities are compatible with this use case
}
