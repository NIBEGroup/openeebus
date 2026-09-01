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
 * @brief MA MPC events handling implementation
 */

#include "src/spine/events/events.h"
#include "src/use_case/actor/ma/ma_events.h"
#include "src/use_case/actor/ma/mpc/ma_mpc_internal.h"
#include "src/use_case/actor/ma/mpc/ma_mpc_measurement.h"
#include "src/use_case/specialization/electrical_connection/electrical_connection_client.h"
#include "src/use_case/specialization/measurement/measurement_client.h"

static void OnRemoteMuAddedHandleElectricalConnection(const MaMpcUseCase* self, EntityRemoteObject* entity);
static void OnRemoteMuAddedHandleMeasurement(const MaMpcUseCase* self, EntityRemoteObject* entity);
static void OnRemoteMuChange(MaMpcUseCase* self, const EventPayload* payload);
static void OnMeasurementDataUpdate(MaMpcUseCase* self, const EventPayload* payload);
static void OnDataChange(MaMpcUseCase* self, const EventPayload* payload);

void OnRemoteMuAddedHandleElectricalConnection(const MaMpcUseCase* self, EntityRemoteObject* entity) {
    const UseCase* const use_case = USE_CASE(self);

    ElectricalConnectionClient electrical_connection;
    if (ElectricalConnectionClientConstruct(&electrical_connection, use_case->local_entity, entity) != kEebusErrorOk) {
        return;
    }

    FeatureInfoClient* feature_info = &electrical_connection.feature_info_client;
    if (!HasSubscription(feature_info)) {
        Subscribe(feature_info);
    }

    // Get descriptions
    ElectricalConnectionClientRequestDescriptions(&electrical_connection, NULL, NULL);

    // Get parameter descriptions
    ElectricalConnectionClientRequestParameterDescriptions(&electrical_connection, NULL, NULL);
}

void OnRemoteMuAddedHandleMeasurement(const MaMpcUseCase* self, EntityRemoteObject* entity) {
    const UseCase* const use_case = USE_CASE(self);

    MeasurementClient measurement;
    if (MeasurementClientConstruct(&measurement, use_case->local_entity, entity) != kEebusErrorOk) {
        return;
    }

    FeatureInfoClient* feature_info = &measurement.feature_info_client;
    if (!HasSubscription(feature_info)) {
        Subscribe(feature_info);
    }

    // Get descriptions
    MeasurementClientRequestDescriptions(&measurement, NULL, NULL);

    // Get constraints
    MeasurementClientRequestConstraints(&measurement, NULL, NULL);
}

void OnRemoteMuChange(MaMpcUseCase* self, const EventPayload* payload) {
    if (!USE_CASE_IS_USE_CASE_COMPATIBLE(USE_CASE_OBJECT(self), payload->use_case_filter)) {
        return;
    }

    const EntityAddressType* const entity_addr = ENTITY_GET_ADDRESS(ENTITY_OBJECT(payload->entity));

    if (payload->change_type == kElementChangeAdd) {
        OnRemoteMuAddedHandleElectricalConnection(self, payload->entity);
        OnRemoteMuAddedHandleMeasurement(self, payload->entity);
        if (self->ma_mpc_listener != NULL) {
            MA_MPC_LISTENER_ON_REMOTE_MU_ADDED(self->ma_mpc_listener, entity_addr);
        }
    } else if (payload->change_type == kElementChangeRemove) {
        if (self->ma_mpc_listener != NULL) {
            MA_MPC_LISTENER_ON_REMOTE_MU_REMOVED(self->ma_mpc_listener, entity_addr);
        }
    }
}

void OnMeasurementDataUpdate(MaMpcUseCase* self, const EventPayload* payload) {
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

        const MaMeasurementObject* const mpc_measurement = MaMpcMeasurementGetInstance(&mcl, &ecl, measurement);
        if (mpc_measurement == NULL) {
            continue;
        }

        ScaledValue value = {0};
        if (MA_MEASUREMENT_GET_DATA(mpc_measurement, use_case->local_entity, payload->entity, &value)
            != kEebusErrorOk) {
            continue;
        }

        const EebusMeasurementNameId name_id = MA_MEASUREMENT_GET_NAME(mpc_measurement);
        if (self->ma_mpc_listener != NULL) {
            MA_MPC_LISTENER_ON_MEASUREMENT_RECEIVE(self->ma_mpc_listener, name_id, &value, entity_addr);
        }
    }
}

static void OnDataChange(MaMpcUseCase* self, const EventPayload* payload) {
    switch (payload->function_type) {
        case kFunctionTypeMeasurementDescriptionListData: {
            MaOnMeasurementDescriptionDataUpdate(USE_CASE(self), payload);
            break;
        }

        case kFunctionTypeMeasurementListData: {
            OnMeasurementDataUpdate(self, payload);
            break;
        }

        default: break;
    }
}

void MaMpcHandleEvent(const EventPayload* payload, void* ctx) {
    MaMpcUseCase* ma_mpc_use_case = (MaMpcUseCase*)ctx;

    if (!USE_CASE_IS_ENTITY_COMPATIBLE(USE_CASE_OBJECT(ma_mpc_use_case), payload->entity)) {
        return;
    }

    if (payload->event_type == kEventTypeUseCaseChange) {
        OnRemoteMuChange(ma_mpc_use_case, payload);
    } else if ((payload->event_type == kEventTypeDataChange) || (payload->change_type == kElementChangeUpdate)) {
        OnDataChange(ma_mpc_use_case, payload);
    }
}
