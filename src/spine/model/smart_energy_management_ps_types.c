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
 * @brief Smart Energy Management PS data helper functions implementation
 */

#include "src/spine/model/smart_energy_management_ps_types.h"

static bool
AlternativeIdMatch(const SmartEnergyManagementPsAlternativesType* alternative, AlternativesIdType alternative_id);

static bool
PowerSequenceIdMatch(const SmartEnergyManagementPsPowerSequenceType* power_sequence, PowerSequenceIdType sequence_id);

static const SmartEnergyManagementPsPowerSequenceType*
GetPowerSequenceWithId(const SmartEnergyManagementPsAlternativesType* alternative, PowerSequenceIdType sequence_id);

bool AlternativeIdMatch(const SmartEnergyManagementPsAlternativesType* alternative, AlternativesIdType alternative_id) {
  if (alternative == NULL) {
    return false;
  }

  if ((alternative->relation == NULL) || (alternative->relation->alternatives_id == NULL)) {
    return false;
  }

  return *alternative->relation->alternatives_id == alternative_id;
}

const SmartEnergyManagementPsAlternativesType* SmartEnergyManagementPsDataGetAlternativeWithId(
    const SmartEnergyManagementPsDataType* sem_ps_data,
    AlternativesIdType alternative_id
) {
  if ((sem_ps_data == NULL) || (sem_ps_data->alternatives == NULL)) {
    return NULL;
  }

  for (size_t i = 0; i < sem_ps_data->alternatives_size; i++) {
    const SmartEnergyManagementPsAlternativesType* const alternative = sem_ps_data->alternatives[i];
    if (AlternativeIdMatch(sem_ps_data->alternatives[i], alternative_id)) {
      return alternative;
    }
  }

  return NULL;
}

bool PowerSequenceIdMatch(
    const SmartEnergyManagementPsPowerSequenceType* power_sequence,
    PowerSequenceIdType sequence_id
) {
  if (power_sequence == NULL) {
    return false;
  }

  if ((power_sequence->description == NULL) || (power_sequence->description->sequence_id == NULL)) {
    return false;
  }

  return *power_sequence->description->sequence_id == sequence_id;
}

const SmartEnergyManagementPsPowerSequenceType*
GetPowerSequenceWithId(const SmartEnergyManagementPsAlternativesType* alternative, PowerSequenceIdType sequence_id) {
  if (alternative == NULL) {
    return NULL;
  }

  for (size_t i = 0; i < alternative->power_sequence_size; i++) {
    const SmartEnergyManagementPsPowerSequenceType* power_sequence = alternative->power_sequence[i];
    if (PowerSequenceIdMatch(power_sequence, sequence_id)) {
      return power_sequence;
    }
  }

  return NULL;
}

const SmartEnergyManagementPsPowerSequenceType* SmartEnergyManagementPsGetPowerSequenceWithId(
    const SmartEnergyManagementPsDataType* sem_ps_data,
    PowerSequenceIdType sequence_id
) {
  if ((sem_ps_data == NULL) || (sem_ps_data->alternatives == NULL)) {
    return NULL;
  }

  for (size_t i = 0; i < sem_ps_data->alternatives_size; i++) {
    const SmartEnergyManagementPsAlternativesType* const alternative = sem_ps_data->alternatives[i];
    const SmartEnergyManagementPsPowerSequenceType* const power_sequence
        = GetPowerSequenceWithId(alternative, sequence_id);
    if (power_sequence != NULL) {
      return power_sequence;
    }
  }

  return NULL;
}

const PowerSequenceStateType* SmartEnergyManagementPsPowerSequenceGetState(
    const SmartEnergyManagementPsPowerSequenceType* power_sequence
) {
  if (power_sequence == NULL) {
    return NULL;
  }

  if ((power_sequence->state == NULL) || (power_sequence->state->state == NULL)) {
    return NULL;
  }

  return power_sequence->state->state;
}

const AbsoluteOrRelativeTimeType* SmartEnergyManagementPsPowerSequenceGetStartTime(
    const SmartEnergyManagementPsPowerSequenceType* power_sequence
) {
  if (power_sequence == NULL) {
    return NULL;
  }

  if (power_sequence->schedule == NULL) {
    return NULL;
  }

  return power_sequence->schedule->start_time;
}

UnitOfMeasurementType SmartEnergyManagementPsPowerSequenceGetUnit(
    const SmartEnergyManagementPsPowerSequenceType* power_sequence
) {
  if (power_sequence == NULL) {
    return kUnitOfMeasurementTypeUnknown;
  }

  if ((power_sequence->description == NULL) || (power_sequence->description->power_unit == NULL)) {
    return kUnitOfMeasurementTypeUnknown;
  }

  return *power_sequence->description->power_unit;
}

const EebusDuration* SmartEnergyManagementPsPowerSequenceGetEarliestStartTime(
    const SmartEnergyManagementPsPowerSequenceType* power_sequence
) {
  if (power_sequence == NULL) {
    return NULL;
  }

  const PowerSequenceScheduleConstraintsDataType* const schedule_constraints = power_sequence->schedule_constraints;
  if (schedule_constraints == NULL) {
    return NULL;
  }

  const AbsoluteOrRelativeTimeType* const earliest_start_time = schedule_constraints->earliest_start_time;
  if (earliest_start_time == NULL) {
    return NULL;
  }

  if (earliest_start_time->type != kAbsoluteOrRelativeTimeTypeDuration) {
    return NULL;
  }

  return &earliest_start_time->duration;
}

