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
 * @brief Energy Guard Limitation of Power use case base class to
 * be used by CS LPC and CS LPP concrete use cases
 */

#ifndef SRC_USE_CASE_ACTOR_EG_EG_LP_H_
#define SRC_USE_CASE_ACTOR_EG_EG_LP_H_

#include "src/spine/api/feature_local_interface.h"
#include "src/spine/entity/entity_local.h"
#include "src/spine/model/common_data_types.h"
#include "src/use_case/api/eg_lp_listener_interface.h"
#include "src/use_case/model/load_limit_types.h"
#include "src/use_case/use_case.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct EgLpUseCaseObject EgLpUseCaseObject;
struct EgLpUseCaseObject {
  /** Inherits the Entity */
  UseCaseObject obj;
};

#define EG_LP_USE_CASE_OBJECT(obj) ((EgLpUseCaseObject*)(obj))

EgLpUseCaseObject* EgLpUseCaseCreate(
    EnergyDirectionType energy_direction,
    const UseCaseInfo* use_case_info,
    EntityLocalObject* local_entity,
    EgLpListenerObject* eg_lp_listener
);

static inline void EgLpUseCaseDelete(EgLpUseCaseObject* eg_lp_use_case) {
  if (eg_lp_use_case != NULL) {
    USE_CASE_DESTRUCT(USE_CASE_OBJECT(eg_lp_use_case));
    EEBUS_FREE(eg_lp_use_case);
  }
}

//-------------------------------------------------------------------------------------------//
//
// Scenario 1
//
//-------------------------------------------------------------------------------------------//

/**
 * @brief Get the active power consumption limit
 *
 * @param self LP EG Use Case instance to get the active power consumption limit with
 * @param remote_entity_addr Remote entity address of the e.g. EVSE
 * @param limit The active power consumption limit output buffer, shall not be NULL
 * @return kEebusErrorOk on success, error code otherwise
 */
EebusError
EgLpGetActivePowerLimit(const EgLpUseCaseObject* self, const EntityAddressType* remote_entity_addr, LoadLimit* limit);

/**
 * @brief Request the active power limit from the Controllable System
 *
 * Sends a READ request for the active power limit matching the energy direction
 * of this use case instance. The reply is delivered asynchronously via @p cb.
 *
 * @param self LP EG Use Case instance to send the read request with
 * @param remote_entity_addr Remote entity address of the e.g. EVSE
 * @param cb Optional callback invoked when the reply is received, may be NULL
 * @param ctx Optional context pointer passed to @p cb, may be NULL
 * @return kEebusErrorOk if the request was sent successfully, error code otherwise
 */
EebusError EgLpReadActivePowerLimit(
    const EgLpUseCaseObject* self,
    const EntityAddressType* remote_entity_addr,
    ReplyMessageCallback cb,
    void* ctx
);

/**
 * @brief Send the new active power consumption limit
 *
 * @param self LP EG Use Case instance to send the active power consumption limit with
 * @param remote_entity_addr Remote entity address of the e.g. EVSE
 * @param limit The active power consumption limit to be sent
 * @return kEebusErrorOk on success, error code otherwise
 */
EebusError EgLpSetActivePowerLimit(
    EgLpUseCaseObject* self,
    const EntityAddressType* remote_entity_addr,
    const LoadLimit* limit,
    ResultMessageCallback cb,
    void* ctx
);

//-------------------------------------------------------------------------------------------//
//
// Scenario 2
//
//-------------------------------------------------------------------------------------------//

/**
 * @brief Get the Failsafe Limit for the consumed active (real) power of the
 * Controllable System. This limit becomes activated in "init" state or "failsafe state".
 *
 * @param self LP EG Use Case instance to get the Failsafe Limit with
 * @param power_limit Output buffer to store the Failsafe Power Limit value
 * @return kEebusErrorOk on success, error code otherwise
 */
EebusError EgLpGetFailsafeActivePowerLimit(
    const EgLpUseCaseObject* self,
    const EntityAddressType* remote_entity_addr,
    ScaledValue* power_limit
);

/**
 * @brief Request the failsafe active power limit from the Controllable System
 *
 * Sends a READ request for the failsafe active power limit key matching the
 * energy direction of this use case instance. The reply is delivered
 * asynchronously via @p cb.
 *
 * @param self LP EG Use Case instance to send the read request with
 * @param remote_entity_addr Remote entity address of the e.g. EVSE
 * @param cb Optional callback invoked when the reply is received, may be NULL
 * @param ctx Optional context pointer passed to @p cb, may be NULL
 * @return kEebusErrorOk if the request was sent successfully, error code otherwise
 */
EebusError EgLpReadFailsafeActivePowerLimit(
    const EgLpUseCaseObject* self,
    const EntityAddressType* remote_entity_addr,
    ReplyMessageCallback cb,
    void* ctx
);

