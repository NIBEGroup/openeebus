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
 * @brief Device Mock "class"
 */

#ifndef TESTS_UNIT_TESTS_MOCKS_SPINE_DEVICE_DEVICE_MOCK_H_
#define TESTS_UNIT_TESTS_MOCKS_SPINE_DEVICE_DEVICE_MOCK_H_

#include <gmock/gmock.h>

#include <memory>

#include "src/common/eebus_malloc.h"
#include "src/spine/api/device_interface.h"

class DeviceGMockInterface {
 public:
  virtual ~DeviceGMockInterface() {};
  virtual void Destruct(DeviceObject* self)                                                        = 0;
  virtual const char* GetAddress(const DeviceObject* self)                                         = 0;
  virtual const DeviceTypeType* GetDeviceType(const DeviceObject* self)                            = 0;
  virtual const NetworkManagementFeatureSetType* GetFeatureSet(const DeviceObject* self)           = 0;
  virtual const NodeManagementDestinationDataType* CreateDestinationData(const DeviceObject* self) = 0;
};

class DeviceGMock : public DeviceGMockInterface {
 public:
  virtual ~DeviceGMock() {};
  MOCK_METHOD1(Destruct, void(DeviceObject*));
  MOCK_METHOD1(GetAddress, const char*(const DeviceObject*));
  MOCK_METHOD1(GetDeviceType, const DeviceTypeType*(const DeviceObject*));
  MOCK_METHOD1(GetFeatureSet, const NetworkManagementFeatureSetType*(const DeviceObject*));
  MOCK_METHOD1(CreateDestinationData, const NodeManagementDestinationDataType*(const DeviceObject*));
};

typedef struct DeviceMock {
  /** Implements the Device Interface */
  DeviceObject obj;
  DeviceGMock* gmock;
} DeviceMock;

#define DEVICE_MOCK(obj) ((DeviceMock*)(obj))

DeviceMock* DeviceMockCreate(void);

static inline void DeviceMockDelete(DeviceMock* self) {
  if (self != nullptr) {
    DEVICE_DESTRUCT(DEVICE_OBJECT(self));
    EEBUS_FREE(self);
  }
}

#endif  // TESTS_UNIT_TESTS_MOCKS_SPINE_DEVICE_DEVICE_MOCK_H_
