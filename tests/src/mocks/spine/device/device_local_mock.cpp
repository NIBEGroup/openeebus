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
 * @brief Device Local mock implementation
 */

#include "device_local_mock.h"

#include <gmock/gmock.h>

#include "src/spine/api/device_local_interface.h"

static void Destruct(DeviceObject* self);
static const char* GetAddress(const DeviceObject* self);
static const DeviceTypeType* GetDeviceType(const DeviceObject* self);
static const NetworkManagementFeatureSetType* GetFeatureSet(const DeviceObject* self);
static const NodeManagementDestinationDataType* CreateDestinationData(const DeviceObject* self);
static EebusError Start(DeviceLocalObject* self);
static void Stop(DeviceLocalObject* self);
static DataReaderObject* SetupRemoteDevice(DeviceLocalObject* self, const char* ski, DataWriterObject* writer);
static void AddRemoteDeviceForSki(DeviceLocalObject* self, const char* ski, DeviceRemoteObject* remote_device);
static EebusError RequestRemoteDetailedDiscoveryData(DeviceLocalObject* self, const DeviceRemoteObject* remote_device);
static void RemoveRemoteDeviceConnection(DeviceLocalObject* self, const char* ski);
static void RemoveRemoteDevice(DeviceLocalObject* self, const char* ski);
static DeviceRemoteObject* GetRemoteDeviceWithAddress(const DeviceLocalObject* self, const char* device_addr);
static DeviceRemoteObject* GetRemoteDeviceWithSki(const DeviceLocalObject* self, const char* ski);
static void AddEntity(DeviceLocalObject* self, EntityLocalObject* entity);
static void RemoveEntity(DeviceLocalObject* self, EntityLocalObject* entity);
static EntityLocalObject*
GetEntity(const DeviceLocalObject* self, const uint32_t* const* entity_ids, size_t entity_ids_size);
static EntityLocalObject* GetEntityWithType(const DeviceLocalObject* self, EntityTypeType entity_type);
static const Vector* GetEntities(const DeviceLocalObject* self);
static FeatureLocalObject* GetFeatureWithAddress(const DeviceLocalObject* self, const FeatureAddressType* feature_addr);
static EebusError HandleMessage(DeviceLocalObject* self, MessageBuffer* msg, DeviceRemoteObject* remote_device);
static NodeManagementObject* GetNodeManagement(const DeviceLocalObject* self);
static BindingManagerObject* GetBindingManager(const DeviceLocalObject* self);
static SubscriptionManagerObject* GetSubscriptionManager(const DeviceLocalObject* self);
static EventsManagerObject* GetEventsManager(const DeviceLocalObject* self);
static void
NotifySubscribers(const DeviceLocalObject* self, const FeatureAddressType* feature_addr, const CmdType* cmd);
static NodeManagementDetailedDiscoveryDeviceInformationType* CreateInformation(const DeviceLocalObject* self);
static void Lock(DeviceLocalObject* self);
static void Unlock(DeviceLocalObject* self);

static const DeviceLocalInterface device_local_methods = {
    .device_interface = {
        .destruct                = Destruct,
        .get_address             = GetAddress,
        .get_device_type         = GetDeviceType,
        .get_feature_set         = GetFeatureSet,
        .create_destination_data = CreateDestinationData,
    },

    .start                                  = Start,
    .stop                                   = Stop,
    .setup_remote_device                    = SetupRemoteDevice,
    .add_remote_device_for_ski              = AddRemoteDeviceForSki,
    .request_remote_detailed_discovery_data = RequestRemoteDetailedDiscoveryData,
    .remove_remote_device_connection        = RemoveRemoteDeviceConnection,
    .remove_remote_device                   = RemoveRemoteDevice,
    .get_remote_device_with_address         = GetRemoteDeviceWithAddress,
    .get_remote_device_with_ski             = GetRemoteDeviceWithSki,
    .add_entity                             = AddEntity,
    .remove_entity                          = RemoveEntity,
    .get_entity                             = GetEntity,
    .get_entity_with_type                   = GetEntityWithType,
    .get_entities                           = GetEntities,
    .get_feature_with_address               = GetFeatureWithAddress,
    .handle_message                         = HandleMessage,
    .get_node_management                    = GetNodeManagement,
    .get_binding_manager                    = GetBindingManager,
    .get_subscription_manager               = GetSubscriptionManager,
    .get_events_manager                     = GetEventsManager,
    .notify_subscribers                     = NotifySubscribers,
    .create_information                     = CreateInformation,
    .lock                                   = Lock,
    .unlock                                 = Unlock,
};

