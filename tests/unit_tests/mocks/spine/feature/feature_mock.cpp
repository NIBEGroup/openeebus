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
 * @brief Feature mock implementation
 */

#include "feature_mock.h"

#include <gmock/gmock.h>

#include "src/spine/api/feature_interface.h"

static void Destruct(FeatureObject* self);
static const FeatureAddressType* GetAddress(const FeatureObject* self);
static FeatureTypeType GetType(const FeatureObject* self);
static RoleType GetRole(const FeatureObject* self);
static const OperationsObject* GetFunctionOperations(const FeatureObject* self, FunctionType fcn_type);
static const char* GetDescription(const FeatureObject* self);
static void SetDescription(FeatureObject* self, const char* description);
static const char* ToString(const FeatureObject* self);

static const FeatureInterface feature_methods = {
    .destruct                = Destruct,
    .get_address             = GetAddress,
    .get_type                = GetType,
    .get_role                = GetRole,
    .get_function_operations = GetFunctionOperations,
    .get_description         = GetDescription,
    .set_description         = SetDescription,
    .to_string               = ToString,
};

static EebusError FeatureMockConstruct(FeatureMock* self);

EebusError FeatureMockConstruct(FeatureMock* self) {
  // Override "virtual functions table"
  FEATURE_INTERFACE(self) = &feature_methods;

  self->gmock = new FeatureGMock();
  if (self->gmock == nullptr) {
    return kEebusErrorMemoryAllocate;
  }

  return kEebusErrorOk;
}

FeatureMock* FeatureMockCreate(void) {
  FeatureMock* const mock = (FeatureMock*)EEBUS_MALLOC(sizeof(FeatureMock));
  if (mock == nullptr) {
    return nullptr;
  }

  if (FeatureMockConstruct(mock) != kEebusErrorOk) {
    FeatureMockDelete(mock);
    return nullptr;
  }

  return mock;
}

void Destruct(FeatureObject* self) {
  FeatureMock* const mock = FEATURE_MOCK(self);
  mock->gmock->Destruct(self);
  delete mock->gmock;
}

const FeatureAddressType* GetAddress(const FeatureObject* self) {
  FeatureMock* const mock = FEATURE_MOCK(self);
  return mock->gmock->GetAddress(self);
}

FeatureTypeType GetType(const FeatureObject* self) {
  FeatureMock* const mock = FEATURE_MOCK(self);
  return mock->gmock->GetType(self);
}

RoleType GetRole(const FeatureObject* self) {
  FeatureMock* const mock = FEATURE_MOCK(self);
  return mock->gmock->GetRole(self);
}

const OperationsObject* GetFunctionOperations(const FeatureObject* self, FunctionType fcn_type) {
  FeatureMock* const mock = FEATURE_MOCK(self);
  return mock->gmock->GetFunctionOperations(self, fcn_type);
}

const char* GetDescription(const FeatureObject* self) {
  FeatureMock* const mock = FEATURE_MOCK(self);
  return mock->gmock->GetDescription(self);
}

void SetDescription(FeatureObject* self, const char* description) {
  FeatureMock* const mock = FEATURE_MOCK(self);
  mock->gmock->SetDescription(self, description);
}

const char* ToString(const FeatureObject* self) {
  FeatureMock* const mock = FEATURE_MOCK(self);
  return mock->gmock->ToString(self);
}