const EebusDuration* SmartEnergyManagementPsPowerSequenceGetLatestEndTime(
    const SmartEnergyManagementPsPowerSequenceType* power_sequence
) {
  if (power_sequence == NULL) {
    return NULL;
  }

  const PowerSequenceScheduleConstraintsDataType* const schedule_constraints = power_sequence->schedule_constraints;
  if (schedule_constraints == NULL) {
    return NULL;
  }

  const AbsoluteOrRelativeTimeType* const latest_end_time = schedule_constraints->latest_end_time;
  if (latest_end_time == NULL) {
    return NULL;
  }

  if (latest_end_time->type != kAbsoluteOrRelativeTimeTypeDuration) {
    return NULL;
  }

  if (schedule_constraints->latest_end_time == NULL
      || schedule_constraints->latest_end_time->type != kAbsoluteOrRelativeTimeTypeDuration) {
    return NULL;
  }

  return &latest_end_time->duration;
}

bool SmartEnergyManagementPsPowerSequenceIsPausable(const SmartEnergyManagementPsPowerSequenceType* power_sequence) {
  if (power_sequence == NULL) {
    return false;
  }

  if ((power_sequence->operating_constraints_interrupt == NULL)
      || (power_sequence->operating_constraints_interrupt->is_pausable == NULL)) {
    return false;
  }

  return *(power_sequence->operating_constraints_interrupt->is_pausable);
}

bool SmartEnergyManagementPsPowerSequenceIsStoppable(const SmartEnergyManagementPsPowerSequenceType* power_sequence) {
  if (power_sequence == NULL) {
    return false;
  }

  if ((power_sequence->operating_constraints_interrupt == NULL)
      || (power_sequence->operating_constraints_interrupt->is_stoppable == NULL)) {
    return false;
  }

  return *(power_sequence->operating_constraints_interrupt->is_stoppable);
}

const EebusDuration* SmartEnergyManagementPsPowerSequenceGetActiveDurationMin(
    const SmartEnergyManagementPsPowerSequenceType* power_sequence
) {
  if (power_sequence == NULL) {
    return NULL;
  }

  const OperatingConstraintsDurationDataType* operating_constraints_duration
      = power_sequence->operating_constraints_duration;

  if (operating_constraints_duration == NULL) {
    return NULL;
  }

  return operating_constraints_duration->active_duration_min;
}

const PowerTimeSlotNumberType* SmartEnergyManagementPsPowerSequenceGetActiveSlotNumber(
    const SmartEnergyManagementPsPowerSequenceType* power_sequence
) {
  if (power_sequence == NULL) {
    return NULL;
  }

  if (power_sequence->state == NULL) {
    return NULL;
  }

  return power_sequence->state->active_slot_number;
}

bool PowerTimeSlotSlotNumberMatch(
    const SmartEnergyManagementPsPowerTimeSlotType* power_time_slot,
    PowerTimeSlotNumberType slot_number
) {
  if (power_time_slot == NULL) {
    return false;
  }

  if ((power_time_slot->schedule == NULL) || (power_time_slot->schedule->slot_number == NULL)) {
    return false;
  }

  return *power_time_slot->schedule->slot_number == slot_number;
}

bool PowerTimeSlotValueTypeMatch(const PowerTimeSlotValueDataType* power_value, PowerTimeSlotValueTypeType value_type) {
  if (power_value == NULL) {
    return false;
  }

  if ((power_value->value_type == NULL)) {
    return false;
  }

  return *power_value->value_type == value_type;
}

const ScaledNumberType* SmartEnergyManagementPsPowerTimeSlotGetPowerMax(
    const SmartEnergyManagementPsPowerTimeSlotType* power_time_slot
) {
  if (power_time_slot == NULL) {
    return NULL;
  }

  if (power_time_slot->value_list == NULL) {
    return NULL;
  }

  const SmartEnergyManagementPsPowerTimeSlotValueListType* const value_list = power_time_slot->value_list;
  if ((value_list->value == NULL) || (value_list->value_size == 0)) {
    return NULL;
  }

  for (size_t i = 0; i < value_list->value_size; i++) {
    const PowerTimeSlotValueDataType* const power_value = value_list->value[i];
    if (PowerTimeSlotValueTypeMatch(power_value, kPowerTimeSlotValueTypeTypePowerMax)) {
      return power_value->value;
    }
  }

  return NULL;
}

const ScaledNumberType* SmartEnergyManagementPsPowerSequenceGetPowerMax(
    const SmartEnergyManagementPsPowerSequenceType* power_sequence,
    PowerTimeSlotNumberType slot_number
) {
  if (power_sequence == NULL) {
    return NULL;
  }

  if ((power_sequence->power_time_slot == NULL) || (power_sequence->power_time_slot_size == 0)) {
    return NULL;
  }

  for (size_t i = 0; i < power_sequence->power_time_slot_size; i++) {
    const SmartEnergyManagementPsPowerTimeSlotType* const power_time_slot = power_sequence->power_time_slot[i];
    if (PowerTimeSlotSlotNumberMatch(power_time_slot, slot_number)) {
      return SmartEnergyManagementPsPowerTimeSlotGetPowerMax(power_time_slot);
    }
  }

  return NULL;
}
