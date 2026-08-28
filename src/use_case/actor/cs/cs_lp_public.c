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
 * @brief CS LPC public functions
 */

#include "src/common/array_util.h"
#include "src/common/eebus_data/eebus_data_list.h"
#include "src/common/eebus_errors.h"
#include "src/spine/model/loadcontrol_types.h"
#include "src/spine/model/model.h"
#include "src/use_case/actor/cs/cs_lp.h"
#include "src/use_case/actor/cs/cs_lp_internal.h"
#include "src/use_case/model/load_limit_types.h"
#include "src/use_case/specialization/device_configuration/device_configuration_server.h"
#include "src/use_case/specialization/electrical_connection/electrical_connection_server.h"
#include "src/use_case/specialization/load_control/load_control_common.h"
#include "src/use_case/specialization/load_control/load_control_server.h"

//-------------------------------------------------------------------------------------------//
//
// Scenario 1
//
//-------------------------------------------------------------------------------------------//
EebusError
CsLpGetLimitId(const CsLpUseCase* self, LoadControlServer* load_control_server, LoadControlLimitIdType* limit_id) {
    if ((load_control_server == NULL) || (limit_id == NULL)) {
        return kEebusErrorInputArgumentNull;
    }

    const LoadControlLimitDescriptionDataType filter = {
        .limit_type      = &(LoadControlLimitTypeType){kLoadControlLimitTypeTypeSignDependentAbsValueLimit},
        .limit_category  = &(LoadControlCategoryType){kLoadControlCategoryTypeObligation},
        .limit_direction = &self->energy_direction,
        .scope_type      = &(ScopeTypeType){kScopeTypeTypeActivePowerLimit},
    };

    const LoadControlLimitDescriptionDataType* const description
        = LoadControlCommonGetLimitDescriptionWithFilter(&load_control_server->load_control_common, &filter);
    if (description == NULL) {
        return kEebusErrorNoChange;
    }

    *limit_id = *description->limit_id;
    return kEebusErrorOk;
}

EebusError CsLpGetActivePowerLimitInternal(const CsLpUseCase* self, LoadLimit* limit) {
    UseCase* const use_case = USE_CASE(self);

    LoadControlServer lcs;
    const EebusError lcs_construct_err = LoadControlServerConstruct(&lcs, use_case->local_entity);
    if (lcs_construct_err != kEebusErrorOk) {
        return lcs_construct_err;
    }

    LoadControlLimitIdType limit_id;
    const EebusError limid_id_err = CsLpGetLimitId(self, &lcs, &limit_id);
    if (limid_id_err != kEebusErrorOk) {
        return limid_id_err;
    }

    const LoadControlLimitDataType* const limit_data
        = LoadControlCommonGetLimitWithId(&lcs.load_control_common, limit_id);

    return LoadLimitInitWithLoadControlLimitData(limit, limit_data);
}

EebusError CsLpGetActivePowerLimit(const CsLpUseCaseObject* self, LoadLimit* limit) {
    UseCase* const use_case = USE_CASE(self);

    DEVICE_LOCAL_LOCK(use_case->local_device);
    const EebusError ret = CsLpGetActivePowerLimitInternal(CS_LP_USE_CASE(self), limit);
    DEVICE_LOCAL_UNLOCK(use_case->local_device);
    return ret;
}

EebusError
CsLpSetActivePowerLimitInternal(CsLpUseCase* self, const ScaledValue* limit, bool is_active, bool is_changeable) {
    UseCase* const use_case = USE_CASE(self);

    LoadControlServer lcs;
    const EebusError lcs_construct_err = LoadControlServerConstruct(&lcs, use_case->local_entity);
    if (lcs_construct_err != kEebusErrorOk) {
        return lcs_construct_err;
    }

    LoadControlLimitIdType limit_id;
    const EebusError limid_id_err = CsLpGetLimitId(self, &lcs, &limit_id);
    if (limid_id_err != kEebusErrorOk) {
        return limid_id_err;
    }

    // TODO: Add duration handling
    const LoadControlLimitDataType limit_data = {
        .is_limit_changeable = &(bool){is_changeable},
        .is_limit_active     = &(bool){is_active},
        .value               = &(ScaledNumberType){.number = &limit->value, .scale = &limit->scale},
    };

    const LoadControlLimitDescriptionDataType filter = {
        .limit_id = &limit_id,
    };

    const LoadControlLimitListDataSelectorsType delete_selectors = {
        .limit_id = &limit_id,
    };

    return LoadControlServerUpdateLimitWithFilter(&lcs, &limit_data, &filter, (void*)&delete_selectors, NULL);
}

