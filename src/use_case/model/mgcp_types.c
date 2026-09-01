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
 * @brief MGCP (MA and GCP) utilities implementation
 */

#include <stddef.h>
#include <string.h>

#include "src/common/array_util.h"
#include "src/use_case/model/mgcp_types.h"

typedef struct MgcpNameMapping MgcpNameMapping;

struct MgcpNameMapping {
    const char* name;
    GcpMeasurementNameId id;
};

static const MgcpNameMapping mgcp_name_lut[] = {
    {     "power_total",     kGcpPowerTotal},
    {  "energy_feed_in",   kGcpEnergyFeedIn},
    { "energy_consumed", kGcpEnergyConsumed},
    { "current_phase_a",  kGcpCurrentPhaseA},
    { "current_phase_b",  kGcpCurrentPhaseB},
    { "current_phase_c",  kGcpCurrentPhaseC},
    { "voltage_phase_a",  kGcpVoltagePhaseA},
    { "voltage_phase_b",  kGcpVoltagePhaseB},
    { "voltage_phase_c",  kGcpVoltagePhaseC},
    {"voltage_phase_ab", kGcpVoltagePhaseAb},
    {"voltage_phase_bc", kGcpVoltagePhaseBc},
    {"voltage_phase_ac", kGcpVoltagePhaseAc},
    {       "frequency",      kGcpFrequency},
};

const GcpMeasurementNameId* GcpMgcpMeasurementGetNameId(const char* name) {
    if (name == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < ARRAY_SIZE(mgcp_name_lut); ++i) {
        if (strcmp(mgcp_name_lut[i].name, name) == 0) {
            return &mgcp_name_lut[i].id;
        }
    }

    return NULL;
}

const char* GcpMgcpMeasurementGetName(GcpMeasurementNameId id) {
    for (size_t i = 0; i < ARRAY_SIZE(mgcp_name_lut); ++i) {
        if (mgcp_name_lut[i].id == id) {
            return mgcp_name_lut[i].name;
        }
    }

    return NULL;
}
