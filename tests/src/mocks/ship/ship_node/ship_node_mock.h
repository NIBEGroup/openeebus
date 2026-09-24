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
 * @brief Ship Node Mock "class"
 */

#ifndef TESTS_SRC_MOCKS_SHIP_SHIP_NODE_SHIP_NODE_MOCK_H_
#define TESTS_SRC_MOCKS_SHIP_SHIP_NODE_SHIP_NODE_MOCK_H_

#include <gmock/gmock.h>

#include <memory>

#include "mocks/ship/api/info_provider_mock.h"
#include "src/common/eebus_malloc.h"
#include "src/ship/api/ship_node_interface.h"

class ShipNodeGMockInterface : public InfoProviderMock {
 public:
  virtual ~ShipNodeGMockInterface() {};
  virtual void Start(ShipNodeObject* self)                                               = 0;
  virtual void Stop(ShipNodeObject* self)                                                = 0;
  virtual void RegisterRemoteSki(ShipNodeObject* self, const char* ski, bool is_trusted) = 0;
  virtual void UnregisterRemoteSki(ShipNodeObject* self, const char* ski)                = 0;
  virtual void CancelPairingWithSki(ShipNodeObject* self, const char* ski)               = 0;
  virtual void ApprovePendingHandshakeWithSki(ShipNodeObject* self, const char* ski)     = 0;
  virtual uint32_t GetPendingWaitingMsWithSki(ShipNodeObject* self, const char* ski)     = 0;
};

class ShipNodeGMock : public ShipNodeGMockInterface {
 public:
  virtual ~ShipNodeGMock() {};
  MOCK_METHOD1(Destruct, void(InfoProviderObject*));
  MOCK_METHOD2(IsRemoteServiceForSkiPaired, bool(InfoProviderObject*, const char*));
  MOCK_METHOD3(HandleConnectionClosed, void(InfoProviderObject*, ShipConnectionObject*, bool));
  MOCK_METHOD3(ReportServiceShipId, void(InfoProviderObject*, const char*, const char*));
  MOCK_METHOD2(IsWaitingForTrustAllowed, bool(InfoProviderObject*, const char*));
  MOCK_METHOD4(HandleShipStateUpdate, void(InfoProviderObject*, const char*, SmeState, const char*));
  MOCK_METHOD3(
      SetupRemoteDevice,
      DataReaderObject*(InfoProviderObject* self, const char* ski, DataWriterObject* data_writer)
  );
  MOCK_METHOD1(Start, void(ShipNodeObject*));
  MOCK_METHOD1(Stop, void(ShipNodeObject*));
  MOCK_METHOD3(RegisterRemoteSki, void(ShipNodeObject*, const char*, bool));
  MOCK_METHOD2(UnregisterRemoteSki, void(ShipNodeObject*, const char*));
  MOCK_METHOD2(CancelPairingWithSki, void(ShipNodeObject*, const char*));
  MOCK_METHOD2(ApprovePendingHandshakeWithSki, void(ShipNodeObject*, const char*));
  MOCK_METHOD2(GetPendingWaitingMsWithSki, uint32_t(ShipNodeObject*, const char*));
};

typedef struct ShipNodeMock {
  /** Implements the Ship Node Interface */
  ShipNodeObject obj;
  ShipNodeGMock* gmock;
} ShipNodeMock;

#define SHIP_NODE_MOCK(obj) ((ShipNodeMock*)(obj))

ShipNodeMock* ShipNodeMockCreate(void);

static inline void ShipNodeMockDelete(ShipNodeMock* self) {
  if (self != nullptr) {
    INFO_PROVIDER_DESTRUCT(INFO_PROVIDER_OBJECT(self));
    EEBUS_FREE(self);
  }
}

#endif  // TESTS_SRC_MOCKS_SHIP_SHIP_NODE_SHIP_NODE_MOCK_H_
