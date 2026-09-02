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
 * @brief Feature Local Mock "class"
 */

#ifndef TESTS_SRC_MOCKS_SPINE_FEATURE_FEATURE_LOCAL_MOCK_H_
#define TESTS_SRC_MOCKS_SPINE_FEATURE_FEATURE_LOCAL_MOCK_H_

#include <gmock/gmock.h>

#include <memory>

#include "feature_mock.h"
#include "src/spine/api/feature_local_interface.h"

class FeatureLocalGMockInterface : public FeatureGMockInterface {
 public:
  virtual ~FeatureLocalGMockInterface() {};
  virtual DeviceLocalObject* GetDevice(const FeatureLocalObject* self)                                       = 0;
  virtual EntityLocalObject* GetEntity(const FeatureLocalObject* self)                                       = 0;
  virtual const void* GetData(const FeatureLocalObject* self, FunctionType function_type)                    = 0;
  virtual void SetFunctionOperations(FeatureLocalObject* self, FunctionType type, bool read, bool write)     = 0;
  virtual EebusError AddWriteApprovalCallback(FeatureLocalObject* self, WriteApprovalCallback cb, void* ctx) = 0;
  virtual EebusError TryApproveWrite(FeatureLocalObject* self, const char* ski, MsgCounterType msg_cnt)      = 0;
  virtual EebusError DenyWrite(FeatureLocalObject* self, const char* ski, MsgCounterType msg_cnt, const ErrorType* err)
      = 0;
  virtual void CleanRemoteDeviceCaches(FeatureLocalObject* self, const DeviceAddressType* remote_addr) = 0;
  virtual void* DataCopy(const FeatureLocalObject* self, FunctionType function_type)                   = 0;
  virtual EebusError UpdateData(
      FeatureLocalObject* self,
      FunctionType fcn_type,
      const void* data,
      const FilterType* filter_partial,
      const FilterType* filter_delete
  )                                                                                                                = 0;
  virtual void SetData(FeatureLocalObject* self, FunctionType function_type, void* data)                           = 0;
  virtual bool HasSubscriptionToRemote(const FeatureLocalObject* self, const FeatureAddressType* remote_addr)      = 0;
  virtual EebusError SubscribeToRemote(FeatureLocalObject* self, const FeatureAddressType* remote_addr)            = 0;
  virtual EebusError RemoveRemoteSubscription(FeatureLocalObject* self, const FeatureAddressType* remote_addr)     = 0;
  virtual void RemoveAllRemoteSubscriptions(FeatureLocalObject* self)                                              = 0;
  virtual bool HasBindingToRemote(const FeatureLocalObject* self, const FeatureAddressType* remote_addr)           = 0;
  virtual EebusError BindToRemote(FeatureLocalObject* self, const FeatureAddressType* remote_addr)                 = 0;
  virtual EebusError RemoveRemoteBinding(FeatureLocalObject* self, const FeatureAddressType* remote_addr)          = 0;
  virtual void RemoveAllRemoteBindings(FeatureLocalObject* self)                                                   = 0;
  virtual EebusError HandleMessage(FeatureLocalObject* self, const Message* msg)                                   = 0;
  virtual NodeManagementDetailedDiscoveryFeatureInformationType* CreateInformation(const FeatureLocalObject* self) = 0;
  virtual void Tick(FeatureLocalObject* self)                                                                      = 0;
  virtual EebusError WriteToRemote(
      FeatureLocalObject* self,
      FeatureRemoteObject* dest_feature,
      FunctionType fcn_type,
      const void* data,
      const FilterType* filter_partial,
      const FilterType* filter_delete,
      ResultMessageCallback cb,
      void* ctx
  ) = 0;
  virtual EebusError ReadFromRemote(
      FeatureLocalObject* self,
      FeatureRemoteObject* dest_feature,
      FunctionType function_type,
      const void* selectors,
      const void* elements,
      ReplyMessageCallback cb,
      void* ctx
  ) = 0;
};

