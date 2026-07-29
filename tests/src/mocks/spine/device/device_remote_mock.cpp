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
 * @brief Device Remote mock implementation
 */

#include "device_remote_mock.h"

#include <gmock/gmock.h>

#include "src/spine/api/device_remote_interface.h"

static void Destruct(DeviceObject* self);
static const char* GetAddress(const DeviceObject* self);
static const DeviceTypeType* GetDeviceType(const DeviceObject* self);
static const NetworkManagementFeatureSetType* GetFeatureSet(const DeviceObject* self);
static const NodeManagementDestinationDataType* CreateDestinationData(const DeviceObject* self);
static DeviceLocalObject* GetLocalDevice(const DeviceRemoteObject* self);
static const char* GetSki(const DeviceRemoteObject* self);
static DataReaderObject* GetDataReader(const DeviceRemoteObject* self);
static void AddEntity(DeviceRemoteObject* self, EntityRemoteObject* entity);
static EntityRemoteObject*
ReleaseEntity(DeviceRemoteObject* self, const uint32_t* const* entity_ids, size_t entity_ids_size);
static EntityRemoteObject*
GetEntity(const DeviceRemoteObject* self, const uint32_t* const* entity_ids, size_t entity_ids_size);
static const Vector* GetEntities(const DeviceRemoteObject* self);
static FeatureRemoteObject*
GetFeatureWithAddress(const DeviceRemoteObject* self, const FeatureAddressType* feature_addr);
static FeatureRemoteObject* GetFeatureWithTypeAndRole(
    const DeviceRemoteObject* self,
    const uint32_t* const* entity_ids,
    size_t entity_ids_size,
    FeatureTypeType feature_type,
    RoleType role
);
static EebusError HandleSpineMesssage(DeviceRemoteObject* self, MessageBuffer* msg);
static NodeManagementRemoteObject* GetNodeManagement(const DeviceRemoteObject* self);
static SenderObject* GetSender(const DeviceRemoteObject* self);
static NodeManagementUseCaseDataType* UseCasesDataCopy(const DeviceRemoteObject* self);
static void UpdateDevice(DeviceRemoteObject* self, const NetworkManagementDeviceDescriptionDataType* description);
static const Vector*
AddEntityAndFeatures(DeviceRemoteObject* self, bool init, const NodeManagementDetailedDiscoveryDataType* data);
static EebusError CheckEntityInformation(
    const DeviceRemoteObject* self,
    bool init,
    const NodeManagementDetailedDiscoveryEntityInformationType* entity_info
);

static const DeviceRemoteInterface device_remote_methods = {
    .device_interface = {
        .destruct                = Destruct,
        .get_address             = GetAddress,
        .get_device_type         = GetDeviceType,
        .get_feature_set         = GetFeatureSet,
        .create_destination_data = CreateDestinationData,
    },

    .get_local_device               = GetLocalDevice,
    .get_ski                        = GetSki,
    .get_data_reader                = GetDataReader,
    .add_entity                     = AddEntity,
    .release_entity                 = ReleaseEntity,
    .get_entity                     = GetEntity,
    .get_entities                   = GetEntities,
    .get_feature_with_address       = GetFeatureWithAddress,
    .get_feature_with_type_and_role = GetFeatureWithTypeAndRole,
    .handle_spine_messsage          = HandleSpineMesssage,
    .get_node_management            = GetNodeManagement,
    .get_sender                     = GetSender,
    .use_cases_data_copy            = UseCasesDataCopy,
    .update_device                  = UpdateDevice,
    .add_entity_and_features        = AddEntityAndFeatures,
    .check_entity_information       = CheckEntityInformation,
};

static EebusError DeviceRemoteMockConstruct(DeviceRemoteMock* self);

EebusError DeviceRemoteMockConstruct(DeviceRemoteMock* self) {
  // Override "virtual functions table"
  DEVICE_REMOTE_INTERFACE(self) = &device_remote_methods;

  self->gmock = new DeviceRemoteGMock();
  if (self->gmock == nullptr) {
    return kEebusErrorMemoryAllocate;
  }

  return kEebusErrorOk;
}

DeviceRemoteMock* DeviceRemoteMockCreate(void) {
  DeviceRemoteMock* const mock = (DeviceRemoteMock*)EEBUS_MALLOC(sizeof(DeviceRemoteMock));
  if (mock == nullptr) {
    return nullptr;
  }

  if (DeviceRemoteMockConstruct(mock) != kEebusErrorOk) {
    DeviceRemoteMockDelete(mock);
    return nullptr;
  }

  return mock;
}

void Destruct(DeviceObject* self) {
  DeviceRemoteMock* const mock = DEVICE_REMOTE_MOCK(self);
  mock->gmock->Destruct(self);
  delete mock->gmock;
}

const char* GetAddress(const DeviceObject* self) {
  DeviceRemoteMock* const mock = DEVICE_REMOTE_MOCK(self);
  return mock->gmock->GetAddress(self);
}

