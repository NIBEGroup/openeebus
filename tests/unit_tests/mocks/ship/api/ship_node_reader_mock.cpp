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
 * @brief Ship Node Reader mock implementation
 */

#include "ship_node_reader_mock.h"

#include <gmock/gmock.h>

#include "src/ship/api/ship_node_reader_interface.h"

static void Destruct(ShipNodeReaderObject* self);
static void OnRemoteSkiConnected(ShipNodeReaderObject* self, const char* ski);
static void OnRemoteSkiDisconnected(ShipNodeReaderObject* self, const char* ski);
static DataReaderObject* SetupRemoteDevice(ShipNodeReaderObject* self, const char* ski, DataWriterObject* data_writer);
static void OnRemoteServicesUpdate(ShipNodeReaderObject* self, const Vector* entries);
static void OnShipIdUpdate(ShipNodeReaderObject* self, const char* ski, const char* ship_id);
static void OnShipStateUpdate(ShipNodeReaderObject* self, const char* ski, SmeState state);
static bool IsWaitingForTrustAllowed(ShipNodeReaderObject* self, const char* ski);

static const ShipNodeReaderInterface ship_node_reader_methods = {
    .destruct                     = Destruct,
    .on_remote_ski_connected      = OnRemoteSkiConnected,
    .on_remote_ski_disconnected   = OnRemoteSkiDisconnected,
    .setup_remote_device          = SetupRemoteDevice,
    .on_remote_services_update    = OnRemoteServicesUpdate,
    .on_ship_id_update            = OnShipIdUpdate,
    .on_ship_state_update         = OnShipStateUpdate,
    .is_waiting_for_trust_allowed = IsWaitingForTrustAllowed,
};

static EebusError ShipNodeReaderMockConstruct(ShipNodeReaderMock* self);

EebusError ShipNodeReaderMockConstruct(ShipNodeReaderMock* self) {
  // Override "virtual functions table"
  SHIP_NODE_READER_INTERFACE(self) = &ship_node_reader_methods;

  self->gmock = new ShipNodeReaderGMock();
  if (self->gmock == nullptr) {
    return kEebusErrorMemoryAllocate;
  }

  return kEebusErrorOk;
}

ShipNodeReaderMock* ShipNodeReaderMockCreate(void) {
  ShipNodeReaderMock* const mock = (ShipNodeReaderMock*)EEBUS_MALLOC(sizeof(ShipNodeReaderMock));
  if (mock == nullptr) {
    return nullptr;
  }

  if (ShipNodeReaderMockConstruct(mock) != kEebusErrorOk) {
    ShipNodeReaderMockDelete(mock);
    return nullptr;
  }

  return mock;
}

void Destruct(ShipNodeReaderObject* self) {
  ShipNodeReaderMock* const mock = SHIP_NODE_READER_MOCK(self);
  mock->gmock->Destruct(self);
  delete mock->gmock;
}

void OnRemoteSkiConnected(ShipNodeReaderObject* self, const char* ski) {
  ShipNodeReaderMock* const mock = SHIP_NODE_READER_MOCK(self);
  mock->gmock->OnRemoteSkiConnected(self, ski);
}

void OnRemoteSkiDisconnected(ShipNodeReaderObject* self, const char* ski) {
  ShipNodeReaderMock* const mock = SHIP_NODE_READER_MOCK(self);
  mock->gmock->OnRemoteSkiDisconnected(self, ski);
}

DataReaderObject* SetupRemoteDevice(ShipNodeReaderObject* self, const char* ski, DataWriterObject* data_writer) {
  ShipNodeReaderMock* const mock = SHIP_NODE_READER_MOCK(self);
  return mock->gmock->SetupRemoteDevice(self, ski, data_writer);
}

void OnRemoteServicesUpdate(ShipNodeReaderObject* self, const Vector* entries) {
  ShipNodeReaderMock* const mock = SHIP_NODE_READER_MOCK(self);
  mock->gmock->OnRemoteServicesUpdate(self, entries);
}

void OnShipIdUpdate(ShipNodeReaderObject* self, const char* ski, const char* ship_id) {
  ShipNodeReaderMock* const mock = SHIP_NODE_READER_MOCK(self);
  mock->gmock->OnShipIdUpdate(self, ski, ship_id);
}

void OnShipStateUpdate(ShipNodeReaderObject* self, const char* ski, SmeState state) {
  ShipNodeReaderMock* const mock = SHIP_NODE_READER_MOCK(self);
  mock->gmock->OnShipStateUpdate(self, ski, state);
}

bool IsWaitingForTrustAllowed(ShipNodeReaderObject* self, const char* ski) {
  ShipNodeReaderMock* const mock = SHIP_NODE_READER_MOCK(self);
  return mock->gmock->IsWaitingForTrustAllowed(self, ski);
}
