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
 * @brief MU MPC Monitor implementation declarations.
 *
 * Use following functions to create a monitor per scenario.
 * Scenario 1: MuMpcMonitorPowerCreate();
 * Scenario 2: MuMpcMonitorEnergyCreate();
 * Scenario 3: MuMpcMonitorCurrentCreate();
 * Scenario 4: MuMpcMonitorVoltageCreate();
 * Scenario 5: MuMpcMonitorFrequencyCreate().
 *
 * Use EebusMonitorDelete() to delete the created MU MPC Monitor instance
 */

#ifndef SRC_USE_CASE_ACTOR_MU_MU_MPC_MONITOR_H_
#define SRC_USE_CASE_ACTOR_MU_MU_MPC_MONITOR_H_

#include <stddef.h>

#include "src/use_case/actor/common/eebus_monitor_base.h"
#include "src/use_case/actor/mu/mpc/mu_mpc_measurement.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef EebusMonitorCurrentConfig MuMpcMonitorCurrentConfig;

/**
 * MuMpcMonitorEnergyConfig is the configuration for the monitor use case
 * If this config is passed via NewMPC, the use case will support energy monitoring as specified
 */
typedef struct MuMpcMonitorEnergyConfig MuMpcMonitorEnergyConfig;

struct MuMpcMonitorEnergyConfig {
    /** The source of the production values (if not NULL is set, the use case will support production) */
    const MuMpcMeasurementConfig* energy_production_cfg;

    /**< The source of the consumption values (if not NULL use case will support consumption) */
    const MuMpcMeasurementConfig* energy_consumption_cfg;
};

typedef EebusMonitorFrequencyConfig MuMpcMonitorFrequencyConfig;

/**
 * MuMpcMonitorPowerConfig is the configuration for the monitor use case
 * This config is required by the mpc use case and must be used in mpc.NewMPC
 */
typedef struct MuMpcMonitorPowerConfig MuMpcMonitorPowerConfig;

struct MuMpcMonitorPowerConfig {
    /** The source of the values from the ac_power_total (required) */
    MuMpcMeasurementConfig power_total_cfg;

    /** Phase A AC Power measurement configuration (required if the phase is supported) */
    const MuMpcMeasurementConfig* power_phase_a_cfg;
    /** Phase B AC Power measurement configuration (required if the phase is supported) */
    const MuMpcMeasurementConfig* power_phase_b_cfg;
    /** Phase C AC Power measurement configuration (required if the phase is supported) */
    const MuMpcMeasurementConfig* power_phase_c_cfg;
};

typedef EebusMonitorVoltageConfig MuMpcMonitorVoltageConfig;

/**
 * @brief Creates a new EebusMonitorObject for the power monitoring (Scenario 1)
 * @param cfg The configuration for the power monitoring use case
 * @return A pointer to the created EebusMonitorObject, or NULL if creation failed
 */
EebusMonitorObject* MuMpcMonitorPowerCreate(const MuMpcMonitorPowerConfig* cfg);

/**
 * @brief Creates a new EebusMonitorObject for the energy monitoring (Scenario 2)
 * @param cfg The configuration for the energy monitoring use case
 * @return A pointer to the created EebusMonitorObject, or NULL if creation failed
 */
EebusMonitorObject* MuMpcMonitorEnergyCreate(const MuMpcMonitorEnergyConfig* cfg);

/**
 * @brief Creates a new EebusMonitorObject for the current monitoring (Scenario 3)
 * @param cfg The configuration for the current monitoring use case
 * @return A pointer to the created EebusMonitorObject, or NULL if creation failed
 */
EebusMonitorObject* MuMpcMonitorCurrentCreate(const MuMpcMonitorCurrentConfig* cfg);

/**
 * @brief Creates a new EebusMonitorObject for the voltage monitoring (Scenario 4)
 * @param cfg The configuration for the voltage monitoring use case
 * @return A pointer to the created EebusMonitorObject, or NULL if creation failed
 */
EebusMonitorObject* MuMpcMonitorVoltageCreate(const MuMpcMonitorVoltageConfig* cfg);

/**
 * @brief Creates a new EebusMonitorObject for the frequency monitoring (Scenario 5)
 * @param cfg The configuration for the frequency monitoring use case
 * @return A pointer to the created EebusMonitorObject, or NULL if creation failed
 */
EebusMonitorObject* MuMpcMonitorFrequencyCreate(const MuMpcMonitorFrequencyConfig* cfg);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_USE_CASE_ACTOR_MU_MU_MPC_MONITOR_H_
