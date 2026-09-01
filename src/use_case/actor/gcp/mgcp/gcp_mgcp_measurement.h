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
 * @brief GCP MGCP Measurement implementation declarations
 */

#ifndef SRC_USE_CASE_ACTOR_GCP_MGCP_GCP_MGCP_MEASUREMENT_H_
#define SRC_USE_CASE_ACTOR_GCP_MGCP_GCP_MGCP_MEASUREMENT_H_

#include "src/common/eebus_malloc.h"
#include "src/use_case/actor/common/eebus_measurement_base.h"
#include "src/use_case/model/mgcp_types.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef EebusMeasurementBaseConfig GcpMgcpMeasurementConfig;

/**
 * @brief Create a power-total measurement with explicit phases
 *
 * Used by GcpMgcpMonitorPowerCreate() to set the connected-phase bitmask on the
 * electrical-connection parameter description.
 *
 * @param phases Connected phases (e.g. kElectricalConnectionPhaseNameTypeAbc)
 * @param cfg    Measurement configuration
 * @return Pointer to created object, or NULL on failure
 */
EebusMeasurementObject*
GcpMgcpMeasurementPowerTotalCreate(ElectricalConnectionPhaseNameType phases, const GcpMgcpMeasurementConfig* cfg);

/**
 * @brief Create a measurement object for the given name id
 *
 * Supported name ids:
 *   Scenario 2: kGcpPowerTotal
 *   Scenario 3: kGcpEnergyFeedIn
 *   Scenario 4: kGcpEnergyConsumed
 *   Scenario 5: kGcpCurrentPhaseA, kGcpCurrentPhaseB, kGcpCurrentPhaseC
 *   Scenario 6: kGcpVoltagePhaseA, kGcpVoltagePhaseB, kGcpVoltagePhaseC,
 *               kGcpVoltagePhaseAb, kGcpVoltagePhaseBc, kGcpVoltagePhaseAc
 *   Scenario 7: kGcpFrequency
 *
 * @param name The measurement name identifier
 * @param cfg  Measurement configuration (value source + optional constraints)
 * @return Pointer to created object, or NULL on failure
 */
EebusMeasurementObject* GcpMgcpMeasurementCreate(GcpMeasurementNameId name, const GcpMgcpMeasurementConfig* cfg);

/**
 * @brief Delete a EebusMeasurementObject
 */
static inline void GcpMgcpMeasurementDelete(EebusMeasurementObject* measurement) {
    if (measurement != NULL) {
        EEBUS_MEASUREMENT_DESTRUCT(measurement);
        EEBUS_FREE(measurement);
    }
}

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_USE_CASE_ACTOR_GCP_MGCP_GCP_MGCP_MEASUREMENT_H_
