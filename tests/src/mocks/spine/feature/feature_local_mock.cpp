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
 * @brief Feature Local mock implementation
 */

#include "feature_local_mock.h"

#include <gmock/gmock.h>

#include "src/spine/api/feature_local_interface.h"

static DeviceLocalObject* GetDevice(const FeatureLocalObject* self);
static EntityLocalObject* GetEntity(const FeatureLocalObject* self);
static const void* GetData(const FeatureLocalObject* self, FunctionType function_type);
static void SetFunctionOperations(FeatureLocalObject* self, FunctionType type, bool read, bool write);
static EebusError AddWriteApprovalCallback(FeatureLocalObject* self, WriteApprovalCallback cb, void* ctx);
static EebusError TryApproveWrite(FeatureLocalObject* self, const char* ski, MsgCounterType msg_cnt);
static EebusError DenyWrite(FeatureLocalObject* self, const char* ski, MsgCounterType msg_cnt, const ErrorType* err);
static void CleanRemoteDeviceCaches(FeatureLocalObject* self, const DeviceAddressType* remote_addr);
static void* DataCopy(const FeatureLocalObject* self, FunctionType function_type);
static EebusError UpdateData(
    FeatureLocalObject* self,
    FunctionType fcn_type,
    const void* data,
    const FilterType* filter_partial,
    const FilterType* filter_delete
);
static void SetData(FeatureLocalObject* self, FunctionType function_type, void* data);
static bool HasSubscriptionToRemote(const FeatureLocalObject* self, const FeatureAddressType* remote_addr);
static EebusError SubscribeToRemote(FeatureLocalObject* self, const FeatureAddressType* remote_addr);
static EebusError RemoveRemoteSubscription(FeatureLocalObject* self, const FeatureAddressType* remote_addr);
static void RemoveAllRemoteSubscriptions(FeatureLocalObject* self);
static bool HasBindingToRemote(const FeatureLocalObject* self, const FeatureAddressType* remote_addr);
static EebusError BindToRemote(FeatureLocalObject* self, const FeatureAddressType* remote_addr);
static EebusError RemoveRemoteBinding(FeatureLocalObject* self, const FeatureAddressType* remote_addr);
static void RemoveAllRemoteBindings(FeatureLocalObject* self);
static EebusError HandleMessage(FeatureLocalObject* self, const Message* msg);
static NodeManagementDetailedDiscoveryFeatureInformationType* CreateInformation(const FeatureLocalObject* self);
static void Tick(FeatureLocalObject* self);
static EebusError WriteToRemote(
    FeatureLocalObject* self,
    FeatureRemoteObject* dest_feature,
    FunctionType fcn_type,
    const void* data,
    const FilterType* filter_partial,
    const FilterType* filter_delete,
    ResultMessageCallback cb,
    void* ctx
);
static EebusError ReadFromRemote(
    FeatureLocalObject* self,
    FeatureRemoteObject* dest_feature,
    FunctionType function_type,
    const void* selectors,
    const void* elements,
    ReplyMessageCallback cb,
    void* ctx
);

static const FeatureLocalInterface feature_local_methods = {
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

    .get_device                            = GetDevice,
    .get_entity                            = GetEntity,
    .get_data                              = GetData,
    .set_function_operations               = SetFunctionOperations,
    .add_write_approval_callback           = AddWriteApprovalCallback,
    .try_approve_write                     = TryApproveWrite,
    .deny_write                            = DenyWrite,
    .clean_remote_device_caches            = CleanRemoteDeviceCaches,
    .data_copy                             = DataCopy,
    .update_data                           = UpdateData,
    .set_data                              = SetData,
    .has_subscription_to_remote            = HasSubscriptionToRemote,
    .subscribe_to_remote                   = SubscribeToRemote,
    .remove_remote_subscription            = RemoveRemoteSubscription,
    .remove_all_remote_subscriptions       = RemoveAllRemoteSubscriptions,
    .has_binding_to_remote                 = HasBindingToRemote,
    .bind_to_remote                        = BindToRemote,
    .remove_remote_binding                 = RemoveRemoteBinding,
    .remove_all_remote_bindings            = RemoveAllRemoteBindings,
    .handle_message                        = HandleMessage,
    .create_information                    = CreateInformation,
    .tick                                  = Tick,
    .write_to_remote                       = WriteToRemote,
    .read_from_remote                      = ReadFromRemote,
};

static EebusError FeatureLocalMockConstruct(FeatureLocalMock* self);

