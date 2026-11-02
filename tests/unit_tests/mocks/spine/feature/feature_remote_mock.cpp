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
 * @brief Feature Remote mock implementation
 */

#include "feature_remote_mock.h"

#include <gmock/gmock.h>

#include "src/spine/api/feature_remote_interface.h"

static void Destruct(FeatureObject* self);
static const FeatureAddressType* GetAddress(const FeatureObject* self);
static FeatureTypeType GetType(const FeatureObject* self);
static RoleType GetRole(const FeatureObject* self);
static const OperationsObject* GetFunctionOperations(const FeatureObject* self, FunctionType fcn_type);
static const char* GetDescription(const FeatureObject* self);
static void SetDescription(FeatureObject* self, const char* description);
static const char* ToString(const FeatureObject* self);
static DeviceRemoteObject* GetDevice(const FeatureRemoteObject* self);
static EntityRemoteObject* GetEntity(const FeatureRemoteObject* self);
static const void* GetData(const FeatureRemoteObject* self, FunctionType function_type);
static void* DataCopy(const FeatureRemoteObject* self, FunctionType fcn_type);
static EebusError UpdateData(
    FeatureRemoteObject* self,
    FunctionType function_type,
    const void* new_data,
    const FilterType* filter_partial,
    const FilterType* filter_delete,
    bool persist
);
static void SetFunctionOperations(
    FeatureRemoteObject* self,
    const FunctionPropertyType* const* supported_functions,
    size_t supported_functions_size
);
static void SetMaxResponseDelay(FeatureRemoteObject* self, uint32_t max_delay);
static uint32_t GetMaxResponseDelay(const FeatureRemoteObject* self);

static const FeatureRemoteInterface feature_remote_methods = {
    .feature_interface = {
        .destruct                = Destruct,
        .get_address             = GetAddress,
        .get_type                = GetType,
        .get_role                = GetRole,
        .get_function_operations = GetFunctionOperations,
        .get_description         = GetDescription,
        .set_description         = SetDescription,
        .to_string               = ToString,
    },

    .get_device              = GetDevice,
    .get_entity              = GetEntity,
    .get_data                = GetData,
    .data_copy               = DataCopy,
    .update_data             = UpdateData,
    .set_function_operations = SetFunctionOperations,
    .set_max_response_delay  = SetMaxResponseDelay,
    .get_max_response_delay  = GetMaxResponseDelay,
};

static EebusError FeatureRemoteMockConstruct(FeatureRemoteMock* self);

EebusError FeatureRemoteMockConstruct(FeatureRemoteMock* self) {
  // Override "virtual functions table"
  FEATURE_REMOTE_INTERFACE(self) = &feature_remote_methods;

  self->gmock = new FeatureRemoteGMock();
  if (self->gmock == nullptr) {
    return kEebusErrorMemoryAllocate;
  }

  return kEebusErrorOk;
}

FeatureRemoteMock* FeatureRemoteMockCreate(void) {
  FeatureRemoteMock* const mock = (FeatureRemoteMock*)EEBUS_MALLOC(sizeof(FeatureRemoteMock));
  if (mock == nullptr) {
    return nullptr;
  }

  if (FeatureRemoteMockConstruct(mock) != kEebusErrorOk) {
    FeatureRemoteMockDelete(mock);
    return nullptr;
  }

  return mock;
}

void Destruct(FeatureObject* self) {
  FeatureRemoteMock* const mock = FEATURE_REMOTE_MOCK(self);
  mock->gmock->Destruct(self);
  delete mock->gmock;
}

const FeatureAddressType* GetAddress(const FeatureObject* self) {
  FeatureRemoteMock* const mock = FEATURE_REMOTE_MOCK(self);
  return mock->gmock->GetAddress(self);
}

FeatureTypeType GetType(const FeatureObject* self) {
  FeatureRemoteMock* const mock = FEATURE_REMOTE_MOCK(self);
  return mock->gmock->GetType(self);
}

RoleType GetRole(const FeatureObject* self) {
  FeatureRemoteMock* const mock = FEATURE_REMOTE_MOCK(self);
  return mock->gmock->GetRole(self);
}

const OperationsObject* GetFunctionOperations(const FeatureObject* self, FunctionType fcn_type) {
  FeatureRemoteMock* const mock = FEATURE_REMOTE_MOCK(self);
  return mock->gmock->GetFunctionOperations(self, fcn_type);
}

const char* GetDescription(const FeatureObject* self) {
  FeatureRemoteMock* const mock = FEATURE_REMOTE_MOCK(self);
  return mock->gmock->GetDescription(self);
}

void SetDescription(FeatureObject* self, const char* description) {
  FeatureRemoteMock* const mock = FEATURE_REMOTE_MOCK(self);
  mock->gmock->SetDescription(self, description);
}

const char* ToString(const FeatureObject* self) {
  FeatureRemoteMock* const mock = FEATURE_REMOTE_MOCK(self);
  return mock->gmock->ToString(self);
}

DeviceRemoteObject* GetDevice(const FeatureRemoteObject* self) {
  FeatureRemoteMock* const mock = FEATURE_REMOTE_MOCK(self);
  return mock->gmock->GetDevice(self);
}

EntityRemoteObject* GetEntity(const FeatureRemoteObject* self) {
  FeatureRemoteMock* const mock = FEATURE_REMOTE_MOCK(self);
  return mock->gmock->GetEntity(self);
}

const void* GetData(const FeatureRemoteObject* self, FunctionType function_type) {
  FeatureRemoteMock* const mock = FEATURE_REMOTE_MOCK(self);
  return mock->gmock->GetData(self, function_type);
}

void* DataCopy(const FeatureRemoteObject* self, FunctionType fcn_type) {
  FeatureRemoteMock* const mock = FEATURE_REMOTE_MOCK(self);
  return mock->gmock->DataCopy(self, fcn_type);
}

EebusError UpdateData(
    FeatureRemoteObject* self,
    FunctionType function_type,
    const void* new_data,
    const FilterType* filter_partial,
    const FilterType* filter_delete,
    bool persist
) {
  FeatureRemoteMock* const mock = FEATURE_REMOTE_MOCK(self);
  return mock->gmock->UpdateData(self, function_type, new_data, filter_partial, filter_delete, persist);
}

void SetFunctionOperations(
    FeatureRemoteObject* self,
    const FunctionPropertyType* const* supported_functions,
    size_t supported_functions_size
) {
  FeatureRemoteMock* const mock = FEATURE_REMOTE_MOCK(self);
  mock->gmock->SetFunctionOperations(self, supported_functions, supported_functions_size);
}

void SetMaxResponseDelay(FeatureRemoteObject* self, uint32_t max_delay) {
  FeatureRemoteMock* const mock = FEATURE_REMOTE_MOCK(self);
  mock->gmock->SetMaxResponseDelay(self, max_delay);
}

uint32_t GetMaxResponseDelay(const FeatureRemoteObject* self) {
  FeatureRemoteMock* const mock = FEATURE_REMOTE_MOCK(self);
  return mock->gmock->GetMaxResponseDelay(self);
}
