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
 * @brief EEBUS Service mock implementation
 */

#include "eebus_service_mock.h"

#include <gmock/gmock.h>

#include "src/service/api/eebus_service_interface.h"

static void Destruct(ShipNodeReaderObject* self);
static void OnRemoteSkiConnected(ShipNodeReaderObject* self, const char* ski);
static void OnRemoteSkiDisconnected(ShipNodeReaderObject* self, const char* ski);
static DataReaderObject* SetupRemoteDevice(ShipNodeReaderObject* self, const char* ski, DataWriterObject* data_writer);
static void OnRemoteServicesUpdate(ShipNodeReaderObject* self, const Vector* entries);
static void OnShipIdUpdate(ShipNodeReaderObject* self, const char* ski, const char* ship_id);
static void OnShipStateUpdate(ShipNodeReaderObject* self, const char* ski, SmeState state);
static bool IsWaitingForTrustAllowed(ShipNodeReaderObject* self, const char* ski);
static void Start(EebusServiceObject* self);
static void Stop(EebusServiceObject* self);
static const ServiceDetails* GetLocalService(const EebusServiceObject* self);
static DeviceLocalObject* GetLocalDevice(const EebusServiceObject* self);
static const ConnectionState* GetConnectionStateWithSki(const EebusServiceObject* self, const char* ski);
static const ServiceDetails* GetRemoteServiceDetailsWithSki(const EebusServiceObject* self, const char* ski);
static void RegisterRemoteSki(EebusServiceObject* self, const char* ski, bool enable);
static void UnregisterRemoteSki(EebusServiceObject* self, const char* ski);
static void CancelPairingWithSki(EebusServiceObject* self, const char* ski);
static void SetPairingPossible(EebusServiceObject* self, bool is_pairing_possible);
static const char* GetLocalSki(EebusServiceObject* self);
static const char* GetQrCodeString(EebusServiceObject* self);

static const EebusServiceInterface eebus_service_methods = {
    .ship_node_reader_interface = {
        .destruct                     = Destruct,
        .on_remote_ski_connected      = OnRemoteSkiConnected,
        .on_remote_ski_disconnected   = OnRemoteSkiDisconnected,
        .setup_remote_device          = SetupRemoteDevice,
        .on_remote_services_update    = OnRemoteServicesUpdate,
        .on_ship_id_update            = OnShipIdUpdate,
        .on_ship_state_update         = OnShipStateUpdate,
        .is_waiting_for_trust_allowed = IsWaitingForTrustAllowed,
    },

    .start                               = Start,
    .stop                                = Stop,
    .get_local_service                   = GetLocalService,
    .get_local_device                    = GetLocalDevice,
    .get_connection_state_with_ski       = GetConnectionStateWithSki,
    .get_remote_service_details_with_ski = GetRemoteServiceDetailsWithSki,
    .register_remote_ski                 = RegisterRemoteSki,
    .unregister_remote_ski               = UnregisterRemoteSki,
    .cancel_pairing_with_ski             = CancelPairingWithSki,
    .set_pairing_possible                = SetPairingPossible,
    .get_local_ski                       = GetLocalSki,
    .get_qr_code_string                  = GetQrCodeString,
};

static EebusError EebusServiceMockConstruct(EebusServiceMock* self);

EebusError EebusServiceMockConstruct(EebusServiceMock* self) {
  // Override "virtual functions table"
  EEBUS_SERVICE_INTERFACE(self) = &eebus_service_methods;

  self->gmock = new EebusServiceGMock();
  if (self->gmock == nullptr) {
    return kEebusErrorMemoryAllocate;
  }

  return kEebusErrorOk;
}

EebusServiceMock* EebusServiceMockCreate(void) {
  EebusServiceMock* const mock = (EebusServiceMock*)EEBUS_MALLOC(sizeof(EebusServiceMock));
  if (mock == nullptr) {
    return nullptr;
  }

  if (EebusServiceMockConstruct(mock) != kEebusErrorOk) {
    EebusServiceMockDelete(mock);
    return nullptr;
  }

  return mock;
}

void Destruct(ShipNodeReaderObject* self) {
  EebusServiceMock* const mock = EEBUS_SERVICE_MOCK(self);
  mock->gmock->Destruct(self);
  delete mock->gmock;
}

void OnRemoteSkiConnected(ShipNodeReaderObject* self, const char* ski) {
  EebusServiceMock* const mock = EEBUS_SERVICE_MOCK(self);
  mock->gmock->OnRemoteSkiConnected(self, ski);
}

