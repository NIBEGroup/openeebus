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
 * @brief MA MGCP Measurement implementation
 */

#include "src/use_case/actor/ma/mgcp/ma_mgcp_measurement.h"

#include "src/common/array_util.h"
#include "src/use_case/actor/ma/ma_measurement_base.h"
#include "src/use_case/model/mgcp_types.h"

#define MA_MGCP_MEASUREMENT_POWER_TOTAL                                      \
    {                                                                        \
        .obj                      = {.interface_ = &ma_measurement_methods}, \
        .name                     = kGcpPowerTotal,                          \
        .measurement_type         = kMeasurementTypeTypePower,               \
        .scope                    = kScopeTypeTypeACPowerTotal,              \
        .phases                   = NULL,                                    \
        .in_reference_to          = NULL,                                    \
        .get_measurement_strategy = MaMeasurementGetPowerStrategy,           \
    }

#define MA_MGCP_MEASUREMENT_ENERGY(name_id, energy_scope)                    \
    {                                                                        \
        .obj                      = {.interface_ = &ma_measurement_methods}, \
        .name                     = name_id,                                 \
        .measurement_type         = kMeasurementTypeTypeEnergy,              \
        .scope                    = energy_scope,                            \
        .phases                   = NULL,                                    \
        .in_reference_to          = NULL,                                    \
        .get_measurement_strategy = MaMeasurementGetEnergyStrategy,          \
    }

#define MA_MGCP_MEASUREMENT_CURRENT(name_id, phase)                                                                  \
    {                                                                                                                \
        .obj                      = {.interface_ = &ma_measurement_methods},                                         \
        .name                     = name_id,                                                                         \
        .measurement_type         = kMeasurementTypeTypeCurrent,                                                     \
        .scope                    = kScopeTypeTypeACCurrent,                                                         \
        .phases                   = &(ElectricalConnectionPhaseNameType){kElectricalConnectionPhaseNameType##phase}, \
        .in_reference_to          = NULL,                                                                            \
        .get_measurement_strategy = MaMeasurementGetCurrentStrategy,                                                 \
    }

#define MA_MGCP_MEASUREMENT_VOLTAGE(name_id, phase, ref_phase)                                                           \
    {                                                                                                                    \
        .obj                      = {.interface_ = &ma_measurement_methods},                                             \
        .name                     = name_id,                                                                             \
        .measurement_type         = kMeasurementTypeTypeVoltage,                                                         \
        .scope                    = kScopeTypeTypeACVoltage,                                                             \
        .phases                   = &(ElectricalConnectionPhaseNameType){kElectricalConnectionPhaseNameType##phase},     \
        .in_reference_to          = &(ElectricalConnectionPhaseNameType){kElectricalConnectionPhaseNameType##ref_phase}, \
        .get_measurement_strategy = MaMeasurementGetVoltageStrategy,                                                     \
    }

#define MA_MGCP_MEASUREMENT_FREQUENCY                                        \
    {                                                                        \
        .obj                      = {.interface_ = &ma_measurement_methods}, \
        .name                     = kGcpFrequency,                           \
        .measurement_type         = kMeasurementTypeTypeFrequency,           \
        .scope                    = kScopeTypeTypeACFrequency,               \
        .phases                   = NULL,                                    \
        .in_reference_to          = NULL,                                    \
        .get_measurement_strategy = MaMeasurementGetFrequencyStrategy,       \
    }

static const MaMeasurementBase measurement_table[] = {
    /* Scenario 2 */
    MA_MGCP_MEASUREMENT_POWER_TOTAL,
    /* Scenario 3 */
    MA_MGCP_MEASUREMENT_ENERGY(kGcpEnergyFeedIn, kScopeTypeTypeGridFeedIn),
    /* Scenario 4 */
    MA_MGCP_MEASUREMENT_ENERGY(kGcpEnergyConsumed, kScopeTypeTypeGridConsumption),
    /* Scenario 5 */
    MA_MGCP_MEASUREMENT_CURRENT(kGcpCurrentPhaseA, A),
    MA_MGCP_MEASUREMENT_CURRENT(kGcpCurrentPhaseB, B),
    MA_MGCP_MEASUREMENT_CURRENT(kGcpCurrentPhaseC, C),
    /* Scenario 6 */
    MA_MGCP_MEASUREMENT_VOLTAGE(kGcpVoltagePhaseA, A, Neutral),
    MA_MGCP_MEASUREMENT_VOLTAGE(kGcpVoltagePhaseB, B, Neutral),
    MA_MGCP_MEASUREMENT_VOLTAGE(kGcpVoltagePhaseC, C, Neutral),
    MA_MGCP_MEASUREMENT_VOLTAGE(kGcpVoltagePhaseAb, A, B),
    MA_MGCP_MEASUREMENT_VOLTAGE(kGcpVoltagePhaseBc, B, C),
    MA_MGCP_MEASUREMENT_VOLTAGE(kGcpVoltagePhaseAc, C, A),
    /* Scenario 7 */
    MA_MGCP_MEASUREMENT_FREQUENCY,
};

const MaMeasurementObject* MaMgcpMeasurementGetInstance(
    const MeasurementClient* mcl,
    ElectricalConnectionClient* eccl,
    const MeasurementDataType* measurement_data
) {
    return MaMeasurementGetInstance(measurement_table, ARRAY_SIZE(measurement_table), mcl, eccl, measurement_data);
}

const MaMeasurementObject* MaMgcpMeasurementGetInstanceWithNameId(EebusMeasurementNameId name) {
    return MaMeasurementGetInstanceWithNameId(measurement_table, ARRAY_SIZE(measurement_table), name);
}
