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
 * @brief Cem Ohpcf Listener mock implementation
 */

#include "cem_ohpcf_listener_mock.h"

#include <gmock/gmock.h>

#include "src/use_case/api/cem_ohpcf_listener_interface.h"

static void Destruct(CemOhpcfListenerObject* self);
static void OnRemoteCompressorAdded(CemOhpcfListenerObject* self, const EntityAddressType* entity_addr);
static void OnRemoteCompressorRemoved(CemOhpcfListenerObject* self, const EntityAddressType* entity_addr);
static void OnAnnounce(
    CemOhpcfListenerObject* self,
    const OptionalPowerConsumption* optional_power_consumption,
    const EntityAddressType* entity_addr
);
static void OnStateReport(
    CemOhpcfListenerObject* self,
    CompressorOhpcfState state,
    const EebusDuration* start_time,
    const EntityAddressType* entity_addr
);
static void OnClearProcess(CemOhpcfListenerObject* self, const EntityAddressType* entity_addr);

static const CemOhpcfListenerInterface cem_ohpcf_listener_methods = {
    .destruct                     = Destruct,
    .on_remote_compressor_added   = OnRemoteCompressorAdded,
    .on_remote_compressor_removed = OnRemoteCompressorRemoved,
    .on_announce                  = OnAnnounce,
    .on_state_report              = OnStateReport,
    .on_clear_process             = OnClearProcess,
};

static EebusError CemOhpcfListenerMockConstruct(CemOhpcfListenerMock* self);

EebusError CemOhpcfListenerMockConstruct(CemOhpcfListenerMock* self) {
  // Override "virtual functions table"
  CEM_OHPCF_LISTENER_INTERFACE(self) = &cem_ohpcf_listener_methods;

  self->gmock = new CemOhpcfListenerGMock();
  if (self->gmock == nullptr) {
    return kEebusErrorMemoryAllocate;
  }

  return kEebusErrorOk;
}

CemOhpcfListenerMock* CemOhpcfListenerMockCreate(void) {
  CemOhpcfListenerMock* const mock = (CemOhpcfListenerMock*)EEBUS_MALLOC(sizeof(CemOhpcfListenerMock));
  if (mock == nullptr) {
    return nullptr;
  }

  if (CemOhpcfListenerMockConstruct(mock) != kEebusErrorOk) {
    CemOhpcfListenerMockDelete(mock);
    return nullptr;
  }

  return mock;
}

void Destruct(CemOhpcfListenerObject* self) {
  CemOhpcfListenerMock* const mock = CEM_OHPCF_LISTENER_MOCK(self);
  mock->gmock->Destruct(self);
  delete mock->gmock;
}

void OnRemoteCompressorAdded(CemOhpcfListenerObject* self, const EntityAddressType* entity_addr) {
  CemOhpcfListenerMock* const mock = CEM_OHPCF_LISTENER_MOCK(self);
  mock->gmock->OnRemoteCompressorAdded(self, entity_addr);
}

void OnRemoteCompressorRemoved(CemOhpcfListenerObject* self, const EntityAddressType* entity_addr) {
  CemOhpcfListenerMock* const mock = CEM_OHPCF_LISTENER_MOCK(self);
  mock->gmock->OnRemoteCompressorRemoved(self, entity_addr);
}

void OnAnnounce(
    CemOhpcfListenerObject* self,
    const OptionalPowerConsumption* optional_power_consumption,
    const EntityAddressType* entity_addr
) {
  CemOhpcfListenerMock* const mock = CEM_OHPCF_LISTENER_MOCK(self);
  mock->gmock->OnAnnounce(self, optional_power_consumption, entity_addr);
}

void OnStateReport(
    CemOhpcfListenerObject* self,
    CompressorOhpcfState state,
    const EebusDuration* start_time,
    const EntityAddressType* entity_addr
) {
  CemOhpcfListenerMock* const mock = CEM_OHPCF_LISTENER_MOCK(self);
  mock->gmock->OnStateReport(self, state, start_time, entity_addr);
}

void OnClearProcess(CemOhpcfListenerObject* self, const EntityAddressType* entity_addr) {
  CemOhpcfListenerMock* const mock = CEM_OHPCF_LISTENER_MOCK(self);
  mock->gmock->OnClearProcess(self, entity_addr);
}
