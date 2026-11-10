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
 * @brief Device mock implementation
 */

#include "device_mock.h"

#include <gmock/gmock.h>

#include "src/spine/api/device_interface.h"

static void Destruct(DeviceObject* self);
static const char* GetAddress(const DeviceObject* self);
static const DeviceTypeType* GetDeviceType(const DeviceObject* self);
static const NetworkManagementFeatureSetType* GetFeatureSet(const DeviceObject* self);
static const NodeManagementDestinationDataType* CreateDestinationData(const DeviceObject* self);

static const DeviceInterface device_methods = {
    .destruct                = Destruct,
    .get_address             = GetAddress,
    .get_device_type         = GetDeviceType,
    .get_feature_set         = GetFeatureSet,
    .create_destination_data = CreateDestinationData,
};

static EebusError DeviceMockConstruct(DeviceMock* self);

EebusError DeviceMockConstruct(DeviceMock* self) {
  // Override "virtual functions table"
  DEVICE_INTERFACE(self) = &device_methods;

  self->gmock = new DeviceGMock();
  if (self->gmock == nullptr) {
    return kEebusErrorMemoryAllocate;
  }

  return kEebusErrorOk;
}

DeviceMock* DeviceMockCreate(void) {
  DeviceMock* const mock = (DeviceMock*)EEBUS_MALLOC(sizeof(DeviceMock));
  if (mock == nullptr) {
    return nullptr;
  }

  if (DeviceMockConstruct(mock) != kEebusErrorOk) {
    DeviceMockDelete(mock);
    return nullptr;
  }

  return mock;
}

void Destruct(DeviceObject* self) {
  DeviceMock* const mock = DEVICE_MOCK(self);
  mock->gmock->Destruct(self);
  delete mock->gmock;
}

const char* GetAddress(const DeviceObject* self) {
  DeviceMock* const mock = DEVICE_MOCK(self);
  return mock->gmock->GetAddress(self);
}

const DeviceTypeType* GetDeviceType(const DeviceObject* self) {
  DeviceMock* const mock = DEVICE_MOCK(self);
  return mock->gmock->GetDeviceType(self);
}

const NetworkManagementFeatureSetType* GetFeatureSet(const DeviceObject* self) {
  DeviceMock* const mock = DEVICE_MOCK(self);
  return mock->gmock->GetFeatureSet(self);
}

const NodeManagementDestinationDataType* CreateDestinationData(const DeviceObject* self) {
  DeviceMock* const mock = DEVICE_MOCK(self);
  return mock->gmock->CreateDestinationData(self);
}
