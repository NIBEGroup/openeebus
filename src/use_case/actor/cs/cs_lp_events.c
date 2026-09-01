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
 * @brief Controllable System LPC events handling implementation
 */

#include "src/spine/events/events.h"
#include "src/spine/model/device_diagnosis_types.h"
#include "src/use_case/actor/cs/cs_lp_internal.h"
#include "src/use_case/specialization/device_configuration/device_configuration_server.h"
#include "src/use_case/specialization/device_diagnosis/device_diagnosis_client.h"
#include "src/use_case/specialization/load_control/load_control_server.h"

static void OnRemoteEgAdded(CsLpUseCase* self, EntityRemoteObject* entity);
static void OnRemoteEgRemoved(CsLpUseCase* self, const EventPayload* payload);
static void OnUseCaseDataUpdate(CsLpUseCase* self, const EventPayload* payload);
static void OnLoadControlLimitDataUpdate(CsLpUseCase* self, const EventPayload* payload);
static void OnConfigurationDataUpdate(const CsLpUseCase* self, const EventPayload* payload);
static void OnHeartbeat(const CsLpUseCase* self, const EventPayload* payload);
static void OnDataChange(CsLpUseCase* self, const EventPayload* payload);

void AddDeviceDiagnosisClient(CsLpUseCase* self, EntityRemoteObject* remote_entity) {
    EntityLocalObject* const local_entity = USE_CASE(self)->local_entity;

    RemoveDeviceDiagnosisClient(self);

    self->heartbeat_diag_client = DeviceDiagnosisClientCreate(local_entity, remote_entity);
    if (self->heartbeat_diag_client != NULL) {
        FeatureInfoClient* const f = &self->heartbeat_diag_client->feature_info_client;
        if (!HasSubscription(f)) {
            Subscribe(f);
        }

        DeviceDiagnosisClientRequestHeartbeat(self->heartbeat_diag_client);
    }
}

void RemoveDeviceDiagnosisClient(CsLpUseCase* self) {
    DeviceDiagnosisClientDelete(self->heartbeat_diag_client);
    self->heartbeat_diag_client = NULL;
}

void OnRemoteEgAdded(CsLpUseCase* self, EntityRemoteObject* entity) {
    AddDeviceDiagnosisClient(self, entity);

    EntityAddressDelete(self->remote_eg_entity_addr);
    self->remote_eg_entity_addr = EntityAddressCopy(ENTITY_GET_ADDRESS(ENTITY_OBJECT(entity)));

    if (self->cs_lp_listener != NULL) {
        CS_LP_LISTENER_ON_REMOTE_EG_ADDED(self->cs_lp_listener, self->remote_eg_entity_addr);
    }
}

void OnRemoteEgRemoved(CsLpUseCase* self, const EventPayload* payload) {
    if (payload->change_type != kElementChangeRemove) {
        return;
    }

    const EntityAddressType* const entity_addr = ENTITY_GET_ADDRESS(ENTITY_OBJECT(payload->entity));
    if ((self->remote_eg_entity_addr == NULL) || !EntityAddressCompare(entity_addr, self->remote_eg_entity_addr)) {
        return;
    }

    EntityAddressDelete(self->remote_eg_entity_addr);
    self->remote_eg_entity_addr = NULL;

    RemoveDeviceDiagnosisClient(self);

    self->heartbeat_keo_workaround = false;

    if (self->cs_lp_listener != NULL) {
        CS_LP_LISTENER_ON_REMOTE_EG_REMOVED(self->cs_lp_listener, entity_addr);
    }
}

