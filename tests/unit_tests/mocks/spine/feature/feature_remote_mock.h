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
 * @brief Feature Remote Mock "class"
 */

#ifndef TESTS_UNIT_TESTS_MOCKS_SPINE_FEATURE_FEATURE_REMOTE_MOCK_H_
#define TESTS_UNIT_TESTS_MOCKS_SPINE_FEATURE_FEATURE_REMOTE_MOCK_H_

#include <gmock/gmock.h>

#include <memory>

#include "feature_mock.h"
#include "src/spine/api/feature_remote_interface.h"

class FeatureRemoteGMockInterface : public FeatureGMockInterface {
 public:
  virtual ~FeatureRemoteGMockInterface() {};
  virtual DeviceRemoteObject* GetDevice(const FeatureRemoteObject* self)                   = 0;
  virtual EntityRemoteObject* GetEntity(const FeatureRemoteObject* self)                   = 0;
  virtual const void* GetData(const FeatureRemoteObject* self, FunctionType function_type) = 0;
  virtual void* DataCopy(const FeatureRemoteObject* self, FunctionType fcn_type)           = 0;
  virtual EebusError UpdateData(
      FeatureRemoteObject* self,
      FunctionType function_type,
      const void* new_data,
      const FilterType* filter_partial,
      const FilterType* filter_delete,
      bool persist
  ) = 0;
  virtual void SetFunctionOperations(
      FeatureRemoteObject* self,
      const FunctionPropertyType* const* supported_functions,
      size_t supported_functions_size
  )                                                                               = 0;
  virtual void SetMaxResponseDelay(FeatureRemoteObject* self, uint32_t max_delay) = 0;
  virtual uint32_t GetMaxResponseDelay(const FeatureRemoteObject* self)           = 0;
};

class FeatureRemoteGMock : public FeatureRemoteGMockInterface {
 public:
  virtual ~FeatureRemoteGMock() {};
  MOCK_METHOD1(Destruct, void(FeatureObject*));
  MOCK_METHOD1(GetAddress, const FeatureAddressType*(const FeatureObject*));
  MOCK_METHOD1(GetType, FeatureTypeType(const FeatureObject*));
  MOCK_METHOD1(GetRole, RoleType(const FeatureObject*));
  MOCK_METHOD2(GetFunctionOperations, const OperationsObject*(const FeatureObject*, FunctionType));
  MOCK_METHOD1(GetDescription, const char*(const FeatureObject*));
  MOCK_METHOD2(SetDescription, void(FeatureObject*, const char*));
  MOCK_METHOD1(ToString, const char*(const FeatureObject*));
  MOCK_METHOD1(GetDevice, DeviceRemoteObject*(const FeatureRemoteObject*));
  MOCK_METHOD1(GetEntity, EntityRemoteObject*(const FeatureRemoteObject*));
  MOCK_METHOD2(GetData, const void*(const FeatureRemoteObject*, FunctionType));
  MOCK_METHOD2(DataCopy, void*(const FeatureRemoteObject*, FunctionType));
  MOCK_METHOD6(
      UpdateData,
      EebusError(FeatureRemoteObject*, FunctionType, const void*, const FilterType*, const FilterType*, bool)
  );
  MOCK_METHOD3(SetFunctionOperations, void(FeatureRemoteObject*, const FunctionPropertyType* const*, size_t));
  MOCK_METHOD2(SetMaxResponseDelay, void(FeatureRemoteObject*, uint32_t));
  MOCK_METHOD1(GetMaxResponseDelay, uint32_t(const FeatureRemoteObject*));
};

typedef struct FeatureRemoteMock {
  /** Implements the Feature Remote Interface */
  FeatureRemoteObject obj;
  FeatureRemoteGMock* gmock;
} FeatureRemoteMock;

#define FEATURE_REMOTE_MOCK(obj) ((FeatureRemoteMock*)(obj))

FeatureRemoteMock* FeatureRemoteMockCreate(void);

static inline void FeatureRemoteMockDelete(FeatureRemoteMock* self) {
  if (self != nullptr) {
    FEATURE_DESTRUCT(FEATURE_OBJECT(self));
    EEBUS_FREE(self);
  }
}

#endif  // TESTS_UNIT_TESTS_MOCKS_SPINE_FEATURE_FEATURE_REMOTE_MOCK_H_
