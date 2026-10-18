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
 * @brief Customer Energy Manager OHPCF use case
 */

#ifndef SRC_USE_CASE_ACTOR_CEM_CEM_OHPCF_H_
#define SRC_USE_CASE_ACTOR_CEM_CEM_OHPCF_H_

#include "src/spine/api/feature_local_interface.h"
#include "src/spine/entity/entity_local.h"
#include "src/use_case/api/cem_ohpcf_listener_interface.h"
#include "src/use_case/model/ohpcf_types.h"
#include "src/use_case/use_case.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct CemOhpcfUseCaseObject CemOhpcfUseCaseObject;
struct CemOhpcfUseCaseObject {
  /** Inherits the Entity */
  UseCaseObject obj;
};

#define CEM_OHPCF_USE_CASE_OBJECT(obj) ((CemOhpcfUseCaseObject*)(obj))

CemOhpcfUseCaseObject*
CemOhpcfUseCaseCreate(EntityLocalObject* local_entity, CemOhpcfListenerObject* cem_ohpcf_listener);

static inline void CemOhpcfUseCaseDelete(CemOhpcfUseCaseObject* cem_ohpcf_use_case) {
  if (cem_ohpcf_use_case != NULL) {
    USE_CASE_DESTRUCT(USE_CASE_OBJECT(cem_ohpcf_use_case));
    EEBUS_FREE(cem_ohpcf_use_case);
  }
}

//-------------------------------------------------------------------------------------------//
//
// Scenario 1
//
//-------------------------------------------------------------------------------------------//
/**
 * @brief Get the announced optional power consumption
 *
 * @param self CEM OHPCF Use Case instance to get the announced optional power consumption with
 * @param remote_entity_addr Remote entity address of the Compressor
 * @param optional_power_consumption The optional power consumption output buffer, shall not be NULL
 * @return kEebusErrorOk on success, error code otherwise
 */
EebusError CemOhpcfGetAnnouncedOptionalPowerConsumption(
    const CemOhpcfUseCaseObject* self,
    const EntityAddressType* remote_entity_addr,
    OptionalPowerConsumption* optional_power_consumption
);

/**
 * @brief Get the compressor state
 *
 * @param self CEM OHPCF Use Case instance to get the compressor state with
 * @param remote_entity_addr Remote entity address of the Compressor
 * @return The current compressor state
 */
CompressorOhpcfState
CemOhpcfGetCompressorState(const CemOhpcfUseCaseObject* self, const EntityAddressType* remote_entity_addr);

//-------------------------------------------------------------------------------------------//
//
// Scenario 2
//
//-------------------------------------------------------------------------------------------//

/**
 * @brief Request Smart Energy Management PS data from the Compressor
 *
 * Sends a READ request for the SmartEnergyManagementPsData from the remote
 * Compressor entity. The reply is delivered asynchronously via @p cb.
 *
 * @param self CEM OHPCF Use Case instance to send the read request with
 * @param remote_entity_addr Remote entity address of the Compressor
 * @param cb Optional callback invoked when the reply is received, may be NULL
 * @param ctx Optional context pointer passed to @p cb, may be NULL
 * @return kEebusErrorOk if the request was sent successfully, error code otherwise
 */
EebusError CemOhpcfReadSmartData(
    const CemOhpcfUseCaseObject* self,
    const EntityAddressType* remote_entity_addr,
    ReplyMessageCallback cb,
    void* ctx
);

/**
 * @brief [Phase A] Scheduling of an optional power consumption process [OHPCF-001]:
 * The Actor CEM may schedule an optional power consumption process at the Actor CEM by sending
 * a proper announcement message [OHPCF-011].
 * @param self CEM OHPCF use case object to schedule the optional power consumption for
 * @param start_time The relative start time of the consumption of power [OHPCF-011/3]
 * @return EebusError error code
 */
EebusError CemOhpcfScheduleOptionalPowerConsumption(
    CemOhpcfUseCaseObject* self,
    const EntityAddressType* remote_entity_addr,
    const EebusDuration* start_time,
    ResultMessageCallback cb,
    void* ctx
);

/**
 * @brief [Phase B] Write a command [OHPCF-022] to Compressor to stop (abort) the process [OHPCF-022/1]
 * @param self CEM OHPCF use case object to send the stop request from
 * @param remote_entity_addr Remote entity address of the Compressor
 * return EebusError error code
 */
EebusError CemOhpcfWriteStopCommand(
    CemOhpcfUseCaseObject* self,
    const EntityAddressType* remote_entity_addr,
    ResultMessageCallback cb,
    void* ctx
);

/**
 * @brief [Phase B] Write a command [OHPCF-022] to Compressor to pause the process [OHPCF-022/2]
 * (if it is currently in a "running" state)
 * @param self CEM OHPCF use case object to send the pause request from
 * @param remote_entity_addr Remote entity address of the Compressor
 */
EebusError CemOhpcfWritePauseCommand(
    CemOhpcfUseCaseObject* self,
    const EntityAddressType* remote_entity_addr,
    ResultMessageCallback cb,
    void* ctx
);

/**
 * @brief [Phase B] Write a command [OHPCF-022] to Compressor to resume the process [OHPCF-022/3]
 * (if it is currently in a "paused" state)
 * @param self CEM OHPCF use case object to send the resume request from
 * @param remote_entity_addr Remote entity address of the Compressor
 */
EebusError CemOhpcfWriteResumeCommand(
    CemOhpcfUseCaseObject* self,
    const EntityAddressType* remote_entity_addr,
    ResultMessageCallback cb,
    void* ctx
);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_USE_CASE_ACTOR_CEM_CEM_OHPCF_H_
