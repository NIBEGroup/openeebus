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
 * @brief Shared measurement base implementation for MU MPC and GCP MGCP use cases
 */

#include "src/use_case/actor/common/eebus_measurement_base.h"

#include "src/common/eebus_malloc.h"
#include "src/spine/model/absolute_or_relative_time.h"
#include "src/spine/model/measurement_types.h"

struct EebusMeasurementBase {
    EebusMeasurementObject obj;

    EebusMeasurementNameId name;
    MeasurementIdType id;
    ScopeTypeType scope;
    ElectricalConnectionPhaseNameType phases;
    MeasurementValueSourceType value_source;
    MeasurementConstraintsDataType* constraints;
    EebusMeasurementConfigureStrategy cfg_strategy;
    MeasurementDataType* measurement_data;
};

#define BASE(obj) ((EebusMeasurementBase*)(obj))

static void Destruct(EebusMeasurementObject* self);
static EebusMeasurementNameId GetName(const EebusMeasurementObject* self);
static EebusError GetDataValue(const EebusMeasurementObject* self, MeasurementServer* msrv, ScaledValue* value);
static const MeasurementConstraintsDataType* GetConstraints(const EebusMeasurementObject* self);
static EebusError Configure(
    EebusMeasurementObject* self,
    MeasurementServer* msrv,
    ElectricalConnectionServer* ecsrv,
    ElectricalConnectionIdType ec_id
);
static EebusError SetDataCache(
    EebusMeasurementObject* self,
    const ScaledValue* value,
    const EebusDateTime* timestamp,
    const MeasurementValueStateType* value_state,
    const EebusDateTime* start_time,
    const EebusDateTime* end_time
);
static MeasurementDataType* ReleaseDataCache(EebusMeasurementObject* self);

static const EebusMeasurementInterface measurement_base_methods = {
    .destruct           = Destruct,
    .get_name           = GetName,
    .get_data_value     = GetDataValue,
    .get_constraints    = GetConstraints,
    .configure          = Configure,
    .set_data_cache     = SetDataCache,
    .release_data_cache = ReleaseDataCache,
};

static EebusError Construct(
    EebusMeasurementBase* self,
    EebusMeasurementNameId name,
    ScopeTypeType scope,
    ElectricalConnectionPhaseNameType phases,
    const EebusMeasurementBaseConfig* cfg,
    EebusMeasurementConfigureStrategy strategy
);
static ElectricalConnectionPhaseNameType VoltagePhaseFrom(ElectricalConnectionPhaseNameType phases);
static ElectricalConnectionPhaseNameType VoltagePhaseTo(ElectricalConnectionPhaseNameType phases);

static EebusError Construct(
    EebusMeasurementBase* self,
    EebusMeasurementNameId name,
    ScopeTypeType scope,
    ElectricalConnectionPhaseNameType phases,
    const EebusMeasurementBaseConfig* cfg,
    EebusMeasurementConfigureStrategy strategy
) {
    EEBUS_MEASUREMENT_INTERFACE(self) = &measurement_base_methods;

    self->name             = name;
    self->id               = 0;
    self->scope            = scope;
    self->phases           = phases;
    self->value_source     = 0;
    self->constraints      = NULL;
    self->cfg_strategy     = strategy;
    self->measurement_data = NULL;

    if (cfg == NULL) {
        return kEebusErrorInputArgumentNull;
    }

    self->value_source = cfg->value_source;

    if (cfg->constraints != NULL) {
        self->constraints = MeasurementConstraintsDataCopy(cfg->constraints);
        if (self->constraints == NULL) {
            return kEebusErrorMemoryAllocate;
        }
    }

    return kEebusErrorOk;
}

EebusMeasurementObject* EebusMeasurementBaseCreate(
    EebusMeasurementNameId name,
    ScopeTypeType scope,
    ElectricalConnectionPhaseNameType phases,
    const EebusMeasurementBaseConfig* cfg,
    EebusMeasurementConfigureStrategy strategy
) {
    EebusMeasurementBase* const m = (EebusMeasurementBase*)EEBUS_MALLOC(sizeof(EebusMeasurementBase));
    if (m == NULL) {
        return NULL;
    }

    if (Construct(m, name, scope, phases, cfg, strategy) != kEebusErrorOk) {
        EEBUS_MEASUREMENT_DESTRUCT(EEBUS_MEASUREMENT_OBJECT(m));
        EEBUS_FREE(m);
        return NULL;
    }

    return EEBUS_MEASUREMENT_OBJECT(m);
}

