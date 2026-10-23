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
 * @brief Eebus Timer mock implementation
 */

#include "eebus_timer_mock.h"

#include <gmock/gmock.h>

#include "src/common/api/eebus_timer_interface.h"
#include "src/common/eebus_arguments.h"
#include "src/common/eebus_errors.h"
#include "src/common/eebus_timer/eebus_timer.h"

static void Destruct(EebusTimerObject* self);
static void Start(EebusTimerObject* self, uint32_t timeout_ms, bool autoreload);
static void Stop(EebusTimerObject* self);
static uint32_t GetRemainingTime(const EebusTimerObject* self);
static EebusTimerState GetTimerState(const EebusTimerObject* self);

static const EebusTimerInterface eebus_timer_methods = {
    .destruct           = Destruct,
    .start              = Start,
    .stop               = Stop,
    .get_remaining_time = GetRemainingTime,
    .get_timer_state    = GetTimerState,
};

static EebusError EebusTimerMockConstruct(EebusTimerMock* self);

EebusError EebusTimerMockConstruct(EebusTimerMock* self) {
  // Override "virtual functions table"
  EEBUS_TIMER_INTERFACE(self) = &eebus_timer_methods;

  self->gmock = new EebusTimerGMock();
  if (self->gmock == nullptr) {
    return kEebusErrorMemoryAllocate;
  }

  self->cb  = nullptr;
  self->ctx = nullptr;

  return kEebusErrorOk;
}

EebusTimerMock* EebusTimerMockCreate(void) {
  EebusTimerMock* const mock = (EebusTimerMock*)EEBUS_MALLOC(sizeof(EebusTimerMock));
  if (mock == nullptr) {
    return nullptr;
  }

  if (EebusTimerMockConstruct(mock) != kEebusErrorOk) {
    EebusTimerMockDelete(mock);
    return nullptr;
  }

  return mock;
}

EebusTimerObject* EebusTimerCreate(EebusTimerTimeoutCallback cb, void* ctx) {
  EebusTimerMock* const mock = EebusTimerMockCreate();
  if (mock == nullptr) {
    return nullptr;
  }

  // Retained so that a test can expire the timer with EebusTimerMockExpire().
  mock->cb  = cb;
  mock->ctx = ctx;

  return EEBUS_TIMER_OBJECT(mock);
}

void Destruct(EebusTimerObject* self) {
  EebusTimerMock* const mock = EEBUS_TIMER_MOCK(self);
  mock->gmock->Destruct(self);
  delete mock->gmock;
}

void Start(EebusTimerObject* self, uint32_t timeout_ms, bool autoreload) {
  EebusTimerMock* const mock = EEBUS_TIMER_MOCK(self);
  mock->gmock->Start(self, timeout_ms, autoreload);
}

void Stop(EebusTimerObject* self) {
  EebusTimerMock* const mock = EEBUS_TIMER_MOCK(self);
  mock->gmock->Stop(self);
}

uint32_t GetRemainingTime(const EebusTimerObject* self) {
  EebusTimerMock* const mock = EEBUS_TIMER_MOCK(self);
  return mock->gmock->GetRemainingTime(self);
}

EebusTimerState GetTimerState(const EebusTimerObject* self) {
  EebusTimerMock* const mock = EEBUS_TIMER_MOCK(self);
  return mock->gmock->GetTimerState(self);
}
