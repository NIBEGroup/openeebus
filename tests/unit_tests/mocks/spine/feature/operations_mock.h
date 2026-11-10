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
 * @brief Operations Mock "class"
 */

#ifndef TESTS_UNIT_TESTS_MOCKS_SPINE_FEATURE_OPERATIONS_MOCK_H_
#define TESTS_UNIT_TESTS_MOCKS_SPINE_FEATURE_OPERATIONS_MOCK_H_

#include <gmock/gmock.h>

#include <memory>

#include "src/common/eebus_malloc.h"
#include "src/spine/api/operations_interface.h"

class OperationsGMockInterface {
 public:
  virtual ~OperationsGMockInterface() {};
  virtual void Destruct(OperationsObject* self)                                      = 0;
  virtual bool GetRead(const OperationsObject* self)                                 = 0;
  virtual bool GetReadPartial(const OperationsObject* self)                          = 0;
  virtual bool GetWrite(const OperationsObject* self)                                = 0;
  virtual bool GetWritePartial(const OperationsObject* self)                         = 0;
  virtual const char* ToString(const OperationsObject* self)                         = 0;
  virtual const PossibleOperationsType* GetInformation(const OperationsObject* self) = 0;
};

class OperationsGMock : public OperationsGMockInterface {
 public:
  virtual ~OperationsGMock() {};
  MOCK_METHOD1(Destruct, void(OperationsObject*));
  MOCK_METHOD1(GetRead, bool(const OperationsObject*));
  MOCK_METHOD1(GetReadPartial, bool(const OperationsObject*));
  MOCK_METHOD1(GetWrite, bool(const OperationsObject*));
  MOCK_METHOD1(GetWritePartial, bool(const OperationsObject*));
  MOCK_METHOD1(ToString, const char*(const OperationsObject*));
  MOCK_METHOD1(GetInformation, const PossibleOperationsType*(const OperationsObject*));
};

typedef struct OperationsMock {
  /** Implements the Operations Interface */
  OperationsObject obj;
  OperationsGMock* gmock;
} OperationsMock;

#define OPERATIONS_MOCK(obj) ((OperationsMock*)(obj))

OperationsMock* OperationsMockCreate(void);

static inline void OperationsMockDelete(OperationsMock* self) {
  if (self != nullptr) {
    OPERATIONS_DESTRUCT(OPERATIONS_OBJECT(self));
    EEBUS_FREE(self);
  }
}

#endif  // TESTS_UNIT_TESTS_MOCKS_SPINE_FEATURE_OPERATIONS_MOCK_H_
