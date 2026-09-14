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
 * @brief Compressor OHPCF use case public API implementation
 */

#include "src/common/array_util.h"
#include "src/spine/model/absolute_or_relative_time.h"
#include "src/use_case/actor/common/ohpcf.h"
#include "src/use_case/actor/compressor/ohpcf/compressor_ohpcf.h"
#include "src/use_case/actor/compressor/ohpcf/compressor_ohpcf_internal.h"
#include "src/use_case/specialization/smart_energy_management_ps/smart_energy_management_ps_server.h"

EebusError CompressorOhpcfAnnounceInternal(
    CompressorOhpcfUseCase* self,
    const OptionalPowerConsumption* optional_power_consumption
) {
  UseCase* const use_case = USE_CASE(self);

  if (optional_power_consumption == NULL) {
    return kEebusErrorInputArgumentNull;
  }

  SmartEnergyManagementPsServer sem_ps_srv = {0};

  EebusError err = SmartEnergyManagementPsServerConstruct(&sem_ps_srv, use_case->local_entity);
  if (err != kEebusErrorOk) {
    return err;
  }

  // clang-format off
  const PowerTimeSlotValueDataType value = {
      .value_type = &(const PowerTimeSlotValueTypeType){kPowerTimeSlotValueTypeTypePowerMax},
      .value      = &(const ScaledNumberType){
          .number = &optional_power_consumption->max_power_w.value,
          .scale  = &optional_power_consumption->max_power_w.scale,
      },
  };
  // clang-format on

  const PowerTimeSlotValueDataType* values[] = {&value};

  const SmartEnergyManagementPsPowerTimeSlotType power_time_slot = {
      .schedule = &(const PowerTimeSlotScheduleDataType){
          .slot_number = &(const PowerTimeSlotNumberType){kOhpcfPowerTimeSlotNumber},
      },

      .value_list = &(const SmartEnergyManagementPsPowerTimeSlotValueListType){
          .value      = values,
          .value_size = ARRAY_SIZE(values),
      },
  };

  const SmartEnergyManagementPsPowerTimeSlotType* power_time_slots[] = {&power_time_slot};

  const SmartEnergyManagementPsPowerSequenceType power_sequence = {
      .description = &(const PowerSequenceDescriptionDataType){
          .sequence_id  = &(const PowerSequenceIdType){kOhpcfPowerSequenceId},
          .power_unit   = &(const UnitOfMeasurementType){kUnitOfMeasurementTypeW},
          .value_source = &(const MeasurementValueSourceType){kMeasurementValueSourceTypeCalculatedValue},
      },

      .state = &(const PowerSequenceStateDataType){
          .state                        = &(const PowerSequenceStateType){kPowerSequenceStateTypeInactive},
          .sequence_remote_controllable = &(const bool){true},
      },

      .schedule_constraints = &(const PowerSequenceScheduleConstraintsDataType){
          .earliest_start_time = &ABSOLUTE_OR_RELATIVE_TIME_WITH_DURATION(optional_power_consumption->earliest_start_time),
          .latest_end_time     = &ABSOLUTE_OR_RELATIVE_TIME_WITH_DURATION(optional_power_consumption->latest_end_time),
      },

      .operating_constraints_interrupt = &(const OperatingConstraintsInterruptDataType){
          .is_pausable  = &optional_power_consumption->is_pausable,
          .is_stoppable = &optional_power_consumption->is_stoppable,
      },

      .operating_constraints_duration = &(const OperatingConstraintsDurationDataType){
          .active_duration_min = &optional_power_consumption->active_duration_min,
      },

      .power_time_slot      = power_time_slots,
      .power_time_slot_size = ARRAY_SIZE(power_time_slots),
  };

  const SmartEnergyManagementPsPowerSequenceType* power_sequences[] = {&power_sequence};

  const SmartEnergyManagementPsAlternativesType alternative = {
      .relation = &(const SmartEnergyManagementPsAlternativesRelationType){
          .alternatives_id = &(const AlternativesIdType){kOhpcfAlternativeId},
      },

      .power_sequence      = power_sequences,
      .power_sequence_size = ARRAY_SIZE(power_sequences),
  };

  const SmartEnergyManagementPsAlternativesType* alternatives[] = {&alternative};

  SmartEnergyManagementPsDataType data = {
      .node_schedule_information = OhpcfGetNodeScheduleAnnounceData(),
      .alternatives              = alternatives,
      .alternatives_size         = ARRAY_SIZE(alternatives),
  };

  err = SmartEnergyManagementPsServerUpdate(&sem_ps_srv, &data);
  if (err == kEebusErrorOk) {
    self->state = kCompressorOhpcfStateAnnounced;
  }

  return err;
}

