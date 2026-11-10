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
 * @brief Use Case mock implementation
 */

#include "use_case_mock.h"

#include <gmock/gmock.h>

#include "src/common/eebus_errors.h"
#include "src/use_case/api/use_case_interface.h"

static void Destruct(UseCaseObject* self);
static bool IsEntityCompatible(const UseCaseObject* self, const EntityRemoteObject* remote_entity);
static bool IsUseCaseCompatible(const UseCaseObject* self, const UseCaseFilterType* use_case_filter);
static EntityRemoteObject*
GetRemoteEntityWithAddress(const UseCaseObject* self, const EntityAddressType* remote_entity_addr);

static const UseCaseInterface use_case_methods = {
    .destruct                       = Destruct,
    .is_entity_compatible           = IsEntityCompatible,
    .is_use_case_compatible         = IsUseCaseCompatible,
    .get_remote_entity_with_address = GetRemoteEntityWithAddress,
};

static EebusError UseCaseMockConstruct(UseCaseMock* self);

EebusError UseCaseMockConstruct(UseCaseMock* self) {
  // Override "virtual functions table"
  USE_CASE_INTERFACE(self) = &use_case_methods;

  self->gmock = new UseCaseGMock();
  if (self->gmock == nullptr) {
    return kEebusErrorMemoryAllocate;
  }

  return kEebusErrorOk;
}

UseCaseMock* UseCaseMockCreate(void) {
  UseCaseMock* const mock = (UseCaseMock*)EEBUS_MALLOC(sizeof(UseCaseMock));
  if (mock == nullptr) {
    return nullptr;
  }

  if (UseCaseMockConstruct(mock) != kEebusErrorOk) {
    UseCaseMockDelete(mock);
    return nullptr;
  }

  return mock;
}

void Destruct(UseCaseObject* self) {
  UseCaseMock* const mock = USE_CASE_MOCK(self);
  mock->gmock->Destruct(self);
  delete mock->gmock;
}

bool IsEntityCompatible(const UseCaseObject* self, const EntityRemoteObject* remote_entity) {
  UseCaseMock* const mock = USE_CASE_MOCK(self);
  return mock->gmock->IsEntityCompatible(self, remote_entity);
}

bool IsUseCaseCompatible(const UseCaseObject* self, const UseCaseFilterType* use_case_filter) {
  UseCaseMock* const mock = USE_CASE_MOCK(self);
  return mock->gmock->IsUseCaseCompatible(self, use_case_filter);
}

EntityRemoteObject* GetRemoteEntityWithAddress(const UseCaseObject* self, const EntityAddressType* remote_entity_addr) {
  UseCaseMock* const mock = USE_CASE_MOCK(self);
  return mock->gmock->GetRemoteEntityWithAddress(self, remote_entity_addr);
}