class FeatureLocalGMock : public FeatureLocalGMockInterface {
 public:
  virtual ~FeatureLocalGMock() {};
  MOCK_METHOD1(Destruct, void(FeatureObject*));
  MOCK_METHOD1(GetAddress, const FeatureAddressType*(const FeatureObject*));
  MOCK_METHOD1(GetType, FeatureTypeType(const FeatureObject*));
  MOCK_METHOD1(GetRole, RoleType(const FeatureObject*));
  MOCK_METHOD2(GetFunctionOperations, const OperationsObject*(const FeatureObject*, FunctionType));
  MOCK_METHOD1(GetDescription, const char*(const FeatureObject*));
  MOCK_METHOD2(SetDescription, void(FeatureObject*, const char*));
  MOCK_METHOD1(ToString, const char*(const FeatureObject*));
  MOCK_METHOD1(GetDevice, DeviceLocalObject*(const FeatureLocalObject*));
  MOCK_METHOD1(GetEntity, EntityLocalObject*(const FeatureLocalObject*));
  MOCK_METHOD2(GetData, const void*(const FeatureLocalObject*, FunctionType));
  MOCK_METHOD4(SetFunctionOperations, void(FeatureLocalObject*, FunctionType, bool, bool));
  MOCK_METHOD3(AddWriteApprovalCallback, EebusError(FeatureLocalObject*, WriteApprovalCallback, void*));
  MOCK_METHOD3(TryApproveWrite, EebusError(FeatureLocalObject*, const char*, MsgCounterType));
  MOCK_METHOD4(DenyWrite, EebusError(FeatureLocalObject*, const char*, MsgCounterType, const ErrorType*));
  MOCK_METHOD2(CleanRemoteDeviceCaches, void(FeatureLocalObject*, const DeviceAddressType*));
  MOCK_METHOD2(DataCopy, void*(const FeatureLocalObject*, FunctionType));
  MOCK_METHOD5(
      UpdateData,
      EebusError(FeatureLocalObject*, FunctionType, const void*, const FilterType*, const FilterType*)
  );
  MOCK_METHOD3(SetData, void(FeatureLocalObject*, FunctionType, void*));
  MOCK_METHOD2(HasSubscriptionToRemote, bool(const FeatureLocalObject*, const FeatureAddressType*));
  MOCK_METHOD2(SubscribeToRemote, EebusError(FeatureLocalObject*, const FeatureAddressType*));
  MOCK_METHOD2(RemoveRemoteSubscription, EebusError(FeatureLocalObject*, const FeatureAddressType*));
  MOCK_METHOD1(RemoveAllRemoteSubscriptions, void(FeatureLocalObject*));
  MOCK_METHOD2(HasBindingToRemote, bool(const FeatureLocalObject*, const FeatureAddressType*));
  MOCK_METHOD2(BindToRemote, EebusError(FeatureLocalObject*, const FeatureAddressType*));
  MOCK_METHOD2(RemoveRemoteBinding, EebusError(FeatureLocalObject*, const FeatureAddressType*));
  MOCK_METHOD1(RemoveAllRemoteBindings, void(FeatureLocalObject*));
  MOCK_METHOD2(HandleMessage, EebusError(FeatureLocalObject*, const Message*));
  MOCK_METHOD1(CreateInformation, NodeManagementDetailedDiscoveryFeatureInformationType*(const FeatureLocalObject*));
  MOCK_METHOD1(Tick, void(FeatureLocalObject*));
  MOCK_METHOD8(
      WriteToRemote,
      EebusError(FeatureLocalObject*, FeatureRemoteObject*, FunctionType, const void*, const FilterType*, const FilterType*, ResultMessageCallback, void*)
  );
  MOCK_METHOD7(
      ReadFromRemote,
      EebusError(FeatureLocalObject*, FeatureRemoteObject*, FunctionType, const void*, const void*, ReplyMessageCallback, void*)
  );
};

typedef struct FeatureLocalMock {
  /** Implements the Feature Local Interface */
  FeatureLocalObject obj;
  FeatureLocalGMock* gmock;
} FeatureLocalMock;

#define FEATURE_LOCAL_MOCK(obj) ((FeatureLocalMock*)(obj))

FeatureLocalMock* FeatureLocalMockCreate(void);

static inline void FeatureLocalMockDelete(FeatureLocalMock* self) {
  if (self != nullptr) {
    FEATURE_DESTRUCT(FEATURE_OBJECT(self));
    EEBUS_FREE(self);
  }
}

#endif  // TESTS_SRC_MOCKS_SPINE_FEATURE_FEATURE_LOCAL_MOCK_H_
