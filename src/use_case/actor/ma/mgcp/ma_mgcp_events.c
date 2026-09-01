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
 * @brief MA MGCP events handling implementation
 */

#include "src/spine/events/events.h"
#include "src/use_case/actor/ma/ma_events.h"
#include "src/use_case/actor/ma/mgcp/ma_mgcp_internal.h"
#include "src/use_case/actor/ma/mgcp/ma_mgcp_measurement.h"
#include "src/use_case/specialization/device_configuration/device_configuration_client.h"
#include "src/use_case/specialization/electrical_connection/electrical_connection_client.h"
#include "src/use_case/specialization/measurement/measurement_client.h"

static void OnEntityAddedHandleDeviceConfiguration(const MaMgcpUseCase* self, EntityRemoteObject* entity);
static void OnEntityAdded(MaMgcpUseCase* self, const EventPayload* payload);
static void OnEntityRemoved(const MaMgcpUseCase* self, const EventPayload* payload);
static void OnMeasurementDataUpdate(MaMgcpUseCase* self, const EventPayload* payload);
static void OnDeviceConfigurationDescriptionDataUpdate(MaMgcpUseCase* self, const EventPayload* payload);
static void OnDeviceConfigurationDataUpdate(MaMgcpUseCase* self, const EventPayload* payload);
static void OnDataChange(MaMgcpUseCase* self, const EventPayload* payload);

static void OnEntityAddedHandleDeviceConfiguration(const MaMgcpUseCase* self, EntityRemoteObject* entity) {
    const UseCase* const use_case = USE_CASE(self);

    DeviceConfigurationClient device_cfg;
    if (DeviceConfigurationClientConstruct(&device_cfg, use_case->local_entity, entity) != kEebusErrorOk) {
        return;
    }

    FeatureInfoClient* const feature_info = &device_cfg.feature_info_client;
    if (!HasSubscription(feature_info)) {
        Subscribe(feature_info);
    }

    DeviceConfigurationClientRequestKeyValueDescription(&device_cfg, NULL, NULL);
}

static void OnEntityAdded(MaMgcpUseCase* self, const EventPayload* payload) {
    EntityRemoteObject* const entity = payload->entity;

    if (!USE_CASE_IS_USE_CASE_COMPATIBLE(USE_CASE_OBJECT(self), payload->use_case_filter)) {
        return;
    }

    MaOnEntityAddedHandleElectricalConnection(USE_CASE(self), entity);
    MaOnEntityAddedHandleMeasurement(USE_CASE(self), entity);
    OnEntityAddedHandleDeviceConfiguration(self, entity);

    if (self->ma_mgcp_listener != NULL) {
        const EntityAddressType* const entity_addr = ENTITY_GET_ADDRESS(ENTITY_OBJECT(entity));
        MA_MGCP_LISTENER_ON_REMOTE_ENTITY_CONNECT(self->ma_mgcp_listener, entity_addr);
    }
}

static void OnEntityRemoved(const MaMgcpUseCase* self, const EventPayload* payload) {
    EntityRemoteObject* const entity = payload->entity;

    if (!USE_CASE_IS_USE_CASE_COMPATIBLE(USE_CASE_OBJECT(self), payload->use_case_filter)) {
        return;
    }

    if (self->ma_mgcp_listener != NULL) {
        const EntityAddressType* const entity_addr = ENTITY_GET_ADDRESS(ENTITY_OBJECT(entity));
        MA_MGCP_LISTENER_ON_REMOTE_ENTITY_DISCONNECT(self->ma_mgcp_listener, entity_addr);
    }
}

static void OnMeasurementDataUpdate(MaMgcpUseCase* self, const EventPayload* payload) {
    const UseCase* const use_case = USE_CASE(self);

    MeasurementClient mcl;
    if (MeasurementClientConstruct(&mcl, use_case->local_entity, payload->entity) != kEebusErrorOk) {
        return;
    }

    ElectricalConnectionClient ecl;
    if (ElectricalConnectionClientConstruct(&ecl, use_case->local_entity, payload->entity) != kEebusErrorOk) {
        return;
    }

    const MeasurementListDataType* const measurement_list = payload->function_data;
    if (measurement_list == NULL) {
        return;
    }

    const EntityAddressType* const entity_addr = ENTITY_GET_ADDRESS(ENTITY_OBJECT(payload->entity));

    for (size_t i = 0; i < measurement_list->measurement_data_size; ++i) {
        const MeasurementDataType* const measurement = measurement_list->measurement_data[i];

        const MaMeasurementObject* const mgcp_measurement = MaMgcpMeasurementGetInstance(&mcl, &ecl, measurement);
        if (mgcp_measurement == NULL) {
            continue;
        }

        ScaledValue value = {0};
        if (MA_MEASUREMENT_GET_DATA(mgcp_measurement, use_case->local_entity, payload->entity, &value)
            != kEebusErrorOk) {
            continue;
        }

        const EebusMeasurementNameId name_id = MA_MEASUREMENT_GET_NAME(mgcp_measurement);
        if (self->ma_mgcp_listener != NULL) {
            MA_MGCP_LISTENER_ON_MEASUREMENT_RECEIVE(self->ma_mgcp_listener, name_id, &value, entity_addr);
        }
    }
}

