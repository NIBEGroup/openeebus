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
 * @brief Energy Guard LP events handling implementation
 */

#include "src/spine/events/events.h"
#include "src/spine/model/device_diagnosis_types.h"
#include "src/use_case/actor/eg/eg_lp_internal.h"
#include "src/use_case/specialization/device_configuration/device_configuration_client.h"
#include "src/use_case/specialization/device_diagnosis/device_diagnosis_client.h"
#include "src/use_case/specialization/electrical_connection/electrical_connection_client.h"
#include "src/use_case/specialization/load_control/load_control_client.h"

static void OnLoadControlLimitDataUpdate(EgLpUseCase* self, const EventPayload* payload);
static void OnConfigurationDataUpdate(const EgLpUseCase* self, const EventPayload* payload);
static void OnHeartbeat(const EgLpUseCase* self, const EventPayload* payload);
static void OnElectricalConnectionCharacteristicDataUpdate(const EgLpUseCase* self, const EventPayload* payload);
static void OnDataChange(EgLpUseCase* self, const EventPayload* payload);
static void OnRemoteCsChange(EgLpUseCase* self, const EventPayload* payload);

void OnRemoteCsAddedHandleLoadControl(const EgLpUseCase* self, EntityRemoteObject* entity) {
    const UseCase* const use_case = USE_CASE(self);

    LoadControlClient load_control;
    if (LoadControlClientConstruct(&load_control, use_case->local_entity, entity) != kEebusErrorOk) {
        return;
    }

    FeatureInfoClient* feature_info = &load_control.feature_info_client;
    if (!HasSubscription(feature_info)) {
        Subscribe(feature_info);
    }

    if (!HasBinding(feature_info)) {
        Bind(feature_info);
    }

    // Get descriptions
    const LoadControlLimitDescriptionListDataSelectorsType selectors = {
        .limit_type      = &(LoadControlLimitTypeType){kLoadControlLimitTypeTypeSignDependentAbsValueLimit},
        .limit_direction = &self->energy_direction,
        .scope_type      = &(ScopeTypeType){kScopeTypeTypeActivePowerLimit},
    };

    LoadControlClientRequestLimitDescriptions(&load_control, &selectors, NULL);
}

void OnRemoteCsAddedHandleDeviceConfiguration(const EgLpUseCase* self, EntityRemoteObject* entity) {
    const UseCase* const use_case = USE_CASE(self);

    DeviceConfigurationClient device_configuraton;
    const EebusError err = DeviceConfigurationClientConstruct(&device_configuraton, use_case->local_entity, entity);
    if (err != kEebusErrorOk) {
        return;
    }

    FeatureInfoClient* feature_info = &device_configuraton.feature_info_client;
    if (!HasSubscription(feature_info)) {
        Subscribe(feature_info);
    }

    if (!HasBinding(feature_info)) {
        Bind(feature_info);
    }

    // Get descriptions
    // don't use selectors yet, as we would have to query 2 which could result in 2 full reads
    DeviceConfigurationClientRequestKeyValueDescription(&device_configuraton, NULL, NULL);
}

void OnRemoteCsAddedHandleElectricalConnection(const EgLpUseCase* self, EntityRemoteObject* entity) {
    const UseCase* const use_case = USE_CASE(self);

    ElectricalConnectionClient electrical_connection;
    const EebusError err = ElectricalConnectionClientConstruct(&electrical_connection, use_case->local_entity, entity);
    if (err != kEebusErrorOk) {
        return;
    }

    FeatureInfoClient* feature_info = &electrical_connection.feature_info_client;
    if (!HasSubscription(feature_info)) {
        Subscribe(feature_info);
    }

    const ElectricalConnectionCharacteristicListDataSelectorsType selectors = {
        .characteristic_context = &kEccContextEntity,
    };

    ElectricalConnectionClientRequestCharacteristics(&electrical_connection, &selectors, NULL, NULL, NULL);
}

void OnRemoteCsAddedHandleDeviceDiagnosis(const EgLpUseCase* self, EntityRemoteObject* entity) {
    const UseCase* const use_case = USE_CASE(self);

    DeviceDiagnosisClient device_diagnosis;
    if (DeviceDiagnosisClientConstruct(&device_diagnosis, use_case->local_entity, entity) != kEebusErrorOk) {
        return;
    }

    FeatureInfoClient* const f = &device_diagnosis.feature_info_client;
    if (!HasSubscription(f)) {
        Subscribe(f);
    }

    DeviceDiagnosisClientRequestHeartbeat(&device_diagnosis);
}

