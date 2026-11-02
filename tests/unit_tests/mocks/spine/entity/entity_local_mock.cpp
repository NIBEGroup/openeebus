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
 * @brief Entity Local mock implementation
 */

#include "entity_local_mock.h"

#include <gmock/gmock.h>

#include "src/spine/api/entity_local_interface.h"

static void Destruct(EntityObject* self);
static const EntityAddressType* GetAddress(const EntityObject* self);
static EntityTypeType GetType(const EntityObject* self);
static const char* GetDescription(const EntityObject* self);
static void SetDescription(EntityObject* self, const char* description);
static uint32_t GetNextFeatureId(EntityObject* self);
static DeviceLocalObject* GetDevice(const EntityLocalObject* self);
static HeartbeatManagerObject* GetHeartbeatManager(const EntityLocalObject* self);
static void AddFeature(EntityLocalObject* self, FeatureLocalObject* feature);
static FeatureLocalObject*
GetFeatureWithTypeAndRole(const EntityLocalObject* self, FeatureTypeType feature_type, RoleType role);
static FeatureLocalObject*
AddFeatureWithTypeAndRole(EntityLocalObject* self, FeatureTypeType feature_type, RoleType role);
static FeatureLocalObject* GetFeatureWithId(const EntityLocalObject* self, const uint32_t* feature_id);
static const Vector* GetFeatures(const EntityLocalObject* self);
static void AddUseCaseSupport(
    EntityLocalObject* self,
    UseCaseActorType actor,
    UseCaseNameType use_case_name_id,
    SpecificationVersionType version,
    const char* sub_revision,
    bool available,
    const UseCaseScenarioSupportType* scenarios,
    size_t scenarios_size
);
static bool HasUseCaseSupport(const EntityLocalObject* self, const UseCaseFilterType* use_case_filter);
static void SetUseCaseAvailability(EntityLocalObject* self, const UseCaseFilterType* use_case_filter, bool available);
static void
RemoveUseCaseSupports(EntityLocalObject* self, const UseCaseFilterType* use_case_filters, size_t use_case_filters_size);
static void RemoveAllUseCaseSupports(EntityLocalObject* self);
static void RemoveAllSubscriptions(EntityLocalObject* self);
static void RemoveAllBindings(EntityLocalObject* self);
static NodeManagementDetailedDiscoveryEntityInformationType* CreateInformation(const EntityLocalObject* self);
static void Tick(EntityLocalObject* self);

static const EntityLocalInterface entity_local_methods = {
    .entity_interface = {
        .destruct            = Destruct,
        .get_address         = GetAddress,
        .get_type            = GetType,
        .get_description     = GetDescription,
        .set_description     = SetDescription,
        .get_next_feature_id = GetNextFeatureId,
    },

    .get_device                     = GetDevice,
    .get_heartbeat_manager          = GetHeartbeatManager,
    .add_feature                    = AddFeature,
    .get_feature_with_type_and_role = GetFeatureWithTypeAndRole,
    .add_feature_with_type_and_role = AddFeatureWithTypeAndRole,
    .get_feature_with_id            = GetFeatureWithId,
    .get_features                   = GetFeatures,
    .add_use_case_support           = AddUseCaseSupport,
    .has_use_case_support           = HasUseCaseSupport,
    .set_use_case_availability      = SetUseCaseAvailability,
    .remove_use_case_supports       = RemoveUseCaseSupports,
    .remove_all_use_case_supports   = RemoveAllUseCaseSupports,
    .remove_all_subscriptions       = RemoveAllSubscriptions,
    .remove_all_bindings            = RemoveAllBindings,
    .create_information             = CreateInformation,
    .tick                           = Tick,
};

static EebusError EntityLocalMockConstruct(EntityLocalMock* self);

EebusError EntityLocalMockConstruct(EntityLocalMock* self) {
  // Override "virtual functions table"
  ENTITY_LOCAL_INTERFACE(self) = &entity_local_methods;

  self->gmock = new EntityLocalGMock();
  if (self->gmock == nullptr) {
    return kEebusErrorMemoryAllocate;
  }

  return kEebusErrorOk;
}

EntityLocalMock* EntityLocalMockCreate(void) {
  EntityLocalMock* const mock = (EntityLocalMock*)EEBUS_MALLOC(sizeof(EntityLocalMock));
  if (mock == nullptr) {
    return nullptr;
  }

  if (EntityLocalMockConstruct(mock) != kEebusErrorOk) {
    EntityLocalMockDelete(mock);
    return nullptr;
  }

  return mock;
}

void Destruct(EntityObject* self) {
  EntityLocalMock* const mock = ENTITY_LOCAL_MOCK(self);
  mock->gmock->Destruct(self);
  delete mock->gmock;
}

const EntityAddressType* GetAddress(const EntityObject* self) {
  EntityLocalMock* const mock = ENTITY_LOCAL_MOCK(self);
  return mock->gmock->GetAddress(self);
}

