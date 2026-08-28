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
 * @brief Shared SPINE feature wiring for measurement-based use cases (MU MPC, GCP MGCP)
 */

#include "src/use_case/actor/common/eebus_monitor_features.h"

#include "src/use_case/specialization/electrical_connection/electrical_connection_server.h"
#include "src/use_case/specialization/measurement/measurement_server.h"

EebusError EebusMonitorFeaturesSetup(
    EebusMonitorContainer* container,
    EntityLocalObject* entity,
    ElectricalConnectionIdType ec_id
) {
    FeatureLocalObject* const ecfl
        = ENTITY_LOCAL_ADD_FEATURE_WITH_TYPE_AND_ROLE(entity, kFeatureTypeTypeElectricalConnection, kRoleTypeServer);
    if (ecfl == NULL) {
        return kEebusErrorInit;
    }

    FEATURE_LOCAL_SET_FUNCTION_OPERATIONS(ecfl, kFunctionTypeElectricalConnectionDescriptionListData, true, false);
    FEATURE_LOCAL_SET_FUNCTION_OPERATIONS(
        ecfl,
        kFunctionTypeElectricalConnectionParameterDescriptionListData,
        true,
        false
    );

    FeatureLocalObject* const mfl
        = ENTITY_LOCAL_ADD_FEATURE_WITH_TYPE_AND_ROLE(entity, kFeatureTypeTypeMeasurement, kRoleTypeServer);
    if (mfl == NULL) {
        return kEebusErrorInit;
    }

    FEATURE_LOCAL_SET_FUNCTION_OPERATIONS(mfl, kFunctionTypeMeasurementDescriptionListData, true, false);
    FEATURE_LOCAL_SET_FUNCTION_OPERATIONS(mfl, kFunctionTypeMeasurementListData, true, false);

    MeasurementServer msrv;
    EebusError err = MeasurementServerConstruct(&msrv, entity);
    if (err != kEebusErrorOk) {
        return err;
    }

    ElectricalConnectionServer ecsrv;
    err = ElectricalConnectionServerConstruct(&ecsrv, entity);
    if (err != kEebusErrorOk) {
        return err;
    }

    if (ElectricalConnectionCommonGetDescriptionWithId(&ecsrv.el_connection_common, ec_id) == NULL) {
        const ElectricalConnectionDescriptionDataType ec_description = {
            .power_supply_type         = &ELECTRICAL_CONNECTION_VOLTAGE_TYPE(Ac),
            .positive_energy_direction = &(EnergyDirectionType){kEnergyDirectionTypeConsume},
        };

        err = ElectricalConnectionServerAddDescriptionWithId(&ecsrv, &ec_description, ec_id);
        if (err != kEebusErrorOk) {
            return err;
        }
    }

    MeasurementConstraintsListDataType* const measurement_constraints = MeasurementConstraintsCreateEmpty();
    if (measurement_constraints == NULL) {
        return kEebusErrorMemoryAllocate;
    }

    for (size_t i = 0; i < VectorGetSize(&container->monitors); ++i) {
        EebusMonitorObject* const monitor = (EebusMonitorObject*)VectorGetElement(&container->monitors, i);

        err = EEBUS_MONITOR_CONFIGURE(monitor, &msrv, &ecsrv, ec_id, measurement_constraints);
        if (err != kEebusErrorOk) {
            MeasurementConstraintsDelete(measurement_constraints);
            return err;
        }
    }

    if (measurement_constraints->measurement_constraints_data_size != 0) {
        FEATURE_LOCAL_SET_FUNCTION_OPERATIONS(mfl, kFunctionTypeMeasurementConstraintsListData, true, false);
        MeasurementServerUpdateMeasurementConstraints(&msrv, measurement_constraints, NULL, NULL);
    }

    MeasurementConstraintsDelete(measurement_constraints);
    return kEebusErrorOk;
}
