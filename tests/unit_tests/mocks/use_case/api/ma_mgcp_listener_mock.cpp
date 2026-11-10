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
 * @brief Ma Mgcp Listener mock implementation
 */

#include "ma_mgcp_listener_mock.h"

#include <gmock/gmock.h>

#include "src/use_case/api/ma_mgcp_listener_interface.h"

static void Destruct(MaMgcpListenerObject* self);
static void OnRemoteEntityConnect(MaMgcpListenerObject* self, const EntityAddressType* entity_addr);
static void OnRemoteEntityDisconnect(MaMgcpListenerObject* self, const EntityAddressType* entity_addr);
static void OnMeasurementReceive(
    MaMgcpListenerObject* self,
    GcpMeasurementNameId name_id,
    const ScaledValue* measurement_value,
    const EntityAddressType* remote_entity_addr
);
static void OnPvCurtailmentLimitFactorReceive(
    MaMgcpListenerObject* self,
    const ScaledValue* value,
    const EntityAddressType* remote_entity_addr
);

static const MaMgcpListenerInterface ma_mgcp_listener_methods = {
    .destruct                               = Destruct,
    .on_remote_entity_connect               = OnRemoteEntityConnect,
    .on_remote_entity_disconnect            = OnRemoteEntityDisconnect,
    .on_measurement_receive                 = OnMeasurementReceive,
    .on_pv_curtailment_limit_factor_receive = OnPvCurtailmentLimitFactorReceive,
};

static EebusError MaMgcpListenerMockConstruct(MaMgcpListenerMock* self);

EebusError MaMgcpListenerMockConstruct(MaMgcpListenerMock* self) {
  // Override "virtual functions table"
  MA_MGCP_LISTENER_INTERFACE(self) = &ma_mgcp_listener_methods;

  self->gmock = new MaMgcpListenerGMock();
  if (self->gmock == nullptr) {
    return kEebusErrorMemoryAllocate;
  }

  return kEebusErrorOk;
}

MaMgcpListenerMock* MaMgcpListenerMockCreate(void) {
  MaMgcpListenerMock* const mock = (MaMgcpListenerMock*)EEBUS_MALLOC(sizeof(MaMgcpListenerMock));
  if (mock == nullptr) {
    return nullptr;
  }

  if (MaMgcpListenerMockConstruct(mock) != kEebusErrorOk) {
    MaMgcpListenerMockDelete(mock);
    return nullptr;
  }

  return mock;
}

void Destruct(MaMgcpListenerObject* self) {
  MaMgcpListenerMock* const mock = MA_MGCP_LISTENER_MOCK(self);
  mock->gmock->Destruct(self);
  delete mock->gmock;
}

void OnRemoteEntityConnect(MaMgcpListenerObject* self, const EntityAddressType* entity_addr) {
  MaMgcpListenerMock* const mock = MA_MGCP_LISTENER_MOCK(self);
  mock->gmock->OnRemoteEntityConnect(self, entity_addr);
}

void OnRemoteEntityDisconnect(MaMgcpListenerObject* self, const EntityAddressType* entity_addr) {
  MaMgcpListenerMock* const mock = MA_MGCP_LISTENER_MOCK(self);
  mock->gmock->OnRemoteEntityDisconnect(self, entity_addr);
}

void OnMeasurementReceive(
    MaMgcpListenerObject* self,
    GcpMeasurementNameId name_id,
    const ScaledValue* measurement_value,
    const EntityAddressType* remote_entity_addr
) {
  MaMgcpListenerMock* const mock = MA_MGCP_LISTENER_MOCK(self);
  mock->gmock->OnMeasurementReceive(self, name_id, measurement_value, remote_entity_addr);
}

void OnPvCurtailmentLimitFactorReceive(
    MaMgcpListenerObject* self,
    const ScaledValue* value,
    const EntityAddressType* remote_entity_addr
) {
  MaMgcpListenerMock* const mock = MA_MGCP_LISTENER_MOCK(self);
  mock->gmock->OnPvCurtailmentLimitFactorReceive(self, value, remote_entity_addr);
}
