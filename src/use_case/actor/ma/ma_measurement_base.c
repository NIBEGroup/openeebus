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
 * @brief Shared MA Measurement base implementation for MPC and MGCP monitoring use cases
 */

#include "src/use_case/actor/ma/ma_measurement_base.h"

#include "src/common/eebus_arguments.h"

#define MA_MEASUREMENT_BASE(obj) ((MaMeasurementBase*)(obj))

static EebusMeasurementNameId GetName(const MaMeasurementObject* self) {
    return MA_MEASUREMENT_BASE(self)->name;
}

static EebusError GetDataValue(
    const MaMeasurementObject* self,
    MeasurementClient* mcl,
    ElectricalConnectionClient* eccl,
    ScaledValue* measurement_value
) {
    const MaMeasurementBase* const measurement = MA_MEASUREMENT_BASE(self);
    return measurement->get_measurement_strategy(measurement, mcl, eccl, measurement_value);
}

static EebusError GetData(
    const MaMeasurementObject* self,
    EntityLocalObject* local_entity,
    EntityRemoteObject* remote_entity,
    ScaledValue* measurement_value
) {
    if ((local_entity == NULL) || (remote_entity == NULL)) {
        return kEebusErrorNoChange;
    }

    MeasurementClient mcl = {0};

    EebusError err = MeasurementClientConstruct(&mcl, local_entity, remote_entity);
    if (err != kEebusErrorOk) {
        return err;
    }

    ElectricalConnectionClient ecl = {0};

    err = ElectricalConnectionClientConstruct(&ecl, local_entity, remote_entity);
    if (err != kEebusErrorOk) {
        return err;
    }

    return GetDataValue(self, &mcl, &ecl, measurement_value);
}

const MaMeasurementInterface ma_measurement_methods = {
    .get_name = GetName,
    .get_data = GetData,
};

static bool PhasesMatch(
    const MaMeasurementBase* measurement,
    const ElectricalConnectionPhaseNameType* phases,
    const ElectricalConnectionPhaseNameType* in_reference_to
) {
    if (measurement->phases != NULL) {
        if ((phases == NULL) || (*measurement->phases != *phases)) {
            return false;
        }
    }

    if (measurement->in_reference_to != NULL) {
        if ((in_reference_to == NULL) || (*measurement->in_reference_to != *in_reference_to)) {
            return false;
        }
    }

    return true;
}

static bool MatchesTypeAndScopeAndPhases(
    const MaMeasurementBase* measurement,
    MeasurementTypeType measurement_type,
    ScopeTypeType scope,
    const ElectricalConnectionPhaseNameType* phases,
    const ElectricalConnectionPhaseNameType* in_reference_to
) {
    if ((measurement->measurement_type != measurement_type) || (measurement->scope != scope)) {
        return false;
    }

    return PhasesMatch(measurement, phases, in_reference_to);
}

static EebusError GetPhasesWithMeasurementId(
    const ElectricalConnectionClient* eccl,
    MeasurementIdType measurement_id,
    const ElectricalConnectionPhaseNameType** phases,
    const ElectricalConnectionPhaseNameType** in_reference_to
) {
    if ((phases == NULL) && (in_reference_to == NULL)) {
        return kEebusErrorInputArgumentNull;
    }

    const ElectricalConnectionParameterDescriptionDataType filter = {
        .measurement_id = &measurement_id,
    };

    const ElectricalConnectionParameterDescriptionDataType* const parameter_description
        = ElectricalConnectionCommonGetParameterDescriptionWithFilter(&eccl->el_connection_common, &filter);

    if (parameter_description == NULL) {
        return kEebusErrorNotAvailable;
    }

    *phases          = parameter_description->ac_measured_phases;
    *in_reference_to = parameter_description->ac_measured_in_reference_to;
    return kEebusErrorOk;
}

