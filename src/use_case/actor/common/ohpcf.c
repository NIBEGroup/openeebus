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
 * @brief Node Schedule helper functions implementation
 */

#include "src/spine/model/model.h"
#include "src/spine/model/power_sequences_types.h"
#include "src/spine/model/smart_energy_management_ps_types.h"

static const PowerSequenceNodeScheduleInformationDataType kNodeScheduleAnnounce = {
    .node_remote_controllable             = &(const bool){true},
    .supports_single_slot_scheduling_only = &(const bool){true},
    .alternatives_count                   = &(const uint32_t){1},
    .total_sequences_count_max            = &(const uint32_t){1},
    .supports_reselection                 = &(const bool){false},
};

static const PowerSequenceNodeScheduleInformationDataType kNodeScheduleClearProcess = {
    .node_remote_controllable             = &(const bool){true},
    .supports_single_slot_scheduling_only = &(const bool){true},
    .alternatives_count                   = &(const uint32_t){0},
    .total_sequences_count_max            = &(const uint32_t){1},
    .supports_reselection                 = &(const bool){false},
};

bool NodeScheduleMatch(
    const PowerSequenceNodeScheduleInformationDataType* node_schedule_info1,
    const PowerSequenceNodeScheduleInformationDataType* node_schedule_info2
);

const PowerSequenceNodeScheduleInformationDataType* OhpcfGetNodeScheduleAnnounceData() {
  return &kNodeScheduleAnnounce;
}

const PowerSequenceNodeScheduleInformationDataType* OhpcfGetNodeScheduleClearProcessData() {
  return &kNodeScheduleClearProcess;
}

bool NodeScheduleMatch(
    const PowerSequenceNodeScheduleInformationDataType* node_schedule_info1,
    const PowerSequenceNodeScheduleInformationDataType* node_schedule_info2
) {
  const EebusDataCfg* const cfg = ModelGetDataCfg(kFunctionTypePowerSequenceNodeScheduleInformationData);
  return EEBUS_DATA_SELECTORS_MATCH(cfg, &node_schedule_info1, cfg, &node_schedule_info2);
}

bool OhpcfNodeScheduleAnnounceMatch(const PowerSequenceNodeScheduleInformationDataType* node_schedule_info) {
  return NodeScheduleMatch(node_schedule_info, &kNodeScheduleAnnounce);
}

bool OhpcfNodeScheduleClearProcessMatch(const PowerSequenceNodeScheduleInformationDataType* node_schedule_info) {
  return NodeScheduleMatch(node_schedule_info, &kNodeScheduleClearProcess);
}

const SmartEnergyManagementPsAlternativesType* OhpcfSmartEnergyManagementPsGetAlternative(
    const SmartEnergyManagementPsDataType* sem_ps_data
) {
  if (sem_ps_data == NULL) {
    return NULL;
  }

  if ((sem_ps_data->alternatives == NULL) || (sem_ps_data->alternatives_size == 0)) {
    return NULL;
  }

  return sem_ps_data->alternatives[0];
}

const AlternativesIdType* OhpcfSmartEnergyManagementPsGetAlternativeId(
    const SmartEnergyManagementPsDataType* sem_ps_data
) {
  const SmartEnergyManagementPsAlternativesType* const alternative
      = OhpcfSmartEnergyManagementPsGetAlternative(sem_ps_data);

  if (alternative == NULL) {
    return NULL;
  }

  if (alternative->relation == NULL) {
    return NULL;
  }

  return alternative->relation->alternatives_id;
}

const SmartEnergyManagementPsPowerSequenceType* OhpcfSmartEnergyManagementPsGetPowerSequence(
    const SmartEnergyManagementPsDataType* sem_ps_data
) {
  const SmartEnergyManagementPsAlternativesType* const alternative
      = OhpcfSmartEnergyManagementPsGetAlternative(sem_ps_data);

  if (alternative == NULL) {
    return NULL;
  }

  if ((alternative->power_sequence == NULL) || (alternative->power_sequence_size == 0)) {
    return NULL;
  }

  return alternative->power_sequence[0];
}

const PowerSequenceIdType* OhpcfSmartEnergyManagementPsGetPowerSequenceId(
    const SmartEnergyManagementPsDataType* sem_ps_data
) {
  const SmartEnergyManagementPsPowerSequenceType* const power_sequence
      = OhpcfSmartEnergyManagementPsGetPowerSequence(sem_ps_data);

  if (power_sequence == NULL) {
    return NULL;
  }

  return power_sequence->description->sequence_id;
}

const SmartEnergyManagementPsPowerTimeSlotType* OhpcfSmartEnergyManagementPsPowerSequenceGetPowerTimeSlot(
    const SmartEnergyManagementPsPowerSequenceType* power_sequence
) {
  if (power_sequence == NULL) {
    return NULL;
  }

  if ((power_sequence->power_time_slot == NULL) || (power_sequence->power_time_slot_size == 0)) {
    return NULL;
  }

  return power_sequence->power_time_slot[0];
}

const ScaledNumberType* OhpcfSmartEnergyManagementPsPowerSequenceGetPowerMax(
    const SmartEnergyManagementPsPowerSequenceType* power_sequence
) {
  const SmartEnergyManagementPsPowerTimeSlotType* const power_time_slot
      = OhpcfSmartEnergyManagementPsPowerSequenceGetPowerTimeSlot(power_sequence);

  if (power_time_slot == NULL) {
    return NULL;
  }

  return SmartEnergyManagementPsPowerTimeSlotGetPowerMax(power_time_slot);
}
