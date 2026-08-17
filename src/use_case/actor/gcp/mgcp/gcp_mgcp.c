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
 * @brief Grid Connection Point MGCP use case implementation
 */

#include "src/use_case/actor/gcp/mgcp/gcp_mgcp.h"

#include "src/common/array_util.h"
#include "src/common/eebus_arguments.h"
#include "src/use_case/actor/common/eebus_monitor_features.h"
#include "src/use_case/actor/gcp/mgcp/gcp_mgcp_internal.h"
#include "src/use_case/actor/gcp/mgcp/gcp_mgcp_monitor.h"
#include "src/use_case/specialization/device_configuration/device_configuration_server.h"
#include "src/use_case/use_case.h"

/* Only the MA (Monitoring Appliance) may connect to this use case */
static const UseCaseActorType valid_actor_types[] = {kUseCaseActorTypeMonitoringAppliance};

static void Destruct(UseCaseObject* self);
static bool IsEntityCompatible(const UseCaseObject* self, const EntityRemoteObject* remote_entity);

static const UseCaseInterface gcp_mgcp_use_case_methods = {
    .destruct                       = Destruct,
    .is_entity_compatible           = IsEntityCompatible,
    .is_use_case_compatible         = UseCaseIsUseCaseCompatible,
    .get_remote_entity_with_address = UseCaseGetRemoteEntityWithAddress,
};

static EebusError AddGcpMgcpScenario1(GcpMgcpUseCase* self) {
  self->has_scenario1 = true;

  static const FeatureTypeType scenario1_features[] = {kFeatureTypeTypeDeviceConfiguration};

  self->gcp_mgcp_scenarios[self->gcp_mgcp_scenarios_size++] = (UseCaseScenario){
      .scenario             = (UseCaseScenarioSupportType)1,
      .mandatory            = false,
      .server_features      = scenario1_features,
      .server_features_size = ARRAY_SIZE(scenario1_features),
  };

  return kEebusErrorOk;
}

static EebusError AddGcpMgcpScenario2(GcpMgcpUseCase* self, const GcpMgcpMonitorPowerConfig* power_cfg) {
  EebusMonitorObject* const power_monitor = GcpMgcpMonitorPowerCreate(power_cfg);
  if (power_monitor == NULL) {
    return kEebusErrorInit;
  }

  EebusMonitorContainerAdd(&self->monitor_container, power_monitor);

  static const FeatureTypeType use_case_scenario_2_features[] = {
      kFeatureTypeTypeElectricalConnection,
      kFeatureTypeTypeMeasurement,
  };

  self->gcp_mgcp_scenarios[self->gcp_mgcp_scenarios_size++] = (UseCaseScenario){
      .scenario             = (UseCaseScenarioSupportType)2,
      .mandatory            = true,
      .server_features      = use_case_scenario_2_features,
      .server_features_size = ARRAY_SIZE(use_case_scenario_2_features),
  };

  return kEebusErrorOk;
}

static EebusError AddGcpMgcpEnergyScenarios(GcpMgcpUseCase* self, const GcpMgcpMonitorEnergyConfig* energy_cfg) {
  EebusMonitorObject* const energy_monitor = GcpMgcpMonitorEnergyCreate(energy_cfg);
  if (energy_monitor == NULL) {
    return kEebusErrorInit;
  }

  EebusMonitorContainerAdd(&self->monitor_container, energy_monitor);

  static const FeatureTypeType energy_features[] = {
      kFeatureTypeTypeElectricalConnection,
      kFeatureTypeTypeMeasurement,
  };

  if (energy_cfg->energy_feed_in_cfg != NULL) {
    self->gcp_mgcp_scenarios[self->gcp_mgcp_scenarios_size++] = (UseCaseScenario){
        .scenario             = (UseCaseScenarioSupportType)3,
        .mandatory            = false,
        .server_features      = energy_features,
        .server_features_size = ARRAY_SIZE(energy_features),
    };
  }

  if (energy_cfg->energy_consumed_cfg != NULL) {
    self->gcp_mgcp_scenarios[self->gcp_mgcp_scenarios_size++] = (UseCaseScenario){
        .scenario             = (UseCaseScenarioSupportType)4,
        .mandatory            = false,
        .server_features      = energy_features,
        .server_features_size = ARRAY_SIZE(energy_features),
    };
  }

  return kEebusErrorOk;
}

