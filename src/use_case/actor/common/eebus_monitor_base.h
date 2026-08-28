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
 * @brief Shared monitor base for MU MPC and GCP MGCP server-side use cases
 */

#ifndef SRC_USE_CASE_ACTOR_COMMON_EEBUS_MONITOR_BASE_H_
#define SRC_USE_CASE_ACTOR_COMMON_EEBUS_MONITOR_BASE_H_

#include <stddef.h>

#include "src/common/eebus_errors.h"
#include "src/common/eebus_malloc.h"
#include "src/common/vector.h"
#include "src/use_case/actor/common/eebus_measurement_base.h"
#include "src/use_case/api/eebus_measurement_interface.h"
#include "src/use_case/api/eebus_monitor_interface.h"
#include "src/use_case/model/eebus_measurement_types.h"
#include "src/use_case/specialization/electrical_connection/electrical_connection_server.h"
#include "src/use_case/specialization/measurement/measurement_server.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @brief Function pointer type for measurement object creation used by the monitor.
 *
 * Concrete implementations (MuMpcMeasurementCreate, GcpMgcpMeasurementCreate) are
 * directly compatible because their parameter types are typedef aliases of
 * EebusMeasurementNameId and EebusMeasurementBaseConfig.
 */
typedef EebusMeasurementObject* (*EebusMonitorMeasurementCreator)(
    EebusMeasurementNameId name_id,
    const EebusMeasurementBaseConfig* cfg
);

/**
 * @brief A (name_id, cfg) pair used to batch-create measurements.
 * A NULL cfg entry is silently skipped (optional measurement).
 */
typedef struct EebusMonitorMeasurementParam {
    EebusMeasurementNameId name_id;
    const EebusMeasurementBaseConfig* cfg;
} EebusMonitorMeasurementParam;

/**
 * @brief Shared internal base embedded in every concrete monitor struct.
 *
 * Embeds EebusMonitorObject as its first field, so any pointer to this struct
 * is also a valid EebusMonitorObject pointer (and transitively, any concrete
 * monitor whose first field is EebusMonitorBase is castable to EebusMonitorObject).
 */
typedef struct EebusMonitorBase {
    EebusMonitorObject obj;                     /**< Vtable pointer — must be first field */
    EebusMeasurementMonitorNameId name;         /**< Monitor group id */
    EebusMeasurementMonitorNameId name_id_mask; /**< Mask to extract the group from a measurement name id */
    Vector measurements;                        /**< Vector of EebusMeasurementObject* */
    EebusMonitorMeasurementCreator measurement_creator;
} EebusMonitorBase;

/**
 * @brief Cast any compatible pointer to EebusMonitorBase*.
 */
#define EEBUS_MONITOR_BASE(obj) ((EebusMonitorBase*)(obj))

/**
 * @brief Initialise an embedded EebusMonitorBase.
 */
EebusError EebusMonitorBaseConstruct(
    EebusMonitorBase* self,
    EebusMeasurementMonitorNameId name,
    EebusMeasurementMonitorNameId name_id_mask,
    EebusMonitorMeasurementCreator measurement_creator
);

/**
 * @brief Create and append measurements from a param array; skips NULL cfg entries.
 */
EebusError
EebusMonitorBaseAddMeasurements(EebusMonitorBase* self, const EebusMonitorMeasurementParam* params, size_t params_size);

/**
 * @brief Destructs and frees an EebusMonitorObject instance.
 */
static inline void EebusMonitorDelete(EebusMonitorObject* monitor) {
    if (monitor != NULL) {
        EEBUS_MONITOR_DESTRUCT(monitor);
        EEBUS_FREE(monitor);
    }
}

/**
 * @brief Allocate, construct, and populate a new monitor; returns NULL on failure.
 */
EebusMonitorObject* EebusMonitorCreate(
    EebusMeasurementMonitorNameId name,
    EebusMeasurementMonitorNameId name_id_mask,
    EebusMonitorMeasurementCreator measurement_creator,
    const EebusMonitorMeasurementParam* params,
    size_t params_size
);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_USE_CASE_ACTOR_COMMON_EEBUS_MONITOR_BASE_H_