static bool CheckPhaseSpecificData(
    const MaMeasurementBase* measurement,
    const ElectricalConnectionClient* eccl,
    const EnergyDirectionType* energy_direction,
    const MeasurementDataType* item
) {
    if ((item->value == NULL) || (item->value->number == NULL) || (item->measurement_id == NULL)) {
        return false;
    }

    if (measurement->phases != NULL) {
        const ElectricalConnectionPhaseNameType* phases          = NULL;
        const ElectricalConnectionPhaseNameType* in_reference_to = NULL;
        if (GetPhasesWithMeasurementId(eccl, *item->measurement_id, &phases, &in_reference_to) != kEebusErrorOk) {
            return false;
        }

        if (!PhasesMatch(measurement, phases, in_reference_to)) {
            return false;
        }
    }

    if (energy_direction != NULL) {
        const ElectricalConnectionParameterDescriptionDataType filter = {
            .measurement_id = item->measurement_id,
        };

        const ElectricalConnectionDescriptionDataType* const description
            = ElectricalConnectionCommonGetDescriptionWithParameterDescriptionFilter(
                &eccl->el_connection_common,
                &filter
            );

        if (description == NULL) {
            return false;
        }

        if ((description->positive_energy_direction == NULL)
            || (*description->positive_energy_direction != *energy_direction)) {
            return false;
        }
    }

    if ((item->value_state != NULL) && (*item->value_state != kMeasurementValueStateTypeNormal)) {
        return false;
    }

    return true;
}

static EebusError GetPhaseSpecificData(
    const MaMeasurementBase* measurement,
    const MeasurementClient* mcl,
    const ElectricalConnectionClient* eccl,
    const EnergyDirectionType* energy_direction,
    ScaledValue* value
) {
    const MeasurementDescriptionDataType filter = {
        .measurement_type = &measurement->measurement_type,
        .commodity_type   = &(CommodityTypeType){kCommodityTypeTypeElectricity},
        .scope_type       = &measurement->scope,
    };

    EebusDataListMatchIterator it = {0};
    MeasurementCommonGetMeasurementDescriptionMatchFirst(&mcl->measurement_common, &filter, &it);

    for (; !EebusDataListMatchIteratorIsDone(&it); EebusDataListMatchIteratorNext(&it)) {
        const MeasurementDescriptionDataType* const description = EebusDataListMatchIteratorGet(&it);

        const MeasurementDataType filter2 = {
            .measurement_id = description->measurement_id,
        };

        const MeasurementListDataType* const measurement_list
            = MeasurementCommonGetMeasurements(&mcl->measurement_common);

        EebusDataListMatchIterator it2 = {0};
        HelperListMatchFirst(kFunctionTypeMeasurementListData, measurement_list, &filter2, &it2);

        for (; !EebusDataListMatchIteratorIsDone(&it2); EebusDataListMatchIteratorNext(&it2)) {
            const MeasurementDataType* const data = EebusDataListMatchIteratorGet(&it2);
            if (CheckPhaseSpecificData(measurement, eccl, energy_direction, data)) {
                return ScaledValueInitWithScaledNumber(value, data->value);
            }
        }
    }

    return kEebusErrorNotAvailable;
}

EebusError MaMeasurementGetPowerStrategy(
    const MaMeasurementBase* measurement,
    MeasurementClient* mcl,
    ElectricalConnectionClient* eccl,
    ScaledValue* value
) {
    static const EnergyDirectionType energy_direction = kEnergyDirectionTypeConsume;
    return GetPhaseSpecificData(measurement, mcl, eccl, &energy_direction, value);
}

EebusError MaMeasurementGetCurrentStrategy(
    const MaMeasurementBase* measurement,
    MeasurementClient* mcl,
    ElectricalConnectionClient* eccl,
    ScaledValue* value
) {
    static const EnergyDirectionType energy_direction = kEnergyDirectionTypeConsume;
    return GetPhaseSpecificData(measurement, mcl, eccl, &energy_direction, value);
}

