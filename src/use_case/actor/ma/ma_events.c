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
 * @brief Shared MA event handler implementations for MPC and MGCP monitoring use cases
 */

#include "src/use_case/actor/ma/ma_events.h"

#include "src/use_case/specialization/electrical_connection/electrical_connection_client.h"
#include "src/use_case/specialization/measurement/measurement_client.h"

void MaOnEntityAddedHandleElectricalConnection(const UseCase* use_case, EntityRemoteObject* entity) {
    ElectricalConnectionClient electrical_connection;
    if (ElectricalConnectionClientConstruct(&electrical_connection, use_case->local_entity, entity) != kEebusErrorOk) {
        return;
    }

    FeatureInfoClient* const feature_info = &electrical_connection.feature_info_client;
    if (!HasSubscription(feature_info)) {
        Subscribe(feature_info);
    }

    ElectricalConnectionClientRequestDescriptions(&electrical_connection, NULL, NULL);
    ElectricalConnectionClientRequestParameterDescriptions(&electrical_connection, NULL, NULL);
}

void MaOnEntityAddedHandleMeasurement(const UseCase* use_case, EntityRemoteObject* entity) {
    MeasurementClient measurement;
    if (MeasurementClientConstruct(&measurement, use_case->local_entity, entity) != kEebusErrorOk) {
        return;
    }

    FeatureInfoClient* const feature_info = &measurement.feature_info_client;
    if (!HasSubscription(feature_info)) {
        Subscribe(feature_info);
    }

    MeasurementClientRequestDescriptions(&measurement, NULL, NULL);
    MeasurementClientRequestConstraints(&measurement, NULL, NULL);
}

void MaOnMeasurementDescriptionDataUpdate(const UseCase* use_case, const EventPayload* payload) {
    MeasurementClient mcl;
    if (MeasurementClientConstruct(&mcl, use_case->local_entity, payload->entity) != kEebusErrorOk) {
        return;
    }

    MeasurementClientRequestData(&mcl, NULL, NULL, NULL, NULL);
}