EebusError
CsLpSetActivePowerLimit(CsLpUseCaseObject* self, const ScaledValue* limit, bool is_active, bool is_changeable) {
    UseCase* const use_case = USE_CASE(self);

    DEVICE_LOCAL_LOCK(use_case->local_device);
    const EebusError ret = CsLpSetActivePowerLimitInternal(CS_LP_USE_CASE(self), limit, is_active, is_changeable);
    DEVICE_LOCAL_UNLOCK(use_case->local_device);
    return ret;
}

// TODO: add pending requests handling API

//-------------------------------------------------------------------------------------------//
//
// Scenario 2
//
//-------------------------------------------------------------------------------------------//
EebusError
CsLpGetFailsafeActivePowerLimitInternal(const CsLpUseCase* self, ScaledValue* power_limit, bool* is_changeable) {
    UseCase* const use_case = USE_CASE(self);

    if ((power_limit == NULL) || (is_changeable == NULL)) {
        return kEebusErrorInputArgumentNull;
    }

    DeviceConfigurationServer dc      = {0};
    const EebusError dc_construct_err = DeviceConfigurationServerConstruct(&dc, use_case->local_entity);
    if (dc_construct_err != kEebusErrorOk) {
        return dc_construct_err;
    }

    DeviceConfigurationKeyValueDescriptionDataType filter = {
        .key_name = &self->failsafe_power_limit_key,
    };

    const DeviceConfigurationKeyValueDataType* const key_data
        = DeviceConfigurationCommonGetKeyValueWithFilter(&dc.device_cfg_common, &filter);
    if (!DeviceConfigurationKeyValueIsValid(key_data)) {
        return kEebusErrorOther;
    }

    const ScaledNumberType* const scaled_number = DeviceConfigurationKeyValueGetScaledNumber(key_data);

    EebusError err = ScaledValueInitWithScaledNumber(power_limit, scaled_number);
    if (err != kEebusErrorOk) {
        return err;
    }

    *is_changeable = DeviceConfigurationKeyValueIsChangeable(key_data);
    return kEebusErrorOk;
}

EebusError
CsLpGetFailsafeActivePowerLimit(const CsLpUseCaseObject* self, ScaledValue* power_limit, bool* is_changeable) {
    UseCase* const use_case = USE_CASE(self);

    DEVICE_LOCAL_LOCK(use_case->local_device);
    const EebusError ret = CsLpGetFailsafeActivePowerLimitInternal(CS_LP_USE_CASE(self), power_limit, is_changeable);
    DEVICE_LOCAL_UNLOCK(use_case->local_device);
    return ret;
}

EebusError
CsLpSetFailsafeActivePowerLimitInternal(CsLpUseCase* self, const ScaledValue* power_limit, bool is_changeable) {
    UseCase* const use_case = USE_CASE(self);

    DeviceConfigurationServer dc      = {0};
    const EebusError dc_construct_err = DeviceConfigurationServerConstruct(&dc, use_case->local_entity);
    if (dc_construct_err != kEebusErrorOk) {
        return dc_construct_err;
    }

    const DeviceConfigurationKeyValueDataType data = {
      .value = &(DeviceConfigurationKeyValueValueType){
          .scaled_number = &(ScaledNumberType){
              .number = &(int64_t){power_limit->value},
              .scale  = &(int8_t){power_limit->scale},
          },
      },

      .is_value_changeable = &is_changeable,
  };

    const DeviceConfigurationKeyValueDescriptionDataType filter = {
        .key_name = &self->failsafe_power_limit_key,
    };

    return DeviceConfigurationServerUpdateKeyValueWithFilter(&dc, &data, NULL, &filter);
}

EebusError
CsLpSetFailsafeActivePowerLimit(CsLpUseCaseObject* self, const ScaledValue* power_limit, bool is_changeable) {
    UseCase* const use_case = USE_CASE(self);

    DEVICE_LOCAL_LOCK(use_case->local_device);
    const EebusError ret = CsLpSetFailsafeActivePowerLimitInternal(CS_LP_USE_CASE(self), power_limit, is_changeable);
    DEVICE_LOCAL_UNLOCK(use_case->local_device);
    return ret;
}

