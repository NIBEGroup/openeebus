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
 * @brief OHPCF (Compressor and CEM) type declarations and constants
 */

#ifndef SRC_USE_CASE_API_OHPCF_TYPES_H_
#define SRC_USE_CASE_API_OHPCF_TYPES_H_

#include "src/common/eebus_date_time/eebus_date_time.h"
#include "src/spine/model/power_sequences_types.h"
#include "src/use_case/model/scaled_value.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

static const AlternativesIdType kOhpcfAlternativeId            = 0;
static const PowerSequenceIdType kOhpcfPowerSequenceId         = 0;
static const PowerTimeSlotNumberType kOhpcfPowerTimeSlotNumber = 0;

/**
 * @brief OHPCF Phase bits enumeration
 */
enum OhpcfPhase {
  kCompressorOhpcfPhaseA = 0x10,
  kCompressorOhpcfPhaseB = 0x20,
  kCompressorOhpcfPhaseC = 0x40,
  kCompressorOhpcfPhaseD = 0x80,
};

/**
 * @brief Compressor OHPCF State [OHPCF-012/2] enumeration
 */
enum CompressorOhpcfState {
  /** Undefined state */
  kCompressorOhpcfStateUndefined = kCompressorOhpcfPhaseD | 0x01,
  /** "announced", announced but not scheduled yet */
  kCompressorOhpcfStateAnnounced = kCompressorOhpcfPhaseA | 0x02,
  /** "scheduled", scheduled but not started yet [OHPCF-012/2/1] */
  kCompressorOhpcfStateScheduled = kCompressorOhpcfPhaseA | 0x03,
  /** "running", currently consume power [OHPCF-012/2/2] */
  kCompressorOhpcfStateRunning = kCompressorOhpcfPhaseB | 0x04,
  /** "paused", paused [OHPCF-012/2/3] */
  kCompressorOhpcfStatePaused = kCompressorOhpcfPhaseB | 0x05,
  /** "invalid", stopped/aborted [OHPCF-012/2/4] */
  kCompressorOhpcfStateStopped = kCompressorOhpcfPhaseC | 0x06,
  /**< "completed", completed [OHPCF-012/2/5] */
  kCompressorOhpcfStateCompleted = kCompressorOhpcfPhaseC | 0x07,
};

/**
 * @brief Compressor OHPCF State [OHPCF-012/2]
 */
typedef enum CompressorOhpcfState CompressorOhpcfState;

/**
 * @brief Optional power consumption parameters
 */
typedef struct OptionalPowerConsumption OptionalPowerConsumption;

/**
 * @brief Optional power consumption parameters structure
 */
struct OptionalPowerConsumption {
  ScaledValue max_power_w;           /**< Maximum power in Watts [OHPCF-011/2/3] and [OHPCF-011/2/1] */
  EebusDuration earliest_start_time; /**< Earliest start time [OHPCF-011/4] */
  EebusDuration latest_end_time;     /**< Latest end time [OHPCF-011/4] */
  EebusDuration active_duration_min; /**< Active duration minimum [OHPCF-008] */
  bool is_stoppable;                 /**< Indicates whether the consumption may be stopped by the CEM [OHPCF-011/5] */
  bool is_pausable; /**< Indicates whether the consumption may be paused and resumed by the CEM [OHPCF-011/6] */
};

/**
 * @brief Get the power sequence state related to the Compressor OHPCF state
 * @param ohpcf_state Compressor OHPCF state
 * @return Power sequence state
 */
PowerSequenceStateType CompressorOhpcfStateGetPowerSequenceState(CompressorOhpcfState ohpcf_state);

/**
 * @brief Get the Compressor OHPCF state related to the power sequence state
 * @param ps_state Power sequence state
 * @return Compressor OHPCF state
 */
CompressorOhpcfState CompressorOhpcfStateGetStateWithPowerSequenceState(PowerSequenceStateType ps_state);

/**
 * @brief Get the Compressor OHPCF state with the state name
 * @param name Name of the state
 * @return Pointer to the Compressor OHPCF state, or NULL if not found
 */
const CompressorOhpcfState* CompressorOhpcfStateGetStateWithName(const char* name);

/**
 * @brief Get the name of the Compressor OHPCF state
 * @param state Compressor OHPCF state
 * @return Name of the state
 */
const char* CompressorOhpcfStateGetName(CompressorOhpcfState state);

/**
 * @brief Print Optional Power Consumption structure
 * @param opc Pointer to Optional Power Consumption structure
 */
void OptionalPowerConsumptionPrint(const OptionalPowerConsumption* opc);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_USE_CASE_API_OHPCF_TYPES_H_
