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
 * @brief CS LP Listener Mock "class"
 */

#ifndef TESTS_SRC_MOCKS_USE_CASE_API_CS_LP_LISTENER_MOCK_H_
#define TESTS_SRC_MOCKS_USE_CASE_API_CS_LP_LISTENER_MOCK_H_

#include <gmock/gmock.h>

#include <memory>

#include "src/common/eebus_malloc.h"
#include "src/use_case/api/cs_lp_listener_interface.h"

class CsLpListenerGMockInterface {
 public:
  virtual ~CsLpListenerGMockInterface() {};
  virtual void Destruct(CsLpListenerObject* self)                                                = 0;
  virtual void OnRemoteEgAdded(CsLpListenerObject* self, const EntityAddressType* entity_addr)   = 0;
  virtual void OnRemoteEgRemoved(CsLpListenerObject* self, const EntityAddressType* entity_addr) = 0;
  virtual void OnPowerLimitReceive(
      CsLpListenerObject* self,
      const ScaledValue* power_limit,
      const DurationType* duration,
      bool is_active
  )                                                                                                  = 0;
  virtual void OnFailsafePowerLimitReceive(CsLpListenerObject* self, const ScaledValue* power_limit) = 0;
  virtual void OnFailsafeDurationReceive(CsLpListenerObject* self, const DurationType* duration)     = 0;
  virtual void OnHeartbeatReceive(CsLpListenerObject* self, uint64_t heartbeat_counter)              = 0;
};

class CsLpListenerGMock : public CsLpListenerGMockInterface {
 public:
  virtual ~CsLpListenerGMock() {};
  MOCK_METHOD1(Destruct, void(CsLpListenerObject*));
  MOCK_METHOD2(OnRemoteEgAdded, void(CsLpListenerObject*, const EntityAddressType*));
  MOCK_METHOD2(OnRemoteEgRemoved, void(CsLpListenerObject*, const EntityAddressType*));
  MOCK_METHOD4(OnPowerLimitReceive, void(CsLpListenerObject*, const ScaledValue*, const DurationType*, bool));
  MOCK_METHOD2(OnFailsafePowerLimitReceive, void(CsLpListenerObject*, const ScaledValue*));
  MOCK_METHOD2(OnFailsafeDurationReceive, void(CsLpListenerObject*, const DurationType*));
  MOCK_METHOD2(OnHeartbeatReceive, void(CsLpListenerObject*, uint64_t));
};

typedef struct CsLpListenerMock {
  /** Implements the CS LP Listener Interface */
  CsLpListenerObject obj;
  CsLpListenerGMock* gmock;
} CsLpListenerMock;

#define CS_LP_LISTENER_MOCK(obj) ((CsLpListenerMock*)(obj))

CsLpListenerMock* CsLpListenerMockCreate(void);

static inline void CsLpListenerMockDelete(CsLpListenerMock* self) {
  if (self != nullptr) {
    CS_LP_LISTENER_DESTRUCT(CS_LP_LISTENER_OBJECT(self));
    EEBUS_FREE(self);
  }
}

#endif  // TESTS_SRC_MOCKS_USE_CASE_API_CS_LP_LISTENER_MOCK_H_