static void Destruct(EebusMeasurementObject* self) {
    EebusMeasurementBase* const m = BASE(self);
    MeasurementConstraintsDataDelete(m->constraints);
    m->constraints = NULL;
    MeasurementDataDelete(m->measurement_data);
    m->measurement_data = NULL;
}

static EebusMeasurementNameId GetName(const EebusMeasurementObject* self) {
    return BASE(self)->name;
}

static EebusError GetDataValue(const EebusMeasurementObject* self, MeasurementServer* msrv, ScaledValue* value) {
    const EebusMeasurementBase* const m = BASE(self);
    if ((msrv == NULL) || (value == NULL)) {
        return kEebusErrorInputArgumentNull;
    }

    const MeasurementDataType* const data = MeasurementCommonGetMeasurementWithId(&msrv->measurement_common, m->id);
    if (data == NULL) {
        return kEebusErrorNoChange;
    }

    return ScaledValueInitWithScaledNumber(value, data->value);
}

static const MeasurementConstraintsDataType* GetConstraints(const EebusMeasurementObject* self) {
    return BASE(self)->constraints;
}

static EebusError Configure(
    EebusMeasurementObject* self,
    MeasurementServer* msrv,
    ElectricalConnectionServer* ecsrv,
    ElectricalConnectionIdType ec_id
) {
    EebusMeasurementBase* const m = BASE(self);
    if (m->cfg_strategy == NULL) {
        return kEebusErrorInit;
    }

    EebusError err = m->cfg_strategy(m, msrv, ecsrv, ec_id);
    if (err != kEebusErrorOk) {
        return err;
    }

    if (m->constraints != NULL) {
        err = MeasurementConstraintsSetId(m->constraints, m->id);
    }

    return err;
}

static EebusError SetDataCache(
    EebusMeasurementObject* self,
    const ScaledValue* measured_value,
    const EebusDateTime* timestamp,
    const MeasurementValueStateType* value_state,
    const EebusDateTime* start_time,
    const EebusDateTime* end_time
) {
    EebusMeasurementBase* const m = BASE(self);

    if (measured_value == NULL) {
        return kEebusErrorInputArgumentNull;
    }

    MeasurementDataDelete(m->measurement_data);

    const AbsoluteOrRelativeTimeType* const start_tmp = ABSOLUTE_OR_RELATIVE_TIME_PTR(start_time);
    const AbsoluteOrRelativeTimeType* const end_tmp   = ABSOLUTE_OR_RELATIVE_TIME_PTR(end_time);

    const TimePeriodType* const eval_period = ((start_tmp != NULL) && (end_tmp != NULL))
                                                  ? &(TimePeriodType){.start_time = start_tmp, .end_time = end_tmp}
                                                  : NULL;

    const MeasurementDataType new_data = {
        .measurement_id    = &m->id,
        .value_type        = &(MeasurementValueTypeType){kMeasurementValueTypeTypeValue},
        .timestamp         = ABSOLUTE_OR_RELATIVE_TIME_PTR(timestamp),
        .value             = &(ScaledNumberType){.number = &measured_value->value, .scale = &measured_value->scale},
        .evaluation_period = eval_period,
        .value_source      = &m->value_source,
        .value_tendency    = NULL,
        .value_state       = value_state,
    };

    m->measurement_data = MeasurementDataCopy(&new_data);
    if (m->measurement_data == NULL) {
        return kEebusErrorMemoryAllocate;
    }

    return kEebusErrorOk;
}

static MeasurementDataType* ReleaseDataCache(EebusMeasurementObject* self) {
    EebusMeasurementBase* const m   = BASE(self);
    MeasurementDataType* const data = m->measurement_data;
    m->measurement_data             = NULL;
    return data;
}

// ---------------------------------------------------------------------------
// Configure strategies
// ---------------------------------------------------------------------------

