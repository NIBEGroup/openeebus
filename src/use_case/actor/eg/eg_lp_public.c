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
#include "src/use_case/model/load_limit_types.h"

#include "src/spine/model/absolute_or_relative_time.h"
#include "src/spine/model/entity_types.h"
#include "src/use_case/actor/eg/eg_lp.h"
#include "src/use_case/actor/eg/eg_lp_internal.h"
#include "src/use_case/model/load_limit_types.h"
#include "src/use_case/specialization/device_configuration/device_configuration_client.h"
#include "src/use_case/specialization/device_diagnosis/device_diagnosis_client.h"
#include "src/use_case/specialization/electrical_connection/electrical_connection_client.h"
#include "src/use_case/specialization/load_control/load_control_client.h"
#include "src/use_case/use_case.h"

static const DeviceConfigurationKeyNameType kFailsafeDurationMinimumKeyName
    = kDeviceConfigurationKeyNameTypeFailsafeDurationMinimum;

//-------------------------------------------------------------------------------------------//
//
// Scenario 1
//
//-------------------------------------------------------------------------------------------//

static const LoadControlLimitDescriptionDataType* EgLpActivePowerLimitFilter(const EgLpUseCase* self) {
    static const LoadControlLimitTypeType kLimitType = kLoadControlLimitTypeTypeSignDependentAbsValueLimit;
    static const ScopeTypeType kScopeType            = kScopeTypeTypeActivePowerLimit;
    static const EnergyDirectionType kConsume        = kEnergyDirectionTypeConsume;

    static const LoadControlLimitDescriptionDataType kConsumeFilter = {
        .limit_type      = &kLimitType,
        .limit_direction = &kConsume,
        .scope_type      = &kScopeType,
    };

    static const EnergyDirectionType kProduce = kEnergyDirectionTypeProduce;

    static const LoadControlLimitDescriptionDataType kProduceFilter = {
        .limit_type      = &kLimitType,
        .limit_direction = &kProduce,
        .scope_type      = &kScopeType,
    };

    return (self->energy_direction == kEnergyDirectionTypeConsume) ? &kConsumeFilter : &kProduceFilter;
}

EebusError EgLpGetActivePowerLimitInternal(
    const EgLpUseCase* self,
    const EntityAddressType* remote_entity_addr,
    LoadLimit* limit
) {
    const UseCase* const use_case = USE_CASE(self);

    EntityRemoteObject* const remote_entity
        = USE_CASE_GET_REMOTE_ENTITY_WITH_ADDRESS(USE_CASE_OBJECT(self), remote_entity_addr);

    if (remote_entity == NULL) {
        return kEebusErrorNoChange;
    }

    LoadControlClient lcc;
    EebusError err = LoadControlClientConstruct(&lcc, use_case->local_entity, remote_entity);
    if (err != kEebusErrorOk) {
        return err;
    }

    const LoadControlLimitDescriptionDataType* const filter = EgLpActivePowerLimitFilter(self);

    const LoadControlLimitDescriptionDataType* const limit_description
        = LoadControlCommonGetLimitDescriptionWithFilter(&lcc.load_control_common, filter);
    if ((limit_description == NULL) || (limit_description->limit_id == NULL)) {
        return kEebusErrorNoChange;
    }

    const LoadControlLimitDataType* const limit_data
        = LoadControlCommonGetLimitWithId(&lcc.load_control_common, *limit_description->limit_id);

    return LoadLimitInitWithLoadControlLimitData(limit, limit_data);
}

EebusError
EgLpGetActivePowerLimit(const EgLpUseCaseObject* self, const EntityAddressType* remote_entity_addr, LoadLimit* limit) {
    const UseCase* const use_case = USE_CASE(self);

    if ((remote_entity_addr == NULL) || (limit == NULL)) {
        return kEebusErrorInputArgumentNull;
    }

    EebusError err = kEebusErrorOk;

    DEVICE_LOCAL_LOCK(use_case->local_device);
    err = EgLpGetActivePowerLimitInternal(EG_LP_USE_CASE(self), remote_entity_addr, limit);
    DEVICE_LOCAL_UNLOCK(use_case->local_device);

    return err;
}

