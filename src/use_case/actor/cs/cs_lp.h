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
 * @brief Controllable System Limitation of Power use case base class to
 * be used by CS LPC and CS LPP concrete use cases
 */

#ifndef SRC_USE_CASE_ACTOR_CS_CS_LP_H_
#define SRC_USE_CASE_ACTOR_CS_CS_LP_H_

#include "src/spine/entity/entity_local.h"
#include "src/spine/model/electrical_connection_types.h"
#include "src/use_case/api/cs_lp_listener_interface.h"
#include "src/use_case/model/load_limit_types.h"
#include "src/use_case/use_case.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct CsLpUseCaseObject CsLpUseCaseObject;
struct CsLpUseCaseObject {
    /** Inherits the Use Case */
    UseCaseObject obj;
};

#define CS_LP_USE_CASE_OBJECT(obj) ((CsLpUseCaseObject*)(obj))

CsLpUseCaseObject* CsLpUseCaseCreate(
    EnergyDirectionType energy_direction,
    const UseCaseInfo* use_case_info,
    EntityLocalObject* local_entity,
    ElectricalConnectionIdType electrical_connection_id,
    CsLpListenerObject* cs_lp_listener
);

static inline void CsLpUseCaseDelete(CsLpUseCaseObject* cs_lp_use_case) {
    if (cs_lp_use_case != NULL) {
        USE_CASE_DESTRUCT(USE_CASE_OBJECT(cs_lp_use_case));
        EEBUS_FREE(cs_lp_use_case);
    }
}

/**
 * @brief Get the current load control limit data
 * @param self CS Limitation of Power instance to get the load control limit with
 * @param limit Buffer to load control limit data into
 * @return kEebusErrorOk on success, error code otherwise
 */
EebusError CsLpGetActivePowerLimit(const CsLpUseCaseObject* self, LoadLimit* limit);

/**
 * @brief Set the active power limit data
 * @param self CS Limitation of Power instance to set the load control limit with
 * @param limit Limit value to be set
 * @param is_active Flag indicating if the limit is active
 * @param is_changeable Flag indicating if the client service can change this value
 * @return kEebusErrorOk on success, error code otherwise
 */
EebusError
CsLpSetActivePowerLimit(CsLpUseCaseObject* self, const ScaledValue* limit, bool is_active, bool is_changeable);

/**
 * @brief Get the Failsafe Limit for the consumed active (real) power of the
 * Controllable System. This limit becomes activated in "init" state or "failsafe state".
 * @param self CS Limitation of Power instance to get the Failsafe Limit with
 * @param power_limit Output buffer to store the Failsafe Power Limit value
 * @param is_changeable Output buffer to store the changeable status
 * @return kEebusErrorOk on success, error code otherwise
 */
EebusError
CsLpGetFailsafeActivePowerLimit(const CsLpUseCaseObject* self, ScaledValue* power_limit, bool* is_changeable);

/**
 * @brief Set the Failsafe Limit for the consumed active (real) power of the
 * Controllable System. This limit becomes activated in "init" state or "failsafe state".
 * @param self CS Limitation of Power instance to set the Failsafe Limit with
 * @param power_limit Failsafe Power Limit value to be set
 * @param is_changeable Flag indicating if the client service can change this value
 * @return kEebusErrorOk on success, error code otherwise
 */
EebusError CsLpSetFailsafeActivePowerLimit(CsLpUseCaseObject* self, const ScaledValue* power_limit, bool is_changeable);

/**
 * @brief Get the minimum time the Controllable System remains in "failsafe state" unless conditions
 * specified in this Use Case permit leaving the "failsafe state"
 * @param self CS Limitation of Power instance to get the Failsafe Duration Minimum with
 * @param duration Output buffer to store the Failsafe Duration Minimum
 * @param is_changeable Output buffer to store the changeable status
 * @return kEebusErrorOk on success, error code otherwise
 */
EebusError CsLpGetFailsafeDurationMinimum(const CsLpUseCaseObject* self, DurationType* duration, bool* is_changeable);

/**
 * @brief Set minimum time the Controllable System remains in "failsafe state" unless conditions
 * specified in this Use Case permit leaving the "failsafe state"
 * @param self CS Limitation of Power instance to set the Failsafe Duration Minimum with
 * @param duration Duration to beset, has to be >= 2h and <= 24h
 * @param is_changeable Flag indicating if the client service can change this value
 * @return kEebusErrorOk on success, error code otherwise
 */
EebusError CsLpSetFailsafeDurationMinimum(CsLpUseCaseObject* self, const DurationType* duration, bool is_changeable);

/**
 * @brief Start sending heartbeat from the local entity supporting this usecase.
 * The heartbeat is started by default when a non 0 timeout is set in the service configuration
 * @param self CS Limitation of Power instance to start the heartbeat with
 */
void CsLpStartHeartbeat(CsLpUseCaseObject* self);

/**
 * @brief Stop sending heartbeat from the local CEM entity
 * @param self CS Limitation of Power instance to stop the heartbeat with
 */
void CsLpStopHeartbeat(CsLpUseCaseObject* self);

/**
 * @brief Check if the currently available heartbeat data is within a time duration
 * @param self CS Limitation of Power instance to check the heartbeat data with
 * @return true if check is passed, false otherwise
 */
bool CsLpIsHeartbeatWithinDuration(CsLpUseCaseObject* self);

/**
 * @brief Get the nominal maximum active (real) power the Controllable System is allowed to consume.
 *
 * If the local device type is an EnergyManagementSystem, the contractual consumption
 * nominal max is returned, otherwise the power consumption nominal max is returned.
 *
 * @param self CS Limitation of Power instance to get the nominal max with
 * @param nominal_max Pointer to the ScaledValue structure to store
 *                    the nominal max power consumption in W.
 * @return EebusError indicating the success or failure of the operation.
 */
EebusError CsLpGetNominalMax(CsLpUseCaseObject* self, ScaledValue* nominal_max);

/**
 * @brief Set the nominal maximum active (real) power the Controllable System is allowed to consume.
 *
 * If the local device type is an EnergyManagementSystem, the contractual consumption
 * nominal max is set, otherwise the power consumption nominal max is set.
 *
 * @param self Pointer to the CsLpUseCaseObject instance.
 * @param new_nominal_max Pointer to the ScaledValue structure containing
 *                        the new nominal max power consumption in W.
 * @return EebusError indicating the success or failure of the operation.
 */
EebusError CsLpSetNominalMax(CsLpUseCaseObject* self, const ScaledValue* new_nominal_max);

/**
 * @brief Get the characteristic type depending on the local entities device devicetype
 * @param self CS Limitation of Power instance to get the characteristic type with
 * @return Electrical connection characteristic type
 */
ElectricalConnectionCharacteristicTypeType CsLpGetElectricalConnectionCharacteristicType(const CsLpUseCaseObject* self);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_USE_CASE_ACTOR_CS_CS_LP_H_
