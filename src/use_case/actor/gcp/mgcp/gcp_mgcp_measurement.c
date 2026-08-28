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
 * @brief GCP MGCP Measurement implementation
 */

#include "src/use_case/actor/gcp/mgcp/gcp_mgcp_measurement.h"

static ScopeTypeType GetEnergyScopeType(GcpMeasurementNameId name) {
    switch (name) {
        case kGcpEnergyFeedIn: return kScopeTypeTypeGridFeedIn;
        case kGcpEnergyConsumed: return kScopeTypeTypeGridConsumption;
        default: return kScopeTypeTypeACEnergy;
    }
}

static ElectricalConnectionPhaseNameType GetCurrentPhase(GcpMeasurementNameId name) {
    switch (name) {
        case kGcpCurrentPhaseA: return kElectricalConnectionPhaseNameTypeA;
        case kGcpCurrentPhaseB: return kElectricalConnectionPhaseNameTypeB;
        case kGcpCurrentPhaseC: return kElectricalConnectionPhaseNameTypeC;
        default: return kElectricalConnectionPhaseNameTypeNone;
    }
}

static ElectricalConnectionPhaseNameType GetVoltagePhase(GcpMeasurementNameId name) {
    switch (name) {
        case kGcpVoltagePhaseA: return kElectricalConnectionPhaseNameTypeA;
        case kGcpVoltagePhaseB: return kElectricalConnectionPhaseNameTypeB;
        case kGcpVoltagePhaseC: return kElectricalConnectionPhaseNameTypeC;
        case kGcpVoltagePhaseAb: return kElectricalConnectionPhaseNameTypeAb;
        case kGcpVoltagePhaseBc: return kElectricalConnectionPhaseNameTypeBc;
        case kGcpVoltagePhaseAc: return kElectricalConnectionPhaseNameTypeAc;
        default: return kElectricalConnectionPhaseNameTypeNone;
    }
}

EebusMeasurementObject*
GcpMgcpMeasurementPowerTotalCreate(ElectricalConnectionPhaseNameType phases, const GcpMgcpMeasurementConfig* cfg) {
    return EebusMeasurementBaseCreate(
        kGcpPowerTotal,
        kScopeTypeTypeACPowerTotal,
        phases,
        cfg,
        EebusMeasurementBaseConfigurePower
    );
}

EebusMeasurementObject* GcpMgcpMeasurementCreate(GcpMeasurementNameId name, const GcpMgcpMeasurementConfig* cfg) {
    ScopeTypeType scope                        = (ScopeTypeType)0;
    ElectricalConnectionPhaseNameType phase    = (ElectricalConnectionPhaseNameType)0;
    EebusMeasurementConfigureStrategy strategy = NULL;
    const int32_t measurement_group            = (int32_t)name & (int32_t)kGcpMonitorNameIdMask;

    if (measurement_group == kGcpMonitorPower) {
        // kGcpPowerTotal is the only power measurement; use GcpMgcpMeasurementPowerTotalCreate() instead
        return NULL;
    } else if (measurement_group == kGcpMonitorEnergy) {
        scope    = GetEnergyScopeType(name);
        phase    = kElectricalConnectionPhaseNameTypeNone;
        strategy = EebusMeasurementBaseConfigureEnergy;
    } else if (measurement_group == kGcpMonitorCurrent) {
        scope    = kScopeTypeTypeACCurrent;
        phase    = GetCurrentPhase(name);
        strategy = EebusMeasurementBaseConfigureCurrent;
    } else if (measurement_group == kGcpMonitorVoltage) {
        scope    = kScopeTypeTypeACVoltage;
        phase    = GetVoltagePhase(name);
        strategy = EebusMeasurementBaseConfigureVoltage;
    } else if (measurement_group == kGcpMonitorFrequency) {
        scope    = kScopeTypeTypeACFrequency;
        phase    = kElectricalConnectionPhaseNameTypeNone;
        strategy = EebusMeasurementBaseConfigureFrequency;
    } else {
        return NULL;
    }

    return EebusMeasurementBaseCreate(name, scope, phase, cfg, strategy);
}
