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
 * @brief Websocket mock implementation
 */

#include "websocket_mock.h"

#include <gmock/gmock.h>

#include "src/common/eebus_errors.h"
#include "src/ship/api/websocket_interface.h"

static void Destruct(WebsocketObject* self);
static int32_t Write(WebsocketObject* self, const uint8_t* msg, size_t msg_size);
static void Close(WebsocketObject* self, int32_t close_code, const char* reason);
static bool IsClosed(const WebsocketObject* self);
static int32_t GetCloseError(const WebsocketObject* self);
static void ScheduleWrite(WebsocketObject* self);

static const WebsocketInterface websocket_methods = {
    .destruct        = Destruct,
    .write           = Write,
    .close           = Close,
    .is_closed       = IsClosed,
    .get_close_error = GetCloseError,
    .schedule_write  = ScheduleWrite,
};

static EebusError WebsocketMockConstruct(WebsocketMock* self);

EebusError WebsocketMockConstruct(WebsocketMock* self) {
  // Override "virtual functions table"
  WEBSOCKET_INTERFACE(self) = &websocket_methods;

  self->gmock = new WebsocketGMock();
  if (self->gmock == nullptr) {
    return kEebusErrorMemoryAllocate;
  }

  return kEebusErrorOk;
}

WebsocketMock* WebsocketMockCreate(void) {
  WebsocketMock* const mock = (WebsocketMock*)EEBUS_MALLOC(sizeof(WebsocketMock));
  if (mock == nullptr) {
    return nullptr;
  }

  if (WebsocketMockConstruct(mock) != kEebusErrorOk) {
    WebsocketMockDelete(mock);
    return nullptr;
  }

  return mock;
}

void Destruct(WebsocketObject* self) {
  WebsocketMock* const mock = WEBSOCKET_MOCK(self);
  mock->gmock->Destruct(self);
  delete mock->gmock;
}

int32_t Write(WebsocketObject* self, const uint8_t* msg, size_t msg_size) {
  WebsocketMock* const mock = WEBSOCKET_MOCK(self);
  return mock->gmock->Write(self, msg, msg_size);
}

void Close(WebsocketObject* self, int32_t close_code, const char* reason) {
  WebsocketMock* const mock = WEBSOCKET_MOCK(self);
  mock->gmock->Close(self, close_code, reason);
}

bool IsClosed(const WebsocketObject* self) {
  WebsocketMock* const mock = WEBSOCKET_MOCK(self);
  return mock->gmock->IsClosed(self);
}

int32_t GetCloseError(const WebsocketObject* self) {
  WebsocketMock* const mock = WEBSOCKET_MOCK(self);
  return mock->gmock->GetCloseError(self);
}

void ScheduleWrite(WebsocketObject* self) {
  WebsocketMock* const mock = WEBSOCKET_MOCK(self);
  mock->gmock->ScheduleWrite(self);
}
