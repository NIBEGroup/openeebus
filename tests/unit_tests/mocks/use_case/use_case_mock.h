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
 * @brief Use Case Mock "class"
 */

#ifndef TESTS_UNIT_TESTS_MOCKS_USE_CASE_USE_CASE_MOCK_H_
#define TESTS_UNIT_TESTS_MOCKS_USE_CASE_USE_CASE_MOCK_H_

#include <gmock/gmock.h>

#include <memory>

#include "src/use_case/api/use_case_interface.h"

class UseCaseGMockInterface {
 public:
  virtual ~UseCaseGMockInterface() {};
  virtual void Destruct(UseCaseObject* self)                                                            = 0;
  virtual bool IsEntityCompatible(const UseCaseObject* self, const EntityRemoteObject* remote_entity)   = 0;
  virtual bool IsUseCaseCompatible(const UseCaseObject* self, const UseCaseFilterType* use_case_filter) = 0;
  virtual EntityRemoteObject* GetRemoteEntityWithAddress(
      const UseCaseObject* self,
      const EntityAddressType* remote_entity_addr
  ) = 0;
};

class UseCaseGMock : public UseCaseGMockInterface {
 public:
  virtual ~UseCaseGMock() {};
  MOCK_METHOD1(Destruct, void(UseCaseObject*));
  MOCK_METHOD2(IsEntityCompatible, bool(const UseCaseObject*, const EntityRemoteObject*));
  MOCK_METHOD2(IsUseCaseCompatible, bool(const UseCaseObject*, const UseCaseFilterType*));
  MOCK_METHOD2(GetRemoteEntityWithAddress, EntityRemoteObject*(const UseCaseObject*, const EntityAddressType*));
};

typedef struct UseCaseMock {
  /** Implements the Use Case Interface */
  UseCaseObject obj;
  UseCaseGMock* gmock;
} UseCaseMock;

#define USE_CASE_MOCK(obj) ((UseCaseMock*)(obj))

UseCaseMock* UseCaseMockCreate(void);

static inline void UseCaseMockDelete(UseCaseMock* self) {
  if (self != nullptr) {
    USE_CASE_DESTRUCT(USE_CASE_OBJECT(self));
    EEBUS_FREE(self);
  }
}

#endif  // TESTS_UNIT_TESTS_MOCKS_USE_CASE_USE_CASE_MOCK_H_
