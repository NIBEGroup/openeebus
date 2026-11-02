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
 * @brief Eebus Thread Mock "class"
 */

#ifndef TESTS_UNIT_TESTS_MOCKS_COMMON_EEBUS_THREAD_EEBUS_THREAD_MOCK_H_
#define TESTS_UNIT_TESTS_MOCKS_COMMON_EEBUS_THREAD_EEBUS_THREAD_MOCK_H_

#include <gmock/gmock.h>

#include <memory>

#include "src/common/api/eebus_thread_interface.h"
#include "src/common/eebus_malloc.h"

class EebusThreadGMockInterface {
 public:
  virtual ~EebusThreadGMockInterface() {};
  virtual void Destruct(EebusThreadObject* self) = 0;
  virtual void Join(EebusThreadObject* self)     = 0;
};

class EebusThreadGMock : public EebusThreadGMockInterface {
 public:
  virtual ~EebusThreadGMock() {};
  MOCK_METHOD1(Destruct, void(EebusThreadObject*));
  MOCK_METHOD1(Join, void(EebusThreadObject*));
};

typedef struct EebusThreadMock {
  /** Implements the Eebus Thread Interface */
  EebusThreadObject obj;
  EebusThreadGMock* gmock;
} EebusThreadMock;

#define EEBUS_THREAD_MOCK(obj) ((EebusThreadMock*)(obj))

EebusThreadMock* EebusThreadMockCreate(void);

static inline void EebusThreadMockDelete(EebusThreadMock* self) {
  if (self != nullptr) {
    EEBUS_THREAD_DESTRUCT(EEBUS_THREAD_OBJECT(self));
    EEBUS_FREE(self);
  }
}

#endif  // TESTS_UNIT_TESTS_MOCKS_COMMON_EEBUS_THREAD_EEBUS_THREAD_MOCK_H_