EebusError FeatureLocalMockConstruct(FeatureLocalMock* self) {
  // Override "virtual functions table"
  FEATURE_LOCAL_INTERFACE(self) = &feature_local_methods;

  self->gmock = new FeatureLocalGMock();
  if (self->gmock == nullptr) {
    return kEebusErrorMemoryAllocate;
  }

  return kEebusErrorOk;
}

FeatureLocalMock* FeatureLocalMockCreate(void) {
  FeatureLocalMock* const mock = (FeatureLocalMock*)EEBUS_MALLOC(sizeof(FeatureLocalMock));
  if (mock == nullptr) {
    return nullptr;
  }

  if (FeatureLocalMockConstruct(mock) != kEebusErrorOk) {
    FeatureLocalMockDelete(mock);
    return nullptr;
  }

  return mock;
}

void Destruct(FeatureObject* self) {
  FeatureLocalMock* const mock = FEATURE_LOCAL_MOCK(self);
  mock->gmock->Destruct(self);
  delete mock->gmock;
}

const FeatureAddressType* GetAddress(const FeatureObject* self) {
  FeatureLocalMock* const mock = FEATURE_LOCAL_MOCK(self);
  return mock->gmock->GetAddress(self);
}

FeatureTypeType GetType(const FeatureObject* self) {
  FeatureLocalMock* const mock = FEATURE_LOCAL_MOCK(self);
  return mock->gmock->GetType(self);
}

RoleType GetRole(const FeatureObject* self) {
  FeatureLocalMock* const mock = FEATURE_LOCAL_MOCK(self);
  return mock->gmock->GetRole(self);
}

const OperationsObject* GetFunctionOperations(const FeatureObject* self, FunctionType fcn_type) {
  FeatureLocalMock* const mock = FEATURE_LOCAL_MOCK(self);
  return mock->gmock->GetFunctionOperations(self, fcn_type);
}

const char* GetDescription(const FeatureObject* self) {
  FeatureLocalMock* const mock = FEATURE_LOCAL_MOCK(self);
  return mock->gmock->GetDescription(self);
}

void SetDescription(FeatureObject* self, const char* description) {
  FeatureLocalMock* const mock = FEATURE_LOCAL_MOCK(self);
  mock->gmock->SetDescription(self, description);
}

const char* ToString(const FeatureObject* self) {
  FeatureLocalMock* const mock = FEATURE_LOCAL_MOCK(self);
  return mock->gmock->ToString(self);
}

DeviceLocalObject* GetDevice(const FeatureLocalObject* self) {
  FeatureLocalMock* const mock = FEATURE_LOCAL_MOCK(self);
  return mock->gmock->GetDevice(self);
}

EntityLocalObject* GetEntity(const FeatureLocalObject* self) {
  FeatureLocalMock* const mock = FEATURE_LOCAL_MOCK(self);
  return mock->gmock->GetEntity(self);
}

const void* GetData(const FeatureLocalObject* self, FunctionType function_type) {
  FeatureLocalMock* const mock = FEATURE_LOCAL_MOCK(self);
  return mock->gmock->GetData(self, function_type);
}

void SetFunctionOperations(FeatureLocalObject* self, FunctionType type, bool read, bool write) {
  FeatureLocalMock* const mock = FEATURE_LOCAL_MOCK(self);
  mock->gmock->SetFunctionOperations(self, type, read, write);
}

EebusError AddWriteApprovalCallback(FeatureLocalObject* self, WriteApprovalCallback cb, void* ctx) {
  FeatureLocalMock* const mock = FEATURE_LOCAL_MOCK(self);
  return mock->gmock->AddWriteApprovalCallback(self, cb, ctx);
}

EebusError TryApproveWrite(FeatureLocalObject* self, const char* ski, MsgCounterType msg_cnt) {
  FeatureLocalMock* const mock = FEATURE_LOCAL_MOCK(self);
  return mock->gmock->TryApproveWrite(self, ski, msg_cnt);
}

EebusError DenyWrite(FeatureLocalObject* self, const char* ski, MsgCounterType msg_cnt, const ErrorType* err) {
  FeatureLocalMock* const mock = FEATURE_LOCAL_MOCK(self);
  return mock->gmock->DenyWrite(self, ski, msg_cnt, err);
}

void CleanRemoteDeviceCaches(FeatureLocalObject* self, const DeviceAddressType* remote_addr) {
  FeatureLocalMock* const mock = FEATURE_LOCAL_MOCK(self);
  mock->gmock->CleanRemoteDeviceCaches(self, remote_addr);
}

