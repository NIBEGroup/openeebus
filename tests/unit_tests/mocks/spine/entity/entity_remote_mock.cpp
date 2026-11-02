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
 * @brief Entity Remote mock implementation
 */

#include "entity_remote_mock.h"

#include <gmock/gmock.h>

#include "src/spine/api/entity_remote_interface.h"

static void Destruct(EntityObject* self);
static const EntityAddressType* GetAddress(const EntityObject* self);
static EntityTypeType GetType(const EntityObject* self);
static const char* GetDescription(const EntityObject* self);
static void SetDescription(EntityObject* self, const char* description);
static uint32_t GetNextFeatureId(EntityObject* self);
static DeviceRemoteObject* GetDevice(const EntityRemoteObject* self);
static void UpdateDeviceAddress(EntityRemoteObject* self, const char* device_addr);
static void AddFeature(EntityRemoteObject* self, FeatureRemoteObject* feature);
static void RemoveAllFeatures(EntityRemoteObject* self);
static FeatureRemoteObject*
GetFeatureWithTypeAndRole(const EntityRemoteObject* self, FeatureTypeType feature_type, RoleType role);
static FeatureRemoteObject* GetFeatureWithId(const EntityRemoteObject* self, const uint32_t* feature_id);
static const Vector* GetFeatures(const EntityRemoteObject* self);
static bool HasUseCaseSupport(const EntityRemoteObject* self, const UseCaseFilterType* use_case_filter);

static const EntityRemoteInterface entity_remote_methods = {
    .entity_interface = {
        .destruct            = Destruct,
        .get_address         = GetAddress,
        .get_type            = GetType,
        .get_description     = GetDescription,
        .set_description     = SetDescription,
        .get_next_feature_id = GetNextFeatureId,
    },

    .get_device                     = GetDevice,
    .update_device_address          = UpdateDeviceAddress,
    .add_feature                    = AddFeature,
    .remove_all_features            = RemoveAllFeatures,
    .get_feature_with_type_and_role = GetFeatureWithTypeAndRole,
    .get_feature_with_id            = GetFeatureWithId,
    .get_features                   = GetFeatures,
    .has_use_case_support           = HasUseCaseSupport,
};

static EebusError EntityRemoteMockConstruct(EntityRemoteMock* self);

EebusError EntityRemoteMockConstruct(EntityRemoteMock* self) {
  // Override "virtual functions table"
  ENTITY_REMOTE_INTERFACE(self) = &entity_remote_methods;

  self->gmock = new EntityRemoteGMock();
  if (self->gmock == nullptr) {
    return kEebusErrorMemoryAllocate;
  }

  return kEebusErrorOk;
}

EntityRemoteMock* EntityRemoteMockCreate(void) {
  EntityRemoteMock* const mock = (EntityRemoteMock*)EEBUS_MALLOC(sizeof(EntityRemoteMock));
  if (mock == nullptr) {
    return nullptr;
  }

  if (EntityRemoteMockConstruct(mock) != kEebusErrorOk) {
    EntityRemoteMockDelete(mock);
    return nullptr;
  }

  return mock;
}

void Destruct(EntityObject* self) {
  EntityRemoteMock* const mock = ENTITY_REMOTE_MOCK(self);
  mock->gmock->Destruct(self);
  delete mock->gmock;
}

const EntityAddressType* GetAddress(const EntityObject* self) {
  EntityRemoteMock* const mock = ENTITY_REMOTE_MOCK(self);
  return mock->gmock->GetAddress(self);
}

EntityTypeType GetType(const EntityObject* self) {
  EntityRemoteMock* const mock = ENTITY_REMOTE_MOCK(self);
  return mock->gmock->GetType(self);
}

const char* GetDescription(const EntityObject* self) {
  EntityRemoteMock* const mock = ENTITY_REMOTE_MOCK(self);
  return mock->gmock->GetDescription(self);
}

void SetDescription(EntityObject* self, const char* description) {
  EntityRemoteMock* const mock = ENTITY_REMOTE_MOCK(self);
  mock->gmock->SetDescription(self, description);
}

uint32_t GetNextFeatureId(EntityObject* self) {
  EntityRemoteMock* const mock = ENTITY_REMOTE_MOCK(self);
  return mock->gmock->GetNextFeatureId(self);
}

DeviceRemoteObject* GetDevice(const EntityRemoteObject* self) {
  EntityRemoteMock* const mock = ENTITY_REMOTE_MOCK(self);
  return mock->gmock->GetDevice(self);
}

void UpdateDeviceAddress(EntityRemoteObject* self, const char* device_addr) {
  EntityRemoteMock* const mock = ENTITY_REMOTE_MOCK(self);
  mock->gmock->UpdateDeviceAddress(self, device_addr);
}

void AddFeature(EntityRemoteObject* self, FeatureRemoteObject* feature) {
  EntityRemoteMock* const mock = ENTITY_REMOTE_MOCK(self);
  mock->gmock->AddFeature(self, feature);
}

void RemoveAllFeatures(EntityRemoteObject* self) {
  EntityRemoteMock* const mock = ENTITY_REMOTE_MOCK(self);
  mock->gmock->RemoveAllFeatures(self);
}

FeatureRemoteObject*
GetFeatureWithTypeAndRole(const EntityRemoteObject* self, FeatureTypeType feature_type, RoleType role) {
  EntityRemoteMock* const mock = ENTITY_REMOTE_MOCK(self);
  return mock->gmock->GetFeatureWithTypeAndRole(self, feature_type, role);
}

FeatureRemoteObject* GetFeatureWithId(const EntityRemoteObject* self, const uint32_t* feature_id) {
  EntityRemoteMock* const mock = ENTITY_REMOTE_MOCK(self);
  return mock->gmock->GetFeatureWithId(self, feature_id);
}

const Vector* GetFeatures(const EntityRemoteObject* self) {
  EntityRemoteMock* const mock = ENTITY_REMOTE_MOCK(self);
  return mock->gmock->GetFeatures(self);
}

bool HasUseCaseSupport(const EntityRemoteObject* self, const UseCaseFilterType* use_case_filter) {
  EntityRemoteMock* const mock = ENTITY_REMOTE_MOCK(self);
  return mock->gmock->HasUseCaseSupport(self, use_case_filter);
}
