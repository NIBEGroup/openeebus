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
 * @brief GCP MGCP Monitor implementation
 */

#include "src/use_case/actor/gcp/mgcp/gcp_mgcp_monitor.h"

#include "src/common/array_util.h"
#include "src/common/eebus_malloc.h"
#include "src/use_case/actor/common/eebus_monitor_base.h"

EebusMonitorObject* GcpMgcpMonitorPowerCreate(const GcpMgcpMonitorPowerConfig* cfg) {
    if (cfg == NULL) {
        return NULL;
    }

    EebusMonitorBase* const base = (EebusMonitorBase*)EEBUS_MALLOC(sizeof(EebusMonitorBase));
    if (base == NULL) {
        return NULL;
    }

    if (EebusMonitorBaseConstruct(base, kGcpMonitorPower, kGcpMonitorNameIdMask, GcpMgcpMeasurementCreate)
        != kEebusErrorOk) {
        EebusMonitorDelete(EEBUS_MONITOR_OBJECT(base));
        return NULL;
    }

    EebusMeasurementObject* const power_total = GcpMgcpMeasurementPowerTotalCreate(cfg->phases, &cfg->power_total_cfg);
    if (power_total == NULL) {
        EebusMonitorDelete(EEBUS_MONITOR_OBJECT(base));
        return NULL;
    }

    VectorPushBack(&base->measurements, power_total);
    return EEBUS_MONITOR_OBJECT(base);
}

EebusMonitorObject* GcpMgcpMonitorEnergyCreate(const GcpMgcpMonitorEnergyConfig* cfg) {
    if (cfg == NULL) {
        return NULL;
    }

    if ((cfg->energy_feed_in_cfg == NULL) && (cfg->energy_consumed_cfg == NULL)) {
        return NULL;
    }

    const EebusMonitorMeasurementParam params[] = {
        {  kGcpEnergyFeedIn,  cfg->energy_feed_in_cfg},
        {kGcpEnergyConsumed, cfg->energy_consumed_cfg},
    };

    return EebusMonitorCreate(
        kGcpMonitorEnergy,
        kGcpMonitorNameIdMask,
        GcpMgcpMeasurementCreate,
        params,
        ARRAY_SIZE(params)
    );
}

EebusMonitorObject* GcpMgcpMonitorCurrentCreate(const GcpMgcpMonitorCurrentConfig* cfg) {
    if (cfg == NULL) {
        return NULL;
    }

    if ((cfg->current_phase_a_cfg == NULL) && (cfg->current_phase_b_cfg == NULL)
        && (cfg->current_phase_c_cfg == NULL)) {
        return NULL;
    }

    const EebusMonitorMeasurementParam params[] = {
        {kGcpCurrentPhaseA, cfg->current_phase_a_cfg},
        {kGcpCurrentPhaseB, cfg->current_phase_b_cfg},
        {kGcpCurrentPhaseC, cfg->current_phase_c_cfg},
    };

    return EebusMonitorCreate(
        kGcpMonitorCurrent,
        kGcpMonitorNameIdMask,
        GcpMgcpMeasurementCreate,
        params,
        ARRAY_SIZE(params)
    );
}

EebusMonitorObject* GcpMgcpMonitorVoltageCreate(const GcpMgcpMonitorVoltageConfig* cfg) {
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
        && ((cfg->voltage_phase_a_cfg == NULL) || (cfg->voltage_phase_c_cfg == NULL))) {
        return NULL;
    }

    const EebusMonitorMeasurementParam params[] = {
        { kGcpVoltagePhaseA,  cfg->voltage_phase_a_cfg},
        { kGcpVoltagePhaseB,  cfg->voltage_phase_b_cfg},
        { kGcpVoltagePhaseC,  cfg->voltage_phase_c_cfg},
        {kGcpVoltagePhaseAb, cfg->voltage_phase_ab_cfg},
        {kGcpVoltagePhaseBc, cfg->voltage_phase_bc_cfg},
        {kGcpVoltagePhaseAc, cfg->voltage_phase_ac_cfg},
    };

    return EebusMonitorCreate(
        kGcpMonitorVoltage,
        kGcpMonitorNameIdMask,
        GcpMgcpMeasurementCreate,
        params,
        ARRAY_SIZE(params)
    );
}

EebusMonitorObject* GcpMgcpMonitorFrequencyCreate(const GcpMgcpMonitorFrequencyConfig* cfg) {
    if (cfg == NULL) {
        return NULL;
    }

    const EebusMonitorMeasurementParam params[] = {
        {kGcpFrequency, &cfg->frequency_cfg},
    };

    return EebusMonitorCreate(
        kGcpMonitorFrequency,
        kGcpMonitorNameIdMask,
        GcpMgcpMeasurementCreate,
        params,
        ARRAY_SIZE(params)
    );
}
