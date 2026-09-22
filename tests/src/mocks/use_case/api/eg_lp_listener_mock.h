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
 * @brief EG LP Listener Mock "class"
 */

#ifndef TESTS_SRC_MOCKS_USE_CASE_API_EG_LP_LISTENER_MOCK_H_
#define TESTS_SRC_MOCKS_USE_CASE_API_EG_LP_LISTENER_MOCK_H_

#include <gmock/gmock.h>

#include <memory>

#include "src/common/eebus_malloc.h"
#include "src/use_case/api/eg_lp_listener_interface.h"

class EgLpListenerGMockInterface {
 public:
  virtual ~EgLpListenerGMockInterface() {};
  virtual void Destruct(EgLpListenerObject* self)                                                = 0;
  virtual void OnRemoteCsAdded(EgLpListenerObject* self, const EntityAddressType* entity_addr)   = 0;
  virtual void OnRemoteCsRemoved(EgLpListenerObject* self, const EntityAddressType* entity_addr) = 0;
  virtual void OnPowerLimitReceive(
      EgLpListenerObject* self,
      const ScaledValue* power_limit,
      const DurationType* duration,
      bool is_active
  )                                                                                                  = 0;
  virtual void OnFailsafePowerLimitReceive(EgLpListenerObject* self, const ScaledValue* power_limit) = 0;
  virtual void OnFailsafeDurationReceive(EgLpListenerObject* self, const DurationType* duration)     = 0;
  virtual void OnHeartbeatReceive(EgLpListenerObject* self, uint64_t heartbeat_counter)              = 0;
  virtual void OnPowerNominalMaxReceive(EgLpListenerObject* self, const ScaledValue* power_limit)    = 0;
};

class EgLpListenerGMock : public EgLpListenerGMockInterface {
 public:
  virtual ~EgLpListenerGMock() {};
  MOCK_METHOD1(Destruct, void(EgLpListenerObject*));
  MOCK_METHOD2(OnRemoteCsAdded, void(EgLpListenerObject*, const EntityAddressType*));
  MOCK_METHOD2(OnRemoteCsRemoved, void(EgLpListenerObject*, const EntityAddressType*));
  MOCK_METHOD4(OnPowerLimitReceive, void(EgLpListenerObject*, const ScaledValue*, const DurationType*, bool));
  MOCK_METHOD2(OnFailsafePowerLimitReceive, void(EgLpListenerObject*, const ScaledValue*));
  MOCK_METHOD2(OnFailsafeDurationReceive, void(EgLpListenerObject*, const DurationType*));
  MOCK_METHOD2(OnHeartbeatReceive, void(EgLpListenerObject*, uint64_t));
  MOCK_METHOD2(OnPowerNominalMaxReceive, void(EgLpListenerObject*, const ScaledValue*));
};

typedef struct EgLpListenerMock {
  /** Implements the Eg Lp Listener Interface */
  EgLpListenerObject obj;
  EgLpListenerGMock* gmock;
} EgLpListenerMock;

#define EG_LP_LISTENER_MOCK(obj) ((EgLpListenerMock*)(obj))

EgLpListenerMock* EgLpListenerMockCreate(void);

static inline void EgLpListenerMockDelete(EgLpListenerMock* self) {
  if (self != nullptr) {
    EG_LP_LISTENER_DESTRUCT(EG_LP_LISTENER_OBJECT(self));
    EEBUS_FREE(self);
  }
}

#endif  // TESTS_SRC_MOCKS_USE_CASE_API_EG_LP_LISTENER_MOCK_H_
