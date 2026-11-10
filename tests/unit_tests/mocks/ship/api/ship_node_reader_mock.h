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
 * @brief Ship Node Reader Mock "class"
 */

#ifndef TESTS_UNIT_TESTS_MOCKS_SHIP_API_SHIP_NODE_READER_MOCK_H_
#define TESTS_UNIT_TESTS_MOCKS_SHIP_API_SHIP_NODE_READER_MOCK_H_

#include <gmock/gmock.h>

#include <memory>

#include "src/common/eebus_malloc.h"
#include "src/ship/api/ship_node_reader_interface.h"

class ShipNodeReaderGMockInterface {
 public:
  virtual ~ShipNodeReaderGMockInterface() {};
  virtual void Destruct(ShipNodeReaderObject* self)                                 = 0;
  virtual void OnRemoteSkiConnected(ShipNodeReaderObject* self, const char* ski)    = 0;
  virtual void OnRemoteSkiDisconnected(ShipNodeReaderObject* self, const char* ski) = 0;
  virtual DataReaderObject*
  SetupRemoteDevice(ShipNodeReaderObject* self, const char* ski, DataWriterObject* data_writer)
      = 0;
  virtual void OnRemoteServicesUpdate(ShipNodeReaderObject* self, const Vector* entries)        = 0;
  virtual void OnShipIdUpdate(ShipNodeReaderObject* self, const char* ski, const char* ship_id) = 0;
  virtual void OnShipStateUpdate(ShipNodeReaderObject* self, const char* ski, SmeState state)   = 0;
  virtual bool IsWaitingForTrustAllowed(ShipNodeReaderObject* self, const char* ski)            = 0;
};

class ShipNodeReaderGMock : public ShipNodeReaderGMockInterface {
 public:
  virtual ~ShipNodeReaderGMock() {};
  MOCK_METHOD1(Destruct, void(ShipNodeReaderObject*));
  MOCK_METHOD2(OnRemoteSkiConnected, void(ShipNodeReaderObject*, const char*));
  MOCK_METHOD2(OnRemoteSkiDisconnected, void(ShipNodeReaderObject*, const char*));
  MOCK_METHOD3(SetupRemoteDevice, DataReaderObject*(ShipNodeReaderObject*, const char*, DataWriterObject*));
  MOCK_METHOD2(OnRemoteServicesUpdate, void(ShipNodeReaderObject*, const Vector*));
  MOCK_METHOD3(OnShipIdUpdate, void(ShipNodeReaderObject*, const char*, const char*));
  MOCK_METHOD3(OnShipStateUpdate, void(ShipNodeReaderObject*, const char*, SmeState));
  MOCK_METHOD2(IsWaitingForTrustAllowed, bool(ShipNodeReaderObject*, const char*));
};

typedef struct ShipNodeReaderMock {
  /** Implements the Ship Node Reader Interface */
  ShipNodeReaderObject obj;
  ShipNodeReaderGMock* gmock;
} ShipNodeReaderMock;

#define SHIP_NODE_READER_MOCK(obj) ((ShipNodeReaderMock*)(obj))

ShipNodeReaderMock* ShipNodeReaderMockCreate(void);

static inline void ShipNodeReaderMockDelete(ShipNodeReaderMock* self) {
  if (self != nullptr) {
    SHIP_NODE_READER_DESTRUCT(SHIP_NODE_READER_OBJECT(self));
    EEBUS_FREE(self);
  }
}

#endif  // TESTS_UNIT_TESTS_MOCKS_SHIP_API_SHIP_NODE_READER_MOCK_H_
