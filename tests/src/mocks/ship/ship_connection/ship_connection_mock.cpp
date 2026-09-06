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
 * @brief Ship Connection mock implementation
 */

#include "ship_connection_mock.h"

#include <gmock/gmock.h>

#include "src/common/eebus_errors.h"
#include "src/ship/api/ship_connection_interface.h"

static void Destruct(DataWriterObject* self);
static void WriteMessage(DataWriterObject* self, const uint8_t* msg, size_t msg_size);
static EebusError Start(ShipConnectionObject* self, WebsocketCreatorObject* wsc);
static void Stop(ShipConnectionObject* self);
static WebsocketObject* GetWebsocketConnection(ShipConnectionObject* self);
static void CloseConnection(ShipConnectionObject* self, bool safe, int32_t code, const char* reason);
static const char* GetRemoteSki(ShipConnectionObject* self);
static void ApprovePendingHandshake(ShipConnectionObject* self);
static void AbortPendingHandshake(ShipConnectionObject* self);
static SmeState GetState(ShipConnectionObject* self, EebusError* err);

static const ShipConnectionInterface ship_connection_methods = {
    .data_writer_interface = {
        .destruct      = Destruct,
        .write_message = WriteMessage,
    },

    .start                     = Start,
    .stop                      = Stop,
    .get_websocket_connection  = GetWebsocketConnection,
    .close_connection          = CloseConnection,
    .get_remote_ski            = GetRemoteSki,
    .approve_pending_handshake = ApprovePendingHandshake,
    .abort_pending_handshake   = AbortPendingHandshake,
    .get_state                 = GetState,
};

static EebusError ShipConnectionMockConstruct(ShipConnectionMock* self);

EebusError ShipConnectionMockConstruct(ShipConnectionMock* self) {
  SHIP_CONNECTION_INTERFACE(self) = &ship_connection_methods;

  self->gmock = new ShipConnectionGMock();
  if (self->gmock == nullptr) {
    return kEebusErrorMemoryAllocate;
  }

  return kEebusErrorOk;
}

ShipConnectionMock* ShipConnectionMockCreate(void) {
  ShipConnectionMock* const mock = (ShipConnectionMock*)EEBUS_MALLOC(sizeof(ShipConnectionMock));
  if (mock == nullptr) {
    return nullptr;
  }

  if (ShipConnectionMockConstruct(mock) != kEebusErrorOk) {
    ShipConnectionMockDelete(mock);
    return nullptr;
  }

  return mock;
}

void Destruct(DataWriterObject* self) {
  ShipConnectionMock* const mock = SHIP_CONNECTION_MOCK(self);
  mock->gmock->Destruct(self);
  delete mock->gmock;
}

void WriteMessage(DataWriterObject* self, const uint8_t* msg, size_t msg_size) {
  ShipConnectionMock* const mock = SHIP_CONNECTION_MOCK(self);
  mock->gmock->WriteMessage(self, msg, msg_size);
}

EebusError Start(ShipConnectionObject* self, WebsocketCreatorObject* wsc) {
  ShipConnectionMock* const mock = SHIP_CONNECTION_MOCK(self);
  return mock->gmock->Start(self, wsc);
}

void Stop(ShipConnectionObject* self) {
  ShipConnectionMock* const mock = SHIP_CONNECTION_MOCK(self);
  mock->gmock->Stop(self);
}

WebsocketObject* GetWebsocketConnection(ShipConnectionObject* self) {
  ShipConnectionMock* const mock = SHIP_CONNECTION_MOCK(self);
  return mock->gmock->GetWebsocketConnection(self);
}

void CloseConnection(ShipConnectionObject* self, bool safe, int32_t code, const char* reason) {
  ShipConnectionMock* const mock = SHIP_CONNECTION_MOCK(self);
  mock->gmock->CloseConnection(self, safe, code, reason);
}

const char* GetRemoteSki(ShipConnectionObject* self) {
  ShipConnectionMock* const mock = SHIP_CONNECTION_MOCK(self);
  return mock->gmock->GetRemoteSki(self);
}

void ApprovePendingHandshake(ShipConnectionObject* self) {
  ShipConnectionMock* const mock = SHIP_CONNECTION_MOCK(self);
  mock->gmock->ApprovePendingHandshake(self);
}

void AbortPendingHandshake(ShipConnectionObject* self) {
  ShipConnectionMock* const mock = SHIP_CONNECTION_MOCK(self);
  mock->gmock->AbortPendingHandshake(self);
}

SmeState GetState(ShipConnectionObject* self, EebusError* err) {
  ShipConnectionMock* const mock = SHIP_CONNECTION_MOCK(self);
  return mock->gmock->GetState(self, err);
}
