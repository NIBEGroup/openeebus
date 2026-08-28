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
 * @brief Shared monitor container implementation
 */

#include "src/use_case/actor/common/eebus_monitor_container.h"

#include "src/common/eebus_mutex/eebus_mutex.h"

static void MonitorDeallocator(void* p) {
    EebusMonitorDelete((EebusMonitorObject*)p);
}

EebusError EebusMonitorContainerConstruct(EebusMonitorContainer* self) {
    VectorConstructWithDeallocator(&self->monitors, MonitorDeallocator);

    self->mutex = EebusMutexCreate();
    if (self->mutex == NULL) {
        VectorDestruct(&self->monitors);
        return kEebusErrorMemoryAllocate;
    }

    return kEebusErrorOk;
}

void EebusMonitorContainerDestruct(EebusMonitorContainer* self) {
    EebusMutexDelete(self->mutex);
    self->mutex = NULL;

    VectorFreeElements(&self->monitors);
    VectorDestruct(&self->monitors);
}

void EebusMonitorContainerAdd(EebusMonitorContainer* self, EebusMonitorObject* monitor) {
    VectorPushBack(&self->monitors, monitor);
}

EebusMeasurementObject*
EebusMonitorContainerGetMeasurement(const EebusMonitorContainer* self, EebusMeasurementNameId name) {
    const EebusMeasurementMonitorNameId monitor_name
        = (EebusMeasurementMonitorNameId)((uint8_t)name & (uint8_t)kEebusMeasurementMonitorNameIdMask);

    for (size_t i = 0; i < VectorGetSize(&self->monitors); ++i) {
        EebusMonitorObject* const monitor = (EebusMonitorObject*)VectorGetElement(&self->monitors, i);
        if (EEBUS_MONITOR_GET_NAME(monitor) == monitor_name) {
            return EEBUS_MONITOR_GET_MEASUREMENT(monitor, name);
        }
    }

    return NULL;
}

static EebusError GetMeasurementDataInternal(
    const EebusMonitorContainer* self,
    EntityLocalObject* local_entity,
    EebusMeasurementNameId name,
    ScaledValue* out
) {
    EebusMeasurementObject* const measurement = EebusMonitorContainerGetMeasurement(self, name);
    if (measurement == NULL) {
        return kEebusErrorNotSupported;
    }

    MeasurementServer msrv = {0};
    const EebusError err   = MeasurementServerConstruct(&msrv, local_entity);
    if (err != kEebusErrorOk) {
        return err;
    }

    return EEBUS_MEASUREMENT_GET_DATA_VALUE(measurement, &msrv, out);
}

EebusError EebusMonitorContainerGetMeasurementData(
    const EebusMonitorContainer* self,
    EntityLocalObject* local_entity,
    DeviceLocalObject* local_device,
    EebusMeasurementNameId name,
    ScaledValue* out
) {
    DEVICE_LOCAL_LOCK(local_device);
    const EebusError err = GetMeasurementDataInternal(self, local_entity, name, out);
    DEVICE_LOCAL_UNLOCK(local_device);

    return err;
}

EebusError EebusMonitorContainerSetMeasurementDataCacheWithTime(
    EebusMonitorContainer* self,
    EebusMeasurementNameId name,
    const ScaledValue* value,
    const EebusDateTime* timestamp,
    const MeasurementValueStateType* value_state,
    const EebusDateTime* start_time,
    const EebusDateTime* end_time
) {
    EebusMeasurementObject* const measurement = EebusMonitorContainerGetMeasurement(self, name);
    if (measurement == NULL) {
        return kEebusErrorNotSupported;
    }

    EEBUS_MUTEX_LOCK(self->mutex);
    const EebusError err
        = EEBUS_MEASUREMENT_SET_DATA_CACHE(measurement, value, timestamp, value_state, start_time, end_time);
    EEBUS_MUTEX_UNLOCK(self->mutex);

    return err;
}

EebusError EebusMonitorContainerSetMeasurementDataCache(
    EebusMonitorContainer* self,
    EebusMeasurementNameId name,
    const ScaledValue* value,
    const EebusDateTime* timestamp,
    const MeasurementValueStateType* value_state
) {
    return EebusMonitorContainerSetMeasurementDataCacheWithTime(self, name, value, timestamp, value_state, NULL, NULL);
}

EebusError EebusMonitorContainerUpdate(
    EebusMonitorContainer* self,
    EntityLocalObject* local_entity,
    DeviceLocalObject* local_device
) {
    MeasurementServer msrv = {0};
    EebusError err         = MeasurementServerConstruct(&msrv, local_entity);
    if (err != kEebusErrorOk) {
        return err;
    }

    MeasurementListDataType* const list = MeasurementsCreateEmpty();
    if (list == NULL) {
        return kEebusErrorMemoryAllocate;
    }

    EEBUS_MUTEX_LOCK(self->mutex);
    for (size_t i = 0; i < VectorGetSize(&self->monitors); ++i) {
        EebusMonitorObject* const monitor = (EebusMonitorObject*)VectorGetElement(&self->monitors, i);

        err = EEBUS_MONITOR_FLUSH_MEASUREMENT_CACHE(monitor, list);
        if (err != kEebusErrorOk) {
            EEBUS_MUTEX_UNLOCK(self->mutex);
            MeasurementsDelete(list);
            return err;
        }
    }

    EEBUS_MUTEX_UNLOCK(self->mutex);

    if (list->measurement_data_size > 0) {
        DEVICE_LOCAL_LOCK(local_device);
        err = MeasurementServerUpdateMeasurements(&msrv, list, NULL, NULL);
        DEVICE_LOCAL_UNLOCK(local_device);
    }

    MeasurementsDelete(list);
    return err;
}
