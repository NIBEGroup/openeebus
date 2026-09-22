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
 * @brief Node Management Remote implementation
 */

#include "src/spine/node_management/node_management_remote.h"
#include "src/common/vector.h"
#include "src/spine/api/device_local_interface.h"
#include "src/spine/api/device_remote_interface.h"
#include "src/spine/events/events.h"
#include "src/spine/feature/feature_remote_internal.h"
#include "src/spine/model/model.h"
#include "src/spine/model/node_management_types.h"

typedef struct NodeManagementRemote NodeManagementRemote;

struct NodeManagementRemote {
  FeatureRemote feature_remote;

  const UseCaseInformationListDataType* use_case_data_prev;
};

#define NODE_MANAGEMENT_REMOTE(obj) ((NodeManagementRemote*)(obj))

static void Destruct(FeatureObject* self);
static EebusError UpdateData(
    FeatureRemoteObject* self,
    FunctionType function_type,
    const void* new_data,
    const FilterType* filter_partial,
    const FilterType* filter_delete,
    bool persist
);

static const FeatureRemoteInterface feature_remote_methods = {
    .feature_interface = {
        .destruct                = Destruct,
        .get_address             = FeatureGetAddress,
        .get_type                = FeatureGetType,
        .get_role                = FeatureGetRole,
        .get_function_operations = FeatureGetFunctionOperations,
        .get_description         = FeatureGetDescription,
        .set_description         = FeatureSetDescription,
        .to_string               = FeatureToString,
    },

    .get_device              = FeatureRemoteGetDevice,
    .get_entity              = FeatureRemoteGetEntity,
    .get_data                = FeatureRemoteGetData,
    .data_copy               = FeatureRemoteDataCopy,
    .update_data             = UpdateData,
    .set_function_operations = FeatureRemoteSetFunctionOperations,
    .set_max_response_delay  = FeatureRemoteSetMaxResponseDelay,
    .get_max_response_delay  = FeatureRemoteGetMaxResponseDelay,
};

static EebusError NodeManagementRemoteConstruct(NodeManagementRemote* self, uint32_t id, EntityRemoteObject* entity);

static EebusError NodeManagementRemoteConstruct(NodeManagementRemote* self, uint32_t id, EntityRemoteObject* entity) {
  const EebusError err
      = FeatureRemoteConstruct(FEATURE_REMOTE(self), id, entity, kFeatureTypeTypeNodeManagement, kRoleTypeSpecial);

  // Override "virtual functions table"
  FEATURE_REMOTE_INTERFACE(self) = &feature_remote_methods;

  self->use_case_data_prev = NULL;

  return err;
}

NodeManagementRemoteObject* NodeManagementRemoteCreate(uint32_t id, EntityRemoteObject* entity) {
  NodeManagementRemote* const nmr = (NodeManagementRemote*)EEBUS_MALLOC(sizeof(NodeManagementRemote));
  if (nmr == NULL) {
    return NULL;
  }

  if (NodeManagementRemoteConstruct(nmr, id, entity) != kEebusErrorOk) {
    NodeManagementRemoteDelete(NODE_MANAGEMENT_REMOTE_OBJECT(nmr));
    return NULL;
  }

  return NODE_MANAGEMENT_REMOTE_OBJECT(nmr);
}

const UseCaseInformationDataType* UseCaseInformationListFind(
    const UseCaseInformationListDataType* use_case_list,
    const FeatureAddressType* addr,
    const UseCaseActorType* actor,
    const UseCaseNameType* use_case_name_id
) {
  if ((use_case_list == NULL) || (addr == NULL) || (actor == NULL) || (use_case_name_id == NULL)) {
    return NULL;
  }

  for (size_t i = 0; i < use_case_list->use_case_information_data_size; i++) {
    const UseCaseInformationDataType* const use_case_info = use_case_list->use_case_information_data[i];

    if (UseCaseInformationMatch(use_case_info, addr, actor, use_case_name_id)) {
      return use_case_info;
    }
  }

  return NULL;
}

void PublishUseCaseSupportedEvent(
    NodeManagementRemote* self,
    const FeatureAddressType* const addr,
    UseCaseActorType actor,
    UseCaseNameType use_case_name_id,
    ElementChangeType change_type
) {
  DeviceRemoteObject* dr = FEATURE_REMOTE_GET_DEVICE(FEATURE_REMOTE_OBJECT(self));
  if (dr == NULL) {
    return;
  }
  EventsManagerObject* const events_manager = DEVICE_LOCAL_GET_EVENTS_MANAGER(DEVICE_REMOTE_GET_LOCAL_DEVICE(dr));

  const UseCaseFilterType use_case_filter = {
      .actor            = actor,
      .use_case_name_id = use_case_name_id,
  };

  EventPayload payload = {
      .ski             = DEVICE_REMOTE_GET_SKI(dr),
      .event_type      = kEventTypeUseCaseChange,
      .change_type     = change_type,
      .device          = dr,
      .entity          = NULL,
      .feature         = NULL,
      .local_feature   = NULL,
      .function_type   = 0,
      .function_data   = NULL,
      .cmd_classifier  = NULL,
      .use_case_filter = &use_case_filter,
  };

  if (addr->entity == NULL) {
    const Vector* const entities = DEVICE_REMOTE_GET_ENTITIES(DEVICE_REMOTE_OBJECT(dr));
    if (entities == NULL) {
      return;
    }

    for (size_t i = 0; i < VectorGetSize(entities); i++) {
      payload.entity = (EntityRemoteObject*)VectorGetElement(entities, i);
      EVENTS_PUBLISH(events_manager, &payload);
    }
  } else {
    payload.entity = DEVICE_REMOTE_GET_ENTITY(DEVICE_REMOTE_OBJECT(dr), addr->entity, addr->entity_size);
    EVENTS_PUBLISH(events_manager, &payload);
  }
}