EebusError
CsLpGetFailsafeDurationMinimumInternal(const CsLpUseCase* self, DurationType* duration, bool* is_changeable) {
    UseCase* const use_case = USE_CASE(self);

    if ((duration == NULL) || (is_changeable == NULL)) {
        return kEebusErrorInputArgumentNull;
    }

    DeviceConfigurationServer dc      = {0};
    const EebusError dc_construct_err = DeviceConfigurationServerConstruct(&dc, use_case->local_entity);
    if (dc_construct_err != kEebusErrorOk) {
        return dc_construct_err;
    }

    DeviceConfigurationKeyValueDescriptionDataType filter = {
        .key_name = &(DeviceConfigurationKeyNameType){kDeviceConfigurationKeyNameTypeFailsafeDurationMinimum},
    };

    const DeviceConfigurationKeyValueDataType* const key_data
        = DeviceConfigurationCommonGetKeyValueWithFilter(&dc.device_cfg_common, &filter);
    if (!DeviceConfigurationKeyValueIsValid(key_data) || (key_data->value->duration == NULL)) {
        return kEebusErrorOther;
    }

    if (DeviceConfigurationKeyValueGetDuration(key_data, duration) != kEebusErrorOk) {
        return kEebusErrorOther;
    }

    *is_changeable = DeviceConfigurationKeyValueIsChangeable(key_data);
    return kEebusErrorOk;
}

EebusError CsLpGetFailsafeDurationMinimum(const CsLpUseCaseObject* self, DurationType* duration, bool* is_changeable) {
    UseCase* const use_case = USE_CASE(self);

    DEVICE_LOCAL_LOCK(use_case->local_device);
    const EebusError ret = CsLpGetFailsafeDurationMinimumInternal(CS_LP_USE_CASE(self), duration, is_changeable);
    DEVICE_LOCAL_UNLOCK(use_case->local_device);
    return ret;
}

EebusError CsLpSetFailsafeDurationMinimumInternal(CsLpUseCase* self, const DurationType* duration, bool is_changeable) {
    UseCase* const use_case = USE_CASE(self);

    // TODO: Add duration range check

    if (duration == NULL) {
        return kEebusErrorInputArgumentNull;
    }

    DeviceConfigurationServer dc      = {0};
    const EebusError dc_construct_err = DeviceConfigurationServerConstruct(&dc, use_case->local_entity);
    if (dc_construct_err != kEebusErrorOk) {
        return dc_construct_err;
    }

    const DeviceConfigurationKeyValueDataType data = {
    .value = &(DeviceConfigurationKeyValueValueType){
      .duration = (DurationType*)duration,
    },

    .is_value_changeable = &is_changeable,
  };

    const DeviceConfigurationKeyValueDescriptionDataType filter = {
        .key_name = &(DeviceConfigurationKeyNameType){kDeviceConfigurationKeyNameTypeFailsafeDurationMinimum},
    };

    return DeviceConfigurationServerUpdateKeyValueWithFilter(&dc, &data, NULL, &filter);
}

EebusError CsLpSetFailsafeDurationMinimum(CsLpUseCaseObject* self, const DurationType* duration, bool is_changeable) {
    UseCase* const use_case = USE_CASE(self);

    DEVICE_LOCAL_LOCK(use_case->local_device);
    const EebusError ret = CsLpSetFailsafeDurationMinimumInternal(CS_LP_USE_CASE(self), duration, is_changeable);
    DEVICE_LOCAL_UNLOCK(use_case->local_device);
    return ret;
}

//-------------------------------------------------------------------------------------------//
//
// Scenario 3
//
//-------------------------------------------------------------------------------------------//
void CsLpStartHeartbeat(CsLpUseCaseObject* self) {
    UseCase* const use_case = USE_CASE(self);

    DEVICE_LOCAL_LOCK(use_case->local_device);
    HeartbeatManagerObject* const hm = ENTITY_LOCAL_GET_HEARTBEAT_MANAGER(use_case->local_entity);
    if (hm != NULL) {
        HEARTBEAT_MANAGER_START(hm);
    }

    DEVICE_LOCAL_UNLOCK(use_case->local_device);
}

void CsLpStopHeartbeat(CsLpUseCaseObject* self) {
    UseCase* const use_case = USE_CASE(self);

    DEVICE_LOCAL_LOCK(use_case->local_device);
    HeartbeatManagerObject* const hm = ENTITY_LOCAL_GET_HEARTBEAT_MANAGER(use_case->local_entity);
    if (hm != NULL) {
        HEARTBEAT_MANAGER_STOP(hm);
    }

    DEVICE_LOCAL_UNLOCK(use_case->local_device);
}