static EebusError DeviceLocalMockConstruct(DeviceLocalMock* self);

EebusError DeviceLocalMockConstruct(DeviceLocalMock* self) {
  // Override "virtual functions table"
  DEVICE_LOCAL_INTERFACE(self) = &device_local_methods;

  self->gmock = new DeviceLocalGMock();
  if (self->gmock == nullptr) {
    return kEebusErrorMemoryAllocate;
  }

  return kEebusErrorOk;
}

DeviceLocalMock* DeviceLocalMockCreate(void) {
  DeviceLocalMock* const mock = (DeviceLocalMock*)EEBUS_MALLOC(sizeof(DeviceLocalMock));
  if (mock == nullptr) {
    return nullptr;
  }

  if (DeviceLocalMockConstruct(mock) != kEebusErrorOk) {
    DeviceLocalMockDelete(mock);
    return nullptr;
  }

  return mock;
}

void Destruct(DeviceObject* self) {
  DeviceLocalMock* const mock = DEVICE_LOCAL_MOCK(self);
  mock->gmock->Destruct(self);
  delete mock->gmock;
}

const char* GetAddress(const DeviceObject* self) {
  DeviceLocalMock* const mock = DEVICE_LOCAL_MOCK(self);
  return mock->gmock->GetAddress(self);
}

const DeviceTypeType* GetDeviceType(const DeviceObject* self) {
  DeviceLocalMock* const mock = DEVICE_LOCAL_MOCK(self);
  return mock->gmock->GetDeviceType(self);
}

const NetworkManagementFeatureSetType* GetFeatureSet(const DeviceObject* self) {
  DeviceLocalMock* const mock = DEVICE_LOCAL_MOCK(self);
  return mock->gmock->GetFeatureSet(self);
}

const NodeManagementDestinationDataType* CreateDestinationData(const DeviceObject* self) {
  DeviceLocalMock* const mock = DEVICE_LOCAL_MOCK(self);
  return mock->gmock->CreateDestinationData(self);
}

EebusError Start(DeviceLocalObject* self) {
  DeviceLocalMock* const mock = DEVICE_LOCAL_MOCK(self);
  return mock->gmock->Start(self);
}

void Stop(DeviceLocalObject* self) {
  DeviceLocalMock* const mock = DEVICE_LOCAL_MOCK(self);
  mock->gmock->Stop(self);
}

DataReaderObject* SetupRemoteDevice(DeviceLocalObject* self, const char* ski, DataWriterObject* writer) {
  DeviceLocalMock* const mock = DEVICE_LOCAL_MOCK(self);
  return mock->gmock->SetupRemoteDevice(self, ski, writer);
}

void AddRemoteDeviceForSki(DeviceLocalObject* self, const char* ski, DeviceRemoteObject* remote_device) {
  DeviceLocalMock* const mock = DEVICE_LOCAL_MOCK(self);
  mock->gmock->AddRemoteDeviceForSki(self, ski, remote_device);
}

EebusError RequestRemoteDetailedDiscoveryData(DeviceLocalObject* self, const DeviceRemoteObject* remote_device) {
  DeviceLocalMock* const mock = DEVICE_LOCAL_MOCK(self);
  return mock->gmock->RequestRemoteDetailedDiscoveryData(self, remote_device);
}

void RemoveRemoteDeviceConnection(DeviceLocalObject* self, const char* ski) {
  DeviceLocalMock* const mock = DEVICE_LOCAL_MOCK(self);
  mock->gmock->RemoveRemoteDeviceConnection(self, ski);
}

void RemoveRemoteDevice(DeviceLocalObject* self, const char* ski) {
  DeviceLocalMock* const mock = DEVICE_LOCAL_MOCK(self);
  mock->gmock->RemoveRemoteDevice(self, ski);
}