void ProcessUseCaseSupport(
    NodeManagementRemote* self,
    const UseCaseInformationListDataType* const use_case_data_a,
    const UseCaseInformationListDataType* const use_case_data_b,
    ElementChangeType change_type
) {
  for (size_t i = 0; i < use_case_data_b->use_case_information_data_size; i++) {
    const UseCaseInformationDataType* const use_case_info_b = use_case_data_b->use_case_information_data[i];

    if ((use_case_info_b == NULL) || (use_case_info_b->address == NULL) || (use_case_info_b->actor == NULL)) {
      continue;
    }

    const FeatureAddressType* const addr = use_case_info_b->address;
    const UseCaseActorType* const actor  = use_case_info_b->actor;

    for (size_t j = 0; j < use_case_info_b->use_case_support_size; j++) {
      const UseCaseSupportType* const use_case_support_b = use_case_info_b->use_case_support[j];

      if ((use_case_support_b == NULL) || (use_case_support_b->use_case_name == NULL)) {
        continue;
      }

      const UseCaseNameType* const use_case_name = use_case_support_b->use_case_name;
      if (UseCaseInformationListFind(use_case_data_a, addr, actor, use_case_name) == NULL) {
        PublishUseCaseSupportedEvent(self, addr, *actor, *use_case_name, change_type);
      }
    }
  }
}

EebusError UpdateUseCaseData(NodeManagementRemote* self, const UseCaseInformationListDataType* use_case_data_new) {
  if (use_case_data_new != NULL) {
    ProcessUseCaseSupport(self, self->use_case_data_prev, use_case_data_new, kElementChangeAdd);
  }

  if (self->use_case_data_prev != NULL) {
    ProcessUseCaseSupport(self, use_case_data_new, self->use_case_data_prev, kElementChangeRemove);
  }

  ModelFunctionDataDelete(kFunctionTypeNodeManagementUseCaseData, (void*)self->use_case_data_prev);
  if (use_case_data_new == NULL) {
    self->use_case_data_prev = NULL;
    return kEebusErrorOk;
  }

  self->use_case_data_prev = ModelFunctionDataCopy(kFunctionTypeNodeManagementUseCaseData, use_case_data_new);
  return (self->use_case_data_prev == NULL) ? kEebusErrorMemoryAllocate : kEebusErrorOk;
}

EebusError UpdateData(
    FeatureRemoteObject* self,
    FunctionType function_type,
    const void* new_data,
    const FilterType* filter_partial,
    const FilterType* filter_delete,
    bool persist
) {
  NodeManagementRemote* const nmr = NODE_MANAGEMENT_REMOTE(self);

  const EebusError err = FeatureRemoteUpdateData(self, function_type, new_data, filter_partial, filter_delete, persist);
  if (err != kEebusErrorOk) {
    return err;
  }

  if (function_type != kFunctionTypeNodeManagementUseCaseData) {
    return kEebusErrorOk;
  }

  // Publish event for remote device being updated with the use case data
  DeviceRemoteObject* const dr = FEATURE_REMOTE_GET_DEVICE(FEATURE_REMOTE_OBJECT(self));

  const EventPayload payload = {
      .ski           = DEVICE_REMOTE_GET_SKI(dr),
      .event_type    = kEventTypeDeviceChange,
      .change_type   = kElementChangeUpdate,
      .device        = dr,
      .feature       = self,
      .function_data = new_data,
      .function_type = kFunctionTypeNodeManagementUseCaseData,
  };

  EVENTS_PUBLISH(DEVICE_LOCAL_GET_EVENTS_MANAGER(DEVICE_REMOTE_GET_LOCAL_DEVICE(dr)), &payload);

  const UseCaseInformationListDataType* const use_case_data_new = (const UseCaseInformationListDataType*)
      FeatureRemoteGetData(FEATURE_REMOTE_OBJECT(self), kFunctionTypeNodeManagementUseCaseData);
  return UpdateUseCaseData(nmr, use_case_data_new);
}

void Destruct(FeatureObject* self) {
  NodeManagementRemote* const nmr = NODE_MANAGEMENT_REMOTE(self);

  // Force to update use case data with NULL to trigger removal events
  UpdateUseCaseData(nmr, NULL);

  ModelFunctionDataDelete(kFunctionTypeNodeManagementUseCaseData, (void*)nmr->use_case_data_prev);
  nmr->use_case_data_prev = NULL;

  FeatureRemoteDestruct(self);
}
