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
 * @brief Operations mock implementation
 */

#include "operations_mock.h"

#include <gmock/gmock.h>

#include "src/spine/api/operations_interface.h"

static void Destruct(OperationsObject* self);
static bool GetRead(const OperationsObject* self);
static bool GetReadPartial(const OperationsObject* self);
static bool GetWrite(const OperationsObject* self);
static bool GetWritePartial(const OperationsObject* self);
static const char* ToString(const OperationsObject* self);
static const PossibleOperationsType* GetInformation(const OperationsObject* self);

static const OperationsInterface operations_methods = {
    .destruct          = Destruct,
    .get_read          = GetRead,
    .get_read_partial  = GetReadPartial,
    .get_write         = GetWrite,
    .get_write_partial = GetWritePartial,
    .to_string         = ToString,
    .get_information   = GetInformation,
};

static EebusError OperationsMockConstruct(OperationsMock* self);

EebusError OperationsMockConstruct(OperationsMock* self) {
  // Override "virtual functions table"
  OPERATIONS_INTERFACE(self) = &operations_methods;

  self->gmock = new OperationsGMock();
  if (self->gmock == nullptr) {
    return kEebusErrorMemoryAllocate;
  }

  return kEebusErrorOk;
}

OperationsMock* OperationsMockCreate(void) {
  OperationsMock* const mock = (OperationsMock*)EEBUS_MALLOC(sizeof(OperationsMock));
  if (mock == nullptr) {
    return nullptr;
  }

  if (OperationsMockConstruct(mock) != kEebusErrorOk) {
    OperationsMockDelete(mock);
    return nullptr;
  }

  return mock;
}

void Destruct(OperationsObject* self) {
  OperationsMock* const mock = OPERATIONS_MOCK(self);
  mock->gmock->Destruct(self);
  delete mock->gmock;
}

bool GetRead(const OperationsObject* self) {
  OperationsMock* const mock = OPERATIONS_MOCK(self);
  return mock->gmock->GetRead(self);
}

bool GetReadPartial(const OperationsObject* self) {
  OperationsMock* const mock = OPERATIONS_MOCK(self);
  return mock->gmock->GetReadPartial(self);
}

bool GetWrite(const OperationsObject* self) {
  OperationsMock* const mock = OPERATIONS_MOCK(self);
  return mock->gmock->GetWrite(self);
}

bool GetWritePartial(const OperationsObject* self) {
  OperationsMock* const mock = OPERATIONS_MOCK(self);
  return mock->gmock->GetWritePartial(self);
}

const char* ToString(const OperationsObject* self) {
  OperationsMock* const mock = OPERATIONS_MOCK(self);
  return mock->gmock->ToString(self);
}

const PossibleOperationsType* GetInformation(const OperationsObject* self) {
  OperationsMock* const mock = OPERATIONS_MOCK(self);
  return mock->gmock->GetInformation(self);
}
