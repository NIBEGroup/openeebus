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
 * @brief Binding Manager implementation
 */

#include "src/common/eebus_errors.h"
#include "src/common/eebus_malloc.h"
#include "src/spine/api/binding_manager_interface.h"
#include "src/spine/api/device_local_interface.h"
#include "src/spine/api/feature_link.h"
#include "src/spine/api/feature_link_container.h"
#include "src/spine/events/events.h"
#include "src/spine/model/entity_types.h"
#include "src/spine/model/node_management_types.h"

typedef struct BindingManager BindingManager;

struct BindingManager {
  /** Implements the Binding Manager Interface */
  BindingManagerObject obj;

  const DeviceLocalObject* local_device;
  uint64_t binding_num;
  FeatureLinkContainer binding_entries;
};

#define BINDING_MANAGER(obj) ((BindingManager*)(obj))

static void Destruct(BindingManagerObject* self);
static EebusError AddBinding(
    BindingManagerObject* self, DeviceRemoteObject* remote_device, const BindingManagementRequestCallType* data);
static EebusError RemoveBinding(
    BindingManagerObject* self, const BindingManagementDeleteCallType* data, DeviceRemoteObject* remote_device);
static void RemoveDeviceBindings(BindingManagerObject* self, DeviceRemoteObject* remote_device);
static void RemoveEntityBindings(BindingManagerObject* self, EntityRemoteObject* remote_entity);
static bool HasBinding(
    const BindingManagerObject* self, const FeatureAddressType* local_addr, const FeatureAddressType* remote_addr);
static NodeManagementBindingDataType* CreateBindingData(
    const BindingManagerObject* self, const DeviceRemoteObject* remote_device);

static const BindingManagerInterface binding_manager_methods = {
    .destruct               = Destruct,
    .add_binding            = AddBinding,
    .remove_binding         = RemoveBinding,
    .remove_device_bindings = RemoveDeviceBindings,
    .remove_entity_bindings = RemoveEntityBindings,
    .has_binding            = HasBinding,
    .create_binding_data    = CreateBindingData,
};

static void BindingManagerConstruct(BindingManager* self, const DeviceLocalObject* local_device);
static BindingManagementEntryDataType* CreateBindingEntryData(const FeatureLink* binding);
static EebusError AddBindingEntries(
    const BindingManager* self, NodeManagementBindingDataType* binding_data, const DeviceRemoteObject* device_remote);

void BindingManagerConstruct(BindingManager* self, const DeviceLocalObject* local_device) {
  // Override "virtual functions table"
  BINDING_MANAGER_INTERFACE(self) = &binding_manager_methods;

  self->local_device = local_device;
  self->binding_num  = 0;
  FeatureLinkContainerConstruct(&self->binding_entries);
}

BindingManagerObject* BindingManagerCreate(const DeviceLocalObject* local_device) {
  BindingManager* const binding_manager = (BindingManager*)EEBUS_MALLOC(sizeof(BindingManager));

  BindingManagerConstruct(binding_manager, local_device);

  return BINDING_MANAGER_OBJECT(binding_manager);
}

void Destruct(BindingManagerObject* self) {
  BindingManager* const bm = BINDING_MANAGER(self);

  FeatureLinkContainerDestruct(&bm->binding_entries);
}

uint64_t GetNextBindingId(BindingManager* self) { return self->binding_num++; }

