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
 * @brief Subscription Manager mock implementation
 */

#include "subscription_manager_mock.h"

#include <gmock/gmock.h>

#include "src/spine/api/subscription_manager_interface.h"

static void Destruct(SubscriptionManagerObject* self);
static EebusError AddSubscription(
    SubscriptionManagerObject* self,
    DeviceRemoteObject* remote_device,
    const SubscriptionManagementRequestCallType* data
);
static EebusError RemoveSubscription(
    SubscriptionManagerObject* self,
    DeviceRemoteObject* remote_device,
    const SubscriptionManagementDeleteCallType* data
);
static void RemoveDeviceSubscriptions(SubscriptionManagerObject* self, DeviceRemoteObject* remote_device);
static void RemoveEntitySubscriptions(SubscriptionManagerObject* self, EntityRemoteObject* remote_entity);
static void Publish(const SubscriptionManagerObject* self, const FeatureAddressType* feature_addr, const CmdType* cmd);
static NodeManagementSubscriptionDataType*
CreateSubscriptionData(const SubscriptionManagerObject* self, DeviceRemoteObject* remote_device);

static const SubscriptionManagerInterface subscription_manager_methods = {
    .destruct                    = Destruct,
    .add_subscription            = AddSubscription,
    .remove_subscription         = RemoveSubscription,
    .remove_device_subscriptions = RemoveDeviceSubscriptions,
    .remove_entity_subscriptions = RemoveEntitySubscriptions,
    .publish                     = Publish,
    .create_subscription_data    = CreateSubscriptionData,
};

static EebusError SubscriptionManagerMockConstruct(SubscriptionManagerMock* self);

EebusError SubscriptionManagerMockConstruct(SubscriptionManagerMock* self) {
  // Override "virtual functions table"
  SUBSCRIPTION_MANAGER_INTERFACE(self) = &subscription_manager_methods;

  self->gmock = new SubscriptionManagerGMock();
  if (self->gmock == nullptr) {
    return kEebusErrorMemoryAllocate;
  }

  return kEebusErrorOk;
}

SubscriptionManagerMock* SubscriptionManagerMockCreate(void) {
  SubscriptionManagerMock* const mock = (SubscriptionManagerMock*)EEBUS_MALLOC(sizeof(SubscriptionManagerMock));
  if (mock == nullptr) {
    return nullptr;
  }

  if (SubscriptionManagerMockConstruct(mock) != kEebusErrorOk) {
    SubscriptionManagerMockDelete(mock);
    return nullptr;
  }

  return mock;
}

void Destruct(SubscriptionManagerObject* self) {
  SubscriptionManagerMock* const mock = SUBSCRIPTION_MANAGER_MOCK(self);
  mock->gmock->Destruct(self);
  delete mock->gmock;
}

EebusError AddSubscription(
    SubscriptionManagerObject* self,
    DeviceRemoteObject* remote_device,
    const SubscriptionManagementRequestCallType* data
) {
  SubscriptionManagerMock* const mock = SUBSCRIPTION_MANAGER_MOCK(self);
  return mock->gmock->AddSubscription(self, remote_device, data);
}

EebusError RemoveSubscription(
    SubscriptionManagerObject* self,
    DeviceRemoteObject* remote_device,
    const SubscriptionManagementDeleteCallType* data
) {
  SubscriptionManagerMock* const mock = SUBSCRIPTION_MANAGER_MOCK(self);
  return mock->gmock->RemoveSubscription(self, remote_device, data);
}

void RemoveDeviceSubscriptions(SubscriptionManagerObject* self, DeviceRemoteObject* remote_device) {
  SubscriptionManagerMock* const mock = SUBSCRIPTION_MANAGER_MOCK(self);
  mock->gmock->RemoveDeviceSubscriptions(self, remote_device);
}

void RemoveEntitySubscriptions(SubscriptionManagerObject* self, EntityRemoteObject* remote_entity) {
  SubscriptionManagerMock* const mock = SUBSCRIPTION_MANAGER_MOCK(self);
  mock->gmock->RemoveEntitySubscriptions(self, remote_entity);
}

void Publish(const SubscriptionManagerObject* self, const FeatureAddressType* feature_addr, const CmdType* cmd) {
  SubscriptionManagerMock* const mock = SUBSCRIPTION_MANAGER_MOCK(self);
  mock->gmock->Publish(self, feature_addr, cmd);
}

NodeManagementSubscriptionDataType*
CreateSubscriptionData(const SubscriptionManagerObject* self, DeviceRemoteObject* remote_device) {
  SubscriptionManagerMock* const mock = SUBSCRIPTION_MANAGER_MOCK(self);
  return mock->gmock->CreateSubscriptionData(self, remote_device);
}
