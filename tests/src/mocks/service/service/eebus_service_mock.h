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
 * @brief EEBUS Service Mock "class"
 */

#ifndef TESTS_SRC_MOCKS_SERVICE_SERVICE_EEBUS_SERVICE_MOCK_H_
#define TESTS_SRC_MOCKS_SERVICE_SERVICE_EEBUS_SERVICE_MOCK_H_

#include <gmock/gmock.h>

#include <memory>

#include "mocks/ship/api/ship_node_reader_mock.h"
#include "src/common/eebus_malloc.h"
#include "src/service/api/eebus_service_interface.h"

class EebusServiceGMockInterface : public ShipNodeReaderGMockInterface {
 public:
  virtual ~EebusServiceGMockInterface() {};
  virtual void Start(EebusServiceObject* self)                                                                  = 0;
  virtual void Stop(EebusServiceObject* self)                                                                   = 0;
  virtual const ServiceDetails* GetLocalService(const EebusServiceObject* self)                                 = 0;
  virtual DeviceLocalObject* GetLocalDevice(const EebusServiceObject* self)                                     = 0;
  virtual const ConnectionState* GetConnectionStateWithSki(const EebusServiceObject* self, const char* ski)     = 0;
  virtual const ServiceDetails* GetRemoteServiceDetailsWithSki(const EebusServiceObject* self, const char* ski) = 0;
  virtual void RegisterRemoteSki(EebusServiceObject* self, const char* ski, bool enable)                        = 0;
  virtual void UnregisterRemoteSki(EebusServiceObject* self, const char* ski)                                   = 0;
  virtual void CancelPairingWithSki(EebusServiceObject* self, const char* ski)                                  = 0;
  virtual void SetPairingPossible(EebusServiceObject* self, bool is_pairing_possible)                           = 0;
  virtual const char* GetLocalSki(EebusServiceObject* self)                                                     = 0;
  virtual const char* GetQrCodeString(EebusServiceObject* self)                                                 = 0;
};

class EebusServiceGMock : public EebusServiceGMockInterface {
 public:
  virtual ~EebusServiceGMock() {};
  MOCK_METHOD1(Destruct, void(ShipNodeReaderObject*));
  MOCK_METHOD2(OnRemoteSkiConnected, void(ShipNodeReaderObject*, const char*));
  MOCK_METHOD2(OnRemoteSkiDisconnected, void(ShipNodeReaderObject*, const char*));
  MOCK_METHOD3(SetupRemoteDevice, DataReaderObject*(ShipNodeReaderObject*, const char*, DataWriterObject*));
  MOCK_METHOD2(OnRemoteServicesUpdate, void(ShipNodeReaderObject*, const Vector*));
  MOCK_METHOD3(OnShipIdUpdate, void(ShipNodeReaderObject*, const char*, const char*));
  MOCK_METHOD3(OnShipStateUpdate, void(ShipNodeReaderObject*, const char*, SmeState));
  MOCK_METHOD2(IsWaitingForTrustAllowed, bool(ShipNodeReaderObject*, const char*));
  MOCK_METHOD1(Start, void(EebusServiceObject*));
  MOCK_METHOD1(Stop, void(EebusServiceObject*));
  MOCK_METHOD1(GetLocalService, const ServiceDetails*(const EebusServiceObject*));
  MOCK_METHOD1(GetLocalDevice, DeviceLocalObject*(const EebusServiceObject*));
  MOCK_METHOD2(GetConnectionStateWithSki, const ConnectionState*(const EebusServiceObject*, const char*));
  MOCK_METHOD2(GetRemoteServiceDetailsWithSki, const ServiceDetails*(const EebusServiceObject*, const char*));
  MOCK_METHOD3(RegisterRemoteSki, void(EebusServiceObject*, const char*, bool));
  MOCK_METHOD2(UnregisterRemoteSki, void(EebusServiceObject*, const char*));
  MOCK_METHOD2(CancelPairingWithSki, void(EebusServiceObject*, const char*));
  MOCK_METHOD2(SetPairingPossible, void(EebusServiceObject*, bool));
  MOCK_METHOD1(GetLocalSki, const char*(EebusServiceObject*));
  MOCK_METHOD1(GetQrCodeString, const char*(EebusServiceObject*));
};

typedef struct EebusServiceMock {
  /** Implements the EEBUS Service Interface */
  EebusServiceObject obj;
  EebusServiceGMock* gmock;
} EebusServiceMock;

#define EEBUS_SERVICE_MOCK(obj) ((EebusServiceMock*)(obj))

EebusServiceMock* EebusServiceMockCreate(void);

static inline void EebusServiceMockDelete(EebusServiceMock* self) {
  if (self != nullptr) {
    EEBUS_SERVICE_DESTRUCT(EEBUS_SERVICE_OBJECT(self));
    EEBUS_FREE(self);
  }
}

#endif  // TESTS_SRC_MOCKS_SERVICE_SERVICE_EEBUS_SERVICE_MOCK_H_