EebusError
EgLpReadLoadControlLimit(EgLpUseCase* self, EntityRemoteObject* remote_entity, ReplyMessageCallback cb, void* ctx) {
    const UseCase* const use_case = USE_CASE(self);

    LoadControlClient lcc;
    const EebusError err = LoadControlClientConstruct(&lcc, use_case->local_entity, remote_entity);
    if (err != kEebusErrorOk) {
        return err;
    }

    const LoadControlLimitDescriptionDataType* const filter = EgLpActivePowerLimitFilter(self);

    return LoadControlClientReadLimit(&lcc, filter, cb, ctx);
}

EebusError EgLpReadActivePowerLimitInternal(
    const EgLpUseCase* self,
    const EntityAddressType* remote_entity_addr,
    ReplyMessageCallback cb,
    void* ctx
) {
    EntityRemoteObject* const remote_entity
        = USE_CASE_GET_REMOTE_ENTITY_WITH_ADDRESS(USE_CASE_OBJECT(self), remote_entity_addr);

    if (remote_entity == NULL) {
        return kEebusErrorNoChange;
    }

    return EgLpReadLoadControlLimit((EgLpUseCase*)self, remote_entity, cb, ctx);
}

EebusError EgLpReadActivePowerLimit(
    const EgLpUseCaseObject* self,
    const EntityAddressType* remote_entity_addr,
    ReplyMessageCallback cb,
    void* ctx
) {
    const UseCase* const use_case = USE_CASE(self);

    if (remote_entity_addr == NULL) {
        return kEebusErrorInputArgumentNull;
    }

    EebusError err = kEebusErrorOk;

    DEVICE_LOCAL_LOCK(use_case->local_device);
    err = EgLpReadActivePowerLimitInternal(EG_LP_USE_CASE(self), remote_entity_addr, cb, ctx);
    DEVICE_LOCAL_UNLOCK(use_case->local_device);

    return err;
}

EebusError EgLpWriteLoadControlLimit(
    EgLpUseCase* self,
    EntityRemoteObject* remote_entity,
    const LoadLimit* limit,
    ResultMessageCallback cb,
    void* ctx
) {
    const UseCase* const use_case = USE_CASE(self);

    LoadControlClient lcc;
    const EebusError err = LoadControlClientConstruct(&lcc, use_case->local_entity, remote_entity);
    if (err != kEebusErrorOk) {
        return err;
    }

    const LoadControlLimitDescriptionDataType* const filter = EgLpActivePowerLimitFilter(self);

    const LoadControlLimitDataType* limit_data = LoadControlCommonGetLimitWithFilter(&lcc.load_control_common, filter);

    if (limit_data == NULL || limit_data->limit_id == NULL) {
        return kEebusErrorNoChange;
    }

    // EEBus_UC_TS_LimitationOfPowerConsumption V1.0.0 3.2.2.2.2.2
    // If set to "true", the timePeriod, value and isLimitActive Elements SHALL be writeable by a client.
    if ((limit_data->is_limit_changeable != NULL) && (!*limit_data->is_limit_changeable)) {
        return kEebusErrorNotSupported;
    }

    const TimePeriodType time_period = {
        .end_time = &ABSOLUTE_OR_RELATIVE_TIME_WITH_DURATION(limit->duration),
    };

    const LoadControlLimitDataType new_limit = {
        .limit_id        = limit_data->limit_id,
        .is_limit_active = &limit->is_active,
        .value           = &(ScaledNumberType){&limit->value.value, &limit->value.scale},
        .time_period     = (EebusDurationToSeconds(&limit->duration) > 0) ? &time_period : NULL,
    };

    const LoadControlLimitListDataType new_limit_list = (LoadControlLimitListDataType){
        .load_control_limit_data      = &(const LoadControlLimitDataType*){&new_limit},
        .load_control_limit_data_size = 1,
    };

    const LoadControlLimitListDataSelectorsType* delete_selectors = &(LoadControlLimitListDataSelectorsType){
        .limit_id = limit_data->limit_id,
    };

    const LoadControlLimitDataElementsType* delete_elements = &(LoadControlLimitDataElementsType){
        .time_period = &(TimePeriodElementsType){.start_time = EEBUS_TAG_RESET, .end_time = EEBUS_TAG_RESET},
    };

    // If timer period should not be deleted, reset delete_selectors and delete_elements
    if (!limit->delete_duration) {
        delete_selectors = NULL;
        delete_elements  = NULL;
    }

    return LoadControlClientWriteLimitList(&lcc, &new_limit_list, delete_selectors, delete_elements, cb, ctx);
}