void OnRemoteSkiDisconnected(ShipNodeReaderObject* self, const char* ski) {
  EebusServiceMock* const mock = EEBUS_SERVICE_MOCK(self);
  mock->gmock->OnRemoteSkiDisconnected(self, ski);
}

DataReaderObject* SetupRemoteDevice(ShipNodeReaderObject* self, const char* ski, DataWriterObject* data_writer) {
  EebusServiceMock* const mock = EEBUS_SERVICE_MOCK(self);
  return mock->gmock->SetupRemoteDevice(self, ski, data_writer);
}

void OnRemoteServicesUpdate(ShipNodeReaderObject* self, const Vector* entries) {
  EebusServiceMock* const mock = EEBUS_SERVICE_MOCK(self);
  mock->gmock->OnRemoteServicesUpdate(self, entries);
}

void OnShipIdUpdate(ShipNodeReaderObject* self, const char* ski, const char* ship_id) {
  EebusServiceMock* const mock = EEBUS_SERVICE_MOCK(self);
  mock->gmock->OnShipIdUpdate(self, ski, ship_id);
}

void OnShipStateUpdate(ShipNodeReaderObject* self, const char* ski, SmeState state) {
  EebusServiceMock* const mock = EEBUS_SERVICE_MOCK(self);
  mock->gmock->OnShipStateUpdate(self, ski, state);
}

bool IsWaitingForTrustAllowed(ShipNodeReaderObject* self, const char* ski) {
  EebusServiceMock* const mock = EEBUS_SERVICE_MOCK(self);
  return mock->gmock->IsWaitingForTrustAllowed(self, ski);
}

void Start(EebusServiceObject* self) {
  EebusServiceMock* const mock = EEBUS_SERVICE_MOCK(self);
  mock->gmock->Start(self);
}

void Stop(EebusServiceObject* self) {
  EebusServiceMock* const mock = EEBUS_SERVICE_MOCK(self);
  mock->gmock->Stop(self);
}

const ServiceDetails* GetLocalService(const EebusServiceObject* self) {
  EebusServiceMock* const mock = EEBUS_SERVICE_MOCK(self);
  return mock->gmock->GetLocalService(self);
}

DeviceLocalObject* GetLocalDevice(const EebusServiceObject* self) {
  EebusServiceMock* const mock = EEBUS_SERVICE_MOCK(self);
  return mock->gmock->GetLocalDevice(self);
}

const ConnectionState* GetConnectionStateWithSki(const EebusServiceObject* self, const char* ski) {
  EebusServiceMock* const mock = EEBUS_SERVICE_MOCK(self);
  return mock->gmock->GetConnectionStateWithSki(self, ski);
}

const ServiceDetails* GetRemoteServiceDetailsWithSki(const EebusServiceObject* self, const char* ski) {
  EebusServiceMock* const mock = EEBUS_SERVICE_MOCK(self);
  return mock->gmock->GetRemoteServiceDetailsWithSki(self, ski);
}

void RegisterRemoteSki(EebusServiceObject* self, const char* ski, bool enable) {
  EebusServiceMock* const mock = EEBUS_SERVICE_MOCK(self);
  mock->gmock->RegisterRemoteSki(self, ski, enable);
}

void UnregisterRemoteSki(EebusServiceObject* self, const char* ski) {
  EebusServiceMock* const mock = EEBUS_SERVICE_MOCK(self);
  mock->gmock->UnregisterRemoteSki(self, ski);
}

void CancelPairingWithSki(EebusServiceObject* self, const char* ski) {
  EebusServiceMock* const mock = EEBUS_SERVICE_MOCK(self);
  mock->gmock->CancelPairingWithSki(self, ski);
}

void SetPairingPossible(EebusServiceObject* self, bool is_pairing_possible) {
  EebusServiceMock* const mock = EEBUS_SERVICE_MOCK(self);
  mock->gmock->SetPairingPossible(self, is_pairing_possible);
}

const char* GetLocalSki(EebusServiceObject* self) {
  EebusServiceMock* const mock = EEBUS_SERVICE_MOCK(self);
  return mock->gmock->GetLocalSki(self);
}

const char* GetQrCodeString(EebusServiceObject* self) {
  EebusServiceMock* const mock = EEBUS_SERVICE_MOCK(self);
  return mock->gmock->GetQrCodeString(self);
}
