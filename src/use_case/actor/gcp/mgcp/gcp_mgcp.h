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
 * @brief Grid Connection Point MGCP use case (Monitoring of Grid Connection Point).
 *
 * Scenarios supported:
 *   Scenario 1 — Publish PV feed-in power limitation factor     (optional)
 *   Scenario 2 — Monitor momentary power consumption/production (mandatory)
 *   Scenario 3 — Monitor total feed-in energy                   (optional)
 *   Scenario 4 — Monitor total consumed energy                  (optional)
 *   Scenario 5 — Monitor momentary current per phase            (optional)
 *   Scenario 6 — Monitor voltage per phase                      (optional)
 *   Scenario 7 — Monitor frequency                              (optional)
 *
 * @code{.c}
 * GcpMgcpUseCaseObject* const gcp_mgcp =
 *     GcpMgcpUseCaseCreate(local_entity, ec_id, &cfg);
 *
 * // Cache a power measurement
 * GcpMgcpSetMeasurementDataCache(gcp_mgcp, kGcpPowerTotal, &(ScaledValue){5000, 0}, NULL, NULL);
 *
 * // Cache energy with evaluation period
 * GcpMgcpSetEnergyFeedInCache(gcp_mgcp, &(ScaledValue){200, 0}, NULL, NULL, &start_time, &end_time);
 * GcpMgcpSetEnergyConsumedCache(gcp_mgcp, &(ScaledValue){800, 0}, NULL, NULL, &start_time, &end_time);
 *
 * // Flush all caches to the local feature and notify remotes
 * GcpMgcpUpdate(gcp_mgcp);
 * @endcode
 */

#ifndef SRC_USE_CASE_ACTOR_GCP_MGCP_GCP_MGCP_H_
#define SRC_USE_CASE_ACTOR_GCP_MGCP_GCP_MGCP_H_

#include <stdint.h>

#include "src/common/eebus_malloc.h"
#include "src/spine/entity/entity_local.h"
#include "src/use_case/actor/gcp/mgcp/gcp_mgcp_monitor.h"
#include "src/use_case/api/use_case_interface.h"
#include "src/use_case/specialization/electrical_connection/electrical_connection_server.h"
#include "src/use_case/use_case.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @brief GCP MGCP Scenario 1 configuration
 *
 * Scenario 1 — Publish PV feed-in power limitation factor (DeviceConfiguration,
 * key "pvCurtailmentLimitFactor", unit %). Presence of a non-NULL pointer in
 * GcpMgcpConfig::pv_curtailment_cfg signals support for this scenario.
 */
typedef struct GcpMgcpPvCurtailmentConfig GcpMgcpPvCurtailmentConfig;

struct GcpMgcpPvCurtailmentConfig {
    uint8_t reserved; /**< Reserved for future extension */
};

/**
 * @brief GCP MGCP use case configuration (scenarios 1–7)
 */
typedef struct GcpMgcpConfig GcpMgcpConfig;

struct GcpMgcpConfig {
    /** Optional: Scenario 1 — PV curtailment limit factor; NULL = not supported */
    const GcpMgcpPvCurtailmentConfig* pv_curtailment_cfg;
    /** Required: Scenario 2 — total active power */
    const GcpMgcpMonitorPowerConfig power_cfg;
    /** Optional: Scenarios 3 and/or 4 — grid energy; NULL = not supported */
    const GcpMgcpMonitorEnergyConfig* energy_cfg;
    /** Optional: Scenario 5 — per-phase AC current; NULL = not supported */
    const GcpMgcpMonitorCurrentConfig* current_cfg;
    /** Optional: Scenario 6 — per-phase AC voltage; NULL = not supported */
    const GcpMgcpMonitorVoltageConfig* voltage_cfg;
    /** Optional: Scenario 7 — AC frequency; NULL = not supported */
    const GcpMgcpMonitorFrequencyConfig* frequency_cfg;
};

typedef struct GcpMgcpUseCaseObject GcpMgcpUseCaseObject;

struct GcpMgcpUseCaseObject {
    /** Inherits the Use Case */
    UseCaseObject obj;
};

#define GCP_MGCP_USE_CASE_OBJECT(obj) ((GcpMgcpUseCaseObject*)(obj))

/**
 * @brief Create a GCP MGCP use case instance
 * @param local_entity  Pointer to the local entity object
 * @param ec_id         Electrical connection id used for all measurements
 * @param cfg           Scenario configuration; must not be NULL
 * @return Pointer to the created instance, or NULL on failure
 */
GcpMgcpUseCaseObject*
GcpMgcpUseCaseCreate(EntityLocalObject* local_entity, ElectricalConnectionIdType ec_id, const GcpMgcpConfig* cfg);

