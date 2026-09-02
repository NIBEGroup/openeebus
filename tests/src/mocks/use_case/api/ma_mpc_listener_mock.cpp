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
 * @brief Ma Mpc Listener mock implementation
 */

#include "ma_mpc_listener_mock.h"

#include <gmock/gmock.h>

#include "src/use_case/api/ma_mpc_listener_interface.h"

static void Destruct(MaMpcListenerObject* self);
static void OnRemoteMuAdded(MaMpcListenerObject* self, const EntityAddressType* entity_addr);
static void OnRemoteMuRemoved(MaMpcListenerObject* self, const EntityAddressType* entity_addr);
static void OnMeasurementReceive(
    MaMpcListenerObject* self,
    MuMpcMeasurementNameId name_id,
    const ScaledValue* measurement_value,
    const EntityAddressType* remote_entity_addr
);

static const MaMpcListenerInterface ma_mpc_listener_methods = {
    .destruct               = Destruct,
    .on_remote_mu_added     = OnRemoteMuAdded,
    .on_remote_mu_removed   = OnRemoteMuRemoved,
    .on_measurement_receive = OnMeasurementReceive,
};

static EebusError MaMpcListenerMockConstruct(MaMpcListenerMock* self);

EebusError MaMpcListenerMockConstruct(MaMpcListenerMock* self) {
  // Override "virtual functions table"
  MA_MPC_LISTENER_INTERFACE(self) = &ma_mpc_listener_methods;

  self->gmock = new MaMpcListenerGMock();
  if (self->gmock == nullptr) {
    return kEebusErrorMemoryAllocate;
  }

  return kEebusErrorOk;
}

MaMpcListenerMock* MaMpcListenerMockCreate(void) {
  MaMpcListenerMock* const mock = (MaMpcListenerMock*)EEBUS_MALLOC(sizeof(MaMpcListenerMock));
  if (mock == nullptr) {
    return nullptr;
  }

  if (MaMpcListenerMockConstruct(mock) != kEebusErrorOk) {
    MaMpcListenerMockDelete(mock);
    return nullptr;
  }

  return mock;
}

void Destruct(MaMpcListenerObject* self) {
  MaMpcListenerMock* const mock = MA_MPC_LISTENER_MOCK(self);
  mock->gmock->Destruct(self);
  delete mock->gmock;
}

void OnRemoteMuAdded(MaMpcListenerObject* self, const EntityAddressType* entity_addr) {
  MaMpcListenerMock* const mock = MA_MPC_LISTENER_MOCK(self);
  mock->gmock->OnRemoteMuAdded(self, entity_addr);
}

void OnRemoteMuRemoved(MaMpcListenerObject* self, const EntityAddressType* entity_addr) {
  MaMpcListenerMock* const mock = MA_MPC_LISTENER_MOCK(self);
  mock->gmock->OnRemoteMuRemoved(self, entity_addr);
}

void OnMeasurementReceive(
    MaMpcListenerObject* self,
    MuMpcMeasurementNameId name_id,
    const ScaledValue* measurement_value,
    const EntityAddressType* remote_entity_addr
) {
  MaMpcListenerMock* const mock = MA_MPC_LISTENER_MOCK(self);
  mock->gmock->OnMeasurementReceive(self, name_id, measurement_value, remote_entity_addr);
}
