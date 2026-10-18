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
 * @brief Compressor Ohpcf Listener Mock "class"
 */

#ifndef TESTS_SRC_MOCKS_USE_CASE_ACTOR_COMPRESSOR_COMPRESSOR_OHPCF_LISTENER_MOCK_H_
#define TESTS_SRC_MOCKS_USE_CASE_ACTOR_COMPRESSOR_COMPRESSOR_OHPCF_LISTENER_MOCK_H_

#include <gmock/gmock.h>

#include <memory>

#include "src/common/eebus_malloc.h"
#include "src/use_case/api/compressor_ohpcf_listener_interface.h"

class CompressorOhpcfListenerGMockInterface {
 public:
  virtual ~CompressorOhpcfListenerGMockInterface() {};
  virtual void Destruct(CompressorOhpcfListenerObject* self)           = 0;
  virtual void OnRemoteCemAdded(CompressorOhpcfListenerObject* self)   = 0;
  virtual void OnRemoteCemRemoved(CompressorOhpcfListenerObject* self) = 0;
  virtual void OnScheduleOptionalPowerConsumption(CompressorOhpcfListenerObject* self, const EebusDuration* start_time)
      = 0;
  virtual void OnStop(CompressorOhpcfListenerObject* self)   = 0;
  virtual void OnPause(CompressorOhpcfListenerObject* self)  = 0;
  virtual void OnResume(CompressorOhpcfListenerObject* self) = 0;
};

class CompressorOhpcfListenerGMock : public CompressorOhpcfListenerGMockInterface {
 public:
  virtual ~CompressorOhpcfListenerGMock() {};
  MOCK_METHOD1(Destruct, void(CompressorOhpcfListenerObject*));
  MOCK_METHOD1(OnRemoteCemAdded, void(CompressorOhpcfListenerObject*));
  MOCK_METHOD1(OnRemoteCemRemoved, void(CompressorOhpcfListenerObject*));
  MOCK_METHOD2(OnScheduleOptionalPowerConsumption, void(CompressorOhpcfListenerObject*, const EebusDuration*));
  MOCK_METHOD1(OnStop, void(CompressorOhpcfListenerObject*));
  MOCK_METHOD1(OnPause, void(CompressorOhpcfListenerObject*));
  MOCK_METHOD1(OnResume, void(CompressorOhpcfListenerObject*));
};

typedef struct CompressorOhpcfListenerMock {
  /** Implements the Compressor Ohpcf Listener Interface */
  CompressorOhpcfListenerObject obj;
  CompressorOhpcfListenerGMock* gmock;
} CompressorOhpcfListenerMock;

#define COMPRESSOR_OHPCF_LISTENER_MOCK(obj) ((CompressorOhpcfListenerMock*)(obj))

CompressorOhpcfListenerMock* CompressorOhpcfListenerMockCreate(void);

static inline void CompressorOhpcfListenerMockDelete(CompressorOhpcfListenerMock* self) {
  if (self != nullptr) {
    COMPRESSOR_OHPCF_LISTENER_DESTRUCT(COMPRESSOR_OHPCF_LISTENER_OBJECT(self));
    EEBUS_FREE(self);
  }
}

#endif  // TESTS_SRC_MOCKS_USE_CASE_ACTOR_COMPRESSOR_COMPRESSOR_OHPCF_LISTENER_MOCK_H_
