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
 * @brief Eebus Timer Mock "class"
 */

#ifndef TESTS_SRC_MOCKS_COMMON_EEBUS_TIMER_EEBUS_TIMER_MOCK_H_
#define TESTS_SRC_MOCKS_COMMON_EEBUS_TIMER_EEBUS_TIMER_MOCK_H_

#include <gmock/gmock.h>

#include <memory>

#include "src/common/api/eebus_timer_interface.h"
#include "src/common/eebus_malloc.h"

class EebusTimerGMockInterface {
 public:
  virtual ~EebusTimerGMockInterface() {};
  virtual void Destruct(EebusTimerObject* self)                                    = 0;
  virtual void Start(EebusTimerObject* self, uint32_t timeout_ms, bool autoreload) = 0;
  virtual void Stop(EebusTimerObject* self)                                        = 0;
  virtual uint32_t GetRemainingTime(const EebusTimerObject* self)                  = 0;
  virtual EebusTimerState GetTimerState(const EebusTimerObject* self)              = 0;
};

class EebusTimerGMock : public EebusTimerGMockInterface {
 public:
  virtual ~EebusTimerGMock() {};
  MOCK_METHOD1(Destruct, void(EebusTimerObject*));
  MOCK_METHOD3(Start, void(EebusTimerObject*, uint32_t, bool));
  MOCK_METHOD1(Stop, void(EebusTimerObject*));
  MOCK_METHOD1(GetRemainingTime, uint32_t(const EebusTimerObject*));
  MOCK_METHOD1(GetTimerState, EebusTimerState(const EebusTimerObject*));
};

typedef struct EebusTimerMock {
  /** Implements the Eebus Timer Interface */
  EebusTimerObject obj;
  EebusTimerGMock* gmock;

  /**
   * @brief What the object under test asked to be called on expiry
   *
   * A mock never expires on its own, so a test that needs to see what happens
   * when the timeout arrives calls this itself, with @ref ctx.
   */
  EebusTimerTimeoutCallback cb;
  void* ctx;
} EebusTimerMock;

/**
 * @brief Calls the timeout callback the object under test registered
 * @param self Mock whose timer is to expire
 */
static inline void EebusTimerMockExpire(EebusTimerMock* self) {
  if ((self != nullptr) && (self->cb != nullptr)) {
    self->cb(self->ctx);
  }
}

#define EEBUS_TIMER_MOCK(obj) ((EebusTimerMock*)(obj))

EebusTimerMock* EebusTimerMockCreate(void);

static inline void EebusTimerMockDelete(EebusTimerMock* self) {
  if (self != nullptr) {
    EEBUS_TIMER_DESTRUCT(EEBUS_TIMER_OBJECT(self));
    EEBUS_FREE(self);
  }
}

#endif  // TESTS_SRC_MOCKS_COMMON_EEBUS_TIMER_EEBUS_TIMER_MOCK_H_
