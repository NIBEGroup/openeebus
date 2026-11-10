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
 * @brief Binding Manager Mock "class"
 */

#ifndef TESTS_UNIT_TESTS_MOCKS_SPINE_BINDING_BINDING_MANAGER_MOCK_H_
#define TESTS_UNIT_TESTS_MOCKS_SPINE_BINDING_BINDING_MANAGER_MOCK_H_

#include <gmock/gmock.h>

#include <memory>

#include "src/common/eebus_malloc.h"
#include "src/spine/api/binding_manager_interface.h"

class BindingManagerGMockInterface {
 public:
  virtual ~BindingManagerGMockInterface() {};
  virtual void Destruct(BindingManagerObject* self) = 0;
  virtual EebusError AddBinding(
      BindingManagerObject* self,
      DeviceRemoteObject* remote_device,
      const BindingManagementRequestCallType* data
  ) = 0;
  virtual EebusError RemoveBinding(
      BindingManagerObject* self,
      const BindingManagementDeleteCallType* data,
      DeviceRemoteObject* remote_device
  )                                                                                                = 0;
  virtual void RemoveDeviceBindings(BindingManagerObject* self, DeviceRemoteObject* remote_device) = 0;
  virtual void RemoveEntityBindings(BindingManagerObject* self, EntityRemoteObject* remote_entity) = 0;
  virtual bool HasBinding(
      const BindingManagerObject* self,
      const FeatureAddressType* local_addr,
      const FeatureAddressType* remote_addr
  ) = 0;
  virtual NodeManagementBindingDataType* CreateBindingData(
      const BindingManagerObject* self,
      const DeviceRemoteObject* remote_device
  ) = 0;
};

class BindingManagerGMock : public BindingManagerGMockInterface {
 public:
  virtual ~BindingManagerGMock() {};
  MOCK_METHOD1(Destruct, void(BindingManagerObject*));
  MOCK_METHOD3(
      AddBinding,
      EebusError(BindingManagerObject*, DeviceRemoteObject*, const BindingManagementRequestCallType*)
  );
  MOCK_METHOD3(
      RemoveBinding,
      EebusError(BindingManagerObject*, const BindingManagementDeleteCallType*, DeviceRemoteObject*)
  );
  MOCK_METHOD2(RemoveDeviceBindings, void(BindingManagerObject*, DeviceRemoteObject*));
  MOCK_METHOD2(RemoveEntityBindings, void(BindingManagerObject*, EntityRemoteObject*));
  MOCK_METHOD3(HasBinding, bool(const BindingManagerObject*, const FeatureAddressType*, const FeatureAddressType*));
  MOCK_METHOD2(
      CreateBindingData,
      NodeManagementBindingDataType*(const BindingManagerObject*, const DeviceRemoteObject*)
  );
};

typedef struct BindingManagerMock {
  /** Implements the Binding Manager Interface */
  BindingManagerObject obj;
  BindingManagerGMock* gmock;
} BindingManagerMock;

#define BINDING_MANAGER_MOCK(obj) ((BindingManagerMock*)(obj))

BindingManagerMock* BindingManagerMockCreate(void);

static inline void BindingManagerMockDelete(BindingManagerMock* self) {
  if (self != nullptr) {
    BINDING_MANAGER_DESTRUCT(BINDING_MANAGER_OBJECT(self));
    EEBUS_FREE(self);
  }
}

#endif  // TESTS_UNIT_TESTS_MOCKS_SPINE_BINDING_BINDING_MANAGER_MOCK_H_
