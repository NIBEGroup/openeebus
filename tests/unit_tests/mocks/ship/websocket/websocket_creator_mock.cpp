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
 * @brief Websocket Creator mock implementation
 */

#include "websocket_creator_mock.h"

#include <gmock/gmock.h>

#include "src/common/eebus_arguments.h"
#include "src/common/eebus_errors.h"
#include "src/ship/api/websocket_creator_interface.h"
#include "websocket_mock.h"

static void Destruct(WebsocketCreatorObject* self);
static WebsocketObject* CreateWebsocket(WebsocketCreatorObject* self, WebsocketCallback cb, void* ctx);

static const WebsocketCreatorInterface websocket_creator_methods = {
    .destruct         = Destruct,
    .create_websocket = CreateWebsocket,
};

static EebusError WebsocketCreatorMockConstruct(WebsocketCreatorMock* self);

EebusError WebsocketCreatorMockConstruct(WebsocketCreatorMock* self) {
  // Override "virtual functions table"
  WEBSOCKET_CREATOR_INTERFACE(self) = &websocket_creator_methods;

  self->gmock = new WebsocketCreatorGMock();
  if (self->gmock == nullptr) {
    return kEebusErrorMemoryAllocate;
  }

  return kEebusErrorOk;
}

WebsocketCreatorMock* WebsocketCreatorMockCreate(void) {
  WebsocketCreatorMock* const mock = (WebsocketCreatorMock*)EEBUS_MALLOC(sizeof(WebsocketCreatorMock));
  if (mock == nullptr) {
    return nullptr;
  }

  if (WebsocketCreatorMockConstruct(mock) != kEebusErrorOk) {
    WebsocketCreatorMockDelete(mock);
    return nullptr;
  }

  return mock;
}

void Destruct(WebsocketCreatorObject* self) {
  WebsocketCreatorMock* const mock = WEBSOCKET_CREATOR_MOCK(self);
  mock->gmock->Destruct(self);
  delete mock->gmock;
}

WebsocketObject* CreateWebsocket(WebsocketCreatorObject* self, WebsocketCallback cb, void* ctx) {
  UNUSED(self);
  UNUSED(cb);
  UNUSED(ctx);
  return WEBSOCKET_OBJECT(WebsocketMockCreate());
}
