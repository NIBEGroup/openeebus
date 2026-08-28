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
 * @brief Energy Guard LPP use case
 */

#ifndef SRC_USE_CASE_ACTOR_EG_LPP_EG_LPP_H_
#define SRC_USE_CASE_ACTOR_EG_LPP_EG_LPP_H_

#include "src/spine/entity/entity_local.h"
#include "src/use_case/actor/eg/eg_lp.h"
#include "src/use_case/api/eg_lp_listener_interface.h"
#include "src/use_case/use_case.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

EgLpUseCaseObject* EgLppUseCaseCreate(EntityLocalObject* local_entity, EgLpListenerObject* eg_lpp_listener);

static inline void EgLppUseCaseDelete(EgLpUseCaseObject* eg_lpp_use_case) {
    EgLpUseCaseDelete(eg_lpp_use_case);
}

//-------------------------------------------------------------------------------------------//
//
// Scenario 1
//
//-------------------------------------------------------------------------------------------//

/**
 * @brief Get the active power production limit
 *
 * @param self EG LPP Use Case instance to get the active power production limit with
 * @param remote_entity_addr Remote entity address of the e.g. Inverter
 * @param limit The active power production limit output buffer, shall not be NULL
 * @return kEebusErrorOk on success, error code otherwise
 */
static inline EebusError EgLppGetActiveProductionPowerLimit(
    const EgLpUseCaseObject* self,
    const EntityAddressType* remote_entity_addr,
    LoadLimit* limit
) {
    return EgLpGetActivePowerLimit(self, remote_entity_addr, limit);
}

static inline EebusError EgLppReadActiveProductionPowerLimit(
    const EgLpUseCaseObject* self,
    const EntityAddressType* remote_entity_addr,
    ReplyMessageCallback cb,
    void* ctx
) {
    return EgLpReadActivePowerLimit(self, remote_entity_addr, cb, ctx);
}

/**
 * @brief Send the new active power production limit
 *
 * @param self EG LPP Use Case instance to send the active power production limit with
 * @param remote_entity_addr Remote entity address of the e.g. Inverter
 * @param limit The active power production limit to be sent
 * @return kEebusErrorOk on success, error code otherwise
 */
static inline EebusError EgLppSetActiveProductionPowerLimit(
    EgLpUseCaseObject* self,
    const EntityAddressType* remote_entity_addr,
    const LoadLimit* limit,
    ResultMessageCallback cb,
    void* ctx
) {
    return EgLpSetActivePowerLimit(self, remote_entity_addr, limit, cb, ctx);
}

//-------------------------------------------------------------------------------------------//
//
// Scenario 2
//
//-------------------------------------------------------------------------------------------//

/**
 * @brief Get the Failsafe Limit for the produced active (real) power of the
 * Controllable System. This limit becomes activated in "init" state or "failsafe state".
 *
 * @param self EG LPP Use Case instance to get the Failsafe Production Limit with
 * @param power_limit Output buffer to store the Failsafe Power Limit value
 * @return kEebusErrorOk on success, error code otherwise
 */
static inline EebusError EgLppGetFailsafeProductionActivePowerLimit(
    const EgLpUseCaseObject* self,
    const EntityAddressType* remote_entity_addr,
    ScaledValue* power_limit
) {
    return EgLpGetFailsafeActivePowerLimit(self, remote_entity_addr, power_limit);
}

static inline EebusError EgLppReadFailsafeProductionActivePowerLimit(
    const EgLpUseCaseObject* self,
    const EntityAddressType* remote_entity_addr,
    ReplyMessageCallback cb,
    void* ctx
) {
    return EgLpReadFailsafeActivePowerLimit(self, remote_entity_addr, cb, ctx);
}

/**
 * @brief Send new Failsafe Production Active Power Limit
 *
 * @param remote_entity_addr Remote entity address of the e.g. Inverter
 * @param power_limit The new limit in W
 * @return kEebusErrorOk on success, error code otherwise
 */
static inline EebusError EgLppSetFailsafeProductionActivePowerLimit(
    EgLpUseCaseObject* self,
    const EntityAddressType* remote_entity_addr,
    const ScaledValue* power_limit,
    ResultMessageCallback cb,
    void* ctx
) {
    return EgLpSetFailsafeActivePowerLimit(self, remote_entity_addr, power_limit, cb, ctx);
}

