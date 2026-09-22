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
 * @brief Mu Mpc Listener Mock "class"
 */

#ifndef TESTS_SRC_MOCKS_USE_CASE_API_MU_MPC_LISTENER_MOCK_H_
#define TESTS_SRC_MOCKS_USE_CASE_API_MU_MPC_LISTENER_MOCK_H_

#include <gmock/gmock.h>

#include <memory>

#include "src/common/eebus_malloc.h"
#include "src/use_case/api/mu_mpc_listener_interface.h"

class MuMpcListenerGMockInterface {
 public:
  virtual ~MuMpcListenerGMockInterface() {};
  virtual void Destruct(MuMpcListenerObject* self)                                                = 0;
  virtual void OnRemoteMaAdded(MuMpcListenerObject* self, const EntityAddressType* entity_addr)   = 0;
  virtual void OnRemoteMaRemoved(MuMpcListenerObject* self, const EntityAddressType* entity_addr) = 0;
};

class MuMpcListenerGMock : public MuMpcListenerGMockInterface {
 public:
  virtual ~MuMpcListenerGMock() {};
  MOCK_METHOD1(Destruct, void(MuMpcListenerObject*));
  MOCK_METHOD2(OnRemoteMaAdded, void(MuMpcListenerObject*, const EntityAddressType*));
  MOCK_METHOD2(OnRemoteMaRemoved, void(MuMpcListenerObject*, const EntityAddressType*));
};

typedef struct MuMpcListenerMock {
  /** Implements the Mu Mpc Listener Interface */
  MuMpcListenerObject obj;
  MuMpcListenerGMock* gmock;
} MuMpcListenerMock;

#define MU_MPC_LISTENER_MOCK(obj) ((MuMpcListenerMock*)(obj))

MuMpcListenerMock* MuMpcListenerMockCreate(void);

static inline void MuMpcListenerMockDelete(MuMpcListenerMock* listener_mock) {
  if (listener_mock != NULL) {
    MU_MPC_LISTENER_DESTRUCT(MU_MPC_LISTENER_OBJECT(listener_mock));
    EEBUS_FREE(listener_mock);
  }
}

#endif  // TESTS_SRC_MOCKS_USE_CASE_API_MU_MPC_LISTENER_MOCK_H_
