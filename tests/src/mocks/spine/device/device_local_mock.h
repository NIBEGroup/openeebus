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
 * @brief Device Local Mock "class"
 */

#ifndef TESTS_SRC_MOCKS_SPINE_DEVICE_DEVICE_LOCAL_MOCK_H_
#define TESTS_SRC_MOCKS_SPINE_DEVICE_DEVICE_LOCAL_MOCK_H_

#include <gmock/gmock.h>

#include <memory>

#include "device_mock.h"
#include "src/spine/api/device_local_interface.h"

class DeviceLocalGMockInterface : public DeviceGMockInterface {
 public:
  virtual ~DeviceLocalGMockInterface() {};
  virtual EebusError Start(DeviceLocalObject* self)                                                               = 0;
  virtual void Stop(DeviceLocalObject* self)                                                                      = 0;
  virtual DataReaderObject* SetupRemoteDevice(DeviceLocalObject* self, const char* ski, DataWriterObject* writer) = 0;
  virtual void AddRemoteDeviceForSki(DeviceLocalObject* self, const char* ski, DeviceRemoteObject* remote_device) = 0;
  virtual EebusError RequestRemoteDetailedDiscoveryData(
      DeviceLocalObject* self,
      const DeviceRemoteObject* remote_device
  )                                                                                                              = 0;
  virtual void RemoveRemoteDeviceConnection(DeviceLocalObject* self, const char* ski)                            = 0;
  virtual void RemoveRemoteDevice(DeviceLocalObject* self, const char* ski)                                      = 0;
  virtual DeviceRemoteObject* GetRemoteDeviceWithAddress(const DeviceLocalObject* self, const char* device_addr) = 0;
  virtual DeviceRemoteObject* GetRemoteDeviceWithSki(const DeviceLocalObject* self, const char* ski)             = 0;
  virtual void AddEntity(DeviceLocalObject* self, EntityLocalObject* entity)                                     = 0;
  virtual void RemoveEntity(DeviceLocalObject* self, EntityLocalObject* entity)                                  = 0;
  virtual EntityLocalObject*
  GetEntity(const DeviceLocalObject* self, const uint32_t* const* entity_ids, size_t entity_ids_size)
      = 0;
  virtual EntityLocalObject* GetEntityWithType(const DeviceLocalObject* self, EntityTypeType entity_type) = 0;
  virtual const Vector* GetEntities(const DeviceLocalObject* self)                                        = 0;
  virtual FeatureLocalObject* GetFeatureWithAddress(
      const DeviceLocalObject* self,
      const FeatureAddressType* feature_addr
  )                                                                                                                = 0;
  virtual EebusError HandleMessage(DeviceLocalObject* self, MessageBuffer* msg, DeviceRemoteObject* remote_device) = 0;
  virtual NodeManagementObject* GetNodeManagement(const DeviceLocalObject* self)                                   = 0;
  virtual BindingManagerObject* GetBindingManager(const DeviceLocalObject* self)                                   = 0;
  virtual SubscriptionManagerObject* GetSubscriptionManager(const DeviceLocalObject* self)                         = 0;
  virtual EventsManagerObject* GetEventsManager(const DeviceLocalObject* self)                                     = 0;
  virtual void
  NotifySubscribers(const DeviceLocalObject* self, const FeatureAddressType* feature_addr, const CmdType* cmd)
      = 0;
  virtual NodeManagementDetailedDiscoveryDeviceInformationType* CreateInformation(const DeviceLocalObject* self) = 0;
  virtual void Lock(DeviceLocalObject* self)                                                                     = 0;
  virtual void Unlock(DeviceLocalObject* self)                                                                   = 0;
};

