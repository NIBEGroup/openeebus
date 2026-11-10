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
 * @brief Binding Manager mock implementation
 */

#include "binding_manager_mock.h"

#include <gmock/gmock.h>

#include "src/spine/api/binding_manager_interface.h"

static void Destruct(BindingManagerObject* self);
static EebusError
AddBinding(BindingManagerObject* self, DeviceRemoteObject* remote_device, const BindingManagementRequestCallType* data);
static EebusError RemoveBinding(
    BindingManagerObject* self,
    const BindingManagementDeleteCallType* data,
    DeviceRemoteObject* remote_device
);
static void RemoveDeviceBindings(BindingManagerObject* self, DeviceRemoteObject* remote_device);
static void RemoveEntityBindings(BindingManagerObject* self, EntityRemoteObject* remote_entity);
static bool HasBinding(
    const BindingManagerObject* self,
    const FeatureAddressType* local_addr,
    const FeatureAddressType* remote_addr
);
static NodeManagementBindingDataType*
CreateBindingData(const BindingManagerObject* self, const DeviceRemoteObject* remote_device);

static const BindingManagerInterface binding_manager_methods = {
    .destruct               = Destruct,
    .add_binding            = AddBinding,
    .remove_binding         = RemoveBinding,
    .remove_device_bindings = RemoveDeviceBindings,
    .remove_entity_bindings = RemoveEntityBindings,
    .has_binding            = HasBinding,
    .create_binding_data    = CreateBindingData,
};

static EebusError BindingManagerMockConstruct(BindingManagerMock* self);

EebusError BindingManagerMockConstruct(BindingManagerMock* self) {
  // Override "virtual functions table"
  BINDING_MANAGER_INTERFACE(self) = &binding_manager_methods;

  self->gmock = new BindingManagerGMock();
  if (self->gmock == nullptr) {
    return kEebusErrorMemoryAllocate;
  }

  return kEebusErrorOk;
}

BindingManagerMock* BindingManagerMockCreate(void) {
  BindingManagerMock* const mock = (BindingManagerMock*)EEBUS_MALLOC(sizeof(BindingManagerMock));
  if (mock == nullptr) {
    return nullptr;
  }

  if (BindingManagerMockConstruct(mock) != kEebusErrorOk) {
    BindingManagerMockDelete(mock);
    return nullptr;
  }

  return mock;
}

void Destruct(BindingManagerObject* self) {
  BindingManagerMock* const mock = BINDING_MANAGER_MOCK(self);
  mock->gmock->Destruct(self);
  delete mock->gmock;
}

EebusError AddBinding(
    BindingManagerObject* self,
    DeviceRemoteObject* remote_device,
    const BindingManagementRequestCallType* data
) {
  BindingManagerMock* const mock = BINDING_MANAGER_MOCK(self);
  return mock->gmock->AddBinding(self, remote_device, data);
}

EebusError RemoveBinding(
    BindingManagerObject* self,
    const BindingManagementDeleteCallType* data,
    DeviceRemoteObject* remote_device
) {
  BindingManagerMock* const mock = BINDING_MANAGER_MOCK(self);
  return mock->gmock->RemoveBinding(self, data, remote_device);
}

void RemoveDeviceBindings(BindingManagerObject* self, DeviceRemoteObject* remote_device) {
  BindingManagerMock* const mock = BINDING_MANAGER_MOCK(self);
  mock->gmock->RemoveDeviceBindings(self, remote_device);
}

void RemoveEntityBindings(BindingManagerObject* self, EntityRemoteObject* remote_entity) {
  BindingManagerMock* const mock = BINDING_MANAGER_MOCK(self);
  mock->gmock->RemoveEntityBindings(self, remote_entity);
}

bool HasBinding(
    const BindingManagerObject* self,
    const FeatureAddressType* local_addr,
    const FeatureAddressType* remote_addr
) {
  BindingManagerMock* const mock = BINDING_MANAGER_MOCK(self);
  return mock->gmock->HasBinding(self, local_addr, remote_addr);
}

NodeManagementBindingDataType*
CreateBindingData(const BindingManagerObject* self, const DeviceRemoteObject* remote_device) {
  BindingManagerMock* const mock = BINDING_MANAGER_MOCK(self);
  return mock->gmock->CreateBindingData(self, remote_device);
}
