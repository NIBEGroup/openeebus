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
 * @brief Mu Mpc Monitor implementation
 */

#include "src/use_case/actor/mu/mpc/mu_mpc_monitor.h"

#include "src/common/array_util.h"
#include "src/common/eebus_malloc.h"
#include "src/use_case/actor/common/eebus_monitor_base.h"

//-------------------------------------------------------------------------------------------//
//
// MuMpcMonitorPower Object Creation (Scenario 1)
//
//-------------------------------------------------------------------------------------------//

static uint8_t GetConnectedPhases(const MuMpcMonitorPowerConfig* cfg) {
    ElectricalConnectionPhaseNameType connected_phases = 0;

    if (cfg->power_phase_a_cfg != NULL) {
        connected_phases |= kPhaseA;
    }

    if (cfg->power_phase_b_cfg != NULL) {
        connected_phases |= kPhaseB;
    }

    if (cfg->power_phase_c_cfg != NULL) {
        connected_phases |= kPhaseC;
    }

    return connected_phases;
}

EebusMonitorObject* MuMpcMonitorPowerCreate(const MuMpcMonitorPowerConfig* cfg) {
    if (cfg == NULL) {
        return NULL;
    }

    EebusMonitorBase* const base = (EebusMonitorBase*)EEBUS_MALLOC(sizeof(EebusMonitorBase));
    if (base == NULL) {
        return NULL;
    }

    if (EebusMonitorBaseConstruct(base, kMpcMonitorPower, kMpcMonitorNameIdMask, MuMpcMeasurementCreate)
        != kEebusErrorOk) {
        EebusMonitorDelete(EEBUS_MONITOR_OBJECT(base));
        return NULL;
    }

    const uint8_t connected_phases = GetConnectedPhases(cfg);
    EebusMeasurementObject* const power_total
        = MuMpcMeasurementPowerTotalCreate(connected_phases, &cfg->power_total_cfg);
    if (power_total == NULL) {
        EebusMonitorDelete(EEBUS_MONITOR_OBJECT(base));
        return NULL;
    }

    VectorPushBack(&base->measurements, power_total);

    if (connected_phases != 0) {
        const EebusMonitorMeasurementParam params[] = {
            {kMpcPowerPhaseA, cfg->power_phase_a_cfg},
            {kMpcPowerPhaseB, cfg->power_phase_b_cfg},
            {kMpcPowerPhaseC, cfg->power_phase_c_cfg},
        };

        if (EebusMonitorBaseAddMeasurements(base, params, ARRAY_SIZE(params)) != kEebusErrorOk) {
            EebusMonitorDelete(EEBUS_MONITOR_OBJECT(base));
            return NULL;
        }
    }

    return EEBUS_MONITOR_OBJECT(base);
}

//-------------------------------------------------------------------------------------------//
//
// MuMpcMonitorEnergy Object Creation (Scenario 2)
//
//-------------------------------------------------------------------------------------------//
EebusMonitorObject* MuMpcMonitorEnergyCreate(const MuMpcMonitorEnergyConfig* cfg) {
    if (cfg == NULL) {
        return NULL;
    }

    if ((cfg->energy_consumption_cfg == NULL) && (cfg->energy_production_cfg == NULL)) {
        return NULL;
    }

    const EebusMonitorMeasurementParam params[] = {
        {kMpcEnergyConsumed, cfg->energy_consumption_cfg},
        {kMpcEnergyProduced,  cfg->energy_production_cfg},
    };

    return EebusMonitorCreate(
        kMpcMonitorEnergy,
        kMpcMonitorNameIdMask,
        MuMpcMeasurementCreate,
        params,
        ARRAY_SIZE(params)
    );
}

//-------------------------------------------------------------------------------------------//
//
// MuMpcMonitorCurrent Object Creation (Scenario 3)
//
//-------------------------------------------------------------------------------------------//
EebusMonitorObject* MuMpcMonitorCurrentCreate(const MuMpcMonitorCurrentConfig* cfg) {
    if (cfg == NULL) {
        return NULL;
    }

    if ((cfg->current_phase_a_cfg == NULL) && (cfg->current_phase_b_cfg == NULL)
        && (cfg->current_phase_c_cfg == NULL)) {
        return NULL;
    }

    const EebusMonitorMeasurementParam params[] = {
        {kMpcCurrentPhaseA, cfg->current_phase_a_cfg},
        {kMpcCurrentPhaseB, cfg->current_phase_b_cfg},
        {kMpcCurrentPhaseC, cfg->current_phase_c_cfg},
    };

    return EebusMonitorCreate(
        kMpcMonitorCurrent,
        kMpcMonitorNameIdMask,
        MuMpcMeasurementCreate,
        params,
        ARRAY_SIZE(params)
    );
}

//-------------------------------------------------------------------------------------------//
//
// MuMpcMonitorVoltage Object Creation (Scenario 4)
//
//-------------------------------------------------------------------------------------------//
EebusMonitorObject* MuMpcMonitorVoltageCreate(const MuMpcMonitorVoltageConfig* cfg) {
    if (cfg == NULL) {
        return NULL;
    }

    if ((cfg->voltage_phase_ab_cfg != NULL)
        && ((cfg->voltage_phase_a_cfg == NULL) || (cfg->voltage_phase_b_cfg == NULL))) {
        return NULL;
    }

    if ((cfg->voltage_phase_bc_cfg != NULL)
        && ((cfg->voltage_phase_b_cfg == NULL) || (cfg->voltage_phase_c_cfg == NULL))) {
        return NULL;
    }

    if ((cfg->voltage_phase_ac_cfg != NULL)
        && ((cfg->voltage_phase_c_cfg == NULL) || (cfg->voltage_phase_a_cfg == NULL))) {
        return NULL;
    }

    const EebusMonitorMeasurementParam params[] = {
        { kMpcVoltagePhaseA,  cfg->voltage_phase_a_cfg},
        { kMpcVoltagePhaseB,  cfg->voltage_phase_b_cfg},
        { kMpcVoltagePhaseC,  cfg->voltage_phase_c_cfg},
        {kMpcVoltagePhaseAb, cfg->voltage_phase_ab_cfg},
        {kMpcVoltagePhaseBc, cfg->voltage_phase_bc_cfg},
        {kMpcVoltagePhaseAc, cfg->voltage_phase_ac_cfg},
    };

    return EebusMonitorCreate(
        kMpcMonitorVoltage,
        kMpcMonitorNameIdMask,
        MuMpcMeasurementCreate,
        params,
        ARRAY_SIZE(params)
    );
}

//-------------------------------------------------------------------------------------------//
//
// MuMpcMonitorFrequency Object Creation (Scenario 5)
//
//-------------------------------------------------------------------------------------------//
EebusMonitorObject* MuMpcMonitorFrequencyCreate(const MuMpcMonitorFrequencyConfig* cfg) {
    if (cfg == NULL) {
        return NULL;
    }

    const EebusMonitorMeasurementParam params[] = {
        {kMpcFrequency, &cfg->frequency_cfg},
    };

    return EebusMonitorCreate(
        kMpcMonitorFrequency,
        kMpcMonitorNameIdMask,
        MuMpcMeasurementCreate,
        params,
        ARRAY_SIZE(params)
    );
}
