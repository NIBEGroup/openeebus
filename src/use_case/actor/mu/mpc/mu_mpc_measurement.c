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

#include "src/use_case/actor/mu/mpc/mu_mpc_measurement.h"

static ScopeTypeType GetEnergyScopeType(MuMpcMeasurementNameId name) {
    switch (name) {
        case kMpcEnergyConsumed: return kScopeTypeTypeACEnergyConsumed;
        case kMpcEnergyProduced: return kScopeTypeTypeACEnergyProduced;
        default: return kScopeTypeTypeACEnergy;
    }
}

static ElectricalConnectionPhaseNameType GetPowerPhase(MuMpcMeasurementNameId name) {
    switch (name) {
        case kMpcPowerPhaseA: return kElectricalConnectionPhaseNameTypeA;
        case kMpcPowerPhaseB: return kElectricalConnectionPhaseNameTypeB;
        case kMpcPowerPhaseC: return kElectricalConnectionPhaseNameTypeC;
        default: return kElectricalConnectionPhaseNameTypeNone;
    }
}

static ElectricalConnectionPhaseNameType GetCurrentPhase(MuMpcMeasurementNameId name) {
    switch (name) {
        case kMpcCurrentPhaseA: return kElectricalConnectionPhaseNameTypeA;
        case kMpcCurrentPhaseB: return kElectricalConnectionPhaseNameTypeB;
        case kMpcCurrentPhaseC: return kElectricalConnectionPhaseNameTypeC;
        default: return kElectricalConnectionPhaseNameTypeNone;
    }
}

static ElectricalConnectionPhaseNameType GetVoltagePhase(MuMpcMeasurementNameId name) {
    switch (name) {
        case kMpcVoltagePhaseA: return kElectricalConnectionPhaseNameTypeA;
        case kMpcVoltagePhaseB: return kElectricalConnectionPhaseNameTypeB;
        case kMpcVoltagePhaseC: return kElectricalConnectionPhaseNameTypeC;
        case kMpcVoltagePhaseAb: return kElectricalConnectionPhaseNameTypeAb;
        case kMpcVoltagePhaseBc: return kElectricalConnectionPhaseNameTypeBc;
        case kMpcVoltagePhaseAc: return kElectricalConnectionPhaseNameTypeAc;
        default: return kElectricalConnectionPhaseNameTypeNone;
    }
}

EebusMeasurementObject*
MuMpcMeasurementPowerTotalCreate(ElectricalConnectionPhaseNameType phases, const MuMpcMeasurementConfig* cfg) {
    return EebusMeasurementBaseCreate(
        kMpcPowerTotal,
        kScopeTypeTypeACPowerTotal,
        phases,
        cfg,
        EebusMeasurementBaseConfigurePower
    );
}

EebusMeasurementObject* MuMpcMeasurementCreate(MuMpcMeasurementNameId name, const MuMpcMeasurementConfig* cfg) {
    ScopeTypeType scope                        = (ScopeTypeType)0;
    ElectricalConnectionPhaseNameType phase    = (ElectricalConnectionPhaseNameType)0;
    EebusMeasurementConfigureStrategy strategy = NULL;
    const int32_t measurement_group            = (int32_t)name & (int32_t)kMpcMonitorNameIdMask;

    if (measurement_group == kMpcMonitorPower) {
        if (name == kMpcPowerTotal) {
            // Use MuMpcMeasurementPowerTotalCreate() instead
            return NULL;
        }

        scope    = kScopeTypeTypeACPower;
        phase    = GetPowerPhase(name);
        strategy = EebusMeasurementBaseConfigurePower;
    } else if (measurement_group == kMpcMonitorEnergy) {
        scope    = GetEnergyScopeType(name);
        phase    = kElectricalConnectionPhaseNameTypeNone;
        strategy = EebusMeasurementBaseConfigureEnergy;
    } else if (measurement_group == kMpcMonitorCurrent) {
        scope    = kScopeTypeTypeACCurrent;
        phase    = GetCurrentPhase(name);
        strategy = EebusMeasurementBaseConfigureCurrent;
    } else if (measurement_group == kMpcMonitorVoltage) {
        scope    = kScopeTypeTypeACVoltage;
        phase    = GetVoltagePhase(name);
        strategy = EebusMeasurementBaseConfigureVoltage;
    } else if (measurement_group == kMpcMonitorFrequency) {
        scope    = kScopeTypeTypeACFrequency;
        phase    = kElectricalConnectionPhaseNameTypeNone;
        strategy = EebusMeasurementBaseConfigureFrequency;
    } else {
        return NULL;
    }

    return EebusMeasurementBaseCreate(name, scope, phase, cfg, strategy);
}
