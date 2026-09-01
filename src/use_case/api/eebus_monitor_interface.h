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
 * @brief Eebus Monitor interface shared between MU MPC and GCP MGCP use cases
 */

#ifndef SRC_USE_CASE_API_EEBUS_MONITOR_INTERFACE_H_
#define SRC_USE_CASE_API_EEBUS_MONITOR_INTERFACE_H_

#include "src/common/eebus_errors.h"
#include "src/use_case/actor/common/eebus_measurement_base.h"
#include "src/use_case/api/eebus_measurement_interface.h"
#include "src/use_case/model/eebus_measurement_types.h"
#include "src/use_case/specialization/electrical_connection/electrical_connection_server.h"
#include "src/use_case/specialization/measurement/measurement_server.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @brief Per-phase AC current monitor configuration.
 * At least one phase pointer must be non-NULL.
 */
typedef struct EebusMonitorCurrentConfig {
    const EebusMeasurementBaseConfig* current_phase_a_cfg;
    const EebusMeasurementBaseConfig* current_phase_b_cfg;
    const EebusMeasurementBaseConfig* current_phase_c_cfg;
} EebusMonitorCurrentConfig;

/**
 * @brief Per-phase AC voltage monitor configuration.
 *
 * Phase-to-phase entries (AB/BC/AC) may only be non-NULL when both related
 * phase-to-neutral entries are also non-NULL. At least one entry must be non-NULL.
 */
typedef struct EebusMonitorVoltageConfig {
    const EebusMeasurementBaseConfig* voltage_phase_a_cfg;
    const EebusMeasurementBaseConfig* voltage_phase_b_cfg;
    const EebusMeasurementBaseConfig* voltage_phase_c_cfg;
    const EebusMeasurementBaseConfig* voltage_phase_ab_cfg;
    const EebusMeasurementBaseConfig* voltage_phase_bc_cfg;
    const EebusMeasurementBaseConfig* voltage_phase_ac_cfg;
} EebusMonitorVoltageConfig;

/**
 * @brief AC frequency monitor configuration.
 */
typedef struct EebusMonitorFrequencyConfig {
    EebusMeasurementBaseConfig frequency_cfg;
} EebusMonitorFrequencyConfig;

/**
 * @brief Eebus Monitor Interface
 * (Eebus Monitor "virtual functions table" declaration)
 */
typedef struct EebusMonitorInterface EebusMonitorInterface;

/**
 * @brief Eebus Monitor Object type definition
 * ("abstract class", has no members but only pointer to
 * "virtual functions table")
 */
typedef struct EebusMonitorObject EebusMonitorObject;

/**
 * @brief EebusMonitor Interface Structure
 */
struct EebusMonitorInterface {
    void (*destruct)(EebusMonitorObject* self);
    EebusMeasurementMonitorNameId (*get_name)(const EebusMonitorObject* self);
    EebusError (*configure)(
        EebusMonitorObject* self,
        MeasurementServer* msrv,
        ElectricalConnectionServer* ecsrv,
        ElectricalConnectionIdType ec_id,
        MeasurementConstraintsListDataType* constraints
    );
    EebusMeasurementObject* (*get_measurement)(const EebusMonitorObject* self, EebusMeasurementNameId name_id);
    EebusError (*flush_measurement_cache)(EebusMonitorObject* self, MeasurementListDataType* list);
};

/**
 * @brief Eebus Monitor Object Structure
 */
struct EebusMonitorObject {
    const EebusMonitorInterface* interface_;
};

/**
 * @brief Eebus Monitor pointer typecast
 */
#define EEBUS_MONITOR_OBJECT(obj) ((EebusMonitorObject*)(obj))

/**
 * @brief Eebus Monitor Interface class pointer typecast
 */
#define EEBUS_MONITOR_INTERFACE(obj) (EEBUS_MONITOR_OBJECT(obj)->interface_)

/**
 * @brief Eebus Monitor Destruct caller definition
 */
#define EEBUS_MONITOR_DESTRUCT(obj) (EEBUS_MONITOR_INTERFACE(obj)->destruct(EEBUS_MONITOR_OBJECT(obj)))

/**
 * @brief Eebus Monitor Get Name caller definition
 */
#define EEBUS_MONITOR_GET_NAME(obj) (EEBUS_MONITOR_INTERFACE(obj)->get_name(EEBUS_MONITOR_OBJECT(obj)))

/**
 * @brief Eebus Monitor Configure caller definition
 */
#define EEBUS_MONITOR_CONFIGURE(obj, msrv, ecsrv, ec_id, constraints) \
    (EEBUS_MONITOR_INTERFACE(obj)->configure(EEBUS_MONITOR_OBJECT(obj), msrv, ecsrv, ec_id, constraints))

/**
 * @brief Eebus Monitor Get Measurement caller definition
 */
#define EEBUS_MONITOR_GET_MEASUREMENT(obj, name_id) \
    (EEBUS_MONITOR_INTERFACE(obj)->get_measurement(EEBUS_MONITOR_OBJECT(obj), name_id))

/**
 * @brief Eebus Monitor Flush Measurement Cache caller definition
 */
#define EEBUS_MONITOR_FLUSH_MEASUREMENT_CACHE(obj, list) \
    (EEBUS_MONITOR_INTERFACE(obj)->flush_measurement_cache(EEBUS_MONITOR_OBJECT(obj), list))

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_USE_CASE_API_EEBUS_MONITOR_INTERFACE_H_
