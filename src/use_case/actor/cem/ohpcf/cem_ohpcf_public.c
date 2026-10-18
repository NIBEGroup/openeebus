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
 * @brief Customer Energy Manager OHPCF use case public API
 */

#include "src/common/eebus_arguments.h"
#include "src/spine/model/absolute_or_relative_time.h"
#include "src/spine/model/entity_types.h"
#include "src/use_case/actor/cem/ohpcf/cem_ohpcf.h"
#include "src/use_case/actor/cem/ohpcf/cem_ohpcf_internal.h"
#include "src/use_case/actor/common/ohpcf.h"
#include "src/use_case/model/load_limit_types.h"
#include "src/use_case/specialization/smart_energy_management_ps/smart_energy_management_ps_client.h"
#include "src/use_case/use_case.h"

//-------------------------------------------------------------------------------------------//
//
// Scenario 1
//
//-------------------------------------------------------------------------------------------//
EebusError CemOhpcfGetAnnouncedOptionalPowerConsumptionInternal(
    const CemOhpcfUseCase* self,
    const EntityAddressType* remote_entity_addr,
    OptionalPowerConsumption* optional_power_consumption
) {
  const UseCase* const use_case = USE_CASE(self);

  EntityRemoteObject* const remote_entity
      = USE_CASE_GET_REMOTE_ENTITY_WITH_ADDRESS(USE_CASE_OBJECT(self), remote_entity_addr);

  if (remote_entity == NULL) {
    return kEebusErrorNoChange;
  }

  SmartEnergyManagementPsClient sem_ps_client;
  EebusError err = SmartEnergyManagementPsClientConstruct(&sem_ps_client, use_case->local_entity, remote_entity);
  if (err != kEebusErrorOk) {
    return err;
  }

  const SmartEnergyManagementPsDataType* const sem_ps_data
      = SmartEnergyManagementPsCommonGetData(&sem_ps_client.sem_ps_common);

  if (sem_ps_data == NULL) {
    return kEebusErrorNotAvailable;
  }

  // 1. Check if node schedule information matches announce criteria
  if (!OhpcfNodeScheduleAnnounceMatch(sem_ps_data->node_schedule_information)) {
    return kEebusErrorNoChange;
  }

  const SmartEnergyManagementPsPowerSequenceType* power_sequence
      = OhpcfSmartEnergyManagementPsGetPowerSequence(sem_ps_data);

  if (power_sequence == NULL) {
    return kEebusErrorNotAvailable;
  }

  const UnitOfMeasurementType measurement_unit = SmartEnergyManagementPsPowerSequenceGetUnit(power_sequence);
  if (measurement_unit != kUnitOfMeasurementTypeW) {
    return kEebusErrorNotAvailable;
  }

  // 2. Get max power value
  const ScaledNumberType* const max_power = OhpcfSmartEnergyManagementPsPowerSequenceGetPowerMax(power_sequence);

  if ((max_power == NULL) || (max_power->number == NULL)) {
    return kEebusErrorNotAvailable;
  }

  optional_power_consumption->max_power_w.value = *max_power->number;
  optional_power_consumption->max_power_w.scale = (max_power->scale != NULL) ? *max_power->scale : 0;

  // 3. Get earliest start time & latest end time
  const EebusDuration* const earliest_start_time
      = SmartEnergyManagementPsPowerSequenceGetEarliestStartTime(power_sequence);
  const EebusDuration* const latest_end_time = SmartEnergyManagementPsPowerSequenceGetLatestEndTime(power_sequence);

  if ((earliest_start_time == NULL) || (latest_end_time == NULL)) {
    return kEebusErrorNotAvailable;
  }

  optional_power_consumption->earliest_start_time = *earliest_start_time;
  optional_power_consumption->latest_end_time     = *latest_end_time;

  // 4. Get active duration min
  const EebusDuration* const active_duration_min
      = SmartEnergyManagementPsPowerSequenceGetActiveDurationMin(power_sequence);

  if (active_duration_min == NULL) {
    return kEebusErrorNotAvailable;
  }

  optional_power_consumption->active_duration_min = *active_duration_min;

  // 5. Get pausable & stoppable
  optional_power_consumption->is_pausable  = SmartEnergyManagementPsPowerSequenceIsPausable(power_sequence);
  optional_power_consumption->is_stoppable = SmartEnergyManagementPsPowerSequenceIsStoppable(power_sequence);

  return kEebusErrorOk;
}

