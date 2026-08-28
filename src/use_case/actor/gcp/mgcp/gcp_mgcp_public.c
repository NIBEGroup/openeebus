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
 * @brief GCP MGCP public API implementation
 */

#include "src/use_case/actor/gcp/mgcp/gcp_mgcp.h"

#include "src/use_case/actor/gcp/mgcp/gcp_mgcp_internal.h"
#include "src/use_case/specialization/device_configuration/device_configuration_server.h"

EebusError GcpMgcpGetMeasurementData(
    const GcpMgcpUseCaseObject* self,
    GcpMeasurementNameId measurement_name,
    ScaledValue* measurement_value
) {
    if (measurement_value == NULL) {
        return kEebusErrorInputArgumentNull;
    }

    GcpMgcpUseCase* const gcp_mgcp = GCP_MGCP_USE_CASE(self);
    const UseCase* const use_case  = USE_CASE(self);
    return EebusMonitorContainerGetMeasurementData(
        &gcp_mgcp->monitor_container,
        use_case->local_entity,
        use_case->local_device,
        measurement_name,
        measurement_value
    );
}

EebusError GcpMgcpSetMeasurementDataCache(
    GcpMgcpUseCaseObject* self,
    GcpMeasurementNameId measurement_name,
    const ScaledValue* measurement_value,
    const EebusDateTime* timestamp,
    const MeasurementValueStateType* value_state
) {
    if (measurement_value == NULL) {
        return kEebusErrorInputArgumentNull;
    }

    GcpMgcpUseCase* const gcp_mgcp = GCP_MGCP_USE_CASE(self);
    return EebusMonitorContainerSetMeasurementDataCache(
        &gcp_mgcp->monitor_container,
        measurement_name,
        measurement_value,
        timestamp,
        value_state
    );
}

EebusError GcpMgcpUpdate(const GcpMgcpUseCaseObject* self) {
    GcpMgcpUseCase* const gcp_mgcp = GCP_MGCP_USE_CASE(self);
    const UseCase* const use_case  = USE_CASE(self);
    return EebusMonitorContainerUpdate(&gcp_mgcp->monitor_container, use_case->local_entity, use_case->local_device);
}

EebusError GcpMgcpSetEnergyFeedInCache(
    GcpMgcpUseCaseObject* self,
    const ScaledValue* energy_feed_in,
    const EebusDateTime* timestamp,
    const MeasurementValueStateType* value_state,
    const EebusDateTime* start_time,
    const EebusDateTime* end_time
) {
    GcpMgcpUseCase* const gcp_mgcp = GCP_MGCP_USE_CASE(self);
    return EebusMonitorContainerSetMeasurementDataCacheWithTime(
        &gcp_mgcp->monitor_container,
        kGcpEnergyFeedIn,
        energy_feed_in,
        timestamp,
        value_state,
        start_time,
        end_time
    );
}

EebusError GcpMgcpSetEnergyConsumedCache(
    GcpMgcpUseCaseObject* self,
    const ScaledValue* energy_consumed,
    const EebusDateTime* timestamp,
    const MeasurementValueStateType* value_state,
    const EebusDateTime* start_time,
    const EebusDateTime* end_time
) {
    GcpMgcpUseCase* const gcp_mgcp = GCP_MGCP_USE_CASE(self);
    return EebusMonitorContainerSetMeasurementDataCacheWithTime(
        &gcp_mgcp->monitor_container,
        kGcpEnergyConsumed,
        energy_consumed,
        timestamp,
        value_state,
        start_time,
        end_time
    );
}

static EebusError GcpMgcpSetPvCurtailmentLimitFactorInternal(GcpMgcpUseCase* self, const ScaledValue* value) {
    const UseCase* const use_case = USE_CASE(self);

    DeviceConfigurationServer dcs = {0};

    const EebusError err = DeviceConfigurationServerConstruct(&dcs, use_case->local_entity);
    if (err != kEebusErrorOk) {
        return err;
    }

    // clang-format off
  const DeviceConfigurationKeyValueDataType data = {
      .value = &(DeviceConfigurationKeyValueValueType){
          .scaled_number = &(ScaledNumberType){
              .number = &(int64_t){value->value},
              .scale  = &(int8_t){value->scale},
          },
      },
  };
    // clang-format on

    const DeviceConfigurationKeyValueDescriptionDataType filter = {
        .key_name = &(DeviceConfigurationKeyNameType){kDeviceConfigurationKeyNameTypePvCurtailmentLimitFactor},
    };

    return DeviceConfigurationServerUpdateKeyValueWithFilter(&dcs, &data, NULL, &filter);
}

EebusError GcpMgcpSetPvCurtailmentLimitFactor(GcpMgcpUseCaseObject* self, const ScaledValue* value) {
    if (value == NULL) {
        return kEebusErrorInputArgumentNull;
    }

    GcpMgcpUseCase* const gcp_mgcp = GCP_MGCP_USE_CASE(self);

    if (!gcp_mgcp->has_scenario1) {
        return kEebusErrorNotSupported;
    }

    const UseCase* const use_case = USE_CASE(self);

    DEVICE_LOCAL_LOCK(use_case->local_device);
    const EebusError err = GcpMgcpSetPvCurtailmentLimitFactorInternal(gcp_mgcp, value);
    DEVICE_LOCAL_UNLOCK(use_case->local_device);

    return err;
}

static EebusError GcpMgcpGetPvCurtailmentLimitFactorInternal(const GcpMgcpUseCase* self, ScaledValue* value) {
    const UseCase* const use_case = USE_CASE(self);

    DeviceConfigurationServer dcs = {0};

    const EebusError err = DeviceConfigurationServerConstruct(&dcs, use_case->local_entity);
    if (err != kEebusErrorOk) {
        return err;
    }

    const DeviceConfigurationKeyValueDescriptionDataType filter = {
        .key_name   = &(DeviceConfigurationKeyNameType){kDeviceConfigurationKeyNameTypePvCurtailmentLimitFactor},
        .value_type = &(DeviceConfigurationKeyValueTypeType){kDeviceConfigurationKeyValueTypeTypeScaledNumber},
    };

    const DeviceConfigurationKeyValueDataType* const key_value
        = DeviceConfigurationCommonGetKeyValueWithFilter(&dcs.device_cfg_common, &filter);
    const ScaledNumberType* const scaled_number = DeviceConfigurationKeyValueGetScaledNumber(key_value);
    return ScaledValueInitWithScaledNumber(value, scaled_number);
}

EebusError GcpMgcpGetPvCurtailmentLimitFactor(const GcpMgcpUseCaseObject* self, ScaledValue* value) {
    if (value == NULL) {
        return kEebusErrorInputArgumentNull;
    }

    GcpMgcpUseCase* const gcp_mgcp = GCP_MGCP_USE_CASE(self);

    if (!gcp_mgcp->has_scenario1) {
        return kEebusErrorNotSupported;
    }

    const UseCase* const use_case = USE_CASE(self);

    DEVICE_LOCAL_LOCK(use_case->local_device);
    const EebusError err = GcpMgcpGetPvCurtailmentLimitFactorInternal(gcp_mgcp, value);
    DEVICE_LOCAL_UNLOCK(use_case->local_device);

    return err;
}
