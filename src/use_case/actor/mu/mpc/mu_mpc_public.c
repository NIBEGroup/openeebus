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
#include "src/use_case/actor/mu/mpc/mu_mpc.h"
#include "src/use_case/actor/mu/mpc/mu_mpc_internal.h"

EebusError MuMpcGetMeasurementData(
    const MuMpcUseCaseObject* self,
    MuMpcMeasurementNameId measurement_element_id,
    ScaledValue* measurement_value
) {
    if (measurement_value == NULL) {
        return kEebusErrorInputArgumentNull;
    }

    MuMpcUseCase* const mu_mpc    = MU_MPC_USE_CASE(self);
    const UseCase* const use_case = USE_CASE(self);
    return EebusMonitorContainerGetMeasurementData(
        &mu_mpc->monitor_container,
        use_case->local_entity,
        use_case->local_device,
        measurement_element_id,
        measurement_value
    );
}

EebusError MuMpcSetMeasurementDataCache(
    MuMpcUseCaseObject* self,
    MuMpcMeasurementNameId measurement_name,
    const ScaledValue* measurement_value,
    const EebusDateTime* timestamp,
    const MeasurementValueStateType* value_state
) {
    if (measurement_value == NULL) {
        return kEebusErrorInputArgumentNull;
    }

    MuMpcUseCase* const mu_mpc = MU_MPC_USE_CASE(self);
    return EebusMonitorContainerSetMeasurementDataCache(
        &mu_mpc->monitor_container,
        measurement_name,
        measurement_value,
        timestamp,
        value_state
    );
}

EebusError MuMpcUpdate(const MuMpcUseCaseObject* self) {
    MuMpcUseCase* const mu_mpc    = MU_MPC_USE_CASE(self);
    const UseCase* const use_case = USE_CASE(self);
    return EebusMonitorContainerUpdate(&mu_mpc->monitor_container, use_case->local_entity, use_case->local_device);
}

EebusError MuMpcSetEnergyConsumedCache(
    MuMpcUseCaseObject* self,
    const ScaledValue* energy_consumed,
    const EebusDateTime* timestamp,
    const MeasurementValueStateType* value_state,
    const EebusDateTime* start_time,
    const EebusDateTime* end_time
) {
    MuMpcUseCase* const mu_mpc = MU_MPC_USE_CASE(self);
    return EebusMonitorContainerSetMeasurementDataCacheWithTime(
        &mu_mpc->monitor_container,
        kMpcEnergyConsumed,
        energy_consumed,
        timestamp,
        value_state,
        start_time,
        end_time
    );
}

EebusError MuMpcSetEnergyProducedCache(
    MuMpcUseCaseObject* self,
    const ScaledValue* energy_produced,
    const EebusDateTime* timestamp,
    const MeasurementValueStateType* value_state,
    const EebusDateTime* start_time,
    const EebusDateTime* end_time
) {
    MuMpcUseCase* const mu_mpc = MU_MPC_USE_CASE(self);
    return EebusMonitorContainerSetMeasurementDataCacheWithTime(
        &mu_mpc->monitor_container,
        kMpcEnergyProduced,
        energy_produced,
        timestamp,
        value_state,
        start_time,
        end_time
    );
}
