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
 * @brief Mdns mock implementation
 */

#include "mdns_mock.h"

#include <gmock/gmock.h>

#include "src/ship/api/ship_mdns_interface.h"

static void Destruct(ShipMdnsObject* self);
static EebusError Start(ShipMdnsObject* self);
static void Stop(ShipMdnsObject* self);
static EebusError RegisterService(ShipMdnsObject* self);
static void DeregisterService(ShipMdnsObject* self);
static void SetAutoaccept(ShipMdnsObject* self, bool autoaccept);
static EebusError StartPairingBrowse(ShipMdnsObject* self, OnShipPairingEntriesFoundCallback cb, void* ctx);
static void StopPairingBrowse(ShipMdnsObject* self);
static EebusError RegisterPairingService(ShipMdnsObject* self, const ShipPairingEntry* entry);
static void DeregisterPairingService(ShipMdnsObject* self);
static bool SupportsPairing(ShipMdnsObject* self);

static const ShipMdnsInterface mdns_methods = {
    .destruct                   = Destruct,
    .start                      = Start,
    .stop                       = Stop,
    .register_service           = RegisterService,
    .deregister_service         = DeregisterService,
    .set_autoaccept             = SetAutoaccept,
    .start_pairing_browse       = StartPairingBrowse,
    .stop_pairing_browse        = StopPairingBrowse,
    .register_pairing_service   = RegisterPairingService,
    .deregister_pairing_service = DeregisterPairingService,
    .supports_pairing           = SupportsPairing,
};

static EebusError MdnsMockConstruct(MdnsMock* self);

EebusError MdnsMockConstruct(MdnsMock* self) {
  // Override "virtual functions table"
  SHIP_MDNS_INTERFACE(self) = &mdns_methods;

  self->gmock = new MdnsGMock();
  if (self->gmock == nullptr) {
    return kEebusErrorMemoryAllocate;
  }

  return kEebusErrorOk;
}

MdnsMock* MdnsMockCreate(void) {
  MdnsMock* const mock = (MdnsMock*)EEBUS_MALLOC(sizeof(MdnsMock));
  if (mock == nullptr) {
    return nullptr;
  }

  if (MdnsMockConstruct(mock) != kEebusErrorOk) {
    MdnsMockDelete(mock);
    return nullptr;
  }

  return mock;
}

void Destruct(ShipMdnsObject* self) {
  MdnsMock* const mock = MDNS_MOCK(self);
  mock->gmock->Destruct(self);
  delete mock->gmock;
}

EebusError Start(ShipMdnsObject* self) {
  MdnsMock* const mock = MDNS_MOCK(self);
  return mock->gmock->Start(self);
}

void Stop(ShipMdnsObject* self) {
  MdnsMock* const mock = MDNS_MOCK(self);
  mock->gmock->Stop(self);
}

EebusError RegisterService(ShipMdnsObject* self) {
  MdnsMock* const mock = MDNS_MOCK(self);
  return mock->gmock->RegisterService(self);
}

void DeregisterService(ShipMdnsObject* self) {
  MdnsMock* const mock = MDNS_MOCK(self);
  mock->gmock->DeregisterService(self);
}

void SetAutoaccept(ShipMdnsObject* self, bool autoaccept) {
  MdnsMock* const mock = MDNS_MOCK(self);
  mock->gmock->SetAutoaccept(self, autoaccept);
}

EebusError StartPairingBrowse(ShipMdnsObject* self, OnShipPairingEntriesFoundCallback cb, void* ctx) {
  MdnsMock* const mock = MDNS_MOCK(self);
  return mock->gmock->StartPairingBrowse(self, cb, ctx);
}

void StopPairingBrowse(ShipMdnsObject* self) {
  MdnsMock* const mock = MDNS_MOCK(self);
  mock->gmock->StopPairingBrowse(self);
}

EebusError RegisterPairingService(ShipMdnsObject* self, const ShipPairingEntry* entry) {
  MdnsMock* const mock = MDNS_MOCK(self);
  return mock->gmock->RegisterPairingService(self, entry);
}

void DeregisterPairingService(ShipMdnsObject* self) {
  MdnsMock* const mock = MDNS_MOCK(self);
  mock->gmock->DeregisterPairingService(self);
}

bool SupportsPairing(ShipMdnsObject* self) {
  MdnsMock* const mock = MDNS_MOCK(self);
  return mock->gmock->SupportsPairing(self);
}
