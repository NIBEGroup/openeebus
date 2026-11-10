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
 * @brief Websocket Mock "class"
 */

#ifndef TESTS_UNIT_TESTS_MOCKS_SHIP_WEBSOCKET_WEBSOCKET_MOCK_H_
#define TESTS_UNIT_TESTS_MOCKS_SHIP_WEBSOCKET_WEBSOCKET_MOCK_H_

#include <gmock/gmock.h>

#include <memory>

#include "src/common/eebus_malloc.h"
#include "src/ship/api/websocket_interface.h"

class WebsocketGMockInterface {
 public:
  virtual ~WebsocketGMockInterface() {};
  virtual void Destruct(WebsocketObject* self)                                      = 0;
  virtual int32_t Write(WebsocketObject* self, const uint8_t* msg, size_t msg_size) = 0;
  virtual void Close(WebsocketObject* self, int32_t close_code, const char* reason) = 0;
  virtual bool IsClosed(const WebsocketObject* self)                                = 0;
  virtual int32_t GetCloseError(const WebsocketObject* self)                        = 0;
  virtual void ScheduleWrite(WebsocketObject* self)                                 = 0;
};

class WebsocketGMock : public WebsocketGMockInterface {
 public:
  virtual ~WebsocketGMock() {};
  MOCK_METHOD1(Destruct, void(WebsocketObject*));
  MOCK_METHOD3(Write, int32_t(WebsocketObject*, const uint8_t*, size_t));
  MOCK_METHOD3(Close, void(WebsocketObject*, int32_t, const char*));
  MOCK_METHOD1(IsClosed, bool(const WebsocketObject*));
  MOCK_METHOD1(GetCloseError, int32_t(const WebsocketObject*));
  MOCK_METHOD1(ScheduleWrite, void(WebsocketObject*));
};

typedef struct WebsocketMock {
  /** Implements the Websocket Interface */
  WebsocketObject obj;
  WebsocketGMock* gmock;
} WebsocketMock;

#define WEBSOCKET_MOCK(obj) ((WebsocketMock*)(obj))

WebsocketMock* WebsocketMockCreate(void);

static inline void WebsocketMockDelete(WebsocketMock* self) {
  if (self != nullptr) {
    WEBSOCKET_DESTRUCT(WEBSOCKET_OBJECT(self));
    EEBUS_FREE(self);
  }
}

#endif  // TESTS_UNIT_TESTS_MOCKS_SHIP_WEBSOCKET_WEBSOCKET_MOCK_H_
