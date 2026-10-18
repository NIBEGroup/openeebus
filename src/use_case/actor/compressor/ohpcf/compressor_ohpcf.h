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
 * @brief Compressor OHPCF use case
 */

#ifndef SRC_USE_CASE_ACTOR_COMPRESSOR_OHPCF_COMPRESSOR_OHPCF_H_
#define SRC_USE_CASE_ACTOR_COMPRESSOR_OHPCF_COMPRESSOR_OHPCF_H_

#include "src/spine/entity/entity_local.h"
#include "src/use_case/api/compressor_ohpcf_listener_interface.h"
#include "src/use_case/model/ohpcf_types.h"
#include "src/use_case/use_case.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct CompressorOhpcfUseCaseObject CompressorOhpcfUseCaseObject;
struct CompressorOhpcfUseCaseObject {
  /** Inherits the Use Case */
  UseCaseObject obj;
};

#define COMPRESSOR_OHPCF_USE_CASE_OBJECT(obj) ((CompressorOhpcfUseCaseObject*)(obj))

CompressorOhpcfUseCaseObject*
CompressorOhpcfUseCaseCreate(EntityLocalObject* local_entity, CompressorOhpcfListenerObject* cp_ohpcf_listener);

static inline void CompressorOhpcfUseCaseDelete(CompressorOhpcfUseCaseObject* cp_ohpcf_use_case) {
  if (cp_ohpcf_use_case != NULL) {
    USE_CASE_DESTRUCT(USE_CASE_OBJECT(cp_ohpcf_use_case));
    EEBUS_FREE(cp_ohpcf_use_case);
  }
}

//-------------------------------------------------------------------------------------------//
//
// Scenario 1
//
//-------------------------------------------------------------------------------------------//

/**
 * @brief [Phase A] Announcement of an optional power consumption process [OHPCF-001]:
 * This phase applies if the Actor Compressor can offer an optional power consumption.
 * @param self Compressor OHPCF use case object to announce the optional power consumption process for
 * @param optional_power_consumption Optional power consumption parameters
 * @return EebusError error code
 */
EebusError
CompressorOhpcfAnnounce(CompressorOhpcfUseCaseObject* self, const OptionalPowerConsumption* optional_power_consumption);

/**
 * @brief [Phase B] Report of the current state of the scheduled or active power consumption process [OHPCF-002]:
 * This phase applies after the Actor CEM has taken a proper choice (as described above) and the Actor
 * Compressor accepted this choice [OHPCF-012].
 * @note "Is pausable" and "is stoppable" [OHPCF-012/3] are not used in current implementation.
 * @param self Compressor OHPCF use case object to set the state for
 * @param state The current state of this power consumption process [OHPCF-012/2]
 * See CompressorOhpcfState for details.
 * @param start_time The relative start time of the consumption of power [OHPCF-012/1]
 * @return EebusError error code
 */
EebusError CompressorOhpcfReportState(
    CompressorOhpcfUseCaseObject* self,
    CompressorOhpcfState state,
    const EebusDuration* start_time
);

/**
 * @brief [Phase D] Announces that there is no process [OHPCF-003]
 * @param self Compressor OHPCF use case object to clear the process for
 * @return EebusError error code
 */
EebusError CompressorOhpcfClearProcess(CompressorOhpcfUseCaseObject* self);

//-------------------------------------------------------------------------------------------//
//
// Scenario 2
//
//-------------------------------------------------------------------------------------------//

/**
 * @brief Get the current state of the scheduled or active power consumption process
 * @param self Compressor OHPCF use case object to get the state from
 * @return CompressorOhpcfState current state
 */
CompressorOhpcfState CompressorOhpcfGetState(CompressorOhpcfUseCaseObject* self);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_USE_CASE_ACTOR_COMPRESSOR_OHPCF_COMPRESSOR_OHPCF_H_