void OnRemoteCsChange(EgLpUseCase* self, const EventPayload* payload) {
    if (!USE_CASE_IS_USE_CASE_COMPATIBLE(USE_CASE_OBJECT(self), payload->use_case_filter)) {
        return;
    }

    const EntityAddressType* const entity_addr = ENTITY_GET_ADDRESS(ENTITY_OBJECT(payload->entity));

    if (payload->change_type == kElementChangeAdd) {
        OnRemoteCsAddedHandleLoadControl(self, payload->entity);
        OnRemoteCsAddedHandleDeviceConfiguration(self, payload->entity);
        OnRemoteCsAddedHandleDeviceDiagnosis(self, payload->entity);
        OnRemoteCsAddedHandleElectricalConnection(self, payload->entity);

        if (self->eg_lp_listener != NULL) {
            EG_LP_LISTENER_ON_REMOTE_CS_ADDED(self->eg_lp_listener, entity_addr);
        }
    } else if (payload->change_type == kElementChangeRemove) {
        if (self->eg_lp_listener != NULL) {
            EG_LP_LISTENER_ON_REMOTE_CS_REMOVED(self->eg_lp_listener, entity_addr);
        }
    }
}

void OnLoadControlLimitDescriptionDataUpdate(const EgLpUseCase* self, const EventPayload* payload) {
    EgLpReadLoadControlLimit((EgLpUseCase*)self, payload->entity, NULL, NULL);
}

void OnLoadControlLimitDataUpdate(EgLpUseCase* self, const EventPayload* payload) {
    const UseCase* const use_case = USE_CASE(self);

    LoadControlClient load_control;
    if (LoadControlClientConstruct(&load_control, use_case->local_entity, payload->entity) != kEebusErrorOk) {
        return;
    }

    const LoadControlLimitDescriptionDataType filter = {
        .limit_type      = &(LoadControlLimitTypeType){kLoadControlLimitTypeTypeSignDependentAbsValueLimit},
        .limit_direction = &self->energy_direction,
        .scope_type      = &(ScopeTypeType){kScopeTypeTypeActivePowerLimit},
    };

    if (!LoadControlCommonCheckLimitWithFilter(&load_control.load_control_common, payload->function_data, &filter)) {
        return;
    }

    LoadLimit limit;
    const EntityAddressType* entity_addr = ENTITY_GET_ADDRESS(ENTITY_OBJECT(payload->entity));

    EebusError ret = EgLpGetActivePowerLimitInternal(self, entity_addr, &limit);
    if (ret == kEebusErrorOk) {
        const DurationType* duration = limit.delete_duration ? NULL : &limit.duration;
        EG_LP_LISTENER_ON_POWER_LIMIT_RECEIVE(
            self->eg_lp_listener,
            entity_addr,
            &limit.value,
            duration,
            limit.is_active
        );
    }
}

void OnConfigurationDescriptionDataUpdate(const EgLpUseCase* self, const EventPayload* payload) {
    const UseCase* const use_case = USE_CASE(self);

    DeviceConfigurationClient dcc;
    if (DeviceConfigurationClientConstruct(&dcc, use_case->local_entity, payload->entity) != kEebusErrorOk) {
        return;
    }

    // Key value descriptions received, now get the data
    DeviceConfigurationClientRequestKeyValue(&dcc, NULL, NULL, NULL, NULL);
}

void OnConfigurationDataUpdate(const EgLpUseCase* self, const EventPayload* payload) {
    const UseCase* const use_case = USE_CASE(self);

    DeviceConfigurationClient dcc;
    if (DeviceConfigurationClientConstruct(&dcc, use_case->local_entity, payload->entity) != kEebusErrorOk) {
        return;
    }

    if (self->eg_lp_listener == NULL) {
        return;
    }

    DeviceConfigurationKeyValueDescriptionDataType filter = {
        .key_name = &self->failsafe_power_limit_key,
    };

    const DeviceConfigurationKeyValueListDataType* const key_value_list = payload->function_data;
    if (DeviceConfigurationCommonCheckKeyValueWithFilter(&dcc.device_cfg_common, key_value_list, &filter)) {
        const EntityAddressType* const entity_addr = ENTITY_GET_ADDRESS(ENTITY_OBJECT(payload->entity));

        ScaledValue power_limit = {0};
        const EebusError err    = EgLpGetFailsafeActivePowerLimitInternal(self, entity_addr, &power_limit);
        if (err == kEebusErrorOk) {
            EG_LP_LISTENER_ON_FAILSAFE_POWER_LIMIT_RECEIVE(self->eg_lp_listener, entity_addr, &power_limit);
        }
    }

    filter.key_name = &(DeviceConfigurationKeyNameType){kDeviceConfigurationKeyNameTypeFailsafeDurationMinimum};

    if (DeviceConfigurationCommonCheckKeyValueWithFilter(&dcc.device_cfg_common, key_value_list, &filter)) {
        const EntityAddressType* const entity_addr = ENTITY_GET_ADDRESS(ENTITY_OBJECT(payload->entity));

        DurationType duration = {0};
        const EebusError err  = EgLpGetFailsafeDurationMinimumInternal(self, entity_addr, &duration);

        if (err == kEebusErrorOk) {
            EG_LP_LISTENER_ON_FAILSAFE_DURATION_RECEIVE(self->eg_lp_listener, entity_addr, &duration);
        }
    }
}