EebusError EgLpSetActivePowerLimitInternal(
    EgLpUseCase* self,
    const EntityAddressType* remote_entity_addr,
    const LoadLimit* limit,
    ResultMessageCallback cb,
    void* ctx
) {
    EntityRemoteObject* const remote_entity
        = USE_CASE_GET_REMOTE_ENTITY_WITH_ADDRESS(USE_CASE_OBJECT(self), remote_entity_addr);

    if (remote_entity == NULL) {
        return kEebusErrorNoChange;
    }

    return EgLpWriteLoadControlLimit(self, remote_entity, limit, cb, ctx);
}

EebusError EgLpSetActivePowerLimit(
    EgLpUseCaseObject* self,
    const EntityAddressType* remote_entity_addr,
    const LoadLimit* limit,
    ResultMessageCallback cb,
    void* ctx
) {
    const UseCase* const use_case = USE_CASE(self);

    if ((remote_entity_addr == NULL) || (limit == NULL)) {
        return kEebusErrorInputArgumentNull;
    }

    EebusError err = kEebusErrorOk;

    DEVICE_LOCAL_LOCK(use_case->local_device);
    err = EgLpSetActivePowerLimitInternal(EG_LP_USE_CASE(self), remote_entity_addr, limit, cb, ctx);
    DEVICE_LOCAL_UNLOCK(use_case->local_device);

    return err;
}

//-------------------------------------------------------------------------------------------//
//
// Scenario 2
//
//-------------------------------------------------------------------------------------------//

EebusError EgLpGetFailsafeActivePowerLimitInternal(
    const EgLpUseCase* self,
    const EntityAddressType* remote_entity_addr,
    ScaledValue* power_limit
) {
    const UseCase* const use_case = USE_CASE(self);

    EntityRemoteObject* const remote_entity
        = USE_CASE_GET_REMOTE_ENTITY_WITH_ADDRESS(USE_CASE_OBJECT(self), remote_entity_addr);

    if (remote_entity == NULL) {
        return kEebusErrorNoChange;
    }

    DeviceConfigurationClient dcc;
    EebusError err = DeviceConfigurationClientConstruct(&dcc, use_case->local_entity, remote_entity);
    if (err != kEebusErrorOk) {
        return err;
    }

    const DeviceConfigurationKeyValueDescriptionDataType filter = {
        .key_name   = &self->failsafe_power_limit_key,
        .value_type = &(DeviceConfigurationKeyValueTypeType){kDeviceConfigurationKeyValueTypeTypeScaledNumber},
    };

    const DeviceConfigurationKeyValueDataType* const key_value
        = DeviceConfigurationCommonGetKeyValueWithFilter(&dcc.device_cfg_common, &filter);

    const ScaledNumberType* const scaled_number = DeviceConfigurationKeyValueGetScaledNumber(key_value);
    return ScaledValueInitWithScaledNumber(power_limit, scaled_number);
}

EebusError EgLpGetFailsafeActivePowerLimit(
    const EgLpUseCaseObject* self,
    const EntityAddressType* remote_entity_addr,
    ScaledValue* power_limit
) {
    const UseCase* const use_case = USE_CASE(self);

    if ((remote_entity_addr == NULL) || (power_limit == NULL)) {
        return kEebusErrorInputArgumentNull;
    }

    EebusError err = kEebusErrorOk;

    DEVICE_LOCAL_LOCK(use_case->local_device);
    err = EgLpGetFailsafeActivePowerLimitInternal(EG_LP_USE_CASE(self), remote_entity_addr, power_limit);
    DEVICE_LOCAL_UNLOCK(use_case->local_device);

    return err;
}

