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
 * @brief MA Measurement interface shared between MPC and MGCP monitoring use cases
 */

#ifndef SRC_USE_CASE_API_MA_MEASUREMENT_INTERFACE_H_
#define SRC_USE_CASE_API_MA_MEASUREMENT_INTERFACE_H_

#include "src/common/eebus_errors.h"
#include "src/use_case/model/eebus_measurement_types.h"
#include "src/use_case/model/scaled_value.h"
#include "src/use_case/specialization/electrical_connection/electrical_connection_client.h"
#include "src/use_case/specialization/measurement/measurement_client.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct MaMeasurementInterface MaMeasurementInterface;
typedef struct MaMeasurementObject MaMeasurementObject;

struct MaMeasurementInterface {
    EebusMeasurementNameId (*get_name)(const MaMeasurementObject* self);
    EebusError (*get_data)(
        const MaMeasurementObject* self,
        EntityLocalObject* local_entity,
        EntityRemoteObject* remote_entity,
        ScaledValue* measurement_value
    );
};

struct MaMeasurementObject {
    const MaMeasurementInterface* interface_;
};

#define MA_MEASUREMENT_OBJECT(obj) ((MaMeasurementObject*)(obj))
#define MA_MEASUREMENT_INTERFACE(obj) (MA_MEASUREMENT_OBJECT(obj)->interface_)
#define MA_MEASUREMENT_GET_NAME(obj) (MA_MEASUREMENT_INTERFACE(obj)->get_name(MA_MEASUREMENT_OBJECT(obj)))
#define MA_MEASUREMENT_GET_DATA(obj, local_entity, remote_entity, measurement_value) \
    (MA_MEASUREMENT_INTERFACE(obj)                                                   \
         ->get_data(MA_MEASUREMENT_OBJECT(obj), local_entity, remote_entity, measurement_value))

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_USE_CASE_API_MA_MEASUREMENT_INTERFACE_H_
