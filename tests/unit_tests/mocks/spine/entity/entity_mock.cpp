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
 * @brief Entity mock implementation
 */

#include "entity_mock.h"

#include <gmock/gmock.h>

#include "src/spine/api/entity_interface.h"

static void Destruct(EntityObject* self);
static const EntityAddressType* GetAddress(const EntityObject* self);
static EntityTypeType GetType(const EntityObject* self);
static const char* GetDescription(const EntityObject* self);
static void SetDescription(EntityObject* self, const char* description);
static uint32_t GetNextFeatureId(EntityObject* self);

static const EntityInterface entity_methods = {
    .destruct            = Destruct,
    .get_address         = GetAddress,
    .get_type            = GetType,
    .get_description     = GetDescription,
    .set_description     = SetDescription,
    .get_next_feature_id = GetNextFeatureId,
};

static EebusError EntityMockConstruct(EntityMock* self);

EebusError EntityMockConstruct(EntityMock* self) {
  // Override "virtual functions table"
  ENTITY_INTERFACE(self) = &entity_methods;

  self->gmock = new EntityGMock();
  if (self->gmock == nullptr) {
    return kEebusErrorMemoryAllocate;
  }

  return kEebusErrorOk;
}

EntityMock* EntityMockCreate(void) {
  EntityMock* const mock = (EntityMock*)EEBUS_MALLOC(sizeof(EntityMock));
  if (mock == nullptr) {
    return nullptr;
  }

  if (EntityMockConstruct(mock) != kEebusErrorOk) {
    EntityMockDelete(mock);
    return nullptr;
  }

  return mock;
}

void Destruct(EntityObject* self) {
  EntityMock* const mock = ENTITY_MOCK(self);
  mock->gmock->Destruct(self);
  delete mock->gmock;
}

const EntityAddressType* GetAddress(const EntityObject* self) {
  EntityMock* const mock = ENTITY_MOCK(self);
  return mock->gmock->GetAddress(self);
}

EntityTypeType GetType(const EntityObject* self) {
  EntityMock* const mock = ENTITY_MOCK(self);
  return mock->gmock->GetType(self);
}

const char* GetDescription(const EntityObject* self) {
  EntityMock* const mock = ENTITY_MOCK(self);
  return mock->gmock->GetDescription(self);
}

void SetDescription(EntityObject* self, const char* description) {
  EntityMock* const mock = ENTITY_MOCK(self);
  mock->gmock->SetDescription(self, description);
}

uint32_t GetNextFeatureId(EntityObject* self) {
  EntityMock* const mock = ENTITY_MOCK(self);
  return mock->gmock->GetNextFeatureId(self);
}
