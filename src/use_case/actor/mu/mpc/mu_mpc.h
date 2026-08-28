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
 * @brief Measurement Unit MPC use case. An example of usage:
 * @code{.c}
 * MuMpcUseCaseObject* const mu_mpc =
 *     MuMpcUseCaseCreate(local_entity, power_cfg, energy_cfg, current_cfg, voltage_cfg, frequency_cfg);
 *
 * // Set the power and curent measurements in the cache
 * MuMpcSetMeasurementDataCache(mu_mpc, kMpcPowerTotal, &(ScaledValue){1000, 0}, NULL, NULL);
 * MuMpcSetMeasurementDataCache(mu_mpc, kMpcCurrentPhaseA, &(ScaledValue){33, -1}, &timestamp, NULL);
 * // ...
 * // Set energy consumed and produced with evaluation time
 * MuMpcSetEnergyConsumedCache(mu_mpc, &(ScaledValue){5000, 0}, NULL, NULL, &start_time, &end_time);
 * MuMpcSetEnergyProducedCache(mu_mpc, &(ScaledValue){2000, 0}, NULL, NULL, &start_time, &end_time);
 * // ...
 * // Set the frequency measurements in the cache
 * MuMpcSetMeasurementDataCache(mu_mpc, kMpcFrequency, &(ScaledValue){50, 0}, NULL, NULL);
 * // Update the MU MPC to send the cached data to the remote features
 * MuMpcUpdate(mu_mpc);
 * // ...
 * ScaledValue ac_power_total = { 0 };
 * MuMpcGetMeasurementData(mu_mpc, &ac_power_total);
 * @endcode
 * @note In case if any specific measurment is written multiple times, only the last written cache will be used.
 * @code{.c}
 * // Set the power measurement in the cache
 * MuMpcSetMeasurementDataCache(mu_mpc, kMpcPowerTotal, &(ScaledValue){1000, 0}, timestamp, value_src);
 * // Set the power measurement in the cache again
 * MuMpcSetMeasurementDataCache(mu_mpc, kMpcPowerTotal, &(ScaledValue){2000, 0}, NULL, NULL);
 *
 * // Only MuMpcSetMeasurementDataCache(mu_mpc, kMpcPowerTotal, &(ScaledValue){1000, 0}, NULL, NULL)
 * // will be applied locally and sent to the remote features, all the values of
 * // MuMpcSetMeasurementDataCache(mu_mpc, kMpcPowerTotal, &(ScaledValue){1000, 0}, timestamp, value_src)
 * // are skipped.
 * @endcode
 */

#ifndef SRC_USE_CASE_ACTOR_MU_MPC_MU_MPC_H_
#define SRC_USE_CASE_ACTOR_MU_MPC_MU_MPC_H_

#include "src/common/eebus_malloc.h"
#include "src/spine/entity/entity_local.h"
#include "src/use_case/actor/mu/mpc/mu_mpc_monitor.h"
#include "src/use_case/api/mu_mpc_listener_interface.h"
#include "src/use_case/api/use_case_interface.h"
#include "src/use_case/use_case.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @brief Mu Mpc Configuration for Scenario 1-5
 */
typedef struct MuMpcConfig MuMpcConfig;

/**
 * @brief Mu Mpc Configuration structure
 */
struct MuMpcConfig {
    /** Required configuration parameters for MPC scenario 1 */
    const MuMpcMonitorPowerConfig power_cfg;
    /** Optional configuration parameters for MPC scenario 2, set NULL if shall not be supported */
    const MuMpcMonitorEnergyConfig* energy_cfg;
    /** Optional configuration parameters for MPC scenario 3, set NULL if shall not be supported */
    const MuMpcMonitorCurrentConfig* current_cfg;
    /** Optional configuration parameters for MPC scenario 4, set NULL if shall not be supported */
    const MuMpcMonitorVoltageConfig* voltage_cfg;
    /** Optional configuration parameters for MPC scenario 5, set NULL if shall not be supported */
    const MuMpcMonitorFrequencyConfig* frequency_cfg;
};

typedef struct MuMpcUseCaseObject MuMpcUseCaseObject;

struct MuMpcUseCaseObject {
    /** Inherits the Use Case */
    UseCaseObject obj;
};

#define MU_MPC_USE_CASE_OBJECT(obj) ((MuMpcUseCaseObject*)(obj))

/**
 * @brief Creates a new MPC usecase instance for a MonitoredUnit entity.
 * @param local_entity The local entity for which to construct an MPC instance
 * @param ec_id Electrical connection id to be used for the MPC use case
 * @param cfg MU MPC Scenario 1-5 configuration parameters
 * @param listener Optional listener to receive remote MA added/removed notifications, may be NULL
 * @return Pointer to created MuMpcUseCaseObject instance
 */
MuMpcUseCaseObject* MuMpcUseCaseCreate(
    EntityLocalObject* local_entity,
    ElectricalConnectionIdType ec_id,
    const MuMpcConfig* cfg,
    MuMpcListenerObject* listener
);