EebusError EgLpReadDeviceConfigurationWithKeyName(
    const EgLpUseCase* self,
    EntityRemoteObject* remote_entity,
    DeviceConfigurationKeyNameType key_name,
    ReplyMessageCallback cb,
    void* ctx
) {
    const UseCase* const use_case = USE_CASE(self);

    DeviceConfigurationClient dcc;
    const EebusError err = DeviceConfigurationClientConstruct(&dcc, use_case->local_entity, remote_entity);
    if (err != kEebusErrorOk) {
        return err;
    }

    const DeviceConfigurationKeyValueDescriptionDataType filter = {
        .key_name = &key_name,
    };

    const DeviceConfigurationKeyValueDescriptionDataType* const description
        = DeviceConfigurationCommonGetKeyValueDescriptionWithFilter(&dcc.device_cfg_common, &filter);

    if ((description == NULL) || (description->key_id == NULL)) {
        return kEebusErrorNotAvailable;
    }

    const DeviceConfigurationKeyValueListDataSelectorsType selectors = {
        .key_id = description->key_id,
    };

    return DeviceConfigurationClientRequestKeyValue(&dcc, &selectors, NULL, cb, ctx);
}

EebusError EgLpReadFailsafeActivePowerLimitInternal(
    const EgLpUseCase* self,
    const EntityAddressType* remote_entity_addr,
    ReplyMessageCallback cb,
    void* ctx
) {
    EntityRemoteObject* const remote_entity
        = USE_CASE_GET_REMOTE_ENTITY_WITH_ADDRESS(USE_CASE_OBJECT(self), remote_entity_addr);

    if (remote_entity == NULL) {
        return kEebusErrorNoChange;
    }

    return EgLpReadDeviceConfigurationWithKeyName(self, remote_entity, self->failsafe_power_limit_key, cb, ctx);
}

EebusError EgLpReadFailsafeActivePowerLimit(
    const EgLpUseCaseObject* self,
    const EntityAddressType* remote_entity_addr,
    ReplyMessageCallback cb,
    void* ctx
) {
    const UseCase* const use_case = USE_CASE(self);

    if (remote_entity_addr == NULL) {
        return kEebusErrorInputArgumentNull;
    }

    EebusError err = kEebusErrorOk;

    DEVICE_LOCAL_LOCK(use_case->local_device);
    err = EgLpReadFailsafeActivePowerLimitInternal(EG_LP_USE_CASE(self), remote_entity_addr, cb, ctx);
    DEVICE_LOCAL_UNLOCK(use_case->local_device);

    return err;
}

EebusError EgLpSetFailsafeActivePowerLimitInternal(
    EgLpUseCase* self,
    const EntityAddressType* remote_entity_addr,
    const ScaledValue* power_limit,
    ResultMessageCallback cb,
    void* ctx
) {
    const UseCase* const use_case = USE_CASE(self);

    EntityRemoteObject* const remote_entity
        = USE_CASE_GET_REMOTE_ENTITY_WITH_ADDRESS(USE_CASE_OBJECT(self), remote_entity_addr);

    if (remote_entity == NULL) {
        return kEebusErrorNoChange;
    }

    DeviceConfigurationClient dcc;
    const EebusError err = DeviceConfigurationClientConstruct(&dcc, use_case->local_entity, remote_entity);
    if (err != kEebusErrorOk) {
        return err;
    }

    const DeviceConfigurationKeyValueDescriptionDataType filter = {
        .key_name = &self->failsafe_power_limit_key,
    };

    const DeviceConfigurationKeyValueDescriptionDataType* const description
        = DeviceConfigurationCommonGetKeyValueDescriptionWithFilter(&dcc.device_cfg_common, &filter);

    if ((description == NULL) || (description->key_id == NULL)) {
        return kEebusErrorNotAvailable;
    }

    // clang-format off
  const DeviceConfigurationKeyValueDataType key_value = {
      .key_id = description->key_id,
      .value  = &(DeviceConfigurationKeyValueValueType){
          .scaled_number = &(ScaledNumberType){
               .number = &power_limit->value,
               .scale  = &power_limit->scale,
          },
      },
  };
    // clang-format on

    const DeviceConfigurationKeyValueListDataType key_value_list = {
        .device_configuration_key_value_data      = &(const DeviceConfigurationKeyValueDataType*){&key_value},
        .device_configuration_key_value_data_size = 1,
    };

    return DeviceConfigurationClientWriteKeyValueList(&dcc, &key_value_list, cb, ctx);
}

