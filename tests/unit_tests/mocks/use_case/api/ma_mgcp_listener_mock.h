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
 * @brief Ma Mgcp Listener Mock "class"
 */

#ifndef TESTS_UNIT_TESTS_MOCKS_USE_CASE_ACTOR_MA_MGCP_MA_MGCP_LISTENER_MOCK_H_
#define TESTS_UNIT_TESTS_MOCKS_USE_CASE_ACTOR_MA_MGCP_MA_MGCP_LISTENER_MOCK_H_

#include <gmock/gmock.h>

#include <memory>

#include "src/common/eebus_malloc.h"
#include "src/use_case/api/ma_mgcp_listener_interface.h"

class MaMgcpListenerGMockInterface {
 public:
  virtual ~MaMgcpListenerGMockInterface() {};
  virtual void Destruct(MaMgcpListenerObject* self)                                                       = 0;
  virtual void OnRemoteEntityConnect(MaMgcpListenerObject* self, const EntityAddressType* entity_addr)    = 0;
  virtual void OnRemoteEntityDisconnect(MaMgcpListenerObject* self, const EntityAddressType* entity_addr) = 0;
  virtual void OnMeasurementReceive(
      MaMgcpListenerObject* self,
      GcpMeasurementNameId name_id,
      const ScaledValue* measurement_value,
      const EntityAddressType* remote_entity_addr
  ) = 0;
  virtual void OnPvCurtailmentLimitFactorReceive(
      MaMgcpListenerObject* self,
      const ScaledValue* value,
      const EntityAddressType* remote_entity_addr
  ) = 0;
};

class MaMgcpListenerGMock : public MaMgcpListenerGMockInterface {
 public:
  virtual ~MaMgcpListenerGMock() {};
  MOCK_METHOD1(Destruct, void(MaMgcpListenerObject*));
  MOCK_METHOD2(OnRemoteEntityConnect, void(MaMgcpListenerObject*, const EntityAddressType*));
  MOCK_METHOD2(OnRemoteEntityDisconnect, void(MaMgcpListenerObject*, const EntityAddressType*));
  MOCK_METHOD4(
      OnMeasurementReceive,
      void(MaMgcpListenerObject*, GcpMeasurementNameId, const ScaledValue*, const EntityAddressType*)
  );
  MOCK_METHOD3(
      OnPvCurtailmentLimitFactorReceive,
      void(MaMgcpListenerObject*, const ScaledValue*, const EntityAddressType*)
  );
};

typedef struct MaMgcpListenerMock {
  /** Implements the Ma Mgcp Listener Interface */
  MaMgcpListenerObject obj;
  MaMgcpListenerGMock* gmock;
} MaMgcpListenerMock;

#define MA_MGCP_LISTENER_MOCK(obj) ((MaMgcpListenerMock*)(obj))

MaMgcpListenerMock* MaMgcpListenerMockCreate(void);

static inline void MaMgcpListenerMockDelete(MaMgcpListenerMock* listener_mock) {
  if (listener_mock != NULL) {
    MA_MGCP_LISTENER_DESTRUCT(MA_MGCP_LISTENER_OBJECT(listener_mock));
    EEBUS_FREE(listener_mock);
  }
}
#endif  // TESTS_UNIT_TESTS_MOCKS_USE_CASE_ACTOR_MA_MGCP_MA_MGCP_LISTENER_MOCK_H_