EebusError CompressorOhpcfAnnounce(
    CompressorOhpcfUseCaseObject* self,
    const OptionalPowerConsumption* optional_power_consumption
) {
  const UseCase* const use_case = USE_CASE(self);

  if (optional_power_consumption == NULL) {
    return kEebusErrorInputArgumentNull;
  }

  EebusError err = kEebusErrorOk;

  DEVICE_LOCAL_LOCK(use_case->local_device);

  err = CompressorOhpcfAnnounceInternal(COMPRESSOR_OHPCF_USE_CASE(self), optional_power_consumption);

  DEVICE_LOCAL_UNLOCK(use_case->local_device);

  return err;
}

EebusError CompressorOhpcfReportStateInternal(
    CompressorOhpcfUseCase* self,
    CompressorOhpcfState state,
    const EebusDuration* start_time
) {
  UseCase* const use_case = USE_CASE(self);

  SmartEnergyManagementPsServer sem_ps_srv = {0};

  EebusError err = SmartEnergyManagementPsServerConstruct(&sem_ps_srv, use_case->local_entity);
  if (err != kEebusErrorOk) {
    return err;
  }

  const AbsoluteOrRelativeTimeType* const start_time_tmp
      = (start_time != NULL) ? &ABSOLUTE_OR_RELATIVE_TIME_WITH_DURATION(*start_time) : NULL;

  const PowerSequenceScheduleDataType* const schedule
      = (start_time != NULL) ? &(const PowerSequenceScheduleDataType){.start_time = start_time_tmp} : NULL;

  const SmartEnergyManagementPsPowerSequenceType power_sequence = {
    .description = &(const PowerSequenceDescriptionDataType){
        .sequence_id  = &(const PowerSequenceIdType){kOhpcfPowerSequenceId},
    },

    .state = &(const PowerSequenceStateDataType){
        .state = &(const PowerSequenceStateType){CompressorOhpcfStateGetPowerSequenceState(state)},
    },

    .schedule = schedule,
  };

  err = SmartEnergyManagementPsServerUpdatePartial(&sem_ps_srv, &kOhpcfAlternativeId, NULL, &power_sequence);
  if (err == kEebusErrorOk) {
    self->state = state;
  }

  return err;
}

EebusError CompressorOhpcfReportState(
    CompressorOhpcfUseCaseObject* self,
    CompressorOhpcfState state,
    const EebusDuration* start_time
) {
  const UseCase* const use_case = USE_CASE(self);

  EebusError err = kEebusErrorOk;

  DEVICE_LOCAL_LOCK(use_case->local_device);

  err = CompressorOhpcfReportStateInternal(COMPRESSOR_OHPCF_USE_CASE(self), state, start_time);

  DEVICE_LOCAL_UNLOCK(use_case->local_device);

  return err;
}

EebusError CompressorOhpcfClearProcessInternal(CompressorOhpcfUseCase* self) {
  UseCase* const use_case = USE_CASE(self);

  SmartEnergyManagementPsServer sem_ps_srv = {0};

  EebusError err = SmartEnergyManagementPsServerConstruct(&sem_ps_srv, use_case->local_entity);
  if (err != kEebusErrorOk) {
    return err;
  }

  SmartEnergyManagementPsDataType sem_ps_data = {
      .node_schedule_information = OhpcfGetNodeScheduleClearProcessData(),
      .alternatives              = NULL,
      .alternatives_size         = 0,
  };

  err = SmartEnergyManagementPsServerUpdate(&sem_ps_srv, &sem_ps_data);
  if (err == kEebusErrorOk) {
    self->state = kCompressorOhpcfStateUndefined;
  }

  return err;
}

EebusError CompressorOhpcfClearProcess(CompressorOhpcfUseCaseObject* self) {
  const UseCase* const use_case = USE_CASE(self);

  EebusError err = kEebusErrorOk;

  DEVICE_LOCAL_LOCK(use_case->local_device);

  err = CompressorOhpcfClearProcessInternal(COMPRESSOR_OHPCF_USE_CASE(self));

  DEVICE_LOCAL_UNLOCK(use_case->local_device);

  return err;
}

CompressorOhpcfState CompressorOhpcfGetState(CompressorOhpcfUseCaseObject* self) {
  return COMPRESSOR_OHPCF_USE_CASE(self)->state;
}