EebusError EgLpSetFailsafeActivePowerLimit(
    EgLpUseCaseObject* self,
    const EntityAddressType* remote_entity_addr,
    const ScaledValue* power_limit,
    ResultMessageCallback cb,
    void* ctx
) {
    const UseCase* const use_case = USE_CASE(self);

    if ((remote_entity_addr == NULL) || (power_limit == NULL)) {
        return kEebusErrorInputArgumentNull;
    }

    EebusError err = kEebusErrorOk;

    DEVICE_LOCAL_LOCK(use_case->local_device);
    err = EgLpSetFailsafeActivePowerLimitInternal(EG_LP_USE_CASE(self), remote_entity_addr, power_limit, cb, ctx);
    DEVICE_LOCAL_UNLOCK(use_case->local_device);

    return err;
}

EebusError EgLpGetFailsafeDurationMinimumInternal(
    const EgLpUseCase* self,
    const EntityAddressType* remote_entity_addr,
    DurationType* duration
) {
    const UseCase* const use_case = USE_CASE(self);

    EntityRemoteObject* const remote_entity
        = USE_CASE_GET_REMOTE_ENTITY_WITH_ADDRESS(USE_CASE_OBJECT(self), remote_entity_addr);

    if (remote_entity == NULL) {
        return kEebusErrorNoChange;
    }

    DeviceConfigurationClient dcc;
    EebusError err = DeviceConfigurationClientConstruct(&dcc, use_case->local_entity, remote_entity);
    if (err != kEebusErrorOk) {
        return err;
    }

    const DeviceConfigurationKeyValueDescriptionDataType filter = {
        .key_name   = &kFailsafeDurationMinimumKeyName,
        .value_type = &(DeviceConfigurationKeyValueTypeType){kDeviceConfigurationKeyValueTypeTypeDuration},
    };

    const DeviceConfigurationKeyValueDataType* const key_value
        = DeviceConfigurationCommonGetKeyValueWithFilter(&dcc.device_cfg_common, &filter);

    return DeviceConfigurationKeyValueGetDuration(key_value, duration);
}

EebusError EgLpGetFailsafeDurationMinimum(
    const EgLpUseCaseObject* self,
    const EntityAddressType* remote_entity_addr,
    DurationType* duration
) {
    const UseCase* const use_case = USE_CASE(self);

    if ((remote_entity_addr == NULL) || (duration == NULL)) {
        return kEebusErrorInputArgumentNull;
    }

    EebusError err = kEebusErrorOk;

    DEVICE_LOCAL_LOCK(use_case->local_device);
    err = EgLpGetFailsafeDurationMinimumInternal(EG_LP_USE_CASE(self), remote_entity_addr, duration);
    DEVICE_LOCAL_UNLOCK(use_case->local_device);

    return err;
}

EebusError EgLpReadFailsafeDurationMinimumInternal(
    const EgLpUseCase* self,
    const EntityAddressType* remote_entity_addr,
    ReplyMessageCallback cb,
    void* ctx
) {
    EntityRemoteObject* const remote_entity
        = USE_CASE_GET_REMOTE_ENTITY_WITH_ADDRESS(USE_CASE_OBJECT(self), remote_entity_addr);

    if (remote_entity == NULL) {
        return kEebusErrorNoChange;
    }

    return EgLpReadDeviceConfigurationWithKeyName(self, remote_entity, kFailsafeDurationMinimumKeyName, cb, ctx);
}

