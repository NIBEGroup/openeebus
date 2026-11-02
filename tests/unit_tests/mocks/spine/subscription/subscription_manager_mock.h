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
 * @brief Subscription Manager Mock "class"
 */

#ifndef TESTS_UNIT_TESTS_MOCKS_SPINE_SUBSCRIPTION_SUBSCRIPTION_MANAGER_MOCK_H_
#define TESTS_UNIT_TESTS_MOCKS_SPINE_SUBSCRIPTION_SUBSCRIPTION_MANAGER_MOCK_H_

#include <gmock/gmock.h>

#include <memory>

#include "src/common/eebus_malloc.h"
#include "src/spine/api/subscription_manager_interface.h"

class SubscriptionManagerGMockInterface {
 public:
  virtual ~SubscriptionManagerGMockInterface() {};
  virtual void Destruct(SubscriptionManagerObject* self) = 0;
  virtual EebusError AddSubscription(
      SubscriptionManagerObject* self,
      DeviceRemoteObject* remote_device,
      const SubscriptionManagementRequestCallType* data
  ) = 0;
  virtual EebusError RemoveSubscription(
      SubscriptionManagerObject* self,
      DeviceRemoteObject* remote_device,
      const SubscriptionManagementDeleteCallType* data
  )                                                                                                          = 0;
  virtual void RemoveDeviceSubscriptions(SubscriptionManagerObject* self, DeviceRemoteObject* remote_device) = 0;
  virtual void RemoveEntitySubscriptions(SubscriptionManagerObject* self, EntityRemoteObject* remote_entity) = 0;
  virtual void
  Publish(const SubscriptionManagerObject* self, const FeatureAddressType* feature_addr, const CmdType* cmd)
      = 0;
  virtual NodeManagementSubscriptionDataType* CreateSubscriptionData(
      const SubscriptionManagerObject* self,
      DeviceRemoteObject* remote_device
  ) = 0;
};

class SubscriptionManagerGMock : public SubscriptionManagerGMockInterface {
 public:
  virtual ~SubscriptionManagerGMock() {};
  MOCK_METHOD1(Destruct, void(SubscriptionManagerObject*));
  MOCK_METHOD3(
      AddSubscription,
      EebusError(SubscriptionManagerObject*, DeviceRemoteObject*, const SubscriptionManagementRequestCallType*)
  );
  MOCK_METHOD3(
      RemoveSubscription,
      EebusError(SubscriptionManagerObject*, DeviceRemoteObject*, const SubscriptionManagementDeleteCallType*)
  );
  MOCK_METHOD2(RemoveDeviceSubscriptions, void(SubscriptionManagerObject*, DeviceRemoteObject*));
  MOCK_METHOD2(RemoveEntitySubscriptions, void(SubscriptionManagerObject*, EntityRemoteObject*));
  MOCK_METHOD3(Publish, void(const SubscriptionManagerObject*, const FeatureAddressType*, const CmdType*));
  MOCK_METHOD2(
      CreateSubscriptionData,
      NodeManagementSubscriptionDataType*(const SubscriptionManagerObject*, DeviceRemoteObject*)
  );
};

typedef struct SubscriptionManagerMock {
  /** Implements the Subscription Manager Interface */
  SubscriptionManagerObject obj;
  SubscriptionManagerGMock* gmock;
} SubscriptionManagerMock;

#define SUBSCRIPTION_MANAGER_MOCK(obj) ((SubscriptionManagerMock*)(obj))

SubscriptionManagerMock* SubscriptionManagerMockCreate(void);

static inline void SubscriptionManagerMockDelete(SubscriptionManagerMock* self) {
  if (self != nullptr) {
    SUBSCRIPTION_MANAGER_DESTRUCT(SUBSCRIPTION_MANAGER_OBJECT(self));
    EEBUS_FREE(self);
  }
}

#endif  // TESTS_UNIT_TESTS_MOCKS_SPINE_SUBSCRIPTION_SUBSCRIPTION_MANAGER_MOCK_H_