EntityTypeType GetType(const EntityObject* self) {
  EntityLocalMock* const mock = ENTITY_LOCAL_MOCK(self);
  return mock->gmock->GetType(self);
}

const char* GetDescription(const EntityObject* self) {
  EntityLocalMock* const mock = ENTITY_LOCAL_MOCK(self);
  return mock->gmock->GetDescription(self);
}

void SetDescription(EntityObject* self, const char* description) {
  EntityLocalMock* const mock = ENTITY_LOCAL_MOCK(self);
  mock->gmock->SetDescription(self, description);
}

uint32_t GetNextFeatureId(EntityObject* self) {
  EntityLocalMock* const mock = ENTITY_LOCAL_MOCK(self);
  return mock->gmock->GetNextFeatureId(self);
}

DeviceLocalObject* GetDevice(const EntityLocalObject* self) {
  EntityLocalMock* const mock = ENTITY_LOCAL_MOCK(self);
  return mock->gmock->GetDevice(self);
}

HeartbeatManagerObject* GetHeartbeatManager(const EntityLocalObject* self) {
  EntityLocalMock* const mock = ENTITY_LOCAL_MOCK(self);
  return mock->gmock->GetHeartbeatManager(self);
}

void AddFeature(EntityLocalObject* self, FeatureLocalObject* feature) {
  EntityLocalMock* const mock = ENTITY_LOCAL_MOCK(self);
  mock->gmock->AddFeature(self, feature);
}

FeatureLocalObject*
GetFeatureWithTypeAndRole(const EntityLocalObject* self, FeatureTypeType feature_type, RoleType role) {
  EntityLocalMock* const mock = ENTITY_LOCAL_MOCK(self);
  return mock->gmock->GetFeatureWithTypeAndRole(self, feature_type, role);
}

FeatureLocalObject* AddFeatureWithTypeAndRole(EntityLocalObject* self, FeatureTypeType feature_type, RoleType role) {
  EntityLocalMock* const mock = ENTITY_LOCAL_MOCK(self);
  return mock->gmock->AddFeatureWithTypeAndRole(self, feature_type, role);
}

FeatureLocalObject* GetFeatureWithId(const EntityLocalObject* self, const uint32_t* feature_id) {
  EntityLocalMock* const mock = ENTITY_LOCAL_MOCK(self);
  return mock->gmock->GetFeatureWithId(self, feature_id);
}

const Vector* GetFeatures(const EntityLocalObject* self) {
  EntityLocalMock* const mock = ENTITY_LOCAL_MOCK(self);
  return mock->gmock->GetFeatures(self);
}

void AddUseCaseSupport(
    EntityLocalObject* self,
    UseCaseActorType actor,
    UseCaseNameType use_case_name_id,
    SpecificationVersionType version,
    const char* sub_revision,
    bool available,
    const UseCaseScenarioSupportType* scenarios,
    size_t scenarios_size
) {
  EntityLocalMock* const mock = ENTITY_LOCAL_MOCK(self);
  mock->gmock
      ->AddUseCaseSupport(self, actor, use_case_name_id, version, sub_revision, available, scenarios, scenarios_size);
}

bool HasUseCaseSupport(const EntityLocalObject* self, const UseCaseFilterType* use_case_filter) {
  EntityLocalMock* const mock = ENTITY_LOCAL_MOCK(self);
  return mock->gmock->HasUseCaseSupport(self, use_case_filter);
}

void SetUseCaseAvailability(EntityLocalObject* self, const UseCaseFilterType* use_case_filter, bool available) {
  EntityLocalMock* const mock = ENTITY_LOCAL_MOCK(self);
  mock->gmock->SetUseCaseAvailability(self, use_case_filter, available);
}

void RemoveUseCaseSupports(
    EntityLocalObject* self,
    const UseCaseFilterType* use_case_filters,
    size_t use_case_filters_size
) {
  EntityLocalMock* const mock = ENTITY_LOCAL_MOCK(self);
  mock->gmock->RemoveUseCaseSupports(self, use_case_filters, use_case_filters_size);
}

void RemoveAllUseCaseSupports(EntityLocalObject* self) {
  EntityLocalMock* const mock = ENTITY_LOCAL_MOCK(self);
  mock->gmock->RemoveAllUseCaseSupports(self);
}

void RemoveAllSubscriptions(EntityLocalObject* self) {
  EntityLocalMock* const mock = ENTITY_LOCAL_MOCK(self);
  mock->gmock->RemoveAllSubscriptions(self);
}

void RemoveAllBindings(EntityLocalObject* self) {
  EntityLocalMock* const mock = ENTITY_LOCAL_MOCK(self);
  mock->gmock->RemoveAllBindings(self);
}

NodeManagementDetailedDiscoveryEntityInformationType* CreateInformation(const EntityLocalObject* self) {
  EntityLocalMock* const mock = ENTITY_LOCAL_MOCK(self);
  return mock->gmock->CreateInformation(self);
}

void Tick(EntityLocalObject* self) {
  EntityLocalMock* const mock = ENTITY_LOCAL_MOCK(self);
  mock->gmock->Tick(self);
}
