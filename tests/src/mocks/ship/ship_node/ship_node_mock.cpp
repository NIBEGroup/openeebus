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
 * @brief Ship Node mock implementation
 */

#include "ship_node_mock.h"

#include <gmock/gmock.h>

#include "src/ship/api/ship_node_interface.h"

void Destruct(InfoProviderObject* self);
bool IsRemoteServiceForSkiPaired(InfoProviderObject* self, const char* ski);
void HandleConnectionClosed(InfoProviderObject* self, ShipConnectionObject* sc, bool had_error);
void ReportServiceShipId(InfoProviderObject* self, const char* service_id, const char* ship_id);
bool IsWaitingForTrustAllowed(InfoProviderObject*, const char* ski);
void HandleShipStateUpdate(InfoProviderObject* self, const char* ski, SmeState state, const char* err);
DataReaderObject* SetupRemoteDevice(InfoProviderObject* self, const char* ski, DataWriterObject* data_writer);
static void Start(ShipNodeObject* self);
static void Stop(ShipNodeObject* self);
static void RegisterRemoteSki(ShipNodeObject* self, const char* ski, bool is_trusted);
static void UnregisterRemoteSki(ShipNodeObject* self, const char* ski);
static void CancelPairingWithSki(ShipNodeObject* self, const char* ski);
static void ApprovePendingHandshakeWithSki(ShipNodeObject* self, const char* ski);
static uint32_t GetPendingWaitingMsWithSki(ShipNodeObject* self, const char* ski);

static const ShipNodeInterface ship_node_methods = {
    .info_provider_interface = {
        .destruct                         = Destruct,
        .is_remote_service_for_ski_paired = IsRemoteServiceForSkiPaired,
        .handle_connection_closed         = HandleConnectionClosed,
        .report_service_ship_id           = ReportServiceShipId,
        .is_waiting_for_trust_allowed     = IsWaitingForTrustAllowed,
        .handle_ship_state_update         = HandleShipStateUpdate,
        .setup_remote_device              = SetupRemoteDevice,
    },

    .start                              = Start,
    .stop                               = Stop,
    .register_remote_ski                = RegisterRemoteSki,
    .unregister_remote_ski              = UnregisterRemoteSki,
    .cancel_pairing_with_ski            = CancelPairingWithSki,
    .approve_pending_handshake_with_ski = ApprovePendingHandshakeWithSki,
    .get_pending_waiting_ms_with_ski    = GetPendingWaitingMsWithSki,
};

static EebusError ShipNodeMockConstruct(ShipNodeMock* self);

EebusError ShipNodeMockConstruct(ShipNodeMock* self) {
  // Override "virtual functions table"
  SHIP_NODE_INTERFACE(self) = &ship_node_methods;

  self->gmock = new ShipNodeGMock();
  if (self->gmock == nullptr) {
    return kEebusErrorMemoryAllocate;
  }

  return kEebusErrorOk;
}

ShipNodeMock* ShipNodeMockCreate(void) {
  ShipNodeMock* const mock = (ShipNodeMock*)EEBUS_MALLOC(sizeof(ShipNodeMock));
  if (mock == nullptr) {
    return nullptr;
  }

  if (ShipNodeMockConstruct(mock) != kEebusErrorOk) {
    ShipNodeMockDelete(mock);
    return nullptr;
  }

  return mock;
}

void Destruct(InfoProviderObject* self) {
  ShipNodeMock* const mock = SHIP_NODE_MOCK(self);
  mock->gmock->Destruct(self);
  delete mock->gmock;
}

bool IsRemoteServiceForSkiPaired(InfoProviderObject* self, const char* ski) {
  ShipNodeMock* const mock = SHIP_NODE_MOCK(self);
  return mock->gmock->IsRemoteServiceForSkiPaired(self, ski);
}

void HandleConnectionClosed(InfoProviderObject* self, ShipConnectionObject* sc, bool had_error) {
  ShipNodeMock* const mock = SHIP_NODE_MOCK(self);
  mock->gmock->HandleConnectionClosed(self, sc, had_error);
}

void ReportServiceShipId(InfoProviderObject* self, const char* service_id, const char* ship_id) {
  ShipNodeMock* const mock = SHIP_NODE_MOCK(self);
  mock->gmock->ReportServiceShipId(self, service_id, ship_id);
}

bool IsWaitingForTrustAllowed(InfoProviderObject* self, const char* ski) {
  ShipNodeMock* const mock = SHIP_NODE_MOCK(self);
  return mock->gmock->IsWaitingForTrustAllowed(self, ski);
}

void HandleShipStateUpdate(InfoProviderObject* self, const char* ski, SmeState state, const char* err) {
  ShipNodeMock* const mock = SHIP_NODE_MOCK(self);
  mock->gmock->HandleShipStateUpdate(self, ski, state, err);
}

DataReaderObject* SetupRemoteDevice(InfoProviderObject* self, const char* ski, DataWriterObject* data_writer) {
  ShipNodeMock* const mock = SHIP_NODE_MOCK(self);
  return mock->gmock->SetupRemoteDevice(self, ski, data_writer);
}

void Start(ShipNodeObject* self) {
  ShipNodeMock* const mock = SHIP_NODE_MOCK(self);
  mock->gmock->Start(self);
}

void Stop(ShipNodeObject* self) {
  ShipNodeMock* const mock = SHIP_NODE_MOCK(self);
  mock->gmock->Stop(self);
}

void RegisterRemoteSki(ShipNodeObject* self, const char* ski, bool is_trusted) {
  ShipNodeMock* const mock = SHIP_NODE_MOCK(self);
  mock->gmock->RegisterRemoteSki(self, ski, is_trusted);
}

void UnregisterRemoteSki(ShipNodeObject* self, const char* ski) {
  ShipNodeMock* const mock = SHIP_NODE_MOCK(self);
  mock->gmock->UnregisterRemoteSki(self, ski);
}

void CancelPairingWithSki(ShipNodeObject* self, const char* ski) {
  ShipNodeMock* const mock = SHIP_NODE_MOCK(self);
  mock->gmock->CancelPairingWithSki(self, ski);
}

void ApprovePendingHandshakeWithSki(ShipNodeObject* self, const char* ski) {
  ShipNodeMock* const mock = SHIP_NODE_MOCK(self);
  mock->gmock->ApprovePendingHandshakeWithSki(self, ski);
}

uint32_t GetPendingWaitingMsWithSki(ShipNodeObject* self, const char* ski) {
  ShipNodeMock* const mock = SHIP_NODE_MOCK(self);
  return mock->gmock->GetPendingWaitingMsWithSki(self, ski);
}
