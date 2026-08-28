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
 * @brief MA MGCP public API implementation
 */

#include "src/common/eebus_errors.h"
#include "src/use_case/actor/ma/mgcp/ma_mgcp.h"
#include "src/use_case/actor/ma/mgcp/ma_mgcp_internal.h"
#include "src/use_case/actor/ma/mgcp/ma_mgcp_measurement.h"
#include "src/use_case/specialization/device_configuration/device_configuration_client.h"
#include "src/use_case/use_case.h"

static EebusError MaMgcpGetPvCurtailmentLimitFactorInternal(
    const MaMgcpUseCase* self,
    const EntityAddressType* remote_entity_addr,
    ScaledValue* value
) {
    const UseCase* const use_case = USE_CASE(self);

    EntityRemoteObject* const remote_entity
        = USE_CASE_GET_REMOTE_ENTITY_WITH_ADDRESS(USE_CASE_OBJECT(self), remote_entity_addr);

    if (remote_entity == NULL) {
        return kEebusErrorNoChange;
    }

    DeviceConfigurationClient dcc = {0};

    const EebusError err = DeviceConfigurationClientConstruct(&dcc, use_case->local_entity, remote_entity);
    if (err != kEebusErrorOk) {
        return err;
    }

    const DeviceConfigurationKeyValueDescriptionDataType filter = {
        .key_name   = &(DeviceConfigurationKeyNameType){kDeviceConfigurationKeyNameTypePvCurtailmentLimitFactor},
        .value_type = &(DeviceConfigurationKeyValueTypeType){kDeviceConfigurationKeyValueTypeTypeScaledNumber},
    };

    const DeviceConfigurationKeyValueDataType* const key_value
        = DeviceConfigurationCommonGetKeyValueWithFilter(&dcc.device_cfg_common, &filter);

    const ScaledNumberType* const scaled_number = DeviceConfigurationKeyValueGetScaledNumber(key_value);
    return ScaledValueInitWithScaledNumber(value, scaled_number);
}

EebusError MaMgcpGetPvCurtailmentLimitFactor(
    const MaMgcpUseCaseObject* self,
    const EntityAddressType* remote_entity_addr,
    ScaledValue* value
) {
    const UseCase* const use_case = USE_CASE(self);

    if (value == NULL) {
        return kEebusErrorInputArgumentNull;
    }

    EebusError err = kEebusErrorOk;

    DEVICE_LOCAL_LOCK(use_case->local_device);
    err = MaMgcpGetPvCurtailmentLimitFactorInternal(MA_MGCP_USE_CASE(self), remote_entity_addr, value);
    DEVICE_LOCAL_UNLOCK(use_case->local_device);

    return err;
}

EebusError MaMgcpGetMeasurementData(
    const MaMgcpUseCaseObject* self,
    GcpMeasurementNameId measurement_name_id,
    const EntityAddressType* remote_entity_addr,
    ScaledValue* measurement_value
) {
    const UseCase* const use_case = USE_CASE(self);

    if (measurement_value == NULL) {
        return kEebusErrorInputArgumentNull;
    }

    EebusError err = kEebusErrorOk;

    DEVICE_LOCAL_LOCK(use_case->local_device);

    EntityRemoteObject* const remote_entity
        = USE_CASE_GET_REMOTE_ENTITY_WITH_ADDRESS(USE_CASE_OBJECT(use_case), remote_entity_addr);

    const MaMeasurementObject* const measurement = MaMgcpMeasurementGetInstanceWithNameId(measurement_name_id);

    if (measurement != NULL) {
        err = MA_MEASUREMENT_GET_DATA(measurement, use_case->local_entity, remote_entity, measurement_value);
    } else {
        err = kEebusErrorNotSupported;
    }

    DEVICE_LOCAL_UNLOCK(use_case->local_device);

    return err;
}
