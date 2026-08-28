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
 * @brief MU MPC Measurement implementation declarations
 * MU MPC Measurement is a helper object keeping minimal required information about
 * specific measurements (w.g. power total, power phase A, frequency, etc.) and
 * also hold the data cache to be written to local feature once also the measurements are
 * ready
 */

#ifndef SRC_USE_CASE_ACTOR_MU_MPC_MEASUREMENT_H_
#define SRC_USE_CASE_ACTOR_MU_MPC_MEASUREMENT_H_

#include "src/common/eebus_malloc.h"
#include "src/use_case/actor/common/eebus_measurement_base.h"
#include "src/use_case/model/mpc_types.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef EebusMeasurementBaseConfig MuMpcMeasurementConfig;

/**
 * @brief Creates a new measurement object for the given name_id and configuration.
 * @param name The name identifier of the measurement to create.
 * Possible values per scenario are listed below.
 * Scenario 1:
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
 * @note For Scenario 1, use MuMpcMeasurementPowerTotalCreate() to create a total power measurement.
 * can be alternatively use if evalutaion time has to be set.
 * @param cfg The configuration for the measurement, including value source and constraints.
 * @return A pointer to the created EebusMeasurementObject, or NULL if creation failed
 */
EebusMeasurementObject* MuMpcMeasurementCreate(MuMpcMeasurementNameId name, const MuMpcMeasurementConfig* cfg);

/**
 * @brief Creates a new measurement object for the total power measurement.
 * @param phases The electrical connection phases to measure (e.g., kElectricalConnectionPhaseNameTypeAbc)
 * @param cfg The configuration for the measurement, including value source and constraints.
 * @return A pointer to the created EebusMeasurementObject, or NULL if creation failed
 */
EebusMeasurementObject*
MuMpcMeasurementPowerTotalCreate(ElectricalConnectionPhaseNameType phases, const MuMpcMeasurementConfig* cfg);

/**
 * @brief Delete the EebusMeasurementObject and free its resources.
 * @param measurement The EebusMeasurementObject to delete.
 * This function will destruct the measurement object and free the memory allocated for it.
 */
static inline void MuMpcMeasurementDelete(EebusMeasurementObject* measurement) {
    if (measurement != NULL) {
        EEBUS_MEASUREMENT_DESTRUCT(measurement);
        EEBUS_FREE(measurement);
    }
}

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_USE_CASE_ACTOR_MU_MPC_MEASUREMENT_H_
