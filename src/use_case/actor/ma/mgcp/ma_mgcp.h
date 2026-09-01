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
 * @brief Monitoring Appliance MGCP use case (Monitoring of Grid Connection Point)
 *
 * Scenarios supported:
 *   Scenario 1 — Read PV feed-in power limitation factor
 *   Scenario 2 — Monitor momentary power consumption/production
 *   Scenario 3 — Monitor total feed-in energy
 *   Scenario 4 — Monitor total consumed energy
 *   Scenario 5 — Monitor momentary current per phase
 *   Scenario 6 — Monitor voltage per phase
 *   Scenario 7 — Monitor frequency
 */

#ifndef SRC_USE_CASE_ACTOR_MA_MGCP_MA_MGCP_H_
#define SRC_USE_CASE_ACTOR_MA_MGCP_MA_MGCP_H_

#include "src/common/eebus_malloc.h"
#include "src/spine/entity/entity_local.h"
#include "src/use_case/api/ma_mgcp_listener_interface.h"
#include "src/use_case/api/use_case_interface.h"
#include "src/use_case/use_case.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct MaMgcpUseCaseObject MaMgcpUseCaseObject;

struct MaMgcpUseCaseObject {
    /** Inherits the Use Case */
    UseCaseObject obj;
};

#define MA_MGCP_USE_CASE_OBJECT(obj) ((MaMgcpUseCaseObject*)(obj))

/**
 * @brief Create a MA MGCP use case instance
 * @param local_entity Pointer to the local entity object
 * @param ma_mgcp_listener Pointer to the MA MGCP listener (may be NULL)
 * @return Pointer to the created MA MGCP use case instance, or NULL on failure
 */
MaMgcpUseCaseObject* MaMgcpUseCaseCreate(EntityLocalObject* local_entity, MaMgcpListenerObject* ma_mgcp_listener);

/**
 * @brief Delete a MA MGCP use case instance
 * @param ma_mgcp_use_case Pointer to the MA MGCP use case instance to delete
 */
static inline void MaMgcpUseCaseDelete(MaMgcpUseCaseObject* ma_mgcp_use_case) {
    UseCaseDelete(USE_CASE_OBJECT(ma_mgcp_use_case));
}

/**
 * @brief Get measurement data for the given measurement name id from the given remote entity
 * @param self MA MGCP use case instance
 * @param measurement_name_id Measurement to retrieve (see GcpMeasurementNameId)
 * @param remote_entity_addr Remote entity address
 * @param measurement_value Output buffer for the measurement value
 * @return kEebusErrorOk on success, error code otherwise
 */
EebusError MaMgcpGetMeasurementData(
    const MaMgcpUseCaseObject* self,
    GcpMeasurementNameId measurement_name_id,
    const EntityAddressType* remote_entity_addr,
    ScaledValue* measurement_value
);

/**
 * @brief Read the PV feed-in power limitation factor from a remote entity (Scenario 1)
 * @param self MA MGCP use case instance
 * @param remote_entity_addr Remote entity address
 * @param value Output buffer for the curtailment factor (0–100 %)
 * @return kEebusErrorOk on success, error code otherwise
 */
EebusError MaMgcpGetPvCurtailmentLimitFactor(
    const MaMgcpUseCaseObject* self,
    const EntityAddressType* remote_entity_addr,
    ScaledValue* value
);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_USE_CASE_ACTOR_MA_MGCP_MA_MGCP_H_
