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
 * @brief Websocket Creator Mock "class"
 */

#ifndef TESTS_UNIT_TESTS_MOCKS_SHIP_WEBSOCKET_WEBSOCKET_CREATOR_MOCK_H_
#define TESTS_UNIT_TESTS_MOCKS_SHIP_WEBSOCKET_WEBSOCKET_CREATOR_MOCK_H_

#include <gmock/gmock.h>

#include <memory>

#include "src/common/eebus_malloc.h"
#include "src/ship/api/websocket_creator_interface.h"

class WebsocketCreatorGMockInterface {
 public:
  virtual ~WebsocketCreatorGMockInterface() {};
  virtual void Destruct(WebsocketCreatorObject* self)                                                     = 0;
  virtual WebsocketObject* CreateWebsocket(WebsocketCreatorObject* self, WebsocketCallback cb, void* ctx) = 0;
};

class WebsocketCreatorGMock : public WebsocketCreatorGMockInterface {
 public:
  virtual ~WebsocketCreatorGMock() {};
  MOCK_METHOD1(Destruct, void(WebsocketCreatorObject*));
  MOCK_METHOD3(CreateWebsocket, WebsocketObject*(WebsocketCreatorObject*, WebsocketCallback, void*));
};

typedef struct WebsocketCreatorMock {
  /** Implements the Websocket Creator Interface */
  WebsocketCreatorObject obj;
  WebsocketCreatorGMock* gmock;
} WebsocketCreatorMock;

#define WEBSOCKET_CREATOR_MOCK(obj) ((WebsocketCreatorMock*)(obj))

WebsocketCreatorMock* WebsocketCreatorMockCreate(void);

static inline void WebsocketCreatorMockDelete(WebsocketCreatorMock* self) {
  if (self != nullptr) {
    WEBSOCKET_CREATOR_DESTRUCT(WEBSOCKET_CREATOR_OBJECT(self));
    EEBUS_FREE(self);
  }
}

#endif  // TESTS_UNIT_TESTS_MOCKS_SHIP_WEBSOCKET_WEBSOCKET_CREATOR_MOCK_H_
