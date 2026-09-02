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

#include "mu_mpc_listener_mock.h"

#include <gmock/gmock.h>

#include "src/common/eebus_errors.h"
#include "src/common/eebus_malloc.h"
#include "src/use_case/api/mu_mpc_listener_interface.h"

static void Destruct(MuMpcListenerObject* self);
static void OnRemoteMaAdded(MuMpcListenerObject* self, const EntityAddressType* entity_addr);
static void OnRemoteMaRemoved(MuMpcListenerObject* self, const EntityAddressType* entity_addr);

static const MuMpcListenerInterface mu_mpc_listener_methods = {
    .destruct             = Destruct,
    .on_remote_ma_added   = OnRemoteMaAdded,
    .on_remote_ma_removed = OnRemoteMaRemoved,
};

static EebusError MuMpcListenerMockConstruct(MuMpcListenerMock* self);

EebusError MuMpcListenerMockConstruct(MuMpcListenerMock* self) {
  MU_MPC_LISTENER_INTERFACE(self) = &mu_mpc_listener_methods;

  self->gmock = new MuMpcListenerGMock();
  if (self->gmock == nullptr) {
    return kEebusErrorMemoryAllocate;
  }

  return kEebusErrorOk;
}

MuMpcListenerMock* MuMpcListenerMockCreate(void) {
  MuMpcListenerMock* const mock = (MuMpcListenerMock*)EEBUS_MALLOC(sizeof(MuMpcListenerMock));
  if (mock == nullptr) {
    return nullptr;
  }

  if (MuMpcListenerMockConstruct(mock) != kEebusErrorOk) {
    MuMpcListenerMockDelete(mock);
    return nullptr;
  }

  return mock;
}

void Destruct(MuMpcListenerObject* self) {
  MuMpcListenerMock* const mock = MU_MPC_LISTENER_MOCK(self);
  mock->gmock->Destruct(self);
  delete mock->gmock;
}

void OnRemoteMaAdded(MuMpcListenerObject* self, const EntityAddressType* entity_addr) {
  MuMpcListenerMock* const mock = MU_MPC_LISTENER_MOCK(self);
  mock->gmock->OnRemoteMaAdded(self, entity_addr);
}

void OnRemoteMaRemoved(MuMpcListenerObject* self, const EntityAddressType* entity_addr) {
  MuMpcListenerMock* const mock = MU_MPC_LISTENER_MOCK(self);
  mock->gmock->OnRemoteMaRemoved(self, entity_addr);
}
