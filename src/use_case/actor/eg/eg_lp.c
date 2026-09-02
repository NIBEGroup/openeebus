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
 * @brief Energy Guard LP use case implementation
 */

#include "src/use_case/actor/eg/eg_lp.h"

#include <stddef.h>

#include "src/common/array_util.h"
#include "src/spine/entity/entity_local.h"
#include "src/spine/feature/feature_local.h"
#include "src/spine/model/loadcontrol_types.h"
#include "src/spine/model/usecase_information_types.h"
#include "src/use_case/actor/eg/eg_lp_events.h"
#include "src/use_case/actor/eg/eg_lp_internal.h"
#include "src/use_case/use_case.h"

static const UseCaseInterface lp_use_case_methods = {
    .destruct                       = UseCaseDestruct,
    .is_entity_compatible           = UseCaseIsEntityCompatible,
    .is_use_case_compatible         = UseCaseIsUseCaseCompatible,
    .get_remote_entity_with_address = UseCaseGetRemoteEntityWithAddress,
};

static EebusError AddFeatures(EntityLocalObject* entity);
static EebusError EgLpUseCaseConstruct(
    EgLpUseCase* self,
    EnergyDirectionType energy_direction,
    const UseCaseInfo* use_case_info,
    EntityLocalObject* local_entity,
    EgLpListenerObject* eg_lp_listener
);

EebusError AddFeatures(EntityLocalObject* entity) {
  // Energy Guard LP client features
  static const FeatureTypeType eg_lp_client_features[] = {
      kFeatureTypeTypeDeviceDiagnosis,
      kFeatureTypeTypeLoadControl,
      kFeatureTypeTypeDeviceConfiguration,
      kFeatureTypeTypeElectricalConnection,
  };

  for (size_t i = 0; i < ARRAY_SIZE(eg_lp_client_features); ++i) {
    ENTITY_LOCAL_ADD_FEATURE_WITH_TYPE_AND_ROLE(entity, eg_lp_client_features[i], kRoleTypeClient);
  }

  // server features
  FeatureLocalObject* const fl
      = ENTITY_LOCAL_ADD_FEATURE_WITH_TYPE_AND_ROLE(entity, kFeatureTypeTypeDeviceDiagnosis, kRoleTypeServer);
  FEATURE_LOCAL_SET_FUNCTION_OPERATIONS(fl, kFunctionTypeDeviceDiagnosisHeartbeatData, true, false);

  return kEebusErrorOk;
}

EebusError EgLpUseCaseConstruct(
    EgLpUseCase* self,
    EnergyDirectionType energy_direction,
    const UseCaseInfo* use_case_info,
    EntityLocalObject* local_entity,
    EgLpListenerObject* eg_lp_listener
) {
  UseCaseConstruct(USE_CASE(self), use_case_info, local_entity, EgLpHandleEvent);
  // Override "virtual functions table"
  USE_CASE_INTERFACE(self) = &lp_use_case_methods;

  self->energy_direction                       = energy_direction;
  self->failsafe_power_limit_key               = (DeviceConfigurationKeyNameType)0;
  self->nominal_max_characteristic             = (ElectricalConnectionCharacteristicTypeType)0;
  self->contractual_nominal_max_characteristic = (ElectricalConnectionCharacteristicTypeType)0;
  self->eg_lp_listener                         = eg_lp_listener;

  if (energy_direction == kEnergyDirectionTypeConsume) {
    self->failsafe_power_limit_key   = kDeviceConfigurationKeyNameTypeFailsafeConsumptionActivePowerLimit;
    self->nominal_max_characteristic = kElectricalConnectionCharacteristicTypeTypePowerConsumptionNominalMax;
    self->contractual_nominal_max_characteristic
        = kElectricalConnectionCharacteristicTypeTypeContractualConsumptionNominalMax;
  } else {
    self->failsafe_power_limit_key   = kDeviceConfigurationKeyNameTypeFailsafeProductionActivePowerLimit;
    self->nominal_max_characteristic = kElectricalConnectionCharacteristicTypeTypePowerProductionNominalMax;
    self->contractual_nominal_max_characteristic
        = kElectricalConnectionCharacteristicTypeTypeContractualProductionNominalMax;
  }

  return AddFeatures(local_entity);
}

EgLpUseCaseObject* EgLpUseCaseCreate(
    EnergyDirectionType energy_direction,
    const UseCaseInfo* use_case_info,
    EntityLocalObject* local_entity,
    EgLpListenerObject* eg_lp_listener
) {
  EgLpUseCase* eg_lp_use_case = EEBUS_MALLOC(sizeof(*eg_lp_use_case));
  if (eg_lp_use_case == NULL) {
    return NULL;
  }

  const EebusError err
      = EgLpUseCaseConstruct(eg_lp_use_case, energy_direction, use_case_info, local_entity, eg_lp_listener);
  if (err != kEebusErrorOk) {
    EgLpUseCaseDelete(EG_LP_USE_CASE_OBJECT(eg_lp_use_case));
    return NULL;
  }

  return EG_LP_USE_CASE_OBJECT(eg_lp_use_case);
}
