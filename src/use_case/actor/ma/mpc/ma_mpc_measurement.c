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
 * @brief Ma Mpc Measurement implementation
 */

#include "src/use_case/actor/ma/mpc/ma_mpc_measurement.h"

#include "src/common/array_util.h"
#include "src/use_case/actor/ma/ma_measurement_base.h"
#include "src/use_case/model/mpc_types.h"

#define MA_MPC_MEASUREMENT_POWER_TOTAL                                       \
    {                                                                        \
        .obj                      = {.interface_ = &ma_measurement_methods}, \
        .name                     = kMpcPowerTotal,                          \
        .measurement_type         = kMeasurementTypeTypePower,               \
        .scope                    = kScopeTypeTypeACPowerTotal,              \
        .phases                   = NULL,                                    \
        .in_reference_to          = NULL,                                    \
        .get_measurement_strategy = MaMeasurementGetPowerStrategy,           \
    }

#define MA_MPC_MEASUREMENT_POWER(name_id, phase)                                                                     \
    {                                                                                                                \
        .obj                      = {.interface_ = &ma_measurement_methods},                                         \
        .name                     = name_id,                                                                         \
        .measurement_type         = kMeasurementTypeTypePower,                                                       \
        .scope                    = kScopeTypeTypeACPower,                                                           \
        .phases                   = &(ElectricalConnectionPhaseNameType){kElectricalConnectionPhaseNameType##phase}, \
        .in_reference_to          = NULL,                                                                            \
        .get_measurement_strategy = MaMeasurementGetPowerStrategy,                                                   \
    }

#define MA_MPC_MEASUREMENT_ENERGY(name_id, energy_scope)                     \
    {                                                                        \
        .obj                      = {.interface_ = &ma_measurement_methods}, \
        .name                     = name_id,                                 \
        .measurement_type         = kMeasurementTypeTypeEnergy,              \
        .scope                    = energy_scope,                            \
        .phases                   = NULL,                                    \
        .in_reference_to          = NULL,                                    \
        .get_measurement_strategy = MaMeasurementGetEnergyStrategy,          \
    }

#define MA_MPC_MEASUREMENT_CURRENT(name_id, phase)                                                                   \
    {                                                                                                                \
        .obj                      = {.interface_ = &ma_measurement_methods},                                         \
        .name                     = name_id,                                                                         \
        .measurement_type         = kMeasurementTypeTypeCurrent,                                                     \
        .scope                    = kScopeTypeTypeACCurrent,                                                         \
        .phases                   = &(ElectricalConnectionPhaseNameType){kElectricalConnectionPhaseNameType##phase}, \
        .in_reference_to          = NULL,                                                                            \
        .get_measurement_strategy = MaMeasurementGetCurrentStrategy,                                                 \
    }

#define MA_MPC_MEASUREMENT_VOLTAGE(name_id, phase, ref_phase)                                                            \
    {                                                                                                                    \
        .obj                      = {.interface_ = &ma_measurement_methods},                                             \
        .name                     = name_id,                                                                             \
        .measurement_type         = kMeasurementTypeTypeVoltage,                                                         \
        .scope                    = kScopeTypeTypeACVoltage,                                                             \
        .phases                   = &(ElectricalConnectionPhaseNameType){kElectricalConnectionPhaseNameType##phase},     \
        .in_reference_to          = &(ElectricalConnectionPhaseNameType){kElectricalConnectionPhaseNameType##ref_phase}, \
        .get_measurement_strategy = MaMeasurementGetVoltageStrategy,                                                     \
    }

#define MA_MPC_MEASUREMENT_FREQUENCY                                         \
    {                                                                        \
        .obj                      = {.interface_ = &ma_measurement_methods}, \
        .name                     = kMpcFrequency,                           \
        .measurement_type         = kMeasurementTypeTypeFrequency,           \
        .scope                    = kScopeTypeTypeACFrequency,               \
        .phases                   = NULL,                                    \
        .in_reference_to          = NULL,                                    \
        .get_measurement_strategy = MaMeasurementGetFrequencyStrategy,       \
    }

static const MaMeasurementBase measurement_table[] = {
    MA_MPC_MEASUREMENT_POWER_TOTAL,
    MA_MPC_MEASUREMENT_POWER(kMpcPowerPhaseA, A),
    MA_MPC_MEASUREMENT_POWER(kMpcPowerPhaseB, B),
    MA_MPC_MEASUREMENT_POWER(kMpcPowerPhaseC, C),
    MA_MPC_MEASUREMENT_ENERGY(kMpcEnergyConsumed, kScopeTypeTypeACEnergyConsumed),
    MA_MPC_MEASUREMENT_ENERGY(kMpcEnergyProduced, kScopeTypeTypeACEnergyProduced),
    MA_MPC_MEASUREMENT_CURRENT(kMpcCurrentPhaseA, A),
    MA_MPC_MEASUREMENT_CURRENT(kMpcCurrentPhaseB, B),
    MA_MPC_MEASUREMENT_CURRENT(kMpcCurrentPhaseC, C),
    MA_MPC_MEASUREMENT_VOLTAGE(kMpcVoltagePhaseA, A, Neutral),
    MA_MPC_MEASUREMENT_VOLTAGE(kMpcVoltagePhaseB, B, Neutral),
    MA_MPC_MEASUREMENT_VOLTAGE(kMpcVoltagePhaseC, C, Neutral),
    MA_MPC_MEASUREMENT_VOLTAGE(kMpcVoltagePhaseAb, A, B),
    MA_MPC_MEASUREMENT_VOLTAGE(kMpcVoltagePhaseBc, B, C),
    MA_MPC_MEASUREMENT_VOLTAGE(kMpcVoltagePhaseAc, C, A),
    MA_MPC_MEASUREMENT_FREQUENCY,
};

const MaMeasurementObject* MaMpcMeasurementGetInstance(
    const MeasurementClient* mcl,
    ElectricalConnectionClient* eccl,
    const MeasurementDataType* measurement_data
) {
    return MaMeasurementGetInstance(measurement_table, ARRAY_SIZE(measurement_table), mcl, eccl, measurement_data);
}

const MaMeasurementObject* MaMpcMeasurementGetInstanceWithNameId(EebusMeasurementNameId name) {
    return MaMeasurementGetInstanceWithNameId(measurement_table, ARRAY_SIZE(measurement_table), name);
}