EebusError AddBinding(
    BindingManagerObject* self, DeviceRemoteObject* remote_device, const BindingManagementRequestCallType* data) {
  BindingManager* const bm = BINDING_MANAGER(self);

  FeatureLocalObject* const server_feature
      = DEVICE_LOCAL_GET_FEATURE_WITH_ADDRESS(bm->local_device, data->server_address);

  FeatureRemoteObject* const client_feature
      = DEVICE_REMOTE_GET_FEATURE_WITH_ADDRESS(remote_device, data->client_address);

  if (data->server_feature_type != NULL) {
    if (!FeatureParametersMatch(FEATURE(server_feature), kRoleTypeServer, *data->server_feature_type)) {
      return kEebusErrorNoChange;
    }

    if (!FeatureParametersMatch(FEATURE(client_feature), kRoleTypeClient, *data->server_feature_type)) {
      return kEebusErrorNoChange;
    }
  }

  // A local feature can only have one remote binding
  if (FeatureLinkContainerHasServer(&bm->binding_entries, data->server_address)) {
    return kEebusErrorNoChange;
  }

  FeatureLinkContainerAdd(&bm->binding_entries, GetNextBindingId(bm), server_feature, client_feature);

  const EventPayload payload = {
      .ski           = DEVICE_REMOTE_GET_SKI(remote_device),
      .event_type    = kEventTypeBindingChange,
      .change_type   = kElementChangeAdd,
      .device        = remote_device,
      .entity        = FEATURE_REMOTE_GET_ENTITY(client_feature),
      .feature       = client_feature,
      .local_feature = server_feature,
      .function_data = data,
      .function_type = kFunctionTypeBindingManagementRequestCall,
  };

  EVENTS_PUBLISH(DEVICE_LOCAL_GET_EVENTS_MANAGER(bm->local_device), &payload);
  return kEebusErrorOk;
}

EebusError RemoveBinding(
    BindingManagerObject* self, const BindingManagementDeleteCallType* data, DeviceRemoteObject* remote_device) {
  BindingManager* const bm = BINDING_MANAGER(self);

  // According to the spec 7.4.4
  // a. The absence of "bindingDelete.client_addr.device" SHALL be treated as if it was
  //    present and set to the sender's "device" address part.
  // b. The absence of "bindingDelete.serverAddress.device" SHALL be treated as if it was
  //    present and set to the recipient's "device" address part.
  FeatureAddressType client_addr = *data->client_address;
  if (client_addr.device == NULL) {
    client_addr.device = (char*)DEVICE_GET_ADDRESS(DEVICE_OBJECT(remote_device));
  }

  FeatureAddressType server_addr = *data->server_address;
  if (server_addr.device == NULL) {
    server_addr.device = (char*)DEVICE_GET_ADDRESS(DEVICE_OBJECT(bm->local_device));
  }

  FeatureRemoteObject* const client_feature = DEVICE_REMOTE_GET_FEATURE_WITH_ADDRESS(remote_device, &client_addr);
  if (client_feature == NULL) {
    return kEebusErrorInputArgumentNull;
  }

  FeatureLocalObject* const server_feature = DEVICE_LOCAL_GET_FEATURE_WITH_ADDRESS(bm->local_device, &server_addr);
  if (server_feature == NULL) {
    return kEebusErrorNoChange;
  }

  if (FEATURE_GET_ROLE(FEATURE_OBJECT(server_feature)) != kRoleTypeServer) {
    return kEebusErrorNoChange;
  }

  FeatureLink* const binding = FeatureLinkContainerFind(&bm->binding_entries, &server_addr, &client_addr);
  if (binding == NULL) {
    return kEebusErrorNoChange;
  }

  FeatureLinkContainerRemove(&bm->binding_entries, binding);

  const EventPayload payload = {
      .ski           = DEVICE_REMOTE_GET_SKI(remote_device),
      .event_type    = kEventTypeBindingChange,
      .change_type   = kElementChangeRemove,
      .device        = remote_device,
      .entity        = FEATURE_REMOTE_GET_ENTITY(client_feature),
      .feature       = client_feature,
      .local_feature = server_feature,
      .function_data = data,
      .function_type = kFunctionTypeBindingManagementDeleteCall,
  };

  EVENTS_PUBLISH(DEVICE_LOCAL_GET_EVENTS_MANAGER(bm->local_device), &payload);
  return kEebusErrorOk;
}

void RemoveDeviceBindings(BindingManagerObject* self, DeviceRemoteObject* remote_device) {
  if (remote_device == NULL) {
    return;
  }

  const Vector* const entities = DEVICE_REMOTE_GET_ENTITIES(remote_device);
  for (size_t i = 0; i < VectorGetSize(entities); ++i) {
    EntityRemoteObject* const entity = (EntityRemoteObject*)VectorGetElement(entities, i);
    BINDING_MANAGER_REMOVE_ENTITY_BINDINGS(self, entity);
  }
}

