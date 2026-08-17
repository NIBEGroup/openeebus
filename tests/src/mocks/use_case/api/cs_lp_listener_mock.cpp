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
 * @brief CS LP Listener mock implementation
 */

#include "cs_lp_listener_mock.h"

#include <gmock/gmock.h>

#include "src/use_case/api/cs_lp_listener_interface.h"

static void Destruct(CsLpListenerObject* self);
static void OnRemoteEgAdded(CsLpListenerObject* self, const EntityAddressType* entity_addr);
static void OnRemoteEgRemoved(CsLpListenerObject* self, const EntityAddressType* entity_addr);
static void OnPowerLimitReceive(
    CsLpListenerObject* self,
    const ScaledValue* power_limit,
    const DurationType* duration,
    bool is_active
);
static void OnFailsafePowerLimitReceive(CsLpListenerObject* self, const ScaledValue* power_limit);
static void OnFailsafeDurationReceive(CsLpListenerObject* self, const DurationType* duration);
static void OnHeartbeatReceive(CsLpListenerObject* self, uint64_t heartbeat_counter);

static const CsLpListenerInterface cs_lp_listener_methods = {
    .destruct                        = Destruct,
    .on_remote_eg_added              = OnRemoteEgAdded,
    .on_remote_eg_removed            = OnRemoteEgRemoved,
    .on_power_limit_receive          = OnPowerLimitReceive,
    .on_failsafe_power_limit_receive = OnFailsafePowerLimitReceive,
    .on_failsafe_duration_receive    = OnFailsafeDurationReceive,
    .on_heartbeat_receive            = OnHeartbeatReceive,
};

static EebusError CsLpListenerMockConstruct(CsLpListenerMock* self);

EebusError CsLpListenerMockConstruct(CsLpListenerMock* self) {
  // Override "virtual functions table"
  CS_LP_LISTENER_INTERFACE(self) = &cs_lp_listener_methods;

  self->gmock = new CsLpListenerGMock();
  if (self->gmock == nullptr) {
    return kEebusErrorMemoryAllocate;
  }

  return kEebusErrorOk;
}

CsLpListenerMock* CsLpListenerMockCreate(void) {
  CsLpListenerMock* const mock = (CsLpListenerMock*)EEBUS_MALLOC(sizeof(CsLpListenerMock));
  if (mock == nullptr) {
    return nullptr;
  }

  if (CsLpListenerMockConstruct(mock) != kEebusErrorOk) {
    CsLpListenerMockDelete(CS_LP_LISTENER_MOCK(mock));
    return nullptr;
  }

  return mock;
}

void Destruct(CsLpListenerObject* self) {
  CsLpListenerMock* const mock = CS_LP_LISTENER_MOCK(self);
  mock->gmock->Destruct(self);
  delete mock->gmock;
}

void OnRemoteEgAdded(CsLpListenerObject* self, const EntityAddressType* entity_addr) {
  CsLpListenerMock* const mock = CS_LP_LISTENER_MOCK(self);
  mock->gmock->OnRemoteEgAdded(self, entity_addr);
}

void OnRemoteEgRemoved(CsLpListenerObject* self, const EntityAddressType* entity_addr) {
  CsLpListenerMock* const mock = CS_LP_LISTENER_MOCK(self);
  mock->gmock->OnRemoteEgRemoved(self, entity_addr);
}

void OnPowerLimitReceive(
    CsLpListenerObject* self,
    const ScaledValue* power_limit,
    const DurationType* duration,
    bool is_active
) {
  CsLpListenerMock* const mock = CS_LP_LISTENER_MOCK(self);
  mock->gmock->OnPowerLimitReceive(self, power_limit, duration, is_active);
}

void OnFailsafePowerLimitReceive(CsLpListenerObject* self, const ScaledValue* power_limit) {
  CsLpListenerMock* const mock = CS_LP_LISTENER_MOCK(self);
  mock->gmock->OnFailsafePowerLimitReceive(self, power_limit);
}

void OnFailsafeDurationReceive(CsLpListenerObject* self, const DurationType* duration) {
  CsLpListenerMock* const mock = CS_LP_LISTENER_MOCK(self);
  mock->gmock->OnFailsafeDurationReceive(self, duration);
}

void OnHeartbeatReceive(CsLpListenerObject* self, uint64_t heartbeat_counter) {
  CsLpListenerMock* const mock = CS_LP_LISTENER_MOCK(self);
  mock->gmock->OnHeartbeatReceive(self, heartbeat_counter);
}
