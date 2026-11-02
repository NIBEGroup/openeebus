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
 * @brief Heartbeat Manager Mock "class"
 */

#ifndef TESTS_UNIT_TESTS_MOCKS_SPINE_HEARTBEAT_HEARTBEAT_MANAGER_MOCK_H_
#define TESTS_UNIT_TESTS_MOCKS_SPINE_HEARTBEAT_HEARTBEAT_MANAGER_MOCK_H_

#include <gmock/gmock.h>

#include <memory>

#include "src/common/eebus_malloc.h"
#include "src/spine/api/heartbeat_manager_interface.h"

class HeartbeatManagerGMockInterface {
 public:
  virtual ~HeartbeatManagerGMockInterface() {};
  virtual void Destruct(HeartbeatManagerObject* self)                 = 0;
  virtual bool IsHeartbeatRunning(const HeartbeatManagerObject* self) = 0;
  virtual void SetLocalFeature(HeartbeatManagerObject* self, EntityLocalObject* entity, FeatureLocalObject* feature)
      = 0;
  virtual void Tick(HeartbeatManagerObject* self)        = 0;
  virtual EebusError Start(HeartbeatManagerObject* self) = 0;
  virtual void Stop(HeartbeatManagerObject* self)        = 0;
};

class HeartbeatManagerGMock : public HeartbeatManagerGMockInterface {
 public:
  virtual ~HeartbeatManagerGMock() {};
  MOCK_METHOD1(Destruct, void(HeartbeatManagerObject*));
  MOCK_METHOD1(IsHeartbeatRunning, bool(const HeartbeatManagerObject*));
  MOCK_METHOD3(SetLocalFeature, void(HeartbeatManagerObject*, EntityLocalObject*, FeatureLocalObject*));
  MOCK_METHOD1(Tick, void(HeartbeatManagerObject*));
  MOCK_METHOD1(Start, EebusError(HeartbeatManagerObject*));
  MOCK_METHOD1(Stop, void(HeartbeatManagerObject*));
};

typedef struct HeartbeatManagerMock {
  /** Implements the Heartbeat Manager Interface */
  HeartbeatManagerObject obj;
  HeartbeatManagerGMock* gmock;
} HeartbeatManagerMock;

#define HEARTBEAT_MANAGER_MOCK(obj) ((HeartbeatManagerMock*)(obj))

HeartbeatManagerMock* HeartbeatManagerMockCreate(void);

static inline void HeartbeatManagerMockDelete(HeartbeatManagerMock* self) {
  if (self != nullptr) {
    HEARTBEAT_MANAGER_DESTRUCT(HEARTBEAT_MANAGER_OBJECT(self));
    EEBUS_FREE(self);
  }
}

#endif  // TESTS_UNIT_TESTS_MOCKS_SPINE_HEARTBEAT_HEARTBEAT_MANAGER_MOCK_H_
