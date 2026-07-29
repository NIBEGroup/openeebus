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
 * @brief Device Remote Mock "class"
 */

#ifndef TESTS_SRC_MOCKS_SPINE_DEVICE_DEVICE_REMOTE_MOCK_H_
#define TESTS_SRC_MOCKS_SPINE_DEVICE_DEVICE_REMOTE_MOCK_H_

#include <gmock/gmock.h>

#include <memory>

#include "device_mock.h"
#include "src/spine/api/device_remote_interface.h"

class DeviceRemoteGMockInterface : public DeviceGMockInterface {
 public:
  virtual ~DeviceRemoteGMockInterface() {};
  virtual DeviceLocalObject* GetLocalDevice(const DeviceRemoteObject* self)    = 0;
  virtual const char* GetSki(const DeviceRemoteObject* self)                   = 0;
  virtual DataReaderObject* GetDataReader(const DeviceRemoteObject* self)      = 0;
  virtual void AddEntity(DeviceRemoteObject* self, EntityRemoteObject* entity) = 0;
  virtual EntityRemoteObject*
  ReleaseEntity(DeviceRemoteObject* self, const uint32_t* const* entity_ids, size_t entity_ids_size)
      = 0;
  virtual EntityRemoteObject*
  GetEntity(const DeviceRemoteObject* self, const uint32_t* const* entity_ids, size_t entity_ids_size)
      = 0;
  virtual const Vector* GetEntities(const DeviceRemoteObject* self) = 0;
  virtual FeatureRemoteObject* GetFeatureWithAddress(
      const DeviceRemoteObject* self,
      const FeatureAddressType* feature_addr
  ) = 0;
  virtual FeatureRemoteObject* GetFeatureWithTypeAndRole(
      const DeviceRemoteObject* self,
      const uint32_t* const* entity_ids,
      size_t entity_ids_size,
      FeatureTypeType feature_type,
      RoleType role
  )                                                                                       = 0;
  virtual EebusError HandleSpineMesssage(DeviceRemoteObject* self, MessageBuffer* msg)    = 0;
  virtual NodeManagementRemoteObject* GetNodeManagement(const DeviceRemoteObject* self)   = 0;
  virtual SenderObject* GetSender(const DeviceRemoteObject* self)                         = 0;
  virtual NodeManagementUseCaseDataType* UseCasesDataCopy(const DeviceRemoteObject* self) = 0;
  virtual void UpdateDevice(DeviceRemoteObject* self, const NetworkManagementDeviceDescriptionDataType* description)
      = 0;
  virtual const Vector*
  AddEntityAndFeatures(DeviceRemoteObject* self, bool init, const NodeManagementDetailedDiscoveryDataType* data)
      = 0;
  virtual EebusError CheckEntityInformation(
      const DeviceRemoteObject* self,
      bool init,
      const NodeManagementDetailedDiscoveryEntityInformationType* entity_info
  ) = 0;
};

class DeviceRemoteGMock : public DeviceRemoteGMockInterface {
 public:
  virtual ~DeviceRemoteGMock() {};
  MOCK_METHOD1(Destruct, void(DeviceObject*));
  MOCK_METHOD1(GetAddress, const char*(const DeviceObject*));
  MOCK_METHOD1(GetDeviceType, const DeviceTypeType*(const DeviceObject*));
  MOCK_METHOD1(GetFeatureSet, const NetworkManagementFeatureSetType*(const DeviceObject*));
  MOCK_METHOD1(CreateDestinationData, const NodeManagementDestinationDataType*(const DeviceObject*));
  MOCK_METHOD1(GetLocalDevice, DeviceLocalObject*(const DeviceRemoteObject*));
  MOCK_METHOD1(GetSki, const char*(const DeviceRemoteObject*));
  MOCK_METHOD1(GetDataReader, DataReaderObject*(const DeviceRemoteObject*));
  MOCK_METHOD2(AddEntity, void(DeviceRemoteObject*, EntityRemoteObject*));
  MOCK_METHOD3(ReleaseEntity, EntityRemoteObject*(DeviceRemoteObject*, const uint32_t* const*, size_t));
  MOCK_METHOD3(GetEntity, EntityRemoteObject*(const DeviceRemoteObject*, const uint32_t* const*, size_t));
  MOCK_METHOD1(GetEntities, const Vector*(const DeviceRemoteObject*));
  MOCK_METHOD2(GetFeatureWithAddress, FeatureRemoteObject*(const DeviceRemoteObject*, const FeatureAddressType*));
  MOCK_METHOD5(
      GetFeatureWithTypeAndRole,
      FeatureRemoteObject*(const DeviceRemoteObject*, const uint32_t* const*, size_t, FeatureTypeType, RoleType)
  );
  MOCK_METHOD2(HandleSpineMesssage, EebusError(DeviceRemoteObject*, MessageBuffer*));
  MOCK_METHOD1(GetNodeManagement, NodeManagementRemoteObject*(const DeviceRemoteObject*));
  MOCK_METHOD1(GetSender, SenderObject*(const DeviceRemoteObject*));
  MOCK_METHOD1(UseCasesDataCopy, NodeManagementUseCaseDataType*(const DeviceRemoteObject*));
  MOCK_METHOD2(UpdateDevice, void(DeviceRemoteObject*, const NetworkManagementDeviceDescriptionDataType*));
  MOCK_METHOD3(
      AddEntityAndFeatures,
      const Vector*(DeviceRemoteObject*, bool, const NodeManagementDetailedDiscoveryDataType*)
  );
  MOCK_METHOD3(
      CheckEntityInformation,
      EebusError(const DeviceRemoteObject*, bool, const NodeManagementDetailedDiscoveryEntityInformationType*)
  );
};

typedef struct DeviceRemoteMock {
  /** Implements the Device Remote Interface */
  DeviceRemoteObject obj;
  DeviceRemoteGMock* gmock;
} DeviceRemoteMock;

#define DEVICE_REMOTE_MOCK(obj) ((DeviceRemoteMock*)(obj))

DeviceRemoteMock* DeviceRemoteMockCreate(void);

static inline void DeviceRemoteMockDelete(DeviceRemoteMock* self) {
  if (self != nullptr) {
    DEVICE_DESTRUCT(DEVICE_OBJECT(self));
    EEBUS_FREE(self);
  }
}

#endif  // TESTS_SRC_MOCKS_SPINE_DEVICE_DEVICE_REMOTE_MOCK_H_