/**
 * @brief Send new Failsafe  Active Power Limit
 *
 * @param remote_entity_addr Remote entity address of the e.g. EVSE
 * @param power_limit The new limit in W
 * @return kEebusErrorOk on success, error code otherwise
 */
EebusError EgLpSetFailsafeActivePowerLimit(
    EgLpUseCaseObject* self,
    const EntityAddressType* remote_entity_addr,
    const ScaledValue* power_limit,
    ResultMessageCallback cb,
    void* ctx
);

/**
 * @brief Get the minimum time the Controllable System remains in "failsafe state" unless conditions
 * specified in this Use Case permit leaving the "failsafe state"
 *
 * @param remote_entity_addr Remote entity address of the e.g. EVSE
 * @param duration The duration output buffer, shall not be NULL
 * @return kEebusErrorOk on success, error code otherwise
 */
EebusError EgLpGetFailsafeDurationMinimum(
    const EgLpUseCaseObject* self,
    const EntityAddressType* remote_entity_addr,
    DurationType* duration
);

/**
 * @brief Request the failsafe duration minimum from the Controllable System
 *
 * Sends a READ request for the failsafe duration minimum key. The reply is
 * delivered asynchronously via @p cb.
 *
 * @param self LP EG Use Case instance to send the read request with
 * @param remote_entity_addr Remote entity address of the e.g. EVSE
 * @param cb Optional callback invoked when the reply is received, may be NULL
 * @param ctx Optional context pointer passed to @p cb, may be NULL
 * @return kEebusErrorOk if the request was sent successfully, error code otherwise
 */
EebusError EgLpReadFailsafeDurationMinimum(
    const EgLpUseCaseObject* self,
    const EntityAddressType* remote_entity_addr,
    ReplyMessageCallback cb,
    void* ctx
);

/**
 * @brief Send the new Failsafe Duration Minimum
 *
 * @param remote_entity_addr Remote entity address of the e.g. EVSE
 * @param duration The duration, must be in range between 2h and 24h
 * @return kEebusErrorOk on success, error code otherwise
 */
EebusError EgLpSetFailsafeDurationMinimum(
    EgLpUseCaseObject* self,
    const EntityAddressType* remote_entity_addr,
    const EebusDuration* duration,
    ResultMessageCallback cb,
    void* ctx
);

//-------------------------------------------------------------------------------------------//
//
// Scenario 4
//
//-------------------------------------------------------------------------------------------//

/**
 * @brief Get the power nominal max from the Controllable System
 *
 * Returns powerConsumptionNominalMax / powerProductionNominalMax depending on the energy direction,
 * or the contractual variant if the nominal max is not available.
 *
 * @param self LP EG Use Case instance
 * @param remote_entity_addr Remote entity address
 * @param power_limit Output buffer for the nominal max value, shall not be NULL
 * @return kEebusErrorOk on success, error code otherwise
 */
EebusError EgLpGetPowerNominalMax(
    const EgLpUseCaseObject* self,
    const EntityAddressType* remote_entity_addr,
    ScaledValue* power_limit
);

/**
 * @brief Request the power nominal max from the Controllable System
 *
 * Sends a READ request for the power nominal max characteristic matching the
 * energy direction of this use case instance. The reply is delivered
 * asynchronously via @p cb.
 *
 * @param self LP EG Use Case instance to send the read request with
 * @param remote_entity_addr Remote entity address of the e.g. EVSE
 * @param cb Optional callback invoked when the reply is received, may be NULL
 * @param ctx Optional context pointer passed to @p cb, may be NULL
 * @return kEebusErrorOk if the request was sent successfully, error code otherwise
 */
EebusError EgLpReadPowerNominalMax(
    const EgLpUseCaseObject* self,
    const EntityAddressType* remote_entity_addr,
    ReplyMessageCallback cb,
    void* ctx
);

//-------------------------------------------------------------------------------------------//
//
// Scenario 3
//
//-------------------------------------------------------------------------------------------//

/**
 * @brief Start sending heartbeat from the local entity supporting this usecase
 *
 * The heartbeat is started by default when a non 0 timeout is set in the service configuration
 *
 * @param self EG LP Use Case instance to start the heartbeat with
 */
void EgLpStartHeartbeat(EgLpUseCaseObject* self);

/**
 * @brief Stop sending heartbeat from the local entity
 *
 * @param self EG LP Use Case instance to stop the heartbeat with
 */
void EgLpStopHeartbeat(EgLpUseCaseObject* self);

/**
 * @brief Check whether there was a heartbeat received within the last 2 minutes
 *
 * @param self EG LP Use Case instance to check the heartbeat data with
 * @param remote_entity_addr Remote entity address to check the heartbeat for
 * @return true if check is passed, false otherwise
 */
bool EgLpIsHeartbeatWithinDuration(EgLpUseCaseObject* self, const EntityAddressType* remote_entity_addr);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_USE_CASE_ACTOR_EG_EG_LP_H_
