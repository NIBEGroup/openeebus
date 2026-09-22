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
 * @brief Customer Energy Manager OHPCF events handling implementation
 */

#include "src/common/eebus_arguments.h"
#include "src/spine/events/events.h"
#include "src/use_case/actor/cem/ohpcf/cem_ohpcf_internal.h"
#include "src/use_case/actor/common/ohpcf.h"
#include "src/use_case/specialization/smart_energy_management_ps/smart_energy_management_ps_client.h"

static void
OnRemoteCompressorAddedHandleSmartEnergyManagementPs(const CemOhpcfUseCase* self, EntityRemoteObject* entity);
static void OnRemoteCompressorChange(CemOhpcfUseCase* self, const EventPayload* payload);
static void OnSmartEnergyManagementPsDataUpdate(CemOhpcfUseCase* self, const EventPayload* payload);
static void OnDataChange(CemOhpcfUseCase* self, const EventPayload* payload);

void OnRemoteCompressorAddedHandleSmartEnergyManagementPs(const CemOhpcfUseCase* self, EntityRemoteObject* entity) {
  const UseCase* const use_case = USE_CASE(self);

  SmartEnergyManagementPsClient sem_ps_client;
  const EebusError err = SmartEnergyManagementPsClientConstruct(&sem_ps_client, use_case->local_entity, entity);
  if (err != kEebusErrorOk) {
    return;
  }

  FeatureInfoClient* feature_info = &sem_ps_client.feature_info_client;
  if (!HasSubscription(feature_info)) {
    Subscribe(feature_info);
  }

  if (!HasBinding(feature_info)) {
    Bind(feature_info);
  }

  // Get the initial data
  SmartEnergyManagementPsClientRequestData(&sem_ps_client, NULL, NULL);
}

void OnRemoteCompressorChange(CemOhpcfUseCase* self, const EventPayload* payload) {
  if (!USE_CASE_IS_USE_CASE_COMPATIBLE(USE_CASE_OBJECT(self), payload->use_case_filter)) {
    return;
  }

  const EntityAddressType* const entity_addr = ENTITY_GET_ADDRESS(ENTITY_OBJECT(payload->entity));
  if (payload->change_type == kElementChangeAdd) {
    OnRemoteCompressorAddedHandleSmartEnergyManagementPs(self, payload->entity);
    if (self->cem_ohpcf_listener != NULL) {
      CEM_OHPCF_LISTENER_ON_REMOTE_COMPRESSOR_ADDED(self->cem_ohpcf_listener, entity_addr);
    }
  } else if (payload->change_type == kElementChangeRemove) {
    if (self->cem_ohpcf_listener != NULL) {
      CEM_OHPCF_LISTENER_ON_REMOTE_COMPRESSOR_REMOVED(self->cem_ohpcf_listener, entity_addr);
    }
  }
}

void OnAnnounce(
    CemOhpcfUseCase* self,
    const SmartEnergyManagementPsDataType* sem_ps_data,
    const EntityAddressType* entity_addr
) {
  UNUSED(sem_ps_data);

  OptionalPowerConsumption optional_power_consumption = {0};

  const EebusError err
      = CemOhpcfGetAnnouncedOptionalPowerConsumptionInternal(self, entity_addr, &optional_power_consumption);

  if (err == kEebusErrorOk) {
    self->state = kCompressorOhpcfStateAnnounced;
    if (self->cem_ohpcf_listener != NULL) {
      CEM_OHPCF_LISTENER_ON_ANNOUNCE(self->cem_ohpcf_listener, &optional_power_consumption, entity_addr);
      CEM_OHPCF_LISTENER_ON_STATE_REPORT(self->cem_ohpcf_listener, self->state, NULL, entity_addr);
    }
  }
}

void OnClearProcess(CemOhpcfUseCase* self, const EntityAddressType* entity_addr) {
  self->state = kCompressorOhpcfStateUndefined;
  CEM_OHPCF_LISTENER_ON_CLEAR_PROCESS(self->cem_ohpcf_listener, entity_addr);
}

void OnCompressorStateReport(
    CemOhpcfUseCase* self,
    const SmartEnergyManagementPsDataType* sem_ps_data,
    const EntityAddressType* entity_addr
) {
  const SmartEnergyManagementPsPowerSequenceType* const power_sequence
      = OhpcfSmartEnergyManagementPsGetPowerSequence(sem_ps_data);

  if (power_sequence == NULL) {
    return;
  }

  const PowerSequenceStateType* const state = SmartEnergyManagementPsPowerSequenceGetState(power_sequence);
  if (state == NULL) {
    return;
  }

  const AbsoluteOrRelativeTimeType* start_time = NULL;

  if (*state == kPowerSequenceStateTypeScheduled) {
    start_time = SmartEnergyManagementPsPowerSequenceGetStartTime(power_sequence);
    if ((start_time == NULL) || (start_time->type != kAbsoluteOrRelativeTimeTypeDuration)) {
      return;
    }
  }

  self->state = CompressorOhpcfStateGetStateWithPowerSequenceState(*state);

  const EebusDuration* start_time_duration = (start_time != NULL) ? &start_time->duration : NULL;
  CEM_OHPCF_LISTENER_ON_STATE_REPORT(self->cem_ohpcf_listener, self->state, start_time_duration, entity_addr);
}

void OnSmartEnergyManagementPsDataUpdate(CemOhpcfUseCase* self, const EventPayload* payload) {
  const SmartEnergyManagementPsDataType* const sem_ps_data
      = (const SmartEnergyManagementPsDataType*)payload->function_data;

  if (sem_ps_data == NULL) {
    return;
  }

  const EntityAddressType* const entity_addr = ENTITY_GET_ADDRESS(ENTITY_OBJECT(payload->entity));

  if (OhpcfNodeScheduleClearProcessMatch(sem_ps_data->node_schedule_information)) {
    OnClearProcess(self, entity_addr);
  } else if (OhpcfNodeScheduleAnnounceMatch(sem_ps_data->node_schedule_information)) {
    OnAnnounce(self, sem_ps_data, entity_addr);
  } else {
    OnCompressorStateReport(self, sem_ps_data, entity_addr);
  }
}

void OnDataChange(CemOhpcfUseCase* self, const EventPayload* payload) {
  switch (payload->function_type) {
    case kFunctionTypeSmartEnergyManagementPsData: OnSmartEnergyManagementPsDataUpdate(self, payload); break;

    default: break;
  }
}

void CemOhpcfHandleEvent(const EventPayload* payload, void* ctx) {
  CemOhpcfUseCase* cem_ohpcf_use_case = (CemOhpcfUseCase*)ctx;

  if (!USE_CASE_IS_ENTITY_COMPATIBLE(USE_CASE_OBJECT(cem_ohpcf_use_case), payload->entity)) {
    return;
  }

  if (payload->event_type == kEventTypeUseCaseChange) {
    OnRemoteCompressorChange(cem_ohpcf_use_case, payload);
  }

  if ((payload->event_type == kEventTypeDataChange) || (payload->change_type == kElementChangeUpdate)) {
    OnDataChange(cem_ohpcf_use_case, payload);
  }
}
