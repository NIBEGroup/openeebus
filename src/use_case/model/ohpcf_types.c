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
 * @brief OHPCF (Compressor and CEM) utilities implementation
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "src/common/array_util.h"
#include "src/spine/model/power_sequences_types.h"
#include "src/use_case/model/ohpcf_types.h"

typedef struct OhpcfStateMapping OhpcfStateMapping;

struct OhpcfStateMapping {
  CompressorOhpcfState ohpcf_state;
  const char* name;
  PowerSequenceStateType ps_state;
};

OhpcfStateMapping ohpcf_state_lut[] = {
    {kCompressorOhpcfStateUndefined, "undefined",   kPowerSequenceStateTypeInvalid},
    {kCompressorOhpcfStateAnnounced, "announced",   kPowerSequenceStateTypeInvalid},
    {kCompressorOhpcfStateScheduled, "scheduled", kPowerSequenceStateTypeScheduled},
    {  kCompressorOhpcfStateRunning,   "running",   kPowerSequenceStateTypeRunning},
    {   kCompressorOhpcfStatePaused,    "paused",    kPowerSequenceStateTypePaused},
    {  kCompressorOhpcfStateStopped,   "stopped",   kPowerSequenceStateTypeInvalid},
    {kCompressorOhpcfStateCompleted, "completed", kPowerSequenceStateTypeCompleted},
};

PowerSequenceStateType CompressorOhpcfStateGetPowerSequenceState(CompressorOhpcfState ohpcf_state) {
  for (size_t i = 0; i < ARRAY_SIZE(ohpcf_state_lut); ++i) {
    if (ohpcf_state_lut[i].ohpcf_state == ohpcf_state) {
      return ohpcf_state_lut[i].ps_state;
    }
  }

  return kPowerSequenceStateTypeInvalid;
}

CompressorOhpcfState CompressorOhpcfStateGetStateWithPowerSequenceState(PowerSequenceStateType ps_state) {
  for (size_t i = 0; i < ARRAY_SIZE(ohpcf_state_lut); ++i) {
    if (ohpcf_state_lut[i].ps_state == ps_state) {
      return ohpcf_state_lut[i].ohpcf_state;
    }
  }

  return kCompressorOhpcfStateUndefined;
}

const CompressorOhpcfState* CompressorOhpcfStateGetStateWithName(const char* name) {
  for (size_t i = 0; i < ARRAY_SIZE(ohpcf_state_lut); i++) {
    if (strcmp(name, ohpcf_state_lut[i].name) == 0) {
      return &ohpcf_state_lut[i].ohpcf_state;
    }
  }

  return NULL;
}

const char* CompressorOhpcfStateGetName(CompressorOhpcfState state) {
  for (size_t i = 0; i < ARRAY_SIZE(ohpcf_state_lut); i++) {
    if (ohpcf_state_lut[i].ohpcf_state == state) {
      return ohpcf_state_lut[i].name;
    }
  }

  return NULL;
}

void OptionalPowerConsumptionPrint(const OptionalPowerConsumption* opc) {
  printf("{\n");
  ScaledValuePrint("  max_power_w         = %sW,\n", &opc->max_power_w);
  EebusDurationPrint("  earliest_start_time = %s,\n", &opc->earliest_start_time);
  EebusDurationPrint("  latest_end_time     = %s,\n", &opc->latest_end_time);
  EebusDurationPrint("  active_duration_min = %s,\n", &opc->active_duration_min);
  printf("  is_stoppable        = %s,\n", opc->is_stoppable ? "true" : "false");
  printf("  is_pausable         = %s\n", opc->is_pausable ? "true" : "false");
  printf("}");
}
