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
 * @brief Compressor OHPCF events handling implementation
 */

#include "src/common/eebus_arguments.h"
#include "src/spine/events/events.h"
#include "src/use_case/actor/compressor/ohpcf/compressor_ohpcf_internal.h"
#include "src/use_case/specialization/smart_energy_management_ps/smart_energy_management_ps_server.h"

static void
OnSchedule(CompressorOhpcfUseCase* self, const EventPayload* payload, const AbsoluteOrRelativeTimeType* start_time);
static void OnCompressorStop(CompressorOhpcfUseCase* self, const EventPayload* payload);
static void OnCompressorPause(CompressorOhpcfUseCase* self, const EventPayload* payload);
static void OnCompressorResume(CompressorOhpcfUseCase* self, const EventPayload* payload);
static void OnRemoteCemChange(CompressorOhpcfUseCase* self, const EventPayload* payload);
static void OnDataChange(CompressorOhpcfUseCase* self, const EventPayload* payload);
static void
OnControlConsumptionProcess(CompressorOhpcfUseCase* self, const EventPayload* payload, PowerSequenceStateType state);
static void OnSmartEnergyManagementPsDataUpdate(CompressorOhpcfUseCase* self, const EventPayload* payload);

void OnSchedule(
    CompressorOhpcfUseCase* self,
    const EventPayload* payload,
    const AbsoluteOrRelativeTimeType* start_time
) {
  UNUSED(payload);

  if (self->state != kCompressorOhpcfStateAnnounced) {
    return;
  }

  // TODO: Add handling of the absolut time
  if (start_time->type != kAbsoluteOrRelativeTimeTypeDuration) {
    return;
  }

  COMPRESSOR_OHPCF_LISTENER_ON_SCHEDULE_OPTIONAL_POWER_CONSUMPTION(self->cp_ohpcf_listener, &start_time->duration);
}

void OnCompressorStop(CompressorOhpcfUseCase* self, const EventPayload* payload) {
  UNUSED(payload);

  self->state = kCompressorOhpcfStateStopped;
  COMPRESSOR_OHPCF_LISTENER_ON_STOP(self->cp_ohpcf_listener);
}

void OnCompressorPause(CompressorOhpcfUseCase* self, const EventPayload* payload) {
  UNUSED(payload);

  if (self->state != kCompressorOhpcfStateRunning) {
    return;
  }

  self->state = kCompressorOhpcfStatePaused;
  COMPRESSOR_OHPCF_LISTENER_ON_PAUSE(self->cp_ohpcf_listener);
}

void OnCompressorResume(CompressorOhpcfUseCase* self, const EventPayload* payload) {
  UNUSED(payload);

  if (self->state != kCompressorOhpcfStatePaused) {
    return;
  }

  self->state = kCompressorOhpcfStateRunning;
  COMPRESSOR_OHPCF_LISTENER_ON_RESUME(self->cp_ohpcf_listener);
}

void OnControlConsumptionProcess(
    CompressorOhpcfUseCase* self,
    const EventPayload* payload,
    PowerSequenceStateType state
) {
  switch (state) {
    case kPowerSequenceStateTypeInvalid:  // "invalid", stop/abort [OHPCF-012/2/4]
      OnCompressorStop(self, payload);
      return;

    case kPowerSequenceStateTypePaused:  // "paused", pause [OHPCF-012/2/3]
      OnCompressorPause(self, payload);
      return;

    case kPowerSequenceStateTypeRunning:  // "running", resume [OHPCF-012/2/2]
      OnCompressorResume(self, payload);
      return;

    default:
      // Other options shall not be handled
      return;
  }
}

void OnSmartEnergyManagementPsDataUpdate(CompressorOhpcfUseCase* self, const EventPayload* payload) {
  const UseCase* const use_case = USE_CASE(self);

  if (self->cp_ohpcf_listener == NULL) {
    return;
  }

  FeatureLocalObject* const fl = ENTITY_LOCAL_GET_FEATURE_WITH_TYPE_AND_ROLE(
      use_case->local_entity,
      kFeatureTypeTypeSmartEnergyManagementPs,
      kRoleTypeServer
  );

  if (payload->local_feature != fl) {
    return;
  }

  // TODO: Make the checks below a part of the Approve/Deny mechanism for OHPCF Compressor use case
  const SmartEnergyManagementPsDataType* const sem_ps_data
      = (const SmartEnergyManagementPsDataType*)payload->function_data;

  // 1. Get power sequence with the identifier kOhpcfPowerSequenceId
  const SmartEnergyManagementPsPowerSequenceType* const power_sequence
      = SmartEnergyManagementPsGetPowerSequenceWithId(sem_ps_data, kOhpcfPowerSequenceId);
  if (power_sequence == NULL) {
    return;
  }

  // 2. Get the state received
  const PowerSequenceStateType* const state = SmartEnergyManagementPsPowerSequenceGetState(power_sequence);
  if (state != NULL) {
    OnControlConsumptionProcess(self, payload, *state);
  } else {
    // 3. Get the start time received
    const AbsoluteOrRelativeTimeType* const start_time
        = SmartEnergyManagementPsPowerSequenceGetStartTime(power_sequence);
    if (start_time != NULL) {
      OnSchedule(self, payload, start_time);
    }
  }
}

void OnDataChange(CompressorOhpcfUseCase* self, const EventPayload* payload) {
  if ((payload->cmd_classifier == NULL)
      || ((*payload->cmd_classifier != kCommandClassifierTypeWrite)
          && (*payload->cmd_classifier != kCommandClassifierTypeNotify))) {
    return;
  }

  switch (payload->function_type) {
    case kFunctionTypeSmartEnergyManagementPsData: OnSmartEnergyManagementPsDataUpdate(self, payload); break;
    default: break;
  }
}

void OnRemoteCemChange(CompressorOhpcfUseCase* self, const EventPayload* payload) {
  if (self->cp_ohpcf_listener == NULL) {
    return;
  }

  if (!USE_CASE_IS_USE_CASE_COMPATIBLE(USE_CASE_OBJECT(self), payload->use_case_filter)) {
    return;
  }

  if (payload->change_type == kElementChangeAdd) {
    COMPRESSOR_OHPCF_LISTENER_ON_REMOTE_CEM_ADDED(self->cp_ohpcf_listener);
  } else if (payload->change_type == kElementChangeRemove) {
    COMPRESSOR_OHPCF_LISTENER_ON_REMOTE_CEM_REMOVED(self->cp_ohpcf_listener);
  }
}

void CompressorOhpcfHandleEvent(const EventPayload* payload, void* ctx) {
  CompressorOhpcfUseCase* cp_ohpcf_use_case = (CompressorOhpcfUseCase*)ctx;

  if (!USE_CASE_IS_ENTITY_COMPATIBLE(USE_CASE_OBJECT(cp_ohpcf_use_case), payload->entity)) {
    return;
  }

  if (payload->event_type == kEventTypeUseCaseChange) {
    OnRemoteCemChange(cp_ohpcf_use_case, payload);
  }

  if ((payload->event_type == kEventTypeDataChange) || (payload->change_type == kElementChangeUpdate)) {
    OnDataChange(cp_ohpcf_use_case, payload);
  }
}
