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
 * @brief MPC (MA and MU) utilities implementation
 */

#include <stddef.h>

#include "src/common/array_util.h"
#include "src/use_case/model/mpc_types.h"

typedef struct MpcNameMapping MpcNameMapping;

struct MpcNameMapping {
    const char* name;
    MuMpcMeasurementNameId id;
};

static const MpcNameMapping mpc_name_lut[] = {
    {     "power_total",     kMpcPowerTotal},
    {   "power_phase_a",    kMpcPowerPhaseA},
    {   "power_phase_b",    kMpcPowerPhaseB},
    {   "power_phase_c",    kMpcPowerPhaseC},
    { "energy_consumed", kMpcEnergyConsumed},
    { "energy_produced", kMpcEnergyProduced},
    { "current_phase_a",  kMpcCurrentPhaseA},
    { "current_phase_b",  kMpcCurrentPhaseB},
    { "current_phase_c",  kMpcCurrentPhaseC},
    { "voltage_phase_a",  kMpcVoltagePhaseA},
    { "voltage_phase_b",  kMpcVoltagePhaseB},
    { "voltage_phase_c",  kMpcVoltagePhaseC},
    {"voltage_phase_ab", kMpcVoltagePhaseAb},
    {"voltage_phase_bc", kMpcVoltagePhaseBc},
    {"voltage_phase_ac", kMpcVoltagePhaseAc},
    {       "frequency",      kMpcFrequency},
};

const MuMpcMeasurementNameId* MuMpcMeasurementGetNameId(const char* name) {
    if (name == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < ARRAY_SIZE(mpc_name_lut); ++i) {
        if (strcmp(mpc_name_lut[i].name, name) == 0) {
            return &mpc_name_lut[i].id;
        }
    }

    return NULL;
}

const char* MuMpcMeasurementGetName(MuMpcMeasurementNameId state) {
    for (size_t i = 0; i < ARRAY_SIZE(mpc_name_lut); ++i) {
        if (mpc_name_lut[i].id == state) {
            return mpc_name_lut[i].name;
        }
    }

    return NULL;
}