EebusError EgLpReadFailsafeDurationMinimum(
    const EgLpUseCaseObject* self,
    const EntityAddressType* remote_entity_addr,
    ReplyMessageCallback cb,
    void* ctx
) {
    const UseCase* const use_case = USE_CASE(self);

    if (remote_entity_addr == NULL) {
        return kEebusErrorInputArgumentNull;
    }

    EebusError err = kEebusErrorOk;

    DEVICE_LOCAL_LOCK(use_case->local_device);
    err = EgLpReadFailsafeDurationMinimumInternal(EG_LP_USE_CASE(self), remote_entity_addr, cb, ctx);
    DEVICE_LOCAL_UNLOCK(use_case->local_device);

    return err;
}

EebusError EgLpSetFailsafeDurationMinimumInternal(
    EgLpUseCase* self,
    const EntityAddressType* remote_entity_addr,
    const EebusDuration* duration,
    ResultMessageCallback cb,
    void* ctx
) {
    const UseCase* const use_case = USE_CASE(self);

    EntityRemoteObject* const remote_entity
        = USE_CASE_GET_REMOTE_ENTITY_WITH_ADDRESS(USE_CASE_OBJECT(self), remote_entity_addr);

    if (remote_entity == NULL) {
        return kEebusErrorNoChange;
    }

    static const EebusDuration two_hours         = {.hours = 2, .minutes = 0, .seconds = 0};
    static const EebusDuration twenty_four_hours = {.hours = 24, .minutes = 0, .seconds = 0};
    if (EebusDurationCompare(duration, &two_hours) < 0 || EebusDurationCompare(duration, &twenty_four_hours) > 0) {
        return kEebusErrorInputArgumentOutOfRange;
    }

    DeviceConfigurationClient dcc;
    EebusError err = DeviceConfigurationClientConstruct(&dcc, use_case->local_entity, remote_entity);
    if (err != kEebusErrorOk) {
        return err;
    }

    const DeviceConfigurationKeyValueDescriptionDataType filter = {
        .key_name = &kFailsafeDurationMinimumKeyName,
    };

    const DeviceConfigurationKeyValueDataType* const key_value_tmp
        = DeviceConfigurationCommonGetKeyValueWithFilter(&dcc.device_cfg_common, &filter);

    if (key_value_tmp == NULL) {
        return kEebusErrorNotAvailable;
    }

    // clang-format off
  const DeviceConfigurationKeyValueDataType* const key_value = &(DeviceConfigurationKeyValueDataType){
      .key_id = key_value_tmp->key_id,
      .value  = &(DeviceConfigurationKeyValueValueType){
          .duration = duration,
      },
  };
    // clang-format on

    const DeviceConfigurationKeyValueListDataType key_value_list = {
        .device_configuration_key_value_data      = &(const DeviceConfigurationKeyValueDataType*){key_value},
        .device_configuration_key_value_data_size = 1,
    };

    return DeviceConfigurationClientWriteKeyValueList(&dcc, &key_value_list, cb, ctx);
}

EebusError EgLpSetFailsafeDurationMinimum(
    EgLpUseCaseObject* self,
    const EntityAddressType* remote_entity_addr,
    const EebusDuration* duration,
    ResultMessageCallback cb,
    void* ctx
) {
    const UseCase* const use_case = USE_CASE(self);

    if ((remote_entity_addr == NULL) || (duration == NULL)) {
        return kEebusErrorInputArgumentNull;
    }

    EebusError err = kEebusErrorOk;

    DEVICE_LOCAL_LOCK(use_case->local_device);
    err = EgLpSetFailsafeDurationMinimumInternal(EG_LP_USE_CASE(self), remote_entity_addr, duration, cb, ctx);
    DEVICE_LOCAL_UNLOCK(use_case->local_device);

    return err;
}

//-------------------------------------------------------------------------------------------//
//
// Scenario 4
//
//-------------------------------------------------------------------------------------------//