EebusError CemOhpcfGetAnnouncedOptionalPowerConsumption(
    const CemOhpcfUseCaseObject* self,
    const EntityAddressType* remote_entity_addr,
    OptionalPowerConsumption* optional_power_consumption
) {
  UseCase* const use_case = USE_CASE(self);

  if ((remote_entity_addr == NULL) || (optional_power_consumption == NULL)) {
    return kEebusErrorInputArgumentNull;
  }

  EebusError ret = kEebusErrorOk;

  DEVICE_LOCAL_LOCK(use_case->local_device);

  ret = CemOhpcfGetAnnouncedOptionalPowerConsumptionInternal(
      CEM_OHPCF_USE_CASE(self),
      remote_entity_addr,
      optional_power_consumption
  );

  DEVICE_LOCAL_UNLOCK(use_case->local_device);
  return ret;
}

CompressorOhpcfState
CemOhpcfGetCompressorState(const CemOhpcfUseCaseObject* self, const EntityAddressType* remote_entity_addr) {
  UNUSED(remote_entity_addr);

  return CEM_OHPCF_USE_CASE(self)->state;
}

static EebusError CemOhpcfReadSmartDataInternal(
    const CemOhpcfUseCase* self,
    const EntityAddressType* remote_entity_addr,
    ReplyMessageCallback cb,
    void* ctx
) {
  const UseCase* const use_case = USE_CASE(self);

  EntityRemoteObject* const remote_entity
      = USE_CASE_GET_REMOTE_ENTITY_WITH_ADDRESS(USE_CASE_OBJECT(self), remote_entity_addr);

  if (remote_entity == NULL) {
    return kEebusErrorNoChange;
  }

  SmartEnergyManagementPsClient sem_ps_client;
  const EebusError err = SmartEnergyManagementPsClientConstruct(&sem_ps_client, use_case->local_entity, remote_entity);
  if (err != kEebusErrorOk) {
    return err;
  }

  return SmartEnergyManagementPsClientRequestData(&sem_ps_client, cb, ctx);
}

EebusError CemOhpcfReadSmartData(
    const CemOhpcfUseCaseObject* self,
    const EntityAddressType* remote_entity_addr,
    ReplyMessageCallback cb,
    void* ctx
) {
  UseCase* const use_case = USE_CASE(self);

  if (remote_entity_addr == NULL) {
    return kEebusErrorInputArgumentNull;
  }

  EebusError err = kEebusErrorOk;

  DEVICE_LOCAL_LOCK(use_case->local_device);
  err = CemOhpcfReadSmartDataInternal(CEM_OHPCF_USE_CASE(self), remote_entity_addr, cb, ctx);
  DEVICE_LOCAL_UNLOCK(use_case->local_device);

  return err;
}

//-------------------------------------------------------------------------------------------//
//
// Scenario 2
//
//-------------------------------------------------------------------------------------------//
EebusError CemOhpcfScheduleOptionalPowerConsumptionInternal(
    CemOhpcfUseCase* self,
    const EntityAddressType* remote_entity_addr,
    const EebusDuration* start_time,
    ResultMessageCallback cb,
    void* ctx
) {
  const UseCase* const use_case = USE_CASE(self);

  EntityRemoteObject* const remote_entity
      = USE_CASE_GET_REMOTE_ENTITY_WITH_ADDRESS(USE_CASE_OBJECT(self), remote_entity_addr);

  if (remote_entity == NULL) {
    return kEebusErrorNoChange;
  }

  SmartEnergyManagementPsClient sem_ps_client;
  EebusError err = SmartEnergyManagementPsClientConstruct(&sem_ps_client, use_case->local_entity, remote_entity);
  if (err != kEebusErrorOk) {
    return err;
  }

  const SmartEnergyManagementPsDataType* const sem_ps_data
      = SmartEnergyManagementPsCommonGetData(&sem_ps_client.sem_ps_common);

  const PowerSequenceIdType* const sequence_id = OhpcfSmartEnergyManagementPsGetPowerSequenceId(sem_ps_data);
  if (sequence_id == NULL) {
    return kEebusErrorNoChange;
  }

  const SmartEnergyManagementPsPowerSequenceType power_sequence = {
    .description = &(const PowerSequenceDescriptionDataType){
        .sequence_id  = sequence_id,
    },

    .schedule = &(const PowerSequenceScheduleDataType){
        .start_time = &ABSOLUTE_OR_RELATIVE_TIME_WITH_DURATION(*start_time)
    },
  };

  return SmartEnergyManagementPsClientWritePartial(&sem_ps_client, NULL, &power_sequence, cb, ctx);
}

EebusError CemOhpcfScheduleOptionalPowerConsumption(
    CemOhpcfUseCaseObject* self,
    const EntityAddressType* remote_entity_addr,
    const EebusDuration* start_time,
    ResultMessageCallback cb,
    void* ctx
) {
  UseCase* const use_case = USE_CASE(self);

  if ((remote_entity_addr == NULL) || (start_time == NULL)) {
    return kEebusErrorInputArgumentNull;
  }

  EebusError err = kEebusErrorOk;

  DEVICE_LOCAL_LOCK(use_case->local_device);

  err = CemOhpcfScheduleOptionalPowerConsumptionInternal(
      CEM_OHPCF_USE_CASE(self),
      remote_entity_addr,
      start_time,
      cb,
      ctx
  );

  DEVICE_LOCAL_UNLOCK(use_case->local_device);

  return err;
}

