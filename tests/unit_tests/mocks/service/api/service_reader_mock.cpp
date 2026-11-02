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
 * @brief Service Reader mock implementation
 */

#include "service_reader_mock.h"

#include <gmock/gmock.h>

#include "src/service/api/service_reader_interface.h"

static void Destruct(ServiceReaderObject* self);
static void OnRemoteSkiConnected(ServiceReaderObject* self, EebusServiceObject* service, const char* ski);
static void OnRemoteSkiDisconnected(ServiceReaderObject* self, EebusServiceObject* service, const char* ski);
static void OnRemoteServicesUpdate(ServiceReaderObject* self, EebusServiceObject* service, const Vector* entries);
static void OnShipIdUpdate(ServiceReaderObject* self, const char* ski, const char* shipd_id);
static void OnShipStateUpdate(ServiceReaderObject* self, const char* ski, SmeState state);
static bool IsWaitingForTrustAllowed(const ServiceReaderObject* self, const char* ski);

static const ServiceReaderInterface service_reader_methods = {
    .destruct                     = Destruct,
    .on_remote_ski_connected      = OnRemoteSkiConnected,
    .on_remote_ski_disconnected   = OnRemoteSkiDisconnected,
    .on_remote_services_update    = OnRemoteServicesUpdate,
    .on_ship_id_update            = OnShipIdUpdate,
    .on_ship_state_update         = OnShipStateUpdate,
    .is_waiting_for_trust_allowed = IsWaitingForTrustAllowed,
};

static EebusError ServiceReaderMockConstruct(ServiceReaderMock* self);

EebusError ServiceReaderMockConstruct(ServiceReaderMock* self) {
  // Override "virtual functions table"
  SERVICE_READER_INTERFACE(self) = &service_reader_methods;

  self->gmock = new ServiceReaderGMock();
  if (self->gmock == nullptr) {
    return kEebusErrorMemoryAllocate;
  }

  return kEebusErrorOk;
}

ServiceReaderMock* ServiceReaderMockCreate(void) {
  ServiceReaderMock* const mock = (ServiceReaderMock*)EEBUS_MALLOC(sizeof(ServiceReaderMock));
  if (mock == nullptr) {
    return nullptr;
  }

  if (ServiceReaderMockConstruct(mock) != kEebusErrorOk) {
    ServiceReaderMockDelete(mock);
    return nullptr;
  }

  return mock;
}

void Destruct(ServiceReaderObject* self) {
  ServiceReaderMock* const mock = SERVICE_READER_MOCK(self);
  mock->gmock->Destruct(self);
  delete mock->gmock;
}

void OnRemoteSkiConnected(ServiceReaderObject* self, EebusServiceObject* service, const char* ski) {
  ServiceReaderMock* const mock = SERVICE_READER_MOCK(self);
  mock->gmock->OnRemoteSkiConnected(self, service, ski);
}

void OnRemoteSkiDisconnected(ServiceReaderObject* self, EebusServiceObject* service, const char* ski) {
  ServiceReaderMock* const mock = SERVICE_READER_MOCK(self);
  mock->gmock->OnRemoteSkiDisconnected(self, service, ski);
}

void OnRemoteServicesUpdate(ServiceReaderObject* self, EebusServiceObject* service, const Vector* entries) {
  ServiceReaderMock* const mock = SERVICE_READER_MOCK(self);
  mock->gmock->OnRemoteServicesUpdate(self, service, entries);
}

void OnShipIdUpdate(ServiceReaderObject* self, const char* ski, const char* shipd_id) {
  ServiceReaderMock* const mock = SERVICE_READER_MOCK(self);
  mock->gmock->OnShipIdUpdate(self, ski, shipd_id);
}

void OnShipStateUpdate(ServiceReaderObject* self, const char* ski, SmeState state) {
  ServiceReaderMock* const mock = SERVICE_READER_MOCK(self);
  mock->gmock->OnShipStateUpdate(self, ski, state);
}

bool IsWaitingForTrustAllowed(const ServiceReaderObject* self, const char* ski) {
  ServiceReaderMock* const mock = SERVICE_READER_MOCK(self);
  return mock->gmock->IsWaitingForTrustAllowed(self, ski);
}