void OnUseCaseDataUpdate(CsLpUseCase* self, const EventPayload* payload) {
    if (payload->device == NULL) {
        return;
    }

    // Check if there is a DeviceDiagnosis server on one or more entities
    EntityRemoteObject* device_diag_entity = NULL;

    const Vector* const entities   = DEVICE_REMOTE_GET_ENTITIES(payload->device);
    bool has_multiple_diag_servers = false;

    for (size_t i = 0; i < VectorGetSize(entities); ++i) {
        EntityRemoteObject* entity = VectorGetElement(entities, i);
        if (USE_CASE_IS_ENTITY_COMPATIBLE(USE_CASE_OBJECT(self), entity)) {
            const FeatureRemoteObject* const fr = ENTITY_REMOTE_GET_FEATURE_WITH_TYPE_AND_ROLE(
                entity,
                kFeatureTypeTypeDeviceDiagnosis,
                kRoleTypeServer
            );
            if (fr != NULL) {
                if (device_diag_entity != NULL) {
                    has_multiple_diag_servers = true;
                    break;
                } else {
                    device_diag_entity = entity;
                }
            }
        }
    }

    // The remote device does not have a DeviceDiagnosis Server, which it should
    if (device_diag_entity == NULL) {
        return;
    }

    if (has_multiple_diag_servers) {
        // More than one matching entity have been found, this is not good
        // according to KEO the subscription should be done on the entity that requests a binding to
        // the local loadControlLimit server feature
        self->heartbeat_keo_workaround = true;
        return;
    }

    // Single matching entity has been found, as it should be, subscribe
    OnRemoteEgAdded(self, device_diag_entity);
}

void OnBindingAdded(CsLpUseCase* self, const EventPayload* payload) {
    if (payload->local_feature == NULL) {
        return;
    }

    const FeatureObject* const f = FEATURE_OBJECT(payload->local_feature);
    if ((FEATURE_GET_TYPE(f) == kFeatureTypeTypeLoadControl) && (FEATURE_GET_ROLE(f) == kRoleTypeServer)) {
        if (self->heartbeat_keo_workaround) {
            OnRemoteEgAdded(self, payload->entity);
        }
    }
}

void OnLoadControlLimitDataUpdate(CsLpUseCase* self, const EventPayload* payload) {
    const UseCase* const use_case = USE_CASE(self);

    if (self->cs_lp_listener == NULL) {
        return;
    }

    FeatureLocalObject* const fl = ENTITY_LOCAL_GET_FEATURE_WITH_TYPE_AND_ROLE(
        use_case->local_entity,
        kFeatureTypeTypeLoadControl,
        kRoleTypeServer
    );

    if (payload->local_feature != fl) {
        return;
    }

    LoadControlServer load_control;
    if (LoadControlServerConstruct(&load_control, use_case->local_entity) != kEebusErrorOk) {
        return;
    }

    const LoadControlLimitDescriptionDataType filter = {
        .limit_type      = &(LoadControlLimitTypeType){kLoadControlLimitTypeTypeSignDependentAbsValueLimit},
        .limit_category  = &(LoadControlCategoryType){kLoadControlCategoryTypeObligation},
        .limit_direction = &self->energy_direction,
        .scope_type      = &(ScopeTypeType){kScopeTypeTypeActivePowerLimit},
    };

    if (!LoadControlCommonCheckLimitWithFilter(&load_control.load_control_common, payload->function_data, &filter)) {
        return;
    }

    LoadLimit limit;
    EebusError ret = CsLpGetActivePowerLimitInternal(CS_LP_USE_CASE(self), &limit);
    if (ret == kEebusErrorOk) {
        const DurationType* const duration = limit.delete_duration ? NULL : &limit.duration;
        CS_LP_LISTENER_ON_POWER_LIMIT_RECEIVE(self->cs_lp_listener, &limit.value, duration, limit.is_active);
    }
}