const DeviceTypeType* GetDeviceType(const DeviceObject* self) {
  DeviceRemoteMock* const mock = DEVICE_REMOTE_MOCK(self);
  return mock->gmock->GetDeviceType(self);
}

const NetworkManagementFeatureSetType* GetFeatureSet(const DeviceObject* self) {
  DeviceRemoteMock* const mock = DEVICE_REMOTE_MOCK(self);
  return mock->gmock->GetFeatureSet(self);
}

const NodeManagementDestinationDataType* CreateDestinationData(const DeviceObject* self) {
  DeviceRemoteMock* const mock = DEVICE_REMOTE_MOCK(self);
  return mock->gmock->CreateDestinationData(self);
}

DeviceLocalObject* GetLocalDevice(const DeviceRemoteObject* self) {
  DeviceRemoteMock* const mock = DEVICE_REMOTE_MOCK(self);
  return mock->gmock->GetLocalDevice(self);
}

const char* GetSki(const DeviceRemoteObject* self) {
  DeviceRemoteMock* const mock = DEVICE_REMOTE_MOCK(self);
  return mock->gmock->GetSki(self);
}

DataReaderObject* GetDataReader(const DeviceRemoteObject* self) {
  DeviceRemoteMock* const mock = DEVICE_REMOTE_MOCK(self);
  return mock->gmock->GetDataReader(self);
}

void AddEntity(DeviceRemoteObject* self, EntityRemoteObject* entity) {
  DeviceRemoteMock* const mock = DEVICE_REMOTE_MOCK(self);
  mock->gmock->AddEntity(self, entity);
}

EntityRemoteObject* ReleaseEntity(DeviceRemoteObject* self, const uint32_t* const* entity_ids, size_t entity_ids_size) {
  DeviceRemoteMock* const mock = DEVICE_REMOTE_MOCK(self);
  return mock->gmock->ReleaseEntity(self, entity_ids, entity_ids_size);
}

EntityRemoteObject*
GetEntity(const DeviceRemoteObject* self, const uint32_t* const* entity_ids, size_t entity_ids_size) {
  DeviceRemoteMock* const mock = DEVICE_REMOTE_MOCK(self);
  return mock->gmock->GetEntity(self, entity_ids, entity_ids_size);
}

const Vector* GetEntities(const DeviceRemoteObject* self) {
  DeviceRemoteMock* const mock = DEVICE_REMOTE_MOCK(self);
  return mock->gmock->GetEntities(self);
}

FeatureRemoteObject* GetFeatureWithAddress(const DeviceRemoteObject* self, const FeatureAddressType* feature_addr) {
  DeviceRemoteMock* const mock = DEVICE_REMOTE_MOCK(self);
  return mock->gmock->GetFeatureWithAddress(self, feature_addr);
}

FeatureRemoteObject* GetFeatureWithTypeAndRole(
    const DeviceRemoteObject* self,
    const uint32_t* const* entity_ids,
    size_t entity_ids_size,
    FeatureTypeType feature_type,
    RoleType role
) {
  DeviceRemoteMock* const mock = DEVICE_REMOTE_MOCK(self);
  return mock->gmock->GetFeatureWithTypeAndRole(self, entity_ids, entity_ids_size, feature_type, role);
}

EebusError HandleSpineMesssage(DeviceRemoteObject* self, MessageBuffer* msg) {
  DeviceRemoteMock* const mock = DEVICE_REMOTE_MOCK(self);
  return mock->gmock->HandleSpineMesssage(self, msg);
}

NodeManagementRemoteObject* GetNodeManagement(const DeviceRemoteObject* self) {
  DeviceRemoteMock* const mock = DEVICE_REMOTE_MOCK(self);
  return mock->gmock->GetNodeManagement(self);
}

SenderObject* GetSender(const DeviceRemoteObject* self) {
  DeviceRemoteMock* const mock = DEVICE_REMOTE_MOCK(self);
  return mock->gmock->GetSender(self);
}

NodeManagementUseCaseDataType* UseCasesDataCopy(const DeviceRemoteObject* self) {
  DeviceRemoteMock* const mock = DEVICE_REMOTE_MOCK(self);
  return mock->gmock->UseCasesDataCopy(self);
}

void UpdateDevice(DeviceRemoteObject* self, const NetworkManagementDeviceDescriptionDataType* description) {
  DeviceRemoteMock* const mock = DEVICE_REMOTE_MOCK(self);
  mock->gmock->UpdateDevice(self, description);
}

const Vector*
AddEntityAndFeatures(DeviceRemoteObject* self, bool init, const NodeManagementDetailedDiscoveryDataType* data) {
  DeviceRemoteMock* const mock = DEVICE_REMOTE_MOCK(self);
  return mock->gmock->AddEntityAndFeatures(self, init, data);
}

EebusError CheckEntityInformation(
    const DeviceRemoteObject* self,
    bool init,
    const NodeManagementDetailedDiscoveryEntityInformationType* entity_info
) {
  DeviceRemoteMock* const mock = DEVICE_REMOTE_MOCK(self);
  return mock->gmock->CheckEntityInformation(self, init, entity_info);
}
