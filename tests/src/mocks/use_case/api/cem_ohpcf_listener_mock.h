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
 * @brief Cem Ohpcf Listener Mock "class"
 */

#ifndef TESTS_SRC_MOCKS_USE_CASE_ACTOR_CEM_OHPCF_CEM_OHPCF_LISTENER_MOCK_H_
#define TESTS_SRC_MOCKS_USE_CASE_ACTOR_CEM_OHPCF_CEM_OHPCF_LISTENER_MOCK_H_

#include <gmock/gmock.h>

#include <memory>

#include "src/common/eebus_malloc.h"
#include "src/use_case/api/cem_ohpcf_listener_interface.h"

class CemOhpcfListenerGMockInterface {
 public:
  virtual ~CemOhpcfListenerGMockInterface() {};
  virtual void Destruct(CemOhpcfListenerObject* self)                                                        = 0;
  virtual void OnRemoteCompressorAdded(CemOhpcfListenerObject* self, const EntityAddressType* entity_addr)   = 0;
  virtual void OnRemoteCompressorRemoved(CemOhpcfListenerObject* self, const EntityAddressType* entity_addr) = 0;
  virtual void OnAnnounce(
      CemOhpcfListenerObject* self,
      const OptionalPowerConsumption* optional_power_consumption,
      const EntityAddressType* entity_addr
  ) = 0;
  virtual void OnStateReport(
      CemOhpcfListenerObject* self,
      CompressorOhpcfState state,
      const EebusDuration* start_time,
      const EntityAddressType* entity_addr
  )                                                                                               = 0;
  virtual void OnClearProcess(CemOhpcfListenerObject* self, const EntityAddressType* entity_addr) = 0;
};

class CemOhpcfListenerGMock : public CemOhpcfListenerGMockInterface {
 public:
  virtual ~CemOhpcfListenerGMock() {};
  MOCK_METHOD1(Destruct, void(CemOhpcfListenerObject*));
  MOCK_METHOD2(OnRemoteCompressorAdded, void(CemOhpcfListenerObject*, const EntityAddressType*));
  MOCK_METHOD2(OnRemoteCompressorRemoved, void(CemOhpcfListenerObject*, const EntityAddressType*));
  MOCK_METHOD3(OnAnnounce, void(CemOhpcfListenerObject*, const OptionalPowerConsumption*, const EntityAddressType*));
  MOCK_METHOD4(
      OnStateReport,
      void(CemOhpcfListenerObject*, CompressorOhpcfState, const EebusDuration*, const EntityAddressType*)
  );
  MOCK_METHOD2(OnClearProcess, void(CemOhpcfListenerObject*, const EntityAddressType*));
};

typedef struct CemOhpcfListenerMock {
  /** Implements the Cem Ohpcf Listener Interface */
  CemOhpcfListenerObject obj;
  CemOhpcfListenerGMock* gmock;
} CemOhpcfListenerMock;

#define CEM_OHPCF_LISTENER_MOCK(obj) ((CemOhpcfListenerMock*)(obj))

CemOhpcfListenerMock* CemOhpcfListenerMockCreate(void);

static inline void CemOhpcfListenerMockDelete(CemOhpcfListenerMock* mock) {
  if (mock != NULL) {
    CEM_OHPCF_LISTENER_DESTRUCT(CEM_OHPCF_LISTENER_OBJECT(mock));
    EEBUS_FREE(mock);
  }
}

#endif  // TESTS_SRC_MOCKS_USE_CASE_ACTOR_CEM_OHPCF_CEM_OHPCF_LISTENER_MOCK_H_