EebusError CemOhpcfWriteState(
    CemOhpcfUseCase* self,
    const EntityAddressType* remote_entity_addr,
    PowerSequenceStateType state,
    ResultMessageCallback cb,
    void* ctx
) {
  const UseCase* const use_case = USE_CASE(self);

  EntityRemoteObject* const remote_entity
      = USE_CASE_GET_REMOTE_ENTITY_WITH_ADDRESS(USE_CASE_OBJECT(self), remote_entity_addr);

  if (remote_entity == NULL) {
    return kEebusErrorNoChange;
  }

  SmartEnergyManagementPsClient sem_ps_client;
  EebusError err = SmartEnergyManagementPsClientConstruct(&sem_ps_client, use_case->local_entity, remote_entity);
  if (err != kEebusErrorOk) {
    return err;
  }

  const SmartEnergyManagementPsDataType* const sem_ps_data
      = SmartEnergyManagementPsCommonGetData(&sem_ps_client.sem_ps_common);

  const AlternativesIdType* const alternative_id = OhpcfSmartEnergyManagementPsGetAlternativeId(sem_ps_data);
  if (alternative_id == NULL) {
    return kEebusErrorNoChange;
  }

  const PowerSequenceIdType* const sequence_id = OhpcfSmartEnergyManagementPsGetPowerSequenceId(sem_ps_data);
  if (sequence_id == NULL) {
    return kEebusErrorNoChange;
  }

  const SmartEnergyManagementPsPowerSequenceType power_sequence = {
    .description = &(const PowerSequenceDescriptionDataType){
        .sequence_id  = sequence_id,
    },

    .state = &(const PowerSequenceStateDataType){
        .state = &state,
    },
  };

  return SmartEnergyManagementPsClientWritePartial(&sem_ps_client, alternative_id, &power_sequence, cb, ctx);
}

EebusError CemOhpcfWriteStopCommand(
    CemOhpcfUseCaseObject* self,
    const EntityAddressType* remote_entity_addr,
    ResultMessageCallback cb,
    void* ctx
) {
  UseCase* const use_case = USE_CASE(self);

  if (remote_entity_addr == NULL) {
    return kEebusErrorInputArgumentNull;
  }

  EebusError err = kEebusErrorOk;

  DEVICE_LOCAL_LOCK(use_case->local_device);

  if (CEM_OHPCF_USE_CASE(self)->state == kCompressorOhpcfStateUndefined) {
    err = kEebusErrorNoChange;
  } else {
    err = CemOhpcfWriteState(CEM_OHPCF_USE_CASE(self), remote_entity_addr, kPowerSequenceStateTypeInvalid, cb, ctx);
  }

  DEVICE_LOCAL_UNLOCK(use_case->local_device);

  return err;
}

EebusError CemOhpcfWritePauseCommand(
    CemOhpcfUseCaseObject* self,
    const EntityAddressType* remote_entity_addr,
    ResultMessageCallback cb,
    void* ctx
) {
  UseCase* const use_case = USE_CASE(self);

  if (remote_entity_addr == NULL) {
    return kEebusErrorInputArgumentNull;
  }

  EebusError err = kEebusErrorOk;

  DEVICE_LOCAL_LOCK(use_case->local_device);

  if (CEM_OHPCF_USE_CASE(self)->state != kCompressorOhpcfStateRunning) {
    err = kEebusErrorNoChange;
  } else {
    err = CemOhpcfWriteState(CEM_OHPCF_USE_CASE(self), remote_entity_addr, kPowerSequenceStateTypePaused, cb, ctx);
  }

  DEVICE_LOCAL_UNLOCK(use_case->local_device);

  return err;
}

EebusError CemOhpcfWriteResumeCommand(
    CemOhpcfUseCaseObject* self,
    const EntityAddressType* remote_entity_addr,
    ResultMessageCallback cb,
    void* ctx
) {
  UseCase* const use_case = USE_CASE(self);

  if (remote_entity_addr == NULL) {
    return kEebusErrorInputArgumentNull;
  }

  EebusError err = kEebusErrorOk;

  DEVICE_LOCAL_LOCK(use_case->local_device);

  if (CEM_OHPCF_USE_CASE(self)->state != kCompressorOhpcfStatePaused) {
    err = kEebusErrorNoChange;
  } else {
    err = CemOhpcfWriteState(CEM_OHPCF_USE_CASE(self), remote_entity_addr, kPowerSequenceStateTypeRunning, cb, ctx);
  }

  DEVICE_LOCAL_UNLOCK(use_case->local_device);

  return err;
}