/**
 * @brief Deletes the MuMpcUseCaseObject instance and frees the memory
 * @param mu_mpc Pointer to the MuMpcUseCaseObject instance to be deleted
 */
static inline void MuMpcUseCaseDelete(MuMpcUseCaseObject* mu_mpc) {
    UseCaseDelete(USE_CASE_OBJECT(mu_mpc));
}

/**
 * @brief Get the measurement data value for the specified measurement name
 * @param self MU MPC Use Case instance to get the measurement data from
 * @param measurement_name The measurement name to get the value for
 * @param measurement_value Pointer to ScaledValue where the measurement value will be stored. Shall not be NULL.
 * @return EebusError indicating the result of the operation
 */
EebusError MuMpcGetMeasurementData(
    const MuMpcUseCaseObject* self,
    MuMpcMeasurementNameId measurement_name,
    ScaledValue* measurement_value
);

/**
 * @brief Set the measurement data in MU MPC cache. This data value will be applied
 * to the local feature fith following remotes update when MuMpcUpdate() is triggered
 * @param self MU MPC Use Case instance to set the measurement for
 * @param measurement_name The measurement name id to set the value for
 * Possible values per scenario are listed below.
 * Scenario 1:
 * - kMpcPowerTotal
 * - kMpcPowerPhaseA
 * - kMpcPowerPhaseB
 * - kMpcPowerPhaseC
 * Scenario 2:
 * - kMpcEnergyConsumed
 * - kMpcEnergyProduced
 * Scenario 3:
 * - kMpcCurrentPhaseA
 * - kMpcCurrentPhaseB
 * - kMpcCurrentPhaseC
 * Scenario 4:
 * - kMpcVoltagePhaseA
 * - kMpcVoltagePhaseB
 * - kMpcVoltagePhaseC
 * - kMpcVoltagePhaseAb
 * - kMpcVoltagePhaseBc
 * - kMpcVoltagePhaseAc
 * Scenario 5:
 * - kMpcFrequency
 * @note For Scenario 2, MuMpcSetEnergyConsumedCache() and MuMpcSetEnergyProducedCache() functions
 * can be alternatively use if evalutaion time has to be set.
 * @param measurement_value The value to set for the measurement. Shall not be NULL.
 * @param timestamp Measurement timestamp, can be NULL
 * @param value_state Value state which shall be set if it differs from the normal, set NULL otherwise
 * @return EebusError indicating the result of the operation
 */
EebusError MuMpcSetMeasurementDataCache(
    MuMpcUseCaseObject* self,
    MuMpcMeasurementNameId measurement_name,
    const ScaledValue* measurement_value,
    const EebusDateTime* timestamp,
    const MeasurementValueStateType* value_state
);

/**
 * @brief Push the measurement data from cache to the local feature
 * (this function will trigger the sending of cached measurement data to remote features)
 * @param self MU MPC Use Case instance to perform the daat update for
 * @return EebusError indicating the result of the operation
 */
EebusError MuMpcUpdate(const MuMpcUseCaseObject* self);

/**
 * @brief Set the total energy consumed value in MU MPC cache. This data value will be applied
 * to the local feature fith following remotes update when MuMpcUpdate() is triggered
 * @param self MU MPC Use Case instance to set the measurement for
 * @param energy_consumed Energy measurement to be set
 * @param timestamp Measurement timestamp, can be NULL
 * @param value_state Value state which shall be set if it differs from the normal, set NULL otherwise
 * @param start_time Evaluation start time, can be NULL
 * @param end_time Evaluation end time, can be NULL
 * @return EebusError indicating the result of the operation
 */
EebusError MuMpcSetEnergyConsumedCache(
    MuMpcUseCaseObject* self,
    const ScaledValue* energy_consumed,
    const EebusDateTime* timestamp,
    const MeasurementValueStateType* value_state,
    const EebusDateTime* start_time,
    const EebusDateTime* end_time
);

/**
 * @brief Set the total energy produced value in MU MPC cache. This data value will be applied
 * to the local feature fith following remotes update when MuMpcUpdate() is triggered
 * @param self MU MPC Use Case instance to set the measurement for
 * @param energy_produced Energy measurement to be set
 * @param timestamp Measurement timestamp, can be NULL
 * @param value_state Value state which shall be set if it differs from the normal, set NULL otherwise
 * @param start_time Evaluation start time, can be NULL
 * @param end_time Evaluation end time, can be NULL
 * @return EebusError indicating the result of the operation
 */
EebusError MuMpcSetEnergyProducedCache(
    MuMpcUseCaseObject* self,
    const ScaledValue* energy_produced,
    const EebusDateTime* timestamp,
    const MeasurementValueStateType* value_state,
    const EebusDateTime* start_time,
    const EebusDateTime* end_time
);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_USE_CASE_ACTOR_MU_MPC_MU_MPC_H_