static EebusError AddGcpMgcpScenario5(GcpMgcpUseCase* self, const GcpMgcpMonitorCurrentConfig* current_cfg) {
  EebusMonitorObject* const current_monitor = GcpMgcpMonitorCurrentCreate(current_cfg);
  if (current_monitor == NULL) {
    return kEebusErrorInit;
  }

  EebusMonitorContainerAdd(&self->monitor_container, current_monitor);

  static const FeatureTypeType use_case_scenario_5_features[] = {
      kFeatureTypeTypeElectricalConnection,
      kFeatureTypeTypeMeasurement,
  };

  self->gcp_mgcp_scenarios[self->gcp_mgcp_scenarios_size++] = (UseCaseScenario){
      .scenario             = (UseCaseScenarioSupportType)5,
      .mandatory            = false,
      .server_features      = use_case_scenario_5_features,
      .server_features_size = ARRAY_SIZE(use_case_scenario_5_features),
  };

  return kEebusErrorOk;
}

static EebusError AddGcpMgcpScenario6(GcpMgcpUseCase* self, const GcpMgcpMonitorVoltageConfig* voltage_cfg) {
  EebusMonitorObject* const voltage_monitor = GcpMgcpMonitorVoltageCreate(voltage_cfg);
  if (voltage_monitor == NULL) {
    return kEebusErrorInit;
  }

  EebusMonitorContainerAdd(&self->monitor_container, voltage_monitor);

  static const FeatureTypeType use_case_scenario_6_features[] = {
      kFeatureTypeTypeElectricalConnection,
      kFeatureTypeTypeMeasurement,
  };

  self->gcp_mgcp_scenarios[self->gcp_mgcp_scenarios_size++] = (UseCaseScenario){
      .scenario             = (UseCaseScenarioSupportType)6,
      .mandatory            = false,
      .server_features      = use_case_scenario_6_features,
      .server_features_size = ARRAY_SIZE(use_case_scenario_6_features),
  };

  return kEebusErrorOk;
}

static EebusError AddGcpMgcpScenario7(GcpMgcpUseCase* self, const GcpMgcpMonitorFrequencyConfig* frequency_cfg) {
  EebusMonitorObject* const frequency_monitor = GcpMgcpMonitorFrequencyCreate(frequency_cfg);
  if (frequency_monitor == NULL) {
    return kEebusErrorInit;
  }

  EebusMonitorContainerAdd(&self->monitor_container, frequency_monitor);

  static const FeatureTypeType use_case_scenario_7_features[] = {
      kFeatureTypeTypeElectricalConnection,
      kFeatureTypeTypeMeasurement,
  };

  self->gcp_mgcp_scenarios[self->gcp_mgcp_scenarios_size++] = (UseCaseScenario){
      .scenario             = (UseCaseScenarioSupportType)7,
      .mandatory            = false,
      .server_features      = use_case_scenario_7_features,
      .server_features_size = ARRAY_SIZE(use_case_scenario_7_features),
  };

  return kEebusErrorOk;
}

static EebusError AddDeviceConfigurationFeature(EntityLocalObject* entity) {
  FeatureLocalObject* const fl
      = ENTITY_LOCAL_ADD_FEATURE_WITH_TYPE_AND_ROLE(entity, kFeatureTypeTypeDeviceConfiguration, kRoleTypeServer);
  if (fl == NULL) {
    return kEebusErrorInit;
  }

  FEATURE_LOCAL_SET_FUNCTION_OPERATIONS(fl, kFunctionTypeDeviceConfigurationKeyValueDescriptionListData, true, false);
  FEATURE_LOCAL_SET_FUNCTION_OPERATIONS(fl, kFunctionTypeDeviceConfigurationKeyValueListData, true, false);

  DeviceConfigurationServer dcs;
  if (DeviceConfigurationServerConstruct(&dcs, entity) != kEebusErrorOk) {
    return kEebusErrorInit;
  }

  const DeviceConfigurationKeyValueDescriptionDataType description = {
      .key_name   = &(DeviceConfigurationKeyNameType){kDeviceConfigurationKeyNameTypePvCurtailmentLimitFactor},
      .value_type = &(DeviceConfigurationKeyValueTypeType){kDeviceConfigurationKeyValueTypeTypeScaledNumber},
      .unit       = &(UnitOfMeasurementType){kUnitOfMeasurementTypepct},
  };

  return DeviceConfigurationServerAddKeyValueDescription(&dcs, &description);
}