const ElectricalConnectionCharacteristicDataType* EgLpNominalMaxPrimaryFilter(const EgLpUseCase* self) {
    // Note: ElectricalConnectionCharacteristicTypeType has exactly two valid values per energy
    // direction — pre-compute all variants as static const to avoid per-call construction.
    static const ElectricalConnectionCharacteristicTypeType kConsumeType
        = kElectricalConnectionCharacteristicTypeTypePowerConsumptionNominalMax;
    static const ElectricalConnectionCharacteristicDataType kConsumeFilter = {
        .characteristic_context = &kEccContextEntity,
        .characteristic_type    = &kConsumeType,
    };

    static const ElectricalConnectionCharacteristicTypeType kProduceType
        = kElectricalConnectionCharacteristicTypeTypePowerProductionNominalMax;
    static const ElectricalConnectionCharacteristicDataType kProduceFilter = {
        .characteristic_context = &kEccContextEntity,
        .characteristic_type    = &kProduceType,
    };

    return (self->energy_direction == kEnergyDirectionTypeConsume) ? &kConsumeFilter : &kProduceFilter;
}

const ElectricalConnectionCharacteristicDataType* EgLpNominalMaxContractualFilter(const EgLpUseCase* self) {
    // Note: ElectricalConnectionCharacteristicTypeType has exactly two valid values per energy
    // direction — pre-compute all variants as static const to avoid per-call construction.
    static const ElectricalConnectionCharacteristicTypeType kConsumeType
        = kElectricalConnectionCharacteristicTypeTypeContractualConsumptionNominalMax;
    static const ElectricalConnectionCharacteristicDataType kConsumeFilter = {
        .characteristic_context = &kEccContextEntity,
        .characteristic_type    = &kConsumeType,
    };

    static const ElectricalConnectionCharacteristicTypeType kProduceType
        = kElectricalConnectionCharacteristicTypeTypeContractualProductionNominalMax;
    static const ElectricalConnectionCharacteristicDataType kProduceFilter = {
        .characteristic_context = &kEccContextEntity,
        .characteristic_type    = &kProduceType,
    };

    return (self->energy_direction == kEnergyDirectionTypeConsume) ? &kConsumeFilter : &kProduceFilter;
}

EebusError EgLpGetPowerNominalMaxInternal(
    const EgLpUseCase* self,
    const EntityAddressType* remote_entity_addr,
    ScaledValue* power_limit
) {
    const UseCase* const use_case = USE_CASE(self);

    EntityRemoteObject* const remote_entity
        = USE_CASE_GET_REMOTE_ENTITY_WITH_ADDRESS(USE_CASE_OBJECT(self), remote_entity_addr);
    if (remote_entity == NULL) {
        return kEebusErrorNoChange;
    }

    ElectricalConnectionClient ecc;
    const EebusError err = ElectricalConnectionClientConstruct(&ecc, use_case->local_entity, remote_entity);
    if (err != kEebusErrorOk) {
        return err;
    }

    const ElectricalConnectionCharacteristicDataType* const filter = EgLpNominalMaxPrimaryFilter(self);

    const ElectricalConnectionCharacteristicDataType* characteristic
        = ElectricalConnectionCommonGetCharacteristicWithFilter(&ecc.el_connection_common, filter);

    if (characteristic == NULL) {
        characteristic = ElectricalConnectionCommonGetCharacteristicWithFilter(
            &ecc.el_connection_common,
            EgLpNominalMaxContractualFilter(self)
        );
    }

    if (characteristic == NULL) {
        return kEebusErrorNoChange;
    }

    return ScaledValueInitWithScaledNumber(power_limit, characteristic->value);
}

EebusError EgLpGetPowerNominalMax(
    const EgLpUseCaseObject* self,
    const EntityAddressType* remote_entity_addr,
    ScaledValue* power_limit
) {
    const UseCase* const use_case = USE_CASE(self);

    if ((remote_entity_addr == NULL) || (power_limit == NULL)) {
        return kEebusErrorInputArgumentNull;
    }

    EebusError err = kEebusErrorOk;

    DEVICE_LOCAL_LOCK(use_case->local_device);
    err = EgLpGetPowerNominalMaxInternal(EG_LP_USE_CASE(self), remote_entity_addr, power_limit);
    DEVICE_LOCAL_UNLOCK(use_case->local_device);

    return err;
}