bool CsLpIsHeartbeatWithinDuration(CsLpUseCaseObject* self) {
    UseCase* const use_case = USE_CASE(self);

    DEVICE_LOCAL_LOCK(use_case->local_device);
    bool ret = false;

    const DeviceDiagnosisClient* const hdc = CS_LP_USE_CASE(self)->heartbeat_diag_client;
    if (hdc != NULL) {
        ret = DeviceDiagnosisCommonIsHeartbeatWithinDuration(&hdc->device_diag_common, &(DurationType){.minutes = 2});
    }

    DEVICE_LOCAL_UNLOCK(use_case->local_device);
    return ret;
}

//-------------------------------------------------------------------------------------------//
//
// Scenario 4
//
//-------------------------------------------------------------------------------------------//
const ElectricalConnectionCharacteristicDataType*
CsLpGetElectricalConnectionCharacteristics(const CsLpUseCase* self, const ElectricalConnectionServer* ecs) {
    ElectricalConnectionCharacteristicDataType filter = {
        .electrical_connection_id = &self->electrical_connection_id,
        .parameter_id             = &(ElectricalConnectionParameterIdType){0},
        .characteristic_context   = &kEccContextEntity,
        .characteristic_type      = &self->nominal_max_characteristic,
    };

    return ElectricalConnectionCommonGetCharacteristicWithFilter(&ecs->el_connection_common, &filter);
}

EebusError CsLpGetNominalMaxInternal(const CsLpUseCase* self, ScaledValue* nominal_max) {
    UseCase* const use_case = USE_CASE(self);

    if (nominal_max == NULL) {
        return kEebusErrorInputArgumentNull;
    }

    ElectricalConnectionServer ecs;
    const EebusError ecs_construct_err = ElectricalConnectionServerConstruct(&ecs, use_case->local_entity);
    if (ecs_construct_err != kEebusErrorOk) {
        return ecs_construct_err;
    }

    const ElectricalConnectionCharacteristicDataType* const characteristic
        = CsLpGetElectricalConnectionCharacteristics(self, &ecs);

    if ((characteristic == NULL) || (characteristic->characteristic_id == NULL) || (characteristic->value == NULL)) {
        return kEebusErrorNoChange;
    }

    return ScaledValueInitWithScaledNumber(nominal_max, characteristic->value);
}

EebusError CsLpGetNominalMax(CsLpUseCaseObject* self, ScaledValue* nominal_max) {
    UseCase* const use_case = USE_CASE(self);

    DEVICE_LOCAL_LOCK(use_case->local_device);
    const EebusError ret = CsLpGetNominalMaxInternal(CS_LP_USE_CASE(self), nominal_max);
    DEVICE_LOCAL_UNLOCK(use_case->local_device);
    return ret;
}

EebusError CsLpSetNominalMaxInternal(CsLpUseCase* self, const ScaledValue* new_nominal_max) {
    UseCase* const use_case = USE_CASE(self);

    if (new_nominal_max == NULL) {
        return kEebusErrorInputArgumentNull;
    }

    ElectricalConnectionServer ecs;
    const EebusError ecs_construct_err = ElectricalConnectionServerConstruct(&ecs, use_case->local_entity);
    if (ecs_construct_err != kEebusErrorOk) {
        return ecs_construct_err;
    }

    const ElectricalConnectionCharacteristicDataType* const characteristic
        = CsLpGetElectricalConnectionCharacteristics(self, &ecs);

    if ((characteristic == NULL) || (characteristic->characteristic_id == NULL)) {
        return kEebusErrorNoChange;
    }

    // clang-format off
  ElectricalConnectionCharacteristicDataType new_characteristic = {
      .electrical_connection_id = &self->electrical_connection_id,
      .parameter_id             = &(ElectricalConnectionParameterIdType){0                                                                         },
      .characteristic_id        = characteristic->characteristic_id,

      .value = &(ScaledNumberType){
          .number = &(int64_t){new_nominal_max->value},
          .scale  = &(int8_t){new_nominal_max->scale},
      },
  };
    // clang-format on

    return ElectricalConnectionServerUpdateCharacteristic(&ecs, &new_characteristic, NULL);
}

EebusError CsLpSetNominalMax(CsLpUseCaseObject* self, const ScaledValue* new_nominal_max) {
    UseCase* const use_case = USE_CASE(self);

    DEVICE_LOCAL_LOCK(use_case->local_device);
    const EebusError ret = CsLpSetNominalMaxInternal(CS_LP_USE_CASE(self), new_nominal_max);
    DEVICE_LOCAL_UNLOCK(use_case->local_device);
    return ret;
}

ElectricalConnectionCharacteristicTypeType CsLpGetElectricalConnectionCharacteristicType(
    const CsLpUseCaseObject* self
) {
    return CS_LP_USE_CASE(self)->nominal_max_characteristic;
}
