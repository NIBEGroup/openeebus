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
 * @brief Use Case basic functionality implementation
 */

#include "src/use_case/use_case.h"

#include "src/common/eebus_malloc.h"
#include "src/spine/api/device_local_interface.h"
#include "src/spine/api/entity_local_interface.h"
#include "src/spine/events/events.h"
#include "src/spine/model/usecase_information_types.h"
#include "src/use_case/api/use_case_interface.h"

static void UseCaseEntityAddUseCaseInfo(UseCase* self);
static bool IsEntityTypeCompatible(const UseCase* self, EntityTypeType entity_type);

void UseCaseEntityAddUseCaseInfo(UseCase* self) {
  const UseCaseInfo* const info = self->info;

  // Allocate array for scenarios
  UseCaseScenarioSupportType* const scenarios
      = (UseCaseScenarioSupportType*)EEBUS_MALLOC(info->use_case_scenarios_size * sizeof(UseCaseScenarioSupportType));

  if (scenarios == NULL) {
    return;  // Handle allocation failure gracefully
  }

  for (size_t i = 0; i < info->use_case_scenarios_size; ++i) {
    scenarios[i] = info->use_case_scenarios[i].scenario;
  }

  ENTITY_LOCAL_ADD_USE_CASE_SUPPORT(self->local_entity, info->actor, info->use_case_name_id, info->version,
      info->sub_revision, info->available, scenarios, info->use_case_scenarios_size);

  EEBUS_FREE(scenarios);
}

void UseCaseConstruct(
    UseCase* self, const UseCaseInfo* info, EntityLocalObject* local_entity, EventHandler event_handler) {
  self->info         = info;
  self->local_device = ENTITY_LOCAL_GET_DEVICE(local_entity);
  self->local_entity = local_entity;
  UseCaseEntityAddUseCaseInfo(self);
  self->event_handler = event_handler;
  if (self->event_handler != NULL) {
    EVENTS_SUBSCRIBE(
        DEVICE_LOCAL_GET_EVENTS_MANAGER(self->local_device),
        kEventHandlerLevelApplication,
        self->event_handler,
        self
    );
  }
}

void UseCaseDestruct(UseCaseObject* self) {
  UseCase* use_case = USE_CASE(self);

  if (use_case->event_handler != NULL) {
    EVENTS_UNSUBSCRIBE(
        DEVICE_LOCAL_GET_EVENTS_MANAGER(use_case->local_device),
        kEventHandlerLevelApplication,
        use_case->event_handler,
        self
    );
  }
}

bool IsVaildEntityType(const UseCase* self, EntityTypeType entity_type) {
  const UseCaseInfo* const info = self->info;
  for (size_t i = 0; i < info->valid_entity_types_size; ++i) {
    if (info->valid_entity_types[i] == entity_type) {
      return true;
    }
  }

  return false;
}

bool IsEntityTypeCompatible(const UseCase* self, EntityTypeType entity_type) {
  const UseCaseInfo* const info = self->info;

  for (size_t i = 0; i < info->valid_entity_types_size; ++i) {
    if (info->valid_entity_types[i] == entity_type) {
      return true;
    }
  }

  return false;
}

bool UseCaseIsEntityCompatible(const UseCaseObject* self, const EntityRemoteObject* remote_entity) {
  const UseCase* const use_case = USE_CASE(self);

  if (remote_entity == NULL) {
    return false;
  }

  const EntityTypeType entity_type = ENTITY_GET_TYPE(ENTITY_OBJECT(remote_entity));
  if (!IsEntityTypeCompatible(use_case, entity_type)) {
    return false;
  }

  for (size_t i = 0; i < use_case->info->valid_actor_types_size; ++i) {
    UseCaseFilterType use_case_filter = {
        .actor            = use_case->info->valid_actor_types[i],
        .use_case_name_id = use_case->info->use_case_name_id,
    };

    if (ENTITY_REMOTE_HAS_USE_CASE_SUPPORT(remote_entity, &use_case_filter)) {
      return true;
    }
  }

  return false;
}

bool UseCaseIsUseCaseCompatible(const UseCaseObject* self, const UseCaseFilterType* use_case_filter) {
  const UseCase* const use_case = USE_CASE(self);

  if (use_case_filter == NULL) {
    return false;
  }

  for (size_t i = 0; i < use_case->info->valid_actor_types_size; ++i) {
    if ((use_case_filter->actor == use_case->info->valid_actor_types[i])
        && (use_case_filter->use_case_name_id == use_case->info->use_case_name_id)) {
      return true;
    }
  }

  return false;
}

EntityRemoteObject*
UseCaseGetRemoteEntityWithAddress(const UseCaseObject* self, const EntityAddressType* remote_entity_addr) {
  const UseCase* const use_case = USE_CASE(self);

  if (remote_entity_addr == NULL) {
    return NULL;
  }

  const DeviceRemoteObject* const remote_device
      = DEVICE_LOCAL_GET_REMOTE_DEVICE_WITH_ADDRESS(use_case->local_device, remote_entity_addr->device);

  if (remote_device == NULL) {
    return NULL;
  }

  EntityRemoteObject* const remote_entity
      = DEVICE_REMOTE_GET_ENTITY(remote_device, remote_entity_addr->entity, remote_entity_addr->entity_size);

  if (remote_entity == NULL) {
    return NULL;
  }

  if (!USE_CASE_IS_ENTITY_COMPATIBLE(USE_CASE_OBJECT(self), remote_entity)) {
    return NULL;
  }

  return remote_entity;
}
