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
 * @brief Heartbeat Manager mock implementation
 */

#include "heartbeat_manager_mock.h"

#include <gmock/gmock.h>

#include "src/spine/api/heartbeat_manager_interface.h"

static void Destruct(HeartbeatManagerObject* self);
static bool IsHeartbeatRunning(const HeartbeatManagerObject* self);
static void SetLocalFeature(HeartbeatManagerObject* self, EntityLocalObject* entity, FeatureLocalObject* feature);
static void Tick(HeartbeatManagerObject* self);
static EebusError Start(HeartbeatManagerObject* self);
static void Stop(HeartbeatManagerObject* self);

static const HeartbeatManagerInterface heartbeat_manager_methods = {
    .destruct             = Destruct,
    .is_heartbeat_running = IsHeartbeatRunning,
    .set_local_feature    = SetLocalFeature,
    .tick                 = Tick,
    .start                = Start,
    .stop                 = Stop,
};

static EebusError HeartbeatManagerMockConstruct(HeartbeatManagerMock* self);

EebusError HeartbeatManagerMockConstruct(HeartbeatManagerMock* self) {
  // Override "virtual functions table"
  HEARTBEAT_MANAGER_INTERFACE(self) = &heartbeat_manager_methods;

  self->gmock = new HeartbeatManagerGMock();
  if (self->gmock == nullptr) {
    return kEebusErrorMemoryAllocate;
  }

  return kEebusErrorOk;
}

HeartbeatManagerMock* HeartbeatManagerMockCreate(void) {
  HeartbeatManagerMock* const mock = (HeartbeatManagerMock*)EEBUS_MALLOC(sizeof(HeartbeatManagerMock));
  if (mock == nullptr) {
    return nullptr;
  }

  if (HeartbeatManagerMockConstruct(mock) != kEebusErrorOk) {
    HeartbeatManagerMockDelete(mock);
    return nullptr;
  }

  return mock;
}

void Destruct(HeartbeatManagerObject* self) {
  HeartbeatManagerMock* const mock = HEARTBEAT_MANAGER_MOCK(self);
  mock->gmock->Destruct(self);
  delete mock->gmock;
}

bool IsHeartbeatRunning(const HeartbeatManagerObject* self) {
  HeartbeatManagerMock* const mock = HEARTBEAT_MANAGER_MOCK(self);
  return mock->gmock->IsHeartbeatRunning(self);
}

void SetLocalFeature(HeartbeatManagerObject* self, EntityLocalObject* entity, FeatureLocalObject* feature) {
  HeartbeatManagerMock* const mock = HEARTBEAT_MANAGER_MOCK(self);
  mock->gmock->SetLocalFeature(self, entity, feature);
}

void Tick(HeartbeatManagerObject* self) {
  HeartbeatManagerMock* const mock = HEARTBEAT_MANAGER_MOCK(self);
  mock->gmock->Tick(self);
}

EebusError Start(HeartbeatManagerObject* self) {
  HeartbeatManagerMock* const mock = HEARTBEAT_MANAGER_MOCK(self);
  return mock->gmock->Start(self);
}

void Stop(HeartbeatManagerObject* self) {
  HeartbeatManagerMock* const mock = HEARTBEAT_MANAGER_MOCK(self);
  mock->gmock->Stop(self);
}
