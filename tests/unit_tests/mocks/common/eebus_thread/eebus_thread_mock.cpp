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
 * @brief Eebus Thread mock implementation
 */

#include "eebus_thread_mock.h"

#include <gmock/gmock.h>

#include "src/common/api/eebus_thread_interface.h"
#include "src/common/eebus_errors.h"

static void Destruct(EebusThreadObject* self);
static void Join(EebusThreadObject* self);

static const EebusThreadInterface eebus_thread_methods = {
    .destruct = Destruct,
    .join     = Join,
};

static EebusError EebusThreadMockConstruct(EebusThreadMock* self);

EebusError EebusThreadMockConstruct(EebusThreadMock* self) {
  // Override "virtual functions table"
  EEBUS_THREAD_INTERFACE(self) = &eebus_thread_methods;

  self->gmock = new EebusThreadGMock();
  if (self->gmock == nullptr) {
    return kEebusErrorMemoryAllocate;
  }

  return kEebusErrorOk;
}

EebusThreadMock* EebusThreadMockCreate(void) {
  EebusThreadMock* const mock = (EebusThreadMock*)EEBUS_MALLOC(sizeof(EebusThreadMock));
  if (mock == nullptr) {
    return nullptr;
  }

  if (EebusThreadMockConstruct(mock) != kEebusErrorOk) {
    EebusThreadMockDelete(mock);
    return nullptr;
  }

  return mock;
}

void Destruct(EebusThreadObject* self) {
  EebusThreadMock* const mock = EEBUS_THREAD_MOCK(self);
  mock->gmock->Destruct(self);
  delete mock->gmock;
}

void Join(EebusThreadObject* self) {
  EebusThreadMock* const mock = EEBUS_THREAD_MOCK(self);
  mock->gmock->Join(self);
}
