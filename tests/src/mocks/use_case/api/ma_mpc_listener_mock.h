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
 * @brief Ma Mpc Listener Mock "class"
 */

#ifndef TESTS_SRC_MOCKS_USE_CASE_ACTOR_MA_MPC_MA_MPC_LISTENER_MOCK_H_
#define TESTS_SRC_MOCKS_USE_CASE_ACTOR_MA_MPC_MA_MPC_LISTENER_MOCK_H_

#include <gmock/gmock.h>

#include <memory>

#include "src/common/eebus_malloc.h"
#include "src/use_case/api/ma_mpc_listener_interface.h"

class MaMpcListenerGMockInterface {
 public:
  virtual ~MaMpcListenerGMockInterface() {};
  virtual void Destruct(MaMpcListenerObject* self)                                                = 0;
  virtual void OnRemoteMuAdded(MaMpcListenerObject* self, const EntityAddressType* entity_addr)   = 0;
  virtual void OnRemoteMuRemoved(MaMpcListenerObject* self, const EntityAddressType* entity_addr) = 0;
  virtual void OnMeasurementReceive(
      MaMpcListenerObject* self,
      MuMpcMeasurementNameId name_id,
      const ScaledValue* measurement_value,
      const EntityAddressType* remote_entity_addr
  ) = 0;
};

class MaMpcListenerGMock : public MaMpcListenerGMockInterface {
 public:
  virtual ~MaMpcListenerGMock() {};
  MOCK_METHOD1(Destruct, void(MaMpcListenerObject*));
  MOCK_METHOD2(OnRemoteMuAdded, void(MaMpcListenerObject*, const EntityAddressType*));
  MOCK_METHOD2(OnRemoteMuRemoved, void(MaMpcListenerObject*, const EntityAddressType*));
  MOCK_METHOD4(
      OnMeasurementReceive,
      void(MaMpcListenerObject*, MuMpcMeasurementNameId, const ScaledValue*, const EntityAddressType*)
  );
};

typedef struct MaMpcListenerMock {
  /** Implements the Ma Mpc Listener Interface */
  MaMpcListenerObject obj;
  MaMpcListenerGMock* gmock;
} MaMpcListenerMock;

#define MA_MPC_LISTENER_MOCK(obj) ((MaMpcListenerMock*)(obj))

MaMpcListenerMock* MaMpcListenerMockCreate(void);

static inline void MaMpcListenerMockDelete(MaMpcListenerMock* listener_mock) {
  if (listener_mock != NULL) {
    MA_MPC_LISTENER_DESTRUCT(MA_MPC_LISTENER_OBJECT(listener_mock));
    EEBUS_FREE(listener_mock);
  }
}
#endif  // TESTS_SRC_MOCKS_USE_CASE_ACTOR_MA_MPC_MA_MPC_LISTENER_MOCK_H_
