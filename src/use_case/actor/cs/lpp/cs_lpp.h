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
 * @brief Controllable System LPP use case
 */

#ifndef SRC_USE_CASE_ACTOR_CS_LPP_CS_LPP_H_
#define SRC_USE_CASE_ACTOR_CS_LPP_CS_LPP_H_

#include "src/spine/entity/entity_local.h"
#include "src/spine/model/electrical_connection_types.h"
#include "src/use_case/actor/cs/cs_lp.h"
#include "src/use_case/api/cs_lp_listener_interface.h"
#include "src/use_case/model/load_limit_types.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

CsLpUseCaseObject* CsLppUseCaseCreate(
    EntityLocalObject* local_entity,
    ElectricalConnectionIdType ec_id,
    CsLpListenerObject* cs_lp_listener
);

static inline void CsLppUseCaseDelete(CsLpUseCaseObject* cs_lpp_use_case) {
    CsLpUseCaseDelete(CS_LP_USE_CASE_OBJECT(cs_lpp_use_case));
}

/**
 * @brief Get the LPP power production limit
 * @param self CS LPP Use Case instance to get the load control limit with
 * @param limit Buffer to load control limit data into
 * @return kEebusErrorOk on success, error code otherwise
 */
static inline EebusError CsLppGetActiveProductionPowerLimit(const CsLpUseCaseObject* self, LoadLimit* limit) {
    return CsLpGetActivePowerLimit(CS_LP_USE_CASE_OBJECT(self), limit);
}

/**
 * @brief Set the active power production limit
 * @param self CS LPP Use Case instance to set the load control limit with
 * @param limit Limit value to be set
 * @param is_active Flag indicating if the limit is active
 * @param is_changeable Flag indicating if the client service can change this value
 * @return kEebusErrorOk on success, error code otherwise
 */
static inline EebusError CsLppSetActiveProductionPowerLimit(
    CsLpUseCaseObject* self,
    const ScaledValue* limit,
    bool is_active,
    bool is_changeable
) {
    return CsLpSetActivePowerLimit(CS_LP_USE_CASE_OBJECT(self), limit, is_active, is_changeable);
}

/**
 * @brief Get the Failsafe Limit for the produced active (real) power of the
 * Controllable System. This limit becomes activated in "init" state or "failsafe state".
 * @param self CS LPP Use Case instance to get the Failsafe Limit with
 * @param power_limit Output buffer to store the Failsafe Power Limit value
 * @param is_changeable Output buffer to store the changeable status
 * @return kEebusErrorOk on success, error code otherwise
 */
static inline EebusError CsLppGetFailsafeProductionActivePowerLimit(
    const CsLpUseCaseObject* self,
    ScaledValue* power_limit,
    bool* is_changeable
) {
    return CsLpGetFailsafeActivePowerLimit(CS_LP_USE_CASE_OBJECT(self), power_limit, is_changeable);
}

/**
 * @brief Set the Failsafe Limit for the produced active (real) power of the
 * Controllable System. This limit becomes activated in "init" state or "failsafe state".
 * @param self CS LPP Use Case instance to set the Failsafe Limit with
 * @param power_limit Failsafe Power Limit value to be set
 * @param is_changeable Flag indicating if the client service can change this value
 * @return kEebusErrorOk on success, error code otherwise
 */
static inline EebusError CsLppSetFailsafeProductionActivePowerLimit(
    CsLpUseCaseObject* self,
    const ScaledValue* power_limit,
    bool is_changeable
) {
    return CsLpSetFailsafeActivePowerLimit(CS_LP_USE_CASE_OBJECT(self), power_limit, is_changeable);
}

/**
 * @brief Get the minimum time the Controllable System remains in "failsafe state" unless conditions
 * specified in this Use Case permit leaving the "failsafe state"
 * @param self CS LPP Use Case instance to get the Failsafe Duration Minimum with
 * @param duration Output buffer to store the Failsafe Duration Minimum
 * @param is_changeable Output buffer to store the changeable status
 * @return kEebusErrorOk on success, error code otherwise
 */