EebusError EebusMeasurementBaseConfigurePower(
    EebusMeasurementBase* m,
    MeasurementServer* msrv,
    ElectricalConnectionServer* ecsrv,
    ElectricalConnectionIdType ec_id
) {
    const MeasurementDescriptionDataType meas_desc = {
        .measurement_type = &(MeasurementTypeType){kMeasurementTypeTypePower},
        .commodity_type   = &(CommodityTypeType){kCommodityTypeTypeElectricity},
        .unit             = &(UnitOfMeasurementType){kUnitOfMeasurementTypeW},
        .scope_type       = &m->scope,
    };

    EebusError err = MeasurementServerAddDescription(msrv, &meas_desc, &m->id);
    if (err != kEebusErrorOk) {
        return err;
    }

    const ElectricalConnectionParameterDescriptionDataType param_desc = {
        .electrical_connection_id    = &ec_id,
        .measurement_id              = &m->id,
        .voltage_type                = &ELECTRICAL_CONNECTION_VOLTAGE_TYPE(Ac),
        .ac_measured_phases          = &m->phases,
        .ac_measured_in_reference_to = &ELECTRICAL_CONNECTION_PHASE_NAME(Neutral),
        .ac_measurement_type         = &ELECTRICAL_CONNECTION_AC_MEASUREMENT_TYPE(Real),
        .ac_measurement_variant      = &ELECTRICAL_CONNECTION_MEASURAND_VARIANT(Rms),
    };

    ElectricalConnectionParameterIdType param_id;
    return ElectricalConnectionServerAddParameterDescription(ecsrv, &param_desc, &param_id);
}

EebusError EebusMeasurementBaseConfigureEnergy(
    EebusMeasurementBase* m,
    MeasurementServer* msrv,
    ElectricalConnectionServer* ecsrv,
    ElectricalConnectionIdType ec_id
) {
    const MeasurementDescriptionDataType meas_desc = {
        .measurement_type = &(MeasurementTypeType){kMeasurementTypeTypeEnergy},
        .commodity_type   = &(CommodityTypeType){kCommodityTypeTypeElectricity},
        .unit             = &(UnitOfMeasurementType){kUnitOfMeasurementTypeWh},
        .scope_type       = &m->scope,
    };

    EebusError err = MeasurementServerAddDescription(msrv, &meas_desc, &m->id);
    if (err != kEebusErrorOk) {
        return err;
    }

    const ElectricalConnectionParameterDescriptionDataType param_desc = {
        .electrical_connection_id = &ec_id,
        .measurement_id           = &m->id,
        .voltage_type             = &ELECTRICAL_CONNECTION_VOLTAGE_TYPE(Ac),
        .ac_measurement_type      = &ELECTRICAL_CONNECTION_AC_MEASUREMENT_TYPE(Real),
    };

    ElectricalConnectionParameterIdType param_id;
    return ElectricalConnectionServerAddParameterDescription(ecsrv, &param_desc, &param_id);
}

EebusError EebusMeasurementBaseConfigureCurrent(
    EebusMeasurementBase* m,
    MeasurementServer* msrv,
    ElectricalConnectionServer* ecsrv,
    ElectricalConnectionIdType ec_id
) {
    const MeasurementDescriptionDataType meas_desc = {
        .measurement_type = &(MeasurementTypeType){kMeasurementTypeTypeCurrent},
        .commodity_type   = &(CommodityTypeType){kCommodityTypeTypeElectricity},
        .unit             = &(UnitOfMeasurementType){kUnitOfMeasurementTypeA},
        .scope_type       = &m->scope,
    };

    EebusError err = MeasurementServerAddDescription(msrv, &meas_desc, &m->id);
    if (err != kEebusErrorOk) {
        return err;
    }

    const ElectricalConnectionParameterDescriptionDataType param_desc = {
        .electrical_connection_id = &ec_id,
        .measurement_id           = &m->id,
        .voltage_type             = &ELECTRICAL_CONNECTION_VOLTAGE_TYPE(Ac),
        .ac_measured_phases       = &m->phases,
        .ac_measurement_type      = &ELECTRICAL_CONNECTION_AC_MEASUREMENT_TYPE(Real),
        .ac_measurement_variant   = &ELECTRICAL_CONNECTION_MEASURAND_VARIANT(Rms),
    };

    ElectricalConnectionParameterIdType param_id;
    return ElectricalConnectionServerAddParameterDescription(ecsrv, &param_desc, &param_id);
}

