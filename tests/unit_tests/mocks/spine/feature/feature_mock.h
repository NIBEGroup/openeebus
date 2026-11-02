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
 * @brief Feature Mock "class"
 */

#ifndef TESTS_UNIT_TESTS_MOCKS_SPINE_FEATURE_FEATURE_MOCK_H_
#define TESTS_UNIT_TESTS_MOCKS_SPINE_FEATURE_FEATURE_MOCK_H_

#include <gmock/gmock.h>

#include <memory>

#include "src/common/eebus_malloc.h"
#include "src/spine/api/feature_interface.h"

class FeatureGMockInterface {
 public:
  virtual ~FeatureGMockInterface() {};
  virtual void Destruct(FeatureObject* self)                                                              = 0;
  virtual const FeatureAddressType* GetAddress(const FeatureObject* self)                                 = 0;
  virtual FeatureTypeType GetType(const FeatureObject* self)                                              = 0;
  virtual RoleType GetRole(const FeatureObject* self)                                                     = 0;
  virtual const OperationsObject* GetFunctionOperations(const FeatureObject* self, FunctionType fcn_type) = 0;
  virtual const char* GetDescription(const FeatureObject* self)                                           = 0;
  virtual void SetDescription(FeatureObject* self, const char* description)                               = 0;
  virtual const char* ToString(const FeatureObject* self)                                                 = 0;
};

class FeatureGMock : public FeatureGMockInterface {
 public:
  virtual ~FeatureGMock() {};
  MOCK_METHOD1(Destruct, void(FeatureObject*));
  MOCK_METHOD1(GetAddress, const FeatureAddressType*(const FeatureObject*));
  MOCK_METHOD1(GetType, FeatureTypeType(const FeatureObject*));
  MOCK_METHOD1(GetRole, RoleType(const FeatureObject*));
  MOCK_METHOD2(GetFunctionOperations, const OperationsObject*(const FeatureObject*, FunctionType));
  MOCK_METHOD1(GetDescription, const char*(const FeatureObject*));
  MOCK_METHOD2(SetDescription, void(FeatureObject*, const char*));
  MOCK_METHOD1(ToString, const char*(const FeatureObject*));
};

typedef struct FeatureMock {
  /** Implements the Feature Interface */
  FeatureObject obj;
  FeatureGMock* gmock;
} FeatureMock;

#define FEATURE_MOCK(obj) ((FeatureMock*)(obj))

FeatureMock* FeatureMockCreate(void);

static inline void FeatureMockDelete(FeatureMock* self) {
  if (self != nullptr) {
    FEATURE_DESTRUCT(FEATURE_OBJECT(self));
    EEBUS_FREE(self);
  }
}

#endif  // TESTS_UNIT_TESTS_MOCKS_SPINE_FEATURE_FEATURE_MOCK_H_
