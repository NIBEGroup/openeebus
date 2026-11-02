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
#include "info_provider_mock.h"

#include <gmock/gmock.h>

#include <memory>

#include "src/ship/api/info_provider_interface.h"

void Destruct(InfoProviderObject* self);
bool IsRemoteServiceForSkiPaired(InfoProviderObject* self, const char* ski);
void HandleConnectionClosed(InfoProviderObject* self, ShipConnectionObject* sc, bool had_error);
void ReportServiceShipId(InfoProviderObject* self, const char* service_id, const char* ship_id);
bool IsWaitingForTrustAllowed(InfoProviderObject*, const char* ski);
void HandleShipStateUpdate(InfoProviderObject* self, const char* ski, SmeState state, const char* err);
DataReaderObject* SetupRemoteDevice(InfoProviderObject* self, const char* ski, DataWriterObject* data_writer);

static const InfoProviderInterface info_provider_mock_methods = {
    .destruct                         = Destruct,
    .is_remote_service_for_ski_paired = IsRemoteServiceForSkiPaired,
    .handle_connection_closed         = HandleConnectionClosed,
    .report_service_ship_id           = ReportServiceShipId,
    .is_waiting_for_trust_allowed     = IsWaitingForTrustAllowed,
    .handle_ship_state_update         = HandleShipStateUpdate,
    .setup_remote_device              = SetupRemoteDevice,
};

static EebusError InfoProviderMockConstruct(InfoProviderMock* self);

EebusError InfoProviderMockConstruct(InfoProviderMock* self) {
  // Override "virtual functions table"
  INFO_PROVIDER_INTERFACE(self) = &info_provider_mock_methods;

  self->gmock = new InfoProviderGMock();
  if (self->gmock == nullptr) {
    return kEebusErrorMemoryAllocate;
  }

  return kEebusErrorOk;
}

InfoProviderMock* InfoProviderMockCreate(void) {
  InfoProviderMock* const mock = (InfoProviderMock*)EEBUS_MALLOC(sizeof(InfoProviderMock));
  if (mock == nullptr) {
    return nullptr;
  }

  if (InfoProviderMockConstruct(mock) != kEebusErrorOk) {
    InfoProviderMockDelete(mock);
    return nullptr;
  }

  return mock;
}

void Destruct(InfoProviderObject* self) {
  InfoProviderMock* const mock = INFO_PROVIDER_MOCK(self);
  mock->gmock->Destruct(self);
  delete mock->gmock;
}

bool IsRemoteServiceForSkiPaired(InfoProviderObject* self, const char* ski) {
  InfoProviderMock* const mock = INFO_PROVIDER_MOCK(self);
  return mock->gmock->IsRemoteServiceForSkiPaired(self, ski);
}

void HandleConnectionClosed(InfoProviderObject* self, ShipConnectionObject* sc, bool had_error) {
  InfoProviderMock* const mock = INFO_PROVIDER_MOCK(self);
  mock->gmock->HandleConnectionClosed(self, sc, had_error);
}

void ReportServiceShipId(InfoProviderObject* self, const char* service_id, const char* ship_id) {
  InfoProviderMock* const mock = INFO_PROVIDER_MOCK(self);
  mock->gmock->ReportServiceShipId(self, service_id, ship_id);
}

bool IsWaitingForTrustAllowed(InfoProviderObject* self, const char* ski) {
  InfoProviderMock* const mock = INFO_PROVIDER_MOCK(self);
  return mock->gmock->IsWaitingForTrustAllowed(self, ski);
}

void HandleShipStateUpdate(InfoProviderObject* self, const char* ski, SmeState state, const char* err) {
  InfoProviderMock* const mock = INFO_PROVIDER_MOCK(self);
  mock->gmock->HandleShipStateUpdate(self, ski, state, err);
}

DataReaderObject* SetupRemoteDevice(InfoProviderObject* self, const char* ski, DataWriterObject* data_writer) {
  InfoProviderMock* const mock = INFO_PROVIDER_MOCK(self);
  return mock->gmock->SetupRemoteDevice(self, ski, data_writer);
}