void RemoveEntityBindings(BindingManagerObject* self, EntityRemoteObject* remote_entity) {
  BindingManager* const bm = BINDING_MANAGER(self);

  if (remote_entity == NULL) {
    return;
  }

  for (size_t i = 0; i < FeatureLinkContainerGetSize(&bm->binding_entries);) {
    FeatureLink* const binding = FeatureLinkContainerGetElement(&bm->binding_entries, i);
    if (FeatureLinkRemoteEntityMatch(binding, remote_entity)) {
      const FeatureAddressType* const server_addr = FeatureLinkGetServerAddr(binding);
      const FeatureAddressType* const client_addr = FeatureLinkGetClientAddr(binding);

      DeviceRemoteObject* const dr = ENTITY_REMOTE_GET_DEVICE(remote_entity);

      const EventPayload payload = {
          .ski           = DEVICE_REMOTE_GET_SKI(dr),
          .event_type    = kEventTypeBindingChange,
          .change_type   = kElementChangeRemove,
          .device        = dr,
          .entity        = remote_entity,
          .feature       = ENTITY_REMOTE_GET_FEATURE_WITH_ID(remote_entity, client_addr->feature),
          .local_feature = DEVICE_LOCAL_GET_FEATURE_WITH_ADDRESS(bm->local_device, server_addr),
      };

      EVENTS_PUBLISH(DEVICE_LOCAL_GET_EVENTS_MANAGER(bm->local_device), &payload);
      FeatureLinkContainerRemove(&bm->binding_entries, binding);
    } else {
      ++i;
    }
  }
}

bool HasBinding(
    const BindingManagerObject* self, const FeatureAddressType* local_addr, const FeatureAddressType* remote_addr) {
  const BindingManager* const bm = BINDING_MANAGER(self);
  return FeatureLinkContainerFind(&bm->binding_entries, local_addr, remote_addr);
}

BindingManagementEntryDataType* CreateBindingEntryData(const FeatureLink* binding) {
  const FeatureAddressType* const server_addr = FeatureLinkGetServerAddr(binding);
  const FeatureAddressType* const client_addr = FeatureLinkGetClientAddr(binding);

  return BindingManagementEntryDataCreate((BindingIdType)binding->id, server_addr, client_addr);
}

EebusError AddBindingEntries(
    const BindingManager* self, NodeManagementBindingDataType* binding_data, const DeviceRemoteObject* device_remote) {
  const size_t n = FeatureLinkContainerGetRemoteDeviceMatchNum(&self->binding_entries, device_remote);
  if (n == 0) {
    return kEebusErrorOk;
  }

  const size_t buf_size = sizeof(binding_data->binding_entry[0]) * n;

  const BindingManagementEntryDataType** const binding_entry
      = (const BindingManagementEntryDataType**)EEBUS_MALLOC(buf_size);
  binding_data->binding_entry = binding_entry;
  if (binding_data->binding_entry == NULL) {
    return kEebusErrorMemoryAllocate;
  }

  binding_data->binding_entry_size = 0;
  for (size_t i = 0; i < FeatureLinkContainerGetSize(&self->binding_entries); ++i) {
    const FeatureLink* const binding = FeatureLinkContainerGetElement(&self->binding_entries, i);
    if (FeatureLinkRemoteDeviceMatch(binding, device_remote)) {
      binding_entry[i] = CreateBindingEntryData(binding);
      if (binding_entry[i] == NULL) {
        return kEebusErrorMemoryAllocate;
      }

      binding_data->binding_entry_size++;
    }
  }

  return kEebusErrorOk;
}

NodeManagementBindingDataType* CreateBindingData(
    const BindingManagerObject* self, const DeviceRemoteObject* remote_device) {
  NodeManagementBindingDataType* const binding_data = NodeManagementBindingDataCreateEmpty();
  if (binding_data == NULL) {
    return NULL;
  }

  const EebusError err = AddBindingEntries(BINDING_MANAGER(self), binding_data, remote_device);
  if (err != kEebusErrorOk) {
    NodeManagementBindingDataDelete(binding_data);
    return NULL;
  }

  return binding_data;
}
