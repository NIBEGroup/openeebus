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
 * @brief Shared monitor base implementation
 */

#include "src/use_case/actor/common/eebus_monitor_base.h"

#include "src/common/eebus_malloc.h"

static void MeasurementDeallocator(void* p) {
    EebusMeasurementObject* const m = (EebusMeasurementObject*)p;
    if (m != NULL) {
        EEBUS_MEASUREMENT_DESTRUCT(m);
        EEBUS_FREE(m);
    }
}

static void Destruct(EebusMonitorObject* self) {
    EebusMonitorBase* const base = EEBUS_MONITOR_BASE(self);
    VectorFreeElements(&base->measurements);
    VectorDestruct(&base->measurements);
}

static EebusMeasurementMonitorNameId GetName(const EebusMonitorObject* self) {
    return EEBUS_MONITOR_BASE(self)->name;
}

static EebusError Configure(
    EebusMonitorObject* self,
    MeasurementServer* msrv,
    ElectricalConnectionServer* ecsrv,
    ElectricalConnectionIdType ec_id,
    MeasurementConstraintsListDataType* constraints
) {
    if ((msrv == NULL) || (ecsrv == NULL)) {
        return kEebusErrorInputArgumentNull;
    }

    EebusMonitorBase* const base = EEBUS_MONITOR_BASE(self);

    for (size_t i = 0; i < VectorGetSize(&base->measurements); ++i) {
        EebusMeasurementObject* const m = (EebusMeasurementObject*)VectorGetElement(&base->measurements, i);

        EebusError err = EEBUS_MEASUREMENT_CONFIGURE(m, msrv, ecsrv, ec_id);
        if (err != kEebusErrorOk) {
            return err;
        }

        const MeasurementConstraintsDataType* const c = EEBUS_MEASUREMENT_GET_CONSTRAINTS(m);
        if (c != NULL) {
            err = MeasurementConstraintsAdd(constraints, c);
            if (err != kEebusErrorOk) {
                return err;
            }
        }
    }

    return kEebusErrorOk;
}

static EebusMeasurementObject* GetMeasurement(const EebusMonitorObject* self, EebusMeasurementNameId name_id) {
    const EebusMonitorBase* const base = (const EebusMonitorBase*)self;

    if (((uint8_t)name_id & (uint8_t)base->name_id_mask) != (uint8_t)base->name) {
        return NULL;
    }

    for (size_t i = 0; i < VectorGetSize(&base->measurements); ++i) {
        EebusMeasurementObject* const m = (EebusMeasurementObject*)VectorGetElement(&base->measurements, i);
        if (EEBUS_MEASUREMENT_GET_NAME(m) == name_id) {
            return m;
        }
    }

    return NULL;
}

static EebusError FlushMeasurementCache(EebusMonitorObject* self, MeasurementListDataType* list) {
    if (list == NULL) {
        return kEebusErrorInputArgumentNull;
    }

    EebusMonitorBase* const base = EEBUS_MONITOR_BASE(self);

    for (size_t i = 0; i < VectorGetSize(&base->measurements); ++i) {
        EebusMeasurementObject* const m = (EebusMeasurementObject*)VectorGetElement(&base->measurements, i);

        MeasurementDataType* const data = EEBUS_MEASUREMENT_RELEASE_DATA_CACHE(m);
        if (data != NULL) {
            EebusError err
                = EebusDataListDataAppend((void***)&list->measurement_data, &list->measurement_data_size, data);
            if (err != kEebusErrorOk) {
                MeasurementDataDelete(data);
                return err;
            }
        }
    }

    return kEebusErrorOk;
}

static const EebusMonitorInterface kEebusMonitorMethods = {
    .destruct                = Destruct,
    .get_name                = GetName,
    .configure               = Configure,
    .get_measurement         = GetMeasurement,
    .flush_measurement_cache = FlushMeasurementCache,
};

EebusError EebusMonitorBaseConstruct(
    EebusMonitorBase* self,
    EebusMeasurementMonitorNameId name,
    EebusMeasurementMonitorNameId name_id_mask,
    EebusMonitorMeasurementCreator measurement_creator
) {
    EEBUS_MONITOR_INTERFACE(self) = &kEebusMonitorMethods;
    self->name                    = name;
    self->name_id_mask            = name_id_mask;
    self->measurement_creator     = measurement_creator;
    VectorConstructWithDeallocator(&self->measurements, MeasurementDeallocator);
    return kEebusErrorOk;
}

EebusError EebusMonitorBaseAddMeasurements(
    EebusMonitorBase* self,
    const EebusMonitorMeasurementParam* params,
    size_t params_size
) {
    for (size_t i = 0; i < params_size; ++i) {
        if (params[i].cfg == NULL) {
            continue;
        }

        EebusMeasurementObject* const m = self->measurement_creator(params[i].name_id, params[i].cfg);
        if (m == NULL) {
            return kEebusErrorInit;
        }

        VectorPushBack(&self->measurements, m);
    }

    return kEebusErrorOk;
}

EebusMonitorObject* EebusMonitorCreate(
    EebusMeasurementMonitorNameId name,
    EebusMeasurementMonitorNameId name_id_mask,
    EebusMonitorMeasurementCreator measurement_creator,
    const EebusMonitorMeasurementParam* params,
    size_t params_size
) {
    EebusMonitorBase* const base = (EebusMonitorBase*)EEBUS_MALLOC(sizeof(EebusMonitorBase));
    if (base == NULL) {
        return NULL;
    }

    if (EebusMonitorBaseConstruct(base, name, name_id_mask, measurement_creator) != kEebusErrorOk) {
        EebusMonitorDelete(EEBUS_MONITOR_OBJECT(base));
        return NULL;
    }

    if (EebusMonitorBaseAddMeasurements(base, params, params_size) != kEebusErrorOk) {
        EebusMonitorDelete(EEBUS_MONITOR_OBJECT(base));
        return NULL;
    }

    return EEBUS_MONITOR_OBJECT(base);
}