void OnConfigurationDataUpdate(const CsLpUseCase* self, const EventPayload* payload) {
    const UseCase* const use_case = USE_CASE(self);

    if (self->cs_lp_listener == NULL) {
        return;
    }

    DeviceConfigurationServer dcs;
    if (DeviceConfigurationServerConstruct(&dcs, use_case->local_entity) != kEebusErrorOk) {
        return;
    }

    const DeviceConfigurationKeyValueDescriptionDataType power_limit_description = {
        .key_name = &self->failsafe_power_limit_key,
    };

    if (DeviceConfigurationCommonCheckKeyValueWithFilter(
            &dcs.device_cfg_common,
            payload->function_data,
            &power_limit_description
        )) {
        ScaledValue power_limit = {0};
        bool is_changeable      = false;

        const EebusError err = CsLpGetFailsafeActivePowerLimitInternal(self, &power_limit, &is_changeable);
        if (err == kEebusErrorOk) {
            CS_LP_LISTENER_ON_FAILSAFE_POWER_LIMIT_RECEIVE(self->cs_lp_listener, &power_limit);
        }
    }

    DeviceConfigurationKeyValueDescriptionDataType duration_description = {
        .key_name = &(DeviceConfigurationKeyNameType){kDeviceConfigurationKeyNameTypeFailsafeDurationMinimum},
    };

    if (DeviceConfigurationCommonCheckKeyValueWithFilter(
            &dcs.device_cfg_common,
            payload->function_data,
            &duration_description
        )) {
        EebusDuration duration = {0};
        bool is_changeable     = false;

        const EebusError err = CsLpGetFailsafeDurationMinimumInternal(self, &duration, &is_changeable);
        if (err == kEebusErrorOk) {
            CS_LP_LISTENER_ON_FAILSAFE_DURATION_RECEIVE(self->cs_lp_listener, &duration);
        }
    }
}

void OnHeartbeat(const CsLpUseCase* self, const EventPayload* payload) {
    if ((payload->cmd_classifier == NULL) || (*payload->cmd_classifier != kCommandClassifierTypeNotify)) {
        return;
    }

    if ((self->heartbeat_diag_client == NULL)
        || (payload->local_feature != self->heartbeat_diag_client->feature_info_client.local_feature)) {
        return;
    }

    const DeviceDiagnosisHeartbeatDataType* const data = payload->function_data;
    if ((data == NULL) || (data->heartbeat_counter == NULL)) {
        return;
    }

    if (self->cs_lp_listener != NULL) {
        CS_LP_LISTENER_ON_HEARTBEAT_RECEIVE(self->cs_lp_listener, *data->heartbeat_counter);
    }
}

void OnDataChange(CsLpUseCase* self, const EventPayload* payload) {
    if ((payload->cmd_classifier == NULL)
        || ((*payload->cmd_classifier != kCommandClassifierTypeWrite)
            && (*payload->cmd_classifier != kCommandClassifierTypeNotify))) {
        return;
    }

    switch (payload->function_type) {
        case kFunctionTypeLoadControlLimitListData: OnLoadControlLimitDataUpdate(self, payload); break;
        case kFunctionTypeDeviceConfigurationKeyValueListData: OnConfigurationDataUpdate(self, payload); break;
        case kFunctionTypeDeviceDiagnosisHeartbeatData: OnHeartbeat(self, payload); break;
        default: break;
    }
}

void CsLpHandleEvent(const EventPayload* payload, void* ctx) {
    CsLpUseCase* cs_lpc_use_case = (CsLpUseCase*)ctx;

    if ((payload->event_type == kEventTypeDeviceChange) && (payload->change_type == kElementChangeUpdate)
        && (payload->function_type == kFunctionTypeNodeManagementUseCaseData)) {
        OnUseCaseDataUpdate(cs_lpc_use_case, payload);
        return;
    }

    if (!USE_CASE_IS_ENTITY_COMPATIBLE(USE_CASE_OBJECT(cs_lpc_use_case), payload->entity)) {
        return;
    }

    if ((payload->event_type == kEventTypeBindingChange) && (payload->change_type == kElementChangeAdd)) {
        OnBindingAdded(cs_lpc_use_case, payload);
    } else if (payload->event_type == kEventTypeUseCaseChange) {
        OnRemoteEgRemoved(cs_lpc_use_case, payload);
    } else if ((payload->event_type == kEventTypeDataChange) || (payload->change_type == kElementChangeUpdate)) {
        OnDataChange(cs_lpc_use_case, payload);
    }
}
