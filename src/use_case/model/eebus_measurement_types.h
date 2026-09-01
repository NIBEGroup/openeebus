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
 * @brief Measurement type declarations shared between MU MPC and GCP MGCP use cases
 */

#ifndef SRC_USE_CASE_MODEL_EEBUS_MEASUREMENT_TYPES_H_
#define SRC_USE_CASE_MODEL_EEBUS_MEASUREMENT_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @brief Measurement monitor group identifiers (high nibble of EebusMeasurementNameId)
 */
enum EebusMeasurementMonitorNameId {
    kEebusMeasurementMonitorPower      = 0x10, /**< Active power measurements */
    kEebusMeasurementMonitorEnergy     = 0x20, /**< Energy measurements */
    kEebusMeasurementMonitorCurrent    = 0x30, /**< Per-phase AC current measurements */
    kEebusMeasurementMonitorVoltage    = 0x40, /**< Per-phase AC voltage measurements */
    kEebusMeasurementMonitorFrequency  = 0x50, /**< AC frequency measurements */
    kEebusMeasurementMonitorNameIdMask = 0xF0, /**< Mask to extract the monitor group */
};

typedef enum EebusMeasurementMonitorNameId EebusMeasurementMonitorNameId;

/**
 * @brief Measurement name identifiers shared by MU MPC and GCP MGCP
 *
 * High nibble selects the monitor group; low nibble identifies the specific
 * measurement within that group.
 *
 * MU MPC scenarios:
 *   kMuPowerTotal, kMuPowerPhaseA–C  (scenario 1 — total and per-phase power)
 *   kMuEnergyConsumed, kMuEnergyProduced  (scenario 2 — energy)
 *   kMuCurrentPhaseA–C  (scenario 3 — current)
 *   kMuVoltagePhaseA–C, kMuVoltagePhaseAb/Bc/Ac  (scenario 4 — voltage)
 *   kMuFrequency  (scenario 5 — frequency)
 *
 * GCP MGCP scenarios:
 *   kMuPowerTotal  (scenario 2 — total power)
 *   kMuEnergyFeedIn  (scenario 3 — grid feed-in energy)
 *   kMuEnergyConsumed  (scenario 4 — grid consumed energy)
 *   kMuCurrentPhaseA–C  (scenario 5 — current)
 *   kMuVoltagePhaseA–C, kMuVoltagePhaseAb/Bc/Ac  (scenario 6 — voltage)
 *   kMuFrequency  (scenario 7 — frequency)
 */
enum EebusMeasurementNameId {
    /* Active power */
    kMuPowerTotal  = kEebusMeasurementMonitorPower | 0x01, /**< Total active power (W) */
    kMuPowerPhaseA = kEebusMeasurementMonitorPower | 0x02, /**< Phase A active power (W) */
    kMuPowerPhaseB = kEebusMeasurementMonitorPower | 0x03, /**< Phase B active power (W) */
    kMuPowerPhaseC = kEebusMeasurementMonitorPower | 0x04, /**< Phase C active power (W) */
    /* Energy */
    kMuEnergyConsumed = kEebusMeasurementMonitorEnergy | 0x01, /**< Consumed energy (Wh) */
    kMuEnergyProduced = kEebusMeasurementMonitorEnergy | 0x02, /**< Produced energy (Wh) — MU MPC only */
    kMuEnergyFeedIn   = kEebusMeasurementMonitorEnergy | 0x03, /**< Grid feed-in energy (Wh) — GCP MGCP only */
    /* Per-phase AC current */
    kMuCurrentPhaseA = kEebusMeasurementMonitorCurrent | 0x01, /**< Phase A RMS current (A) */
    kMuCurrentPhaseB = kEebusMeasurementMonitorCurrent | 0x02, /**< Phase B RMS current (A) */
    kMuCurrentPhaseC = kEebusMeasurementMonitorCurrent | 0x03, /**< Phase C RMS current (A) */
    /* Per-phase AC voltage */
    kMuVoltagePhaseA  = kEebusMeasurementMonitorVoltage | 0x01, /**< Phase A to neutral RMS voltage (V) */
    kMuVoltagePhaseB  = kEebusMeasurementMonitorVoltage | 0x02, /**< Phase B to neutral RMS voltage (V) */
    kMuVoltagePhaseC  = kEebusMeasurementMonitorVoltage | 0x03, /**< Phase C to neutral RMS voltage (V) */
    kMuVoltagePhaseAb = kEebusMeasurementMonitorVoltage | 0x04, /**< Phase A to B RMS voltage (V) */
    kMuVoltagePhaseBc = kEebusMeasurementMonitorVoltage | 0x05, /**< Phase B to C RMS voltage (V) */
    kMuVoltagePhaseAc = kEebusMeasurementMonitorVoltage | 0x06, /**< Phase C to A RMS voltage (V) */
    /* AC frequency */
    kMuFrequency = kEebusMeasurementMonitorFrequency | 0x01, /**< Grid frequency (Hz) */
};

typedef enum EebusMeasurementNameId EebusMeasurementNameId;

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_USE_CASE_MODEL_EEBUS_MEASUREMENT_TYPES_H_