void OnElectricalConnectionCharacteristicDataUpdate(const EgLpUseCase* self, const EventPayload* payload) {
    if (self->eg_lp_listener == NULL) {
        return;
    }

    const ElectricalConnectionCharacteristicDataType* const primary_filter     = EgLpNominalMaxPrimaryFilter(self);
    const ElectricalConnectionCharacteristicDataType* const contractual_filter = EgLpNominalMaxContractualFilter(self);

    if (ElectricalConnectionCommonCheckCharacteristicWithFilter(payload->function_data, primary_filter)
        || ElectricalConnectionCommonCheckCharacteristicWithFilter(payload->function_data, contractual_filter)) {
        const EntityAddressType* const entity_addr = ENTITY_GET_ADDRESS(ENTITY_OBJECT(payload->entity));

        ScaledValue power_limit = {0};
        const EebusError err    = EgLpGetPowerNominalMaxInternal(self, entity_addr, &power_limit);
        if (err != kEebusErrorOk) {
            return;
        }

        EG_LP_LISTENER_ON_POWER_NOMINAL_MAX_RECEIVE(self->eg_lp_listener, entity_addr, &power_limit);
    }
}

void OnHeartbeat(const EgLpUseCase* self, const EventPayload* payload) {
    if ((payload->cmd_classifier == NULL) || (*payload->cmd_classifier != kCommandClassifierTypeNotify)) {
        return;
    }

    const DeviceDiagnosisHeartbeatDataType* const data = payload->function_data;
    if ((data == NULL) || (data->heartbeat_counter == NULL)) {
        return;
    }

    if (self->eg_lp_listener != NULL) {
        const EntityAddressType* const entity_addr = ENTITY_GET_ADDRESS(ENTITY_OBJECT(payload->entity));
        EG_LP_LISTENER_ON_HEARTBEAT_RECEIVE(self->eg_lp_listener, entity_addr, *data->heartbeat_counter);
    }
}

void OnDataChange(EgLpUseCase* self, const EventPayload* payload) {
    switch (payload->function_type) {
        case kFunctionTypeLoadControlLimitDescriptionListData:
            OnLoadControlLimitDescriptionDataUpdate(self, payload);
            break;

        case kFunctionTypeLoadControlLimitListData: OnLoadControlLimitDataUpdate(self, payload); break;

        case kFunctionTypeDeviceConfigurationKeyValueDescriptionListData:
            OnConfigurationDescriptionDataUpdate(self, payload);
            break;

        case kFunctionTypeDeviceConfigurationKeyValueListData: OnConfigurationDataUpdate(self, payload); break;

        case kFunctionTypeDeviceDiagnosisHeartbeatData: OnHeartbeat(self, payload); break;

        case kFunctionTypeElectricalConnectionCharacteristicListData:
            OnElectricalConnectionCharacteristicDataUpdate(self, payload);
            break;

        default: break;
    }
}

void EgLpHandleEvent(const EventPayload* payload, void* ctx) {
    EgLpUseCase* eg_lp_use_case = (EgLpUseCase*)ctx;

    if (!USE_CASE_IS_ENTITY_COMPATIBLE(USE_CASE_OBJECT(eg_lp_use_case), payload->entity)) {
        return;
    }

    if (payload->event_type == kEventTypeUseCaseChange) {
        OnRemoteCsChange(eg_lp_use_case, payload);
    } else if ((payload->event_type == kEventTypeDataChange) || (payload->change_type == kElementChangeUpdate)) {
        OnDataChange(eg_lp_use_case, payload);
    }
}