static ElectricalConnectionPhaseNameType VoltagePhaseFrom(ElectricalConnectionPhaseNameType phases) {
    switch (phases) {
        case kElectricalConnectionPhaseNameTypeA: return kElectricalConnectionPhaseNameTypeA;
        case kElectricalConnectionPhaseNameTypeB: return kElectricalConnectionPhaseNameTypeB;
        case kElectricalConnectionPhaseNameTypeC: return kElectricalConnectionPhaseNameTypeC;
        case kElectricalConnectionPhaseNameTypeAb: return kElectricalConnectionPhaseNameTypeA;
        case kElectricalConnectionPhaseNameTypeBc: return kElectricalConnectionPhaseNameTypeB;
        case kElectricalConnectionPhaseNameTypeAc: return kElectricalConnectionPhaseNameTypeC;
        default: return 0;
    }
}

static ElectricalConnectionPhaseNameType VoltagePhaseTo(ElectricalConnectionPhaseNameType phases) {
    switch (phases) {
        case kElectricalConnectionPhaseNameTypeA: return kElectricalConnectionPhaseNameTypeNeutral;
        case kElectricalConnectionPhaseNameTypeB: return kElectricalConnectionPhaseNameTypeNeutral;
        case kElectricalConnectionPhaseNameTypeC: return kElectricalConnectionPhaseNameTypeNeutral;
        case kElectricalConnectionPhaseNameTypeAb: return kElectricalConnectionPhaseNameTypeB;
        case kElectricalConnectionPhaseNameTypeBc: return kElectricalConnectionPhaseNameTypeC;
        case kElectricalConnectionPhaseNameTypeAc: return kElectricalConnectionPhaseNameTypeA;
        default: return 0;
    }
}

EebusError EebusMeasurementBaseConfigureVoltage(
    EebusMeasurementBase* m,
    MeasurementServer* msrv,
    ElectricalConnectionServer* ecsrv,
    ElectricalConnectionIdType ec_id
) {
    const MeasurementDescriptionDataType meas_desc = {
        .measurement_type = &(MeasurementTypeType){kMeasurementTypeTypeVoltage},
        .commodity_type   = &(CommodityTypeType){kCommodityTypeTypeElectricity},
        .unit             = &(UnitOfMeasurementType){kUnitOfMeasurementTypeV},
        .scope_type       = &m->scope,
    };

    EebusError err = MeasurementServerAddDescription(msrv, &meas_desc, &m->id);
    if (err != kEebusErrorOk) {
        return err;
    }

    const ElectricalConnectionParameterDescriptionDataType param_desc = {
        .electrical_connection_id    = &ec_id,
        .measurement_id              = &m->id,
        .voltage_type                = &ELECTRICAL_CONNECTION_VOLTAGE_TYPE(Ac),
        .ac_measured_phases          = &(ElectricalConnectionPhaseNameType){VoltagePhaseFrom(m->phases)},
        .ac_measured_in_reference_to = &(ElectricalConnectionPhaseNameType){VoltagePhaseTo(m->phases)},
        .ac_measurement_type         = &ELECTRICAL_CONNECTION_AC_MEASUREMENT_TYPE(Apparent),
        .ac_measurement_variant      = &ELECTRICAL_CONNECTION_MEASURAND_VARIANT(Rms),
    };

    ElectricalConnectionParameterIdType param_id;
    return ElectricalConnectionServerAddParameterDescription(ecsrv, &param_desc, &param_id);
}

EebusError EebusMeasurementBaseConfigureFrequency(
    EebusMeasurementBase* m,
    MeasurementServer* msrv,
    ElectricalConnectionServer* ecsrv,
    ElectricalConnectionIdType ec_id
) {
    const MeasurementDescriptionDataType meas_desc = {
        .measurement_type = &(MeasurementTypeType){kMeasurementTypeTypeFrequency},
        .commodity_type   = &(CommodityTypeType){kCommodityTypeTypeElectricity},
        .unit             = &(UnitOfMeasurementType){kUnitOfMeasurementTypeHz},
        .scope_type       = &m->scope,
    };

    EebusError err = MeasurementServerAddDescription(msrv, &meas_desc, &m->id);
    if (err != kEebusErrorOk) {
        return err;
    }

    const ElectricalConnectionParameterDescriptionDataType param_desc = {
        .electrical_connection_id = &ec_id,
        .measurement_id           = &m->id,
        .voltage_type             = &ELECTRICAL_CONNECTION_VOLTAGE_TYPE(Ac),
    };

    ElectricalConnectionParameterIdType param_id;
    return ElectricalConnectionServerAddParameterDescription(ecsrv, &param_desc, &param_id);
}