static inline EebusError
CsLppGetFailsafeDurationMinimum(const CsLpUseCaseObject* self, DurationType* duration, bool* is_changeable) {
    return CsLpGetFailsafeDurationMinimum(CS_LP_USE_CASE_OBJECT(self), duration, is_changeable);
}

/**
 * @brief Set minimum time the Controllable System remains in "failsafe state" unless conditions
 * specified in this Use Case permit leaving the "failsafe state"
 * @param self CS LPP Use Case instance to set the Failsafe Duration Minimum with
 * @param duration Duration to beset, has to be >= 2h and <= 24h
 * @param is_changeable Flag indicating if the client service can change this value
 * @return kEebusErrorOk on success, error code otherwise
 */
static inline EebusError
CsLppSetFailsafeDurationMinimum(CsLpUseCaseObject* self, const DurationType* duration, bool is_changeable) {
    return CsLpSetFailsafeDurationMinimum(CS_LP_USE_CASE_OBJECT(self), duration, is_changeable);
}

/**
 * @brief Start sending heartbeat from the local entity supporting this usecase.
 * The heartbeat is started by default when a non 0 timeout is set in the service configuration
 * @param self CS LPP Use Case instance to start the heartbeat with
 */
static inline void CsLppStartHeartbeat(CsLpUseCaseObject* self) {
    CsLpStartHeartbeat(CS_LP_USE_CASE_OBJECT(self));
}

/**
 * @brief Stop sending heartbeat from the local CEM entity
 * @param self CS LPP Use Case instance to stop the heartbeat with
 */
static inline void CsLppStopHeartbeat(CsLpUseCaseObject* self) {
    CsLpStopHeartbeat(CS_LP_USE_CASE_OBJECT(self));
}

/**
 * @brief Check if the currently available heartbeat data is within a time duration
 * @param self CS LPP Use Case instance to check the heartbeat data with
 * @return true if check is passed, false otherwise
 */
static inline bool CsLppIsHeartbeatWithinDuration(CsLpUseCaseObject* self) {
    return CsLpIsHeartbeatWithinDuration(CS_LP_USE_CASE_OBJECT(self));
}

/**
 * @brief Get the nominal maximum active (real) power the Controllable System is allowed to consume.
 *
 * If the local device type is an EnergyManagementSystem, the contractual production
 * nominal max is returned, otherwise the power production nominal max is returned.
 *
 * @param self CS LPP Use Case instance to get the nominal max with
 * @param nominal_max Pointer to the ScaledValue structure to store
 *                    the nominal max power production in W.
 * @return EebusError indicating the success or failure of the operation.
 */
static inline EebusError CsLppGetProductionNominalMax(CsLpUseCaseObject* self, ScaledValue* nominal_max) {
    return CsLpGetNominalMax(CS_LP_USE_CASE_OBJECT(self), nominal_max);
}

/**
 * @brief Set the nominal maximum active (real) power the Controllable System is allowed to consume.
 *
 * If the local device type is an EnergyManagementSystem, the contractual production
 * nominal max is set, otherwise the power production nominal max is set.
 *
 * @param self CS LPP Use Case instance to set the nominal max with
 * @param new_nominal_max Pointer to the ScaledValue structure containing
 *                        the new nominal max power production in W.
 * @return EebusError indicating the success or failure of the operation.
 */
static inline EebusError CsLppSetProductionNominalMax(CsLpUseCaseObject* self, const ScaledValue* new_nominal_max) {
    return CsLpSetNominalMax(CS_LP_USE_CASE_OBJECT(self), new_nominal_max);
}

/**
 * @brief Get the characteristic type depending on the local entities device devicetype
 * @param self CS LPP Use Case instance to check the heartbeat data with
 * @return Electrical connection characteristic type
 */
static inline ElectricalConnectionCharacteristicTypeType CsLppGetElectricalConnectionCharacteristicType(
    const CsLpUseCaseObject* self
) {
    return CsLpGetElectricalConnectionCharacteristicType(CS_LP_USE_CASE_OBJECT(self));
}

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_USE_CASE_ACTOR_CS_LPP_CS_LPP_H_