static void OnDeviceConfigurationDescriptionDataUpdate(MaMgcpUseCase* self, const EventPayload* payload) {
    const UseCase* const use_case = USE_CASE(self);

    DeviceConfigurationClient dcc;
    if (DeviceConfigurationClientConstruct(&dcc, use_case->local_entity, payload->entity) != kEebusErrorOk) {
        return;
    }

    DeviceConfigurationClientRequestKeyValue(&dcc, NULL, NULL, NULL, NULL);
}

static void OnDeviceConfigurationDataUpdate(MaMgcpUseCase* self, const EventPayload* payload) {
    const UseCase* const use_case = USE_CASE(self);

    if (self->ma_mgcp_listener == NULL) {
        return;
    }

    if (MA_MGCP_LISTENER_INTERFACE(self->ma_mgcp_listener)->on_pv_curtailment_limit_factor_receive == NULL) {
        return;
    }

    DeviceConfigurationClient dcc;
    if (DeviceConfigurationClientConstruct(&dcc, use_case->local_entity, payload->entity) != kEebusErrorOk) {
        return;
    }

    const DeviceConfigurationKeyValueDescriptionDataType filter = {
        .key_name = &(DeviceConfigurationKeyNameType){kDeviceConfigurationKeyNameTypePvCurtailmentLimitFactor},
    };

    const DeviceConfigurationKeyValueListDataType* const key_value_list = payload->function_data;
    if (!DeviceConfigurationCommonCheckKeyValueWithFilter(&dcc.device_cfg_common, key_value_list, &filter)) {
        return;
    }

    const DeviceConfigurationKeyValueDataType* const key_value
        = DeviceConfigurationCommonGetKeyValueWithFilter(&dcc.device_cfg_common, &filter);

    ScaledValue value = {0};

    const ScaledNumberType* const scaled_number = DeviceConfigurationKeyValueGetScaledNumber(key_value);
    if (ScaledValueInitWithScaledNumber(&value, scaled_number) != kEebusErrorOk) {
        return;
    }

    const EntityAddressType* const entity_addr = ENTITY_GET_ADDRESS(ENTITY_OBJECT(payload->entity));
    MA_MGCP_LISTENER_ON_PV_CURTAILMENT_LIMIT_FACTOR_RECEIVE(self->ma_mgcp_listener, &value, entity_addr);
}

static void OnDataChange(MaMgcpUseCase* self, const EventPayload* payload) {
    switch (payload->function_type) {
        case kFunctionTypeMeasurementDescriptionListData: {
            MaOnMeasurementDescriptionDataUpdate(USE_CASE(self), payload);
            break;
        }
        case kFunctionTypeMeasurementListData: {
            OnMeasurementDataUpdate(self, payload);
            break;
        }
        case kFunctionTypeDeviceConfigurationKeyValueDescriptionListData: {
            OnDeviceConfigurationDescriptionDataUpdate(self, payload);
            break;
        }
        case kFunctionTypeDeviceConfigurationKeyValueListData: {
            OnDeviceConfigurationDataUpdate(self, payload);
            break;
        }
        default: break;
    }
}

void MaMgcpHandleEvent(const EventPayload* payload, void* ctx) {
    MaMgcpUseCase* const ma_mgcp_use_case = (MaMgcpUseCase*)ctx;

    if (!USE_CASE_IS_ENTITY_COMPATIBLE(USE_CASE_OBJECT(ma_mgcp_use_case), payload->entity)) {
        return;
    }

    if (payload->event_type == kEventTypeUseCaseChange) {
        if (payload->change_type == kElementChangeAdd) {
            OnEntityAdded(ma_mgcp_use_case, payload);
        } else if (payload->change_type == kElementChangeRemove) {
            OnEntityRemoved(ma_mgcp_use_case, payload);
        }
    } else if ((payload->event_type == kEventTypeDataChange) || (payload->change_type == kElementChangeUpdate)) {
        OnDataChange(ma_mgcp_use_case, payload);
    }
}
