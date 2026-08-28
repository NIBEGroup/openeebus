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
 * @brief Shared MA Measurement base for MPC and MGCP monitoring use cases
 */

#ifndef SRC_USE_CASE_ACTOR_MA_MA_MEASUREMENT_BASE_H_
#define SRC_USE_CASE_ACTOR_MA_MA_MEASUREMENT_BASE_H_

#include <stddef.h>

#include "src/use_case/api/ma_measurement_interface.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct MaMeasurementBase MaMeasurementBase;

typedef EebusError (*MaGetMeasurementDataStrategy)(
    const MaMeasurementBase* measurement,
    MeasurementClient* mcl,
    ElectricalConnectionClient* eccl,
    ScaledValue* value
);

struct MaMeasurementBase {
    MaMeasurementObject obj;
    EebusMeasurementNameId name;
    MeasurementTypeType measurement_type;
    ScopeTypeType scope;
    const ElectricalConnectionPhaseNameType* phases;
    const ElectricalConnectionPhaseNameType* in_reference_to;
    MaGetMeasurementDataStrategy get_measurement_strategy;
};

extern const MaMeasurementInterface ma_measurement_methods;

EebusError MaMeasurementGetPowerStrategy(
    const MaMeasurementBase* measurement,
    MeasurementClient* mcl,
    ElectricalConnectionClient* eccl,
    ScaledValue* value
);
EebusError MaMeasurementGetCurrentStrategy(
    const MaMeasurementBase* measurement,
    MeasurementClient* mcl,
    ElectricalConnectionClient* eccl,
    ScaledValue* value
);
EebusError MaMeasurementGetEnergyStrategy(
    const MaMeasurementBase* measurement,
    MeasurementClient* mcl,
    ElectricalConnectionClient* eccl,
    ScaledValue* value
);
EebusError MaMeasurementGetVoltageStrategy(
    const MaMeasurementBase* measurement,
    MeasurementClient* mcl,
    ElectricalConnectionClient* eccl,
    ScaledValue* value
);
EebusError MaMeasurementGetFrequencyStrategy(
    const MaMeasurementBase* measurement,
    MeasurementClient* mcl,
    ElectricalConnectionClient* eccl,
    ScaledValue* value
);

const MaMeasurementObject* MaMeasurementGetInstance(
    const MaMeasurementBase* table,
    size_t table_size,
    const MeasurementClient* mcl,
    ElectricalConnectionClient* eccl,
    const MeasurementDataType* measurement_data
);

const MaMeasurementObject*
MaMeasurementGetInstanceWithNameId(const MaMeasurementBase* table, size_t table_size, EebusMeasurementNameId name);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_USE_CASE_ACTOR_MA_MA_MEASUREMENT_BASE_H_
