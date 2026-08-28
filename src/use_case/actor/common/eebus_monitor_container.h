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
 * @brief Shared monitor container for measurement-based use cases (MU MPC, GCP MGCP)
 */

#ifndef SRC_USE_CASE_ACTOR_COMMON_EEBUS_MONITOR_CONTAINER_H_
#define SRC_USE_CASE_ACTOR_COMMON_EEBUS_MONITOR_CONTAINER_H_

#include "src/common/api/eebus_mutex_interface.h"
#include "src/common/eebus_errors.h"
#include "src/common/vector.h"
#include "src/spine/api/device_local_interface.h"
#include "src/spine/api/entity_local_interface.h"
#include "src/use_case/actor/common/eebus_monitor_base.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct EebusMonitorContainer {
    Vector monitors;
    EebusMutexObject* mutex;
} EebusMonitorContainer;

EebusError EebusMonitorContainerConstruct(EebusMonitorContainer* self);

void EebusMonitorContainerDestruct(EebusMonitorContainer* self);

void EebusMonitorContainerAdd(EebusMonitorContainer* self, EebusMonitorObject* monitor);

EebusMeasurementObject*
EebusMonitorContainerGetMeasurement(const EebusMonitorContainer* self, EebusMeasurementNameId name);

EebusError EebusMonitorContainerGetMeasurementData(
    const EebusMonitorContainer* self,
    EntityLocalObject* local_entity,
    DeviceLocalObject* local_device,
    EebusMeasurementNameId name,
    ScaledValue* out
);

EebusError EebusMonitorContainerSetMeasurementDataCacheWithTime(
    EebusMonitorContainer* self,
    EebusMeasurementNameId name,
    const ScaledValue* value,
    const EebusDateTime* timestamp,
    const MeasurementValueStateType* value_state,
    const EebusDateTime* start_time,
    const EebusDateTime* end_time
);

EebusError EebusMonitorContainerSetMeasurementDataCache(
    EebusMonitorContainer* self,
    EebusMeasurementNameId name,
    const ScaledValue* value,
    const EebusDateTime* timestamp,
    const MeasurementValueStateType* value_state
);

EebusError EebusMonitorContainerUpdate(
    EebusMonitorContainer* self,
    EntityLocalObject* local_entity,
    DeviceLocalObject* local_device
);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_USE_CASE_ACTOR_COMMON_EEBUS_MONITOR_CONTAINER_H_