EebusError EgLpReadPowerNominalMaxInternal(
    const EgLpUseCase* self,
    const EntityAddressType* remote_entity_addr,
    ReplyMessageCallback cb,
    void* ctx
) {
    const UseCase* const use_case = USE_CASE(self);

    EntityRemoteObject* const remote_entity
        = USE_CASE_GET_REMOTE_ENTITY_WITH_ADDRESS(USE_CASE_OBJECT(self), remote_entity_addr);

    if (remote_entity == NULL) {
        return kEebusErrorNoChange;
    }

    ElectricalConnectionClient ecc;
    const EebusError err = ElectricalConnectionClientConstruct(&ecc, use_case->local_entity, remote_entity);
    if (err != kEebusErrorOk) {
        return err;
    }

    const ElectricalConnectionCharacteristicListDataSelectorsType selectors = {
        .characteristic_context = &kEccContextEntity,
        .characteristic_type    = &self->nominal_max_characteristic,
    };

    return ElectricalConnectionClientRequestCharacteristics(&ecc, &selectors, NULL, cb, ctx);
}

EebusError EgLpReadPowerNominalMax(
    const EgLpUseCaseObject* self,
    const EntityAddressType* remote_entity_addr,
    ReplyMessageCallback cb,
    void* ctx
) {
    const UseCase* const use_case = USE_CASE(self);

    if (remote_entity_addr == NULL) {
        return kEebusErrorInputArgumentNull;
    }

    EebusError err = kEebusErrorOk;

    DEVICE_LOCAL_LOCK(use_case->local_device);
    err = EgLpReadPowerNominalMaxInternal(EG_LP_USE_CASE(self), remote_entity_addr, cb, ctx);
    DEVICE_LOCAL_UNLOCK(use_case->local_device);

    return err;
}

//-------------------------------------------------------------------------------------------//
//
// Scenario 3
//
//-------------------------------------------------------------------------------------------//

void EgLpStartHeartbeat(EgLpUseCaseObject* self) {
    UseCase* const use_case = USE_CASE(self);

    DEVICE_LOCAL_LOCK(use_case->local_device);
    HeartbeatManagerObject* const hm = ENTITY_LOCAL_GET_HEARTBEAT_MANAGER(use_case->local_entity);
    if (hm != NULL) {
        HEARTBEAT_MANAGER_START(hm);
    }

    DEVICE_LOCAL_UNLOCK(use_case->local_device);
}

void EgLpStopHeartbeat(EgLpUseCaseObject* self) {
    UseCase* const use_case = USE_CASE(self);

    DEVICE_LOCAL_LOCK(use_case->local_device);
    HeartbeatManagerObject* const hm = ENTITY_LOCAL_GET_HEARTBEAT_MANAGER(use_case->local_entity);
    if (hm != NULL) {
        HEARTBEAT_MANAGER_STOP(hm);
    }

    DEVICE_LOCAL_UNLOCK(use_case->local_device);
}

bool EgLpIsHeartbeatWithinDuration(EgLpUseCaseObject* self, const EntityAddressType* remote_entity_addr) {
    const UseCase* const use_case = USE_CASE(self);

    if (remote_entity_addr == NULL) {
        return false;
    }

    DEVICE_LOCAL_LOCK(use_case->local_device);
    bool ret = false;

    EntityRemoteObject* const remote_entity
        = USE_CASE_GET_REMOTE_ENTITY_WITH_ADDRESS(USE_CASE_OBJECT(self), remote_entity_addr);

    if (remote_entity != NULL) {
        DeviceDiagnosisClient ddc;
        if (DeviceDiagnosisClientConstruct(&ddc, use_case->local_entity, remote_entity) == kEebusErrorOk) {
            ret = DeviceDiagnosisCommonIsHeartbeatWithinDuration(
                &ddc.device_diag_common,
                &(DurationType){.minutes = 2}
            );
        }
    }

    DEVICE_LOCAL_UNLOCK(use_case->local_device);
    return ret;
}