void* DataCopy(const FeatureLocalObject* self, FunctionType function_type) {
  FeatureLocalMock* const mock = FEATURE_LOCAL_MOCK(self);
  return mock->gmock->DataCopy(self, function_type);
}

EebusError UpdateData(
    FeatureLocalObject* self,
    FunctionType fcn_type,
    const void* data,
    const FilterType* filter_partial,
    const FilterType* filter_delete
) {
  FeatureLocalMock* const mock = FEATURE_LOCAL_MOCK(self);
  return mock->gmock->UpdateData(self, fcn_type, data, filter_partial, filter_delete);
}

void SetData(FeatureLocalObject* self, FunctionType function_type, void* data) {
  FeatureLocalMock* const mock = FEATURE_LOCAL_MOCK(self);
  mock->gmock->SetData(self, function_type, data);
}

bool HasSubscriptionToRemote(const FeatureLocalObject* self, const FeatureAddressType* remote_addr) {
  FeatureLocalMock* const mock = FEATURE_LOCAL_MOCK(self);
  return mock->gmock->HasSubscriptionToRemote(self, remote_addr);
}

EebusError SubscribeToRemote(FeatureLocalObject* self, const FeatureAddressType* remote_addr) {
  FeatureLocalMock* const mock = FEATURE_LOCAL_MOCK(self);
  return mock->gmock->SubscribeToRemote(self, remote_addr);
}

EebusError RemoveRemoteSubscription(FeatureLocalObject* self, const FeatureAddressType* remote_addr) {
  FeatureLocalMock* const mock = FEATURE_LOCAL_MOCK(self);
  return mock->gmock->RemoveRemoteSubscription(self, remote_addr);
}

void RemoveAllRemoteSubscriptions(FeatureLocalObject* self) {
  FeatureLocalMock* const mock = FEATURE_LOCAL_MOCK(self);
  mock->gmock->RemoveAllRemoteSubscriptions(self);
}

bool HasBindingToRemote(const FeatureLocalObject* self, const FeatureAddressType* remote_addr) {
  FeatureLocalMock* const mock = FEATURE_LOCAL_MOCK(self);
  return mock->gmock->HasBindingToRemote(self, remote_addr);
}

EebusError BindToRemote(FeatureLocalObject* self, const FeatureAddressType* remote_addr) {
  FeatureLocalMock* const mock = FEATURE_LOCAL_MOCK(self);
  return mock->gmock->BindToRemote(self, remote_addr);
}

EebusError RemoveRemoteBinding(FeatureLocalObject* self, const FeatureAddressType* remote_addr) {
  FeatureLocalMock* const mock = FEATURE_LOCAL_MOCK(self);
  return mock->gmock->RemoveRemoteBinding(self, remote_addr);
}

void RemoveAllRemoteBindings(FeatureLocalObject* self) {
  FeatureLocalMock* const mock = FEATURE_LOCAL_MOCK(self);
  mock->gmock->RemoveAllRemoteBindings(self);
}

EebusError HandleMessage(FeatureLocalObject* self, const Message* msg) {
  FeatureLocalMock* const mock = FEATURE_LOCAL_MOCK(self);
  return mock->gmock->HandleMessage(self, msg);
}

NodeManagementDetailedDiscoveryFeatureInformationType* CreateInformation(const FeatureLocalObject* self) {
  FeatureLocalMock* const mock = FEATURE_LOCAL_MOCK(self);
  return mock->gmock->CreateInformation(self);
}

void Tick(FeatureLocalObject* self) {
  FeatureLocalMock* const mock = FEATURE_LOCAL_MOCK(self);
  mock->gmock->Tick(self);
}

EebusError WriteToRemote(
    FeatureLocalObject* self,
    FeatureRemoteObject* dest_feature,
    FunctionType fcn_type,
    const void* data,
    const FilterType* filter_partial,
    const FilterType* filter_delete,
    ResultMessageCallback cb,
    void* ctx
) {
  FeatureLocalMock* const mock = FEATURE_LOCAL_MOCK(self);
  return mock->gmock->WriteToRemote(self, dest_feature, fcn_type, data, filter_partial, filter_delete, cb, ctx);
}

EebusError ReadFromRemote(
    FeatureLocalObject* self,
    FeatureRemoteObject* dest_feature,
    FunctionType function_type,
    const void* selectors,
    const void* elements,
    ReplyMessageCallback cb,
    void* ctx
) {
  FeatureLocalMock* const mock = FEATURE_LOCAL_MOCK(self);
  return mock->gmock->ReadFromRemote(self, dest_feature, function_type, selectors, elements, cb, ctx);
}