class DeviceLocalGMock : public DeviceLocalGMockInterface {
 public:
  virtual ~DeviceLocalGMock() {};
  MOCK_METHOD1(Destruct, void(DeviceObject*));
  MOCK_METHOD1(GetAddress, const char*(const DeviceObject*));
  MOCK_METHOD1(GetDeviceType, const DeviceTypeType*(const DeviceObject*));
  MOCK_METHOD1(GetFeatureSet, const NetworkManagementFeatureSetType*(const DeviceObject*));
  MOCK_METHOD1(CreateDestinationData, const NodeManagementDestinationDataType*(const DeviceObject*));
  MOCK_METHOD1(Start, EebusError(DeviceLocalObject*));
  MOCK_METHOD1(Stop, void(DeviceLocalObject*));
  MOCK_METHOD3(SetupRemoteDevice, DataReaderObject*(DeviceLocalObject*, const char*, DataWriterObject*));
  MOCK_METHOD3(AddRemoteDeviceForSki, void(DeviceLocalObject*, const char*, DeviceRemoteObject*));
  MOCK_METHOD2(RequestRemoteDetailedDiscoveryData, EebusError(DeviceLocalObject*, const DeviceRemoteObject*));
  MOCK_METHOD2(RemoveRemoteDeviceConnection, void(DeviceLocalObject*, const char*));
  MOCK_METHOD2(RemoveRemoteDevice, void(DeviceLocalObject*, const char*));
  MOCK_METHOD2(GetRemoteDeviceWithAddress, DeviceRemoteObject*(const DeviceLocalObject*, const char*));
  MOCK_METHOD2(GetRemoteDeviceWithSki, DeviceRemoteObject*(const DeviceLocalObject*, const char*));
  MOCK_METHOD2(AddEntity, void(DeviceLocalObject*, EntityLocalObject*));
  MOCK_METHOD2(RemoveEntity, void(DeviceLocalObject*, EntityLocalObject*));
  MOCK_METHOD3(GetEntity, EntityLocalObject*(const DeviceLocalObject*, const uint32_t* const*, size_t));
  MOCK_METHOD2(GetEntityWithType, EntityLocalObject*(const DeviceLocalObject*, EntityTypeType));
  MOCK_METHOD1(GetEntities, const Vector*(const DeviceLocalObject*));
  MOCK_METHOD2(GetFeatureWithAddress, FeatureLocalObject*(const DeviceLocalObject*, const FeatureAddressType*));
  MOCK_METHOD3(HandleMessage, EebusError(DeviceLocalObject*, MessageBuffer*, DeviceRemoteObject*));
  MOCK_METHOD1(GetNodeManagement, NodeManagementObject*(const DeviceLocalObject*));
  MOCK_METHOD1(GetBindingManager, BindingManagerObject*(const DeviceLocalObject*));
  MOCK_METHOD1(GetSubscriptionManager, SubscriptionManagerObject*(const DeviceLocalObject*));
  MOCK_METHOD1(GetEventsManager, EventsManagerObject*(const DeviceLocalObject*));
  MOCK_METHOD3(NotifySubscribers, void(const DeviceLocalObject*, const FeatureAddressType*, const CmdType*));
  MOCK_METHOD1(CreateInformation, NodeManagementDetailedDiscoveryDeviceInformationType*(const DeviceLocalObject*));
  MOCK_METHOD1(Lock, void(DeviceLocalObject*));
  MOCK_METHOD1(Unlock, void(DeviceLocalObject*));
};

typedef struct DeviceLocalMock {
  /** Implements the Device Local Interface */
  DeviceLocalObject obj;
  DeviceLocalGMock* gmock;
} DeviceLocalMock;

#define DEVICE_LOCAL_MOCK(obj) ((DeviceLocalMock*)(obj))

DeviceLocalMock* DeviceLocalMockCreate(void);

static inline void DeviceLocalMockDelete(DeviceLocalMock* self) {
  if (self != nullptr) {
    DEVICE_DESTRUCT(DEVICE_OBJECT(self));
    EEBUS_FREE(self);
  }
}

#endif  // TESTS_SRC_MOCKS_SPINE_DEVICE_DEVICE_LOCAL_MOCK_H_