EebusError MaMeasurementGetEnergyStrategy(
    const MaMeasurementBase* measurement,
    MeasurementClient* mcl,
    ElectricalConnectionClient* eccl,
    ScaledValue* value
) {
    UNUSED(eccl);

    const MeasurementDescriptionDataType filter = {
        .measurement_type = &measurement->measurement_type,
        .commodity_type   = &(CommodityTypeType){kCommodityTypeTypeElectricity},
        .scope_type       = &measurement->scope,
    };

    const MeasurementDataType* const measurement_data
        = MeasurementCommonGetMeasurementWithFilter(&mcl->measurement_common, &filter);
    if (measurement_data == NULL) {
        return kEebusErrorNotAvailable;
    }

    if ((measurement_data->value_state != NULL)
        && (*measurement_data->value_state != kMeasurementValueStateTypeNormal)) {
        return kEebusErrorInvalid;
    }

    return ScaledValueInitWithScaledNumber(value, measurement_data->value);
}

EebusError MaMeasurementGetVoltageStrategy(
    const MaMeasurementBase* measurement,
    MeasurementClient* mcl,
    ElectricalConnectionClient* eccl,
    ScaledValue* value
) {
    return GetPhaseSpecificData(measurement, mcl, eccl, NULL, value);
}

EebusError MaMeasurementGetFrequencyStrategy(
    const MaMeasurementBase* measurement,
    MeasurementClient* mcl,
    ElectricalConnectionClient* eccl,
    ScaledValue* value
) {
    UNUSED(measurement);
    UNUSED(eccl);

    const MeasurementDescriptionDataType filter = {
        .measurement_type = &(MeasurementTypeType){kMeasurementTypeTypeFrequency},
        .commodity_type   = &(CommodityTypeType){kCommodityTypeTypeElectricity},
        .scope_type       = &(ScopeTypeType){kScopeTypeTypeACFrequency},
    };

    const MeasurementDataType* const measurement_data
        = MeasurementCommonGetMeasurementWithFilter(&mcl->measurement_common, &filter);

    if (measurement_data == NULL) {
        return kEebusErrorNotAvailable;
    }

    if ((measurement_data->value_state != NULL)
        && (*measurement_data->value_state != kMeasurementValueStateTypeNormal)) {
        return kEebusErrorInvalid;
    }

    return ScaledValueInitWithScaledNumber(value, measurement_data->value);
}

const MaMeasurementObject* MaMeasurementGetInstance(
    const MaMeasurementBase* table,
    size_t table_size,
    const MeasurementClient* mcl,
    ElectricalConnectionClient* eccl,
    const MeasurementDataType* measurement_data
) {
    if ((measurement_data == NULL) || (measurement_data->measurement_id == NULL)) {
        return NULL;
    }

    const MeasurementDescriptionDataType* const description
        = MeasurementCommonGetMeasurementDescriptionWithId(&mcl->measurement_common, *measurement_data->measurement_id);

    if ((description == NULL) || (description->measurement_type == NULL) || (description->scope_type == NULL)) {
        return NULL;
    }

    const MeasurementTypeType measurement_type = *description->measurement_type;
    const ScopeTypeType scope                  = *description->scope_type;

    const ElectricalConnectionPhaseNameType* phases          = NULL;
    const ElectricalConnectionPhaseNameType* in_reference_to = NULL;
    if (GetPhasesWithMeasurementId(eccl, *measurement_data->measurement_id, &phases, &in_reference_to)
        != kEebusErrorOk) {
        return NULL;
    }

    for (size_t i = 0; i < table_size; ++i) {
        if (MatchesTypeAndScopeAndPhases(&table[i], measurement_type, scope, phases, in_reference_to)) {
            return MA_MEASUREMENT_OBJECT(&table[i]);
        }
    }

    return NULL;
}

const MaMeasurementObject*
MaMeasurementGetInstanceWithNameId(const MaMeasurementBase* table, size_t table_size, EebusMeasurementNameId name) {
    for (size_t i = 0; i < table_size; ++i) {
        if (table[i].name == name) {
            return MA_MEASUREMENT_OBJECT(&table[i]);
        }
    }

    return NULL;
}
