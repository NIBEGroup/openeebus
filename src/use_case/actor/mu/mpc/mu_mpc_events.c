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
 * @brief Monitored Unit MPC events handling implementation
 */

#include "src/spine/events/events.h"
#include "src/use_case/actor/mu/mpc/mu_mpc_internal.h"

static void OnRemoteMaChange(MuMpcUseCase* self, const EventPayload* payload);

void OnRemoteMaChange(MuMpcUseCase* self, const EventPayload* payload) {
  if (!USE_CASE_IS_USE_CASE_COMPATIBLE(USE_CASE_OBJECT(self), payload->use_case_filter)) {
    return;
  }

  if (self->mu_mpc_listener == NULL) {
    return;
  }

  const EntityAddressType* const entity_addr = ENTITY_GET_ADDRESS(ENTITY_OBJECT(payload->entity));

  if (payload->change_type == kElementChangeAdd) {
    MU_MPC_LISTENER_ON_REMOTE_MA_ADDED(self->mu_mpc_listener, entity_addr);
  } else if (payload->change_type == kElementChangeRemove) {
    MU_MPC_LISTENER_ON_REMOTE_MA_REMOVED(self->mu_mpc_listener, entity_addr);
  }
}

void MuMpcHandleEvent(const EventPayload* payload, void* ctx) {
  MuMpcUseCase* const mu_mpc_use_case = (MuMpcUseCase*)ctx;

  if (payload->event_type == kEventTypeUseCaseChange) {
    OnRemoteMaChange(mu_mpc_use_case, payload);
  }
}
