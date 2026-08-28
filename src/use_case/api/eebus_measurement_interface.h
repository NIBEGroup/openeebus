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
 * @brief Eebus Measurement interface shared between MU MPC and GCP MGCP use cases
 */

#ifndef SRC_USE_CASE_API_EEBUS_MEASUREMENT_INTERFACE_H_
#define SRC_USE_CASE_API_EEBUS_MEASUREMENT_INTERFACE_H_

#include "src/common/eebus_errors.h"
#include "src/spine/model/common_data_types.h"
#include "src/spine/model/measurement_types.h"
#include "src/use_case/model/eebus_measurement_types.h"
#include "src/use_case/model/scaled_value.h"
#include "src/use_case/specialization/electrical_connection/electrical_connection_server.h"
#include "src/use_case/specialization/measurement/measurement_server.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct EebusMeasurementInterface EebusMeasurementInterface;
typedef struct EebusMeasurementObject EebusMeasurementObject;

struct EebusMeasurementInterface {
    void (*destruct)(EebusMeasurementObject* self);
    EebusMeasurementNameId (*get_name)(const EebusMeasurementObject* self);
    EebusError (*get_data_value)(
        const EebusMeasurementObject* self,
        MeasurementServer* msrv,
        ScaledValue* measurement_value
    );
    const MeasurementConstraintsDataType* (*get_constraints)(const EebusMeasurementObject* self);
    EebusError (*configure)(
        EebusMeasurementObject* self,
        MeasurementServer* msrv,
        ElectricalConnectionServer* ecsrv,
        ElectricalConnectionIdType electrical_connection_id
    );
    EebusError (*set_data_cache)(
        EebusMeasurementObject* self,
        const ScaledValue* measured_value,
        const EebusDateTime* timestamp,
        const MeasurementValueStateType* value_state,
        const EebusDateTime* start_time,
        const EebusDateTime* end_time
    );
    MeasurementDataType* (*release_data_cache)(EebusMeasurementObject* self);
};

struct EebusMeasurementObject {
    const EebusMeasurementInterface* interface_;
};

#define EEBUS_MEASUREMENT_OBJECT(obj) ((EebusMeasurementObject*)(obj))
#define EEBUS_MEASUREMENT_INTERFACE(obj) (EEBUS_MEASUREMENT_OBJECT(obj)->interface_)

#define EEBUS_MEASUREMENT_DESTRUCT(obj) (EEBUS_MEASUREMENT_INTERFACE(obj)->destruct(EEBUS_MEASUREMENT_OBJECT(obj)))

#define EEBUS_MEASUREMENT_GET_NAME(obj) (EEBUS_MEASUREMENT_INTERFACE(obj)->get_name(EEBUS_MEASUREMENT_OBJECT(obj)))

#define EEBUS_MEASUREMENT_GET_DATA_VALUE(obj, msrv, measurement_value) \
    (EEBUS_MEASUREMENT_INTERFACE(obj)->get_data_value(EEBUS_MEASUREMENT_OBJECT(obj), msrv, measurement_value))

#define EEBUS_MEASUREMENT_GET_CONSTRAINTS(obj) \
    (EEBUS_MEASUREMENT_INTERFACE(obj)->get_constraints(EEBUS_MEASUREMENT_OBJECT(obj)))

#define EEBUS_MEASUREMENT_CONFIGURE(obj, msrv, ecsrv, electrical_connection_id) \
    (EEBUS_MEASUREMENT_INTERFACE(obj)->configure(EEBUS_MEASUREMENT_OBJECT(obj), msrv, ecsrv, electrical_connection_id))

#define EEBUS_MEASUREMENT_SET_DATA_CACHE(obj, measured_value, timestamp, value_state, start_time, end_time) \
    (EEBUS_MEASUREMENT_INTERFACE(obj)->set_data_cache(                                                      \
        EEBUS_MEASUREMENT_OBJECT(obj),                                                                      \
        measured_value,                                                                                     \
        timestamp,                                                                                          \
        value_state,                                                                                        \
        start_time,                                                                                         \
        end_time                                                                                            \
    ))

#define EEBUS_MEASUREMENT_RELEASE_DATA_CACHE(obj) \
    (EEBUS_MEASUREMENT_INTERFACE(obj)->release_data_cache(EEBUS_MEASUREMENT_OBJECT(obj)))

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_USE_CASE_API_EEBUS_MEASUREMENT_INTERFACE_H_
