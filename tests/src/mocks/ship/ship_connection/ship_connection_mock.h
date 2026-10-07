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
 * @brief Ship Connection Mock "class"
 */

#ifndef TESTS_SRC_MOCKS_SHIP_SHIP_CONNECTION_SHIP_CONNECTION_MOCK_H_
#define TESTS_SRC_MOCKS_SHIP_SHIP_CONNECTION_SHIP_CONNECTION_MOCK_H_

#include <gmock/gmock.h>

#include <memory>

#include "src/common/eebus_errors.h"
#include "src/common/eebus_malloc.h"
#include "src/ship/api/ship_connection_interface.h"
#include "src/ship/api/websocket_creator_interface.h"
#include "src/ship/api/websocket_interface.h"

class ShipConnectionGMockInterface {
 public:
  virtual ~ShipConnectionGMockInterface() {};
  virtual void Destruct(DataWriterObject* self)                                                         = 0;
  virtual void WriteMessage(DataWriterObject* self, const uint8_t* msg, size_t msg_size)                = 0;
  virtual EebusError Start(ShipConnectionObject* self, WebsocketCreatorObject* wsc)                     = 0;
  virtual void Stop(ShipConnectionObject* self)                                                         = 0;
  virtual WebsocketObject* GetWebsocketConnection(ShipConnectionObject* self)                           = 0;
  virtual void CloseConnection(ShipConnectionObject* self, bool safe, int32_t code, const char* reason) = 0;
  virtual const char* GetRemoteSki(ShipConnectionObject* self)                                          = 0;
  virtual void ApprovePendingHandshake(ShipConnectionObject* self)                                      = 0;
  virtual void AbortPendingHandshake(ShipConnectionObject* self)                                        = 0;
  virtual SmeState GetState(ShipConnectionObject* self, EebusError* err)                                = 0;
};

class ShipConnectionGMock : public ShipConnectionGMockInterface {
 public:
  virtual ~ShipConnectionGMock() {};
  MOCK_METHOD1(Destruct, void(DataWriterObject*));
  MOCK_METHOD3(WriteMessage, void(DataWriterObject*, const uint8_t*, size_t));
  MOCK_METHOD2(Start, EebusError(ShipConnectionObject*, WebsocketCreatorObject*));
  MOCK_METHOD1(Stop, void(ShipConnectionObject*));
  MOCK_METHOD1(GetWebsocketConnection, WebsocketObject*(ShipConnectionObject*));
  MOCK_METHOD4(CloseConnection, void(ShipConnectionObject*, bool, int32_t, const char*));
  MOCK_METHOD1(GetRemoteSki, const char*(ShipConnectionObject*));
  MOCK_METHOD1(ApprovePendingHandshake, void(ShipConnectionObject*));
  MOCK_METHOD1(AbortPendingHandshake, void(ShipConnectionObject*));
  MOCK_METHOD2(GetState, SmeState(ShipConnectionObject*, EebusError*));
};

typedef struct ShipConnectionMock {
  /** Implements the Ship Connection Interface */
  ShipConnectionObject obj;
  ShipConnectionGMock* gmock;
} ShipConnectionMock;

#define SHIP_CONNECTION_MOCK(obj) ((ShipConnectionMock*)(obj))

ShipConnectionMock* ShipConnectionMockCreate(void);

static inline void ShipConnectionMockDelete(ShipConnectionMock* self) {
  if (self != nullptr) {
    SHIP_CONNECTION_DESTRUCT(SHIP_CONNECTION_OBJECT(self));
    EEBUS_FREE(self);
  }
}

class ShipConnectionMockGuard {
 public:
  explicit ShipConnectionMockGuard(ShipConnectionMock*& mock) : mock_(mock) {}
  ~ShipConnectionMockGuard() {
    if (mock_ != nullptr) {
      EXPECT_CALL(*mock_->gmock, Destruct(testing::_)).WillOnce(testing::Return());
      ShipConnectionMockDelete(mock_);
      mock_ = nullptr;
    }
  }

  ShipConnectionMockGuard(const ShipConnectionMockGuard&)            = delete;
  ShipConnectionMockGuard& operator=(const ShipConnectionMockGuard&) = delete;

 private:
  ShipConnectionMock*& mock_;
};

#endif  // TESTS_SRC_MOCKS_SHIP_SHIP_CONNECTION_SHIP_CONNECTION_MOCK_H_