/**
 * @brief Get the minimum time the Controllable System remains in "failsafe state" unless conditions
 * specified in this Use Case permit leaving the "failsafe state"
 *
 * @param remote_entity_addr Remote entity address of the e.g. Inverter
 * @param duration The duration output buffer, shall not be NULL
 * @return kEebusErrorOk on success, error code otherwise
 */
static inline EebusError EgLppGetFailsafeDurationMinimum(
    const EgLpUseCaseObject* self,
    const EntityAddressType* remote_entity_addr,
    DurationType* duration
) {
    return EgLpGetFailsafeDurationMinimum(self, remote_entity_addr, duration);
}

/**
 * @brief Send the new Failsafe Duration Minimum
 *
 * @param remote_entity_addr Remote entity address of the e.g. Inverter
 * @param duration The duration, must be in range between 2h and 24h
 * @return kEebusErrorOk on success, error code otherwise
 */
static inline EebusError EgLppReadFailsafeDurationMinimum(
    const EgLpUseCaseObject* self,
    const EntityAddressType* remote_entity_addr,
    ReplyMessageCallback cb,
    void* ctx
) {
    return EgLpReadFailsafeDurationMinimum(self, remote_entity_addr, cb, ctx);
}

static inline EebusError EgLppSetFailsafeDurationMinimum(
    EgLpUseCaseObject* self,
    const EntityAddressType* remote_entity_addr,
    const EebusDuration* duration,
    ResultMessageCallback cb,
    void* ctx
) {
    return EgLpSetFailsafeDurationMinimum(self, remote_entity_addr, duration, cb, ctx);
}

//-------------------------------------------------------------------------------------------//
//
// Scenario 4
//
//-------------------------------------------------------------------------------------------//

/**
 * @brief Get the power production nominal max from the Controllable System
 *
 * Returns powerProductionNominalMax for non-CEM CS or contractualProductionNominalMax for CEM CS.
 *
 * @param self EG LPP Use Case instance
 * @param remote_entity_addr Remote entity address
 * @param power_limit Output buffer for the nominal max value, shall not be NULL
 * @return kEebusErrorOk on success, error code otherwise
 */
static inline EebusError EgLppGetPowerProductionNominalMax(
    const EgLpUseCaseObject* self,
    const EntityAddressType* remote_entity_addr,
    ScaledValue* power_limit
) {
    return EgLpGetPowerNominalMax(self, remote_entity_addr, power_limit);
}

static inline EebusError EgLppReadPowerProductionNominalMax(
    const EgLpUseCaseObject* self,
    const EntityAddressType* remote_entity_addr,
    ReplyMessageCallback cb,
    void* ctx
) {
    return EgLpReadPowerNominalMax(self, remote_entity_addr, cb, ctx);
}

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
 * @param self EG LPP Use Case instance to start the heartbeat with
 */
static inline void EgLppStartHeartbeat(EgLpUseCaseObject* self) {
    EgLpStartHeartbeat(self);
}

/**
 * @brief Stop sending heartbeat from the local entity
 *
 * @param self EG LPP Use Case instance to stop the heartbeat with
 *
 * @warning Stopping the heartbeat freezes the timestamp published in the local
 *          DeviceDiagnosis/Server feature. If a remote CS reconnects while the
 *          heartbeat is stopped, it will receive that stale timestamp, compute
 *          that the heartbeat window has already expired, and immediately
 *          disconnect (SHIP state error). The reference HEMS implementation
 *          never calls this function; the heartbeat manager is started once
 *          by SetLocalFeature() and must keep running for the lifetime of the
 *          service. Only call EgLppStopHeartbeat() when you also intend to
 *          stop the entire service.
 */
static inline void EgLppStopHeartbeat(EgLpUseCaseObject* self) {
    EgLpStopHeartbeat(self);
}

/**
 * @brief Check whether there was a heartbeat received within the last 2 minutes
 *
 * @param self EG LPP Use Case instance to check the heartbeat data with
 * @param remote_entity_addr Remote entity address to check the heartbeat for
 * @return true if check is passed, false otherwise
 */
static inline bool
EgLppIsHeartbeatWithinDuration(EgLpUseCaseObject* self, const EntityAddressType* remote_entity_addr) {
    return EgLpIsHeartbeatWithinDuration(self, remote_entity_addr);
}

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_USE_CASE_ACTOR_EG_LPP_EG_LPP_H_
