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
 * @brief Monitoring Appliance MPC use case (Monitoring of Power Consumption)
 */

#ifndef SRC_USE_CASE_ACTOR_MA_MPC_MA_MPC_H_
#define SRC_USE_CASE_ACTOR_MA_MPC_MA_MPC_H_

#include "src/spine/api/feature_local_interface.h"
#include "src/spine/entity/entity_local.h"
#include "src/use_case/api/ma_mpc_listener_interface.h"
#include "src/use_case/use_case.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct MaMpcUseCaseObject MaMpcUseCaseObject;
struct MaMpcUseCaseObject {
    /** Inherits the Entity */
    UseCaseObject obj;
};

#define MA_MPC_USE_CASE_OBJECT(obj) ((MaMpcUseCaseObject*)(obj))

/**
 * @brief Create a MA MPC use case instance
 * @param local_entity Pointer to the local entity object
 * @param ma_mpc_listener Pointer to the MA MPC listener object
 * @return Pointer to the created MA MPC use case instance or NULL if creation failed
 */
MaMpcUseCaseObject* MaMpcUseCaseCreate(EntityLocalObject* local_entity, MaMpcListenerObject* ma_mpc_listener);

/**
 * @brief Delete a MA MPC use case instance
 * @param ma_mpc_use_case Pointer to the MA MPC use case instance to delete
 */
static inline void MaMpcUseCaseDelete(MaMpcUseCaseObject* ma_mpc_use_case) {
    UseCaseDelete(USE_CASE_OBJECT(ma_mpc_use_case));
}

/**
 * @brief Get measurement data for the given measurement name id from the given remote entity
 * @param self MA MPC use case instance to get the measurement data from
 * @param measurement_name_id Measurement Name Id to get the data for (see MpcMeasurementNameId enum)
 * @param remote_entity_addr Remote entity address (device and entity ids)
 * @param measurement_value Output buffer to store the measurement value in
 * @return kEebusErrorOk if successful, error code otherwise
 */
EebusError MaMpcGetMeasurementData(
    const MaMpcUseCaseObject* self,
    MuMpcMeasurementNameId measurement_name_id,
    const EntityAddressType* remote_entity_addr,
    ScaledValue* measurement_value
);

/**
 * @brief Request measurement data from the given remote entity
 *
 * Sends a READ request for all measurement list data from the remote entity.
 * The reply is delivered asynchronously via @p cb.
 *
 * @param self MA MPC use case instance to send the read request with
 * @param remote_entity_addr Remote entity address (device and entity ids)
 * @param cb Optional callback invoked when the reply is received, may be NULL
 * @param ctx Optional context pointer passed to @p cb, may be NULL
 * @return kEebusErrorOk if the request was sent successfully, error code otherwise
 */
EebusError MaMpcReadMeasurementsData(
    const MaMpcUseCaseObject* self,
    const EntityAddressType* remote_entity_addr,
    ReplyMessageCallback cb,
    void* ctx
);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_USE_CASE_ACTOR_MA_MPC_MA_MPC_H_