static EebusError GcpMgcpUseCaseConstruct(
    GcpMgcpUseCase* self,
    EntityLocalObject* local_entity,
    ElectricalConnectionIdType ec_id,
    const GcpMgcpConfig* cfg
) {
  USE_CASE_INTERFACE(self) = &gcp_mgcp_use_case_methods;

  self->electrical_connection_id = ec_id;
  self->gcp_mgcp_scenarios_size  = 0;
  self->has_scenario1            = false;

  const EebusError container_err = EebusMonitorContainerConstruct(&self->monitor_container);
  if (container_err != kEebusErrorOk) {
    return container_err;
  }

  // Scenario 1: PV curtailment limit factor (optional)
  if (cfg->pv_curtailment_cfg != NULL) {
    const EebusError err = AddGcpMgcpScenario1(self);
    if (err != kEebusErrorOk) {
      return err;
    }
  }

  // Scenario 2: power total (mandatory)
  EebusError err = AddGcpMgcpScenario2(self, &cfg->power_cfg);
  if (err != kEebusErrorOk) {
    return err;
  }

  // Scenarios 3 and/or 4: energy (optional)
  if (cfg->energy_cfg != NULL) {
    err = AddGcpMgcpEnergyScenarios(self, cfg->energy_cfg);
    if (err != kEebusErrorOk) {
      return err;
    }
  }

  // Scenario 5: per-phase current (optional)
  if (cfg->current_cfg != NULL) {
    err = AddGcpMgcpScenario5(self, cfg->current_cfg);
    if (err != kEebusErrorOk) {
      return err;
    }
  }

  // Scenario 6: per-phase voltage (optional)
  if (cfg->voltage_cfg != NULL) {
    err = AddGcpMgcpScenario6(self, cfg->voltage_cfg);
    if (err != kEebusErrorOk) {
      return err;
    }
  }

  // Scenario 7: frequency (optional)
  if (cfg->frequency_cfg != NULL) {
    err = AddGcpMgcpScenario7(self, cfg->frequency_cfg);
    if (err != kEebusErrorOk) {
      return err;
    }
  }

  self->gcp_mgcp_use_case_info = (UseCaseInfo){
      .valid_actor_types       = valid_actor_types,
      .valid_actor_types_size  = ARRAY_SIZE(valid_actor_types),
      .valid_entity_types      = NULL,
      .valid_entity_types_size = 0,
      .use_case_scenarios      = self->gcp_mgcp_scenarios,
      .use_case_scenarios_size = self->gcp_mgcp_scenarios_size,
      .actor                   = kUseCaseActorTypeGridConnectionPoint,
      .use_case_name_id        = kUseCaseNameTypeMonitoringOfGridConnectionPoint,
      .version                 = "1.0.0",
      .sub_revision            = "release",
      .available               = true,
  };

  UseCaseConstruct(USE_CASE(self), &self->gcp_mgcp_use_case_info, local_entity, NULL);

  const EebusError features_err
      = EebusMonitorFeaturesSetup(&self->monitor_container, local_entity, self->electrical_connection_id);
  if (features_err != kEebusErrorOk) {
    return features_err;
  }

  if (self->has_scenario1) {
    return AddDeviceConfigurationFeature(local_entity);
  }

  return kEebusErrorOk;
}

GcpMgcpUseCaseObject*
GcpMgcpUseCaseCreate(EntityLocalObject* local_entity, ElectricalConnectionIdType ec_id, const GcpMgcpConfig* cfg) {
  if (cfg == NULL) {
    return NULL;
  }

  GcpMgcpUseCase* const gcp_mgcp_use_case = (GcpMgcpUseCase*)EEBUS_MALLOC(sizeof(GcpMgcpUseCase));
  if (gcp_mgcp_use_case == NULL) {
    return NULL;
  }

  if (GcpMgcpUseCaseConstruct(gcp_mgcp_use_case, local_entity, ec_id, cfg) != kEebusErrorOk) {
    GcpMgcpUseCaseDelete(GCP_MGCP_USE_CASE_OBJECT(gcp_mgcp_use_case));
    return NULL;
  }

  return GCP_MGCP_USE_CASE_OBJECT(gcp_mgcp_use_case);
}

static void Destruct(UseCaseObject* self) {
  GcpMgcpUseCase* const gcp_mgcp = GCP_MGCP_USE_CASE(self);

  EebusMonitorContainerDestruct(&gcp_mgcp->monitor_container);

  UseCaseDestruct(self);
}

static bool IsEntityCompatible(const UseCaseObject* self, const EntityRemoteObject* remote_entity) {
  UNUSED(self);
  UNUSED(remote_entity);

  return true;
}