/**
 * @brief Delete a GCP MGCP use case instance
 * @param gcp_mgcp Pointer to the GCP MGCP use case instance to delete
 */
static inline void GcpMgcpUseCaseDelete(GcpMgcpUseCaseObject* gcp_mgcp) {
    UseCaseDelete(USE_CASE_OBJECT(gcp_mgcp));
}

/**
 * @brief Read the current value of a measurement from the local feature
 * @param self             GCP MGCP use case instance
 * @param measurement_name Measurement to read (see GcpMeasurementNameId)
 * @param measurement_value Output buffer; must not be NULL
 * @return kEebusErrorOk on success, error code otherwise
 */
EebusError GcpMgcpGetMeasurementData(
    const GcpMgcpUseCaseObject* self,
    GcpMeasurementNameId measurement_name,
    ScaledValue* measurement_value
);

/**
 * @brief Stage a measurement value for the next GcpMgcpUpdate() call
 *
 * For energy measurements with an evaluation period, prefer
 * GcpMgcpSetEnergyFeedInCache() / GcpMgcpSetEnergyConsumedCache().
 *
 * @param self             GCP MGCP use case instance
 * @param measurement_name Measurement to set (see GcpMeasurementNameId)
 * @param measurement_value Value to stage; must not be NULL
 * @param timestamp        Optional measurement timestamp; may be NULL
 * @param value_state      Optional value state; may be NULL
 * @return kEebusErrorOk on success, error code otherwise
 */
EebusError GcpMgcpSetMeasurementDataCache(
    GcpMgcpUseCaseObject* self,
    GcpMeasurementNameId measurement_name,
    const ScaledValue* measurement_value,
    const EebusDateTime* timestamp,
    const MeasurementValueStateType* value_state
);

/**
 * @brief Flush all staged measurement values to the local feature
 *        and notify connected remote features
 * @param self GCP MGCP use case instance
 * @return kEebusErrorOk on success, error code otherwise
 */
EebusError GcpMgcpUpdate(const GcpMgcpUseCaseObject* self);

/**
 * @brief Stage a feed-in energy value (Scenario 3) with optional evaluation period
 * @param self          GCP MGCP use case instance
 * @param energy_feed_in Feed-in energy value; must not be NULL
 * @param timestamp     Optional measurement timestamp; may be NULL
 * @param value_state   Optional value state; may be NULL
 * @param start_time    Evaluation period start; may be NULL
 * @param end_time      Evaluation period end; may be NULL
 * @return kEebusErrorOk on success, error code otherwise
 */
EebusError GcpMgcpSetEnergyFeedInCache(
    GcpMgcpUseCaseObject* self,
    const ScaledValue* energy_feed_in,
    const EebusDateTime* timestamp,
    const MeasurementValueStateType* value_state,
    const EebusDateTime* start_time,
    const EebusDateTime* end_time
);

/**
 * @brief Stage a consumed energy value (Scenario 4) with optional evaluation period
 * @param self            GCP MGCP use case instance
 * @param energy_consumed Consumed energy value; must not be NULL
 * @param timestamp       Optional measurement timestamp; may be NULL
 * @param value_state     Optional value state; may be NULL
 * @param start_time      Evaluation period start; may be NULL
 * @param end_time        Evaluation period end; may be NULL
 * @return kEebusErrorOk on success, error code otherwise
 */
EebusError GcpMgcpSetEnergyConsumedCache(
    GcpMgcpUseCaseObject* self,
    const ScaledValue* energy_consumed,
    const EebusDateTime* timestamp,
    const MeasurementValueStateType* value_state,
    const EebusDateTime* start_time,
    const EebusDateTime* end_time
);

/**
 * @brief Publish the PV feed-in power limitation factor (Scenario 1)
 *
 * Updates the DeviceConfiguration key "pvCurtailmentLimitFactor" on the local
 * server feature and notifies any subscribed remote entities.
 *
 * @param self  GCP MGCP use case instance
 * @param value Curtailment factor (0–100 %); must not be NULL
 * @return kEebusErrorOk on success, error code otherwise
 */
EebusError GcpMgcpSetPvCurtailmentLimitFactor(GcpMgcpUseCaseObject* self, const ScaledValue* value);

/**
 * @brief Read the current PV feed-in power limitation factor (Scenario 1)
 *
 * Reads the DeviceConfiguration key "pvCurtailmentLimitFactor" from the local
 * server feature.
 *
 * @param self  GCP MGCP use case instance
 * @param value Output buffer; must not be NULL
 * @return kEebusErrorOk on success, error code otherwise
 */
EebusError GcpMgcpGetPvCurtailmentLimitFactor(const GcpMgcpUseCaseObject* self, ScaledValue* value);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_USE_CASE_ACTOR_GCP_MGCP_GCP_MGCP_H_
