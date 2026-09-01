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
 * @brief MGCP (MA and GCP) type declarations, constants and utilities
 */

#ifndef SRC_USE_CASE_MODEL_MGCP_TYPES_H_
#define SRC_USE_CASE_MODEL_MGCP_TYPES_H_

#include "src/use_case/model/eebus_measurement_types.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef EebusMeasurementMonitorNameId GcpMonitorNameId;
typedef EebusMeasurementNameId GcpMeasurementNameId;

#define kGcpMonitorPower kEebusMeasurementMonitorPower         /**< Scenario 2: momentary power */
#define kGcpMonitorEnergy kEebusMeasurementMonitorEnergy       /**< Scenarios 3 and 4: energy */
#define kGcpMonitorCurrent kEebusMeasurementMonitorCurrent     /**< Scenario 5: per-phase AC current */
#define kGcpMonitorVoltage kEebusMeasurementMonitorVoltage     /**< Scenario 6: per-phase AC voltage */
#define kGcpMonitorFrequency kEebusMeasurementMonitorFrequency /**< Scenario 7: AC frequency */
#define kGcpMonitorNameIdMask kEebusMeasurementMonitorNameIdMask

/* Scenario 2 — momentary power */
#define kGcpPowerTotal kMuPowerTotal /**< Total active power (W) */
#define kGcpPowerPhaseA kMuPowerPhaseA /**< Phase A active power (W) */
#define kGcpPowerPhaseB kMuPowerPhaseB /**< Phase B active power (W) */
#define kGcpPowerPhaseC kMuPowerPhaseC /**< Phase C active power (W) */

/* Scenario 3 — grid feed-in energy */
#define kGcpEnergyFeedIn kMuEnergyFeedIn /**< Cumulative feed-in energy (Wh) */

/* Scenario 4 — grid consumed energy */
#define kGcpEnergyConsumed kMuEnergyConsumed /**< Cumulative consumed energy (Wh) */

/* Scenario 5 — per-phase AC current */
#define kGcpCurrentPhaseA kMuCurrentPhaseA /**< Phase A RMS current (A) */
#define kGcpCurrentPhaseB kMuCurrentPhaseB /**< Phase B RMS current (A) */
#define kGcpCurrentPhaseC kMuCurrentPhaseC /**< Phase C RMS current (A) */

/* Scenario 6 — per-phase AC voltage */
#define kGcpVoltagePhaseA kMuVoltagePhaseA   /**< Phase A to neutral RMS voltage (V) */
#define kGcpVoltagePhaseB kMuVoltagePhaseB   /**< Phase B to neutral RMS voltage (V) */
#define kGcpVoltagePhaseC kMuVoltagePhaseC   /**< Phase C to neutral RMS voltage (V) */
#define kGcpVoltagePhaseAb kMuVoltagePhaseAb /**< Phase A to B RMS voltage (V) */
#define kGcpVoltagePhaseBc kMuVoltagePhaseBc /**< Phase B to C RMS voltage (V) */
#define kGcpVoltagePhaseAc kMuVoltagePhaseAc /**< Phase C to A RMS voltage (V) */

/* Scenario 7 — AC frequency */
#define kGcpFrequency kMuFrequency /**< Grid frequency (Hz) */

/**
 * @brief Look up a GcpMeasurementNameId by its string name
 * @param name String name of the measurement (e.g. "power_total", "current_phase_a")
 * @return Pointer to the matching GcpMeasurementNameId, or NULL if not found
 */
const GcpMeasurementNameId* GcpMgcpMeasurementGetNameId(const char* name);

/**
 * @brief Get the string name of a GcpMeasurementNameId
 * @param id The measurement name identifier
 * @return String name, or NULL if the identifier is unknown
 */
const char* GcpMgcpMeasurementGetName(GcpMeasurementNameId id);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_USE_CASE_MODEL_MGCP_TYPES_H_