DeviceRemoteObject* GetRemoteDeviceWithAddress(const DeviceLocalObject* self, const char* device_addr) {
  DeviceLocalMock* const mock = DEVICE_LOCAL_MOCK(self);
  return mock->gmock->GetRemoteDeviceWithAddress(self, device_addr);
}

DeviceRemoteObject* GetRemoteDeviceWithSki(const DeviceLocalObject* self, const char* ski) {
  DeviceLocalMock* const mock = DEVICE_LOCAL_MOCK(self);
  return mock->gmock->GetRemoteDeviceWithSki(self, ski);
}

void AddEntity(DeviceLocalObject* self, EntityLocalObject* entity) {
  DeviceLocalMock* const mock = DEVICE_LOCAL_MOCK(self);
  mock->gmock->AddEntity(self, entity);
}

void RemoveEntity(DeviceLocalObject* self, EntityLocalObject* entity) {
  DeviceLocalMock* const mock = DEVICE_LOCAL_MOCK(self);
  mock->gmock->RemoveEntity(self, entity);
}

EntityLocalObject* GetEntity(const DeviceLocalObject* self, const uint32_t* const* entity_ids, size_t entity_ids_size) {
  DeviceLocalMock* const mock = DEVICE_LOCAL_MOCK(self);
  return mock->gmock->GetEntity(self, entity_ids, entity_ids_size);
}

EntityLocalObject* GetEntityWithType(const DeviceLocalObject* self, EntityTypeType entity_type) {
  DeviceLocalMock* const mock = DEVICE_LOCAL_MOCK(self);
  return mock->gmock->GetEntityWithType(self, entity_type);
}

const Vector* GetEntities(const DeviceLocalObject* self) {
  DeviceLocalMock* const mock = DEVICE_LOCAL_MOCK(self);
  return mock->gmock->GetEntities(self);
}

FeatureLocalObject* GetFeatureWithAddress(const DeviceLocalObject* self, const FeatureAddressType* feature_addr) {
  DeviceLocalMock* const mock = DEVICE_LOCAL_MOCK(self);
  return mock->gmock->GetFeatureWithAddress(self, feature_addr);
}

EebusError HandleMessage(DeviceLocalObject* self, MessageBuffer* msg, DeviceRemoteObject* remote_device) {
  DeviceLocalMock* const mock = DEVICE_LOCAL_MOCK(self);
  return mock->gmock->HandleMessage(self, msg, remote_device);
}

NodeManagementObject* GetNodeManagement(const DeviceLocalObject* self) {
  DeviceLocalMock* const mock = DEVICE_LOCAL_MOCK(self);
  return mock->gmock->GetNodeManagement(self);
}

BindingManagerObject* GetBindingManager(const DeviceLocalObject* self) {
  DeviceLocalMock* const mock = DEVICE_LOCAL_MOCK(self);
  return mock->gmock->GetBindingManager(self);
}

SubscriptionManagerObject* GetSubscriptionManager(const DeviceLocalObject* self) {
  DeviceLocalMock* const mock = DEVICE_LOCAL_MOCK(self);
  return mock->gmock->GetSubscriptionManager(self);
}

EventsManagerObject* GetEventsManager(const DeviceLocalObject* self) {
  DeviceLocalMock* const mock = DEVICE_LOCAL_MOCK(self);
  return mock->gmock->GetEventsManager(self);
}

void NotifySubscribers(const DeviceLocalObject* self, const FeatureAddressType* feature_addr, const CmdType* cmd) {
  DeviceLocalMock* const mock = DEVICE_LOCAL_MOCK(self);
  mock->gmock->NotifySubscribers(self, feature_addr, cmd);
}

NodeManagementDetailedDiscoveryDeviceInformationType* CreateInformation(const DeviceLocalObject* self) {
  DeviceLocalMock* const mock = DEVICE_LOCAL_MOCK(self);
  return mock->gmock->CreateInformation(self);
}

void Lock(DeviceLocalObject* self) {
  DeviceLocalMock* const mock = DEVICE_LOCAL_MOCK(self);
  mock->gmock->Lock(self);
}

void Unlock(DeviceLocalObject* self) {
  DeviceLocalMock* const mock = DEVICE_LOCAL_MOCK(self);
  mock->gmock->Unlock(self);
}
