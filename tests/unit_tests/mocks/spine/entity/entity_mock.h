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
 * @brief Entity Mock "class"
 */

#ifndef TESTS_UNIT_TESTS_MOCKS_SPINE_ENTITY_ENTITY_MOCK_H_
#define TESTS_UNIT_TESTS_MOCKS_SPINE_ENTITY_ENTITY_MOCK_H_

#include <gmock/gmock.h>

#include <memory>

#include "src/common/eebus_malloc.h"
#include "src/spine/api/entity_interface.h"

class EntityGMockInterface {
 public:
  virtual ~EntityGMockInterface() {};
  virtual void Destruct(EntityObject* self)                                = 0;
  virtual const EntityAddressType* GetAddress(const EntityObject* self)    = 0;
  virtual EntityTypeType GetType(const EntityObject* self)                 = 0;
  virtual const char* GetDescription(const EntityObject* self)             = 0;
  virtual void SetDescription(EntityObject* self, const char* description) = 0;
  virtual uint32_t GetNextFeatureId(EntityObject* self)                    = 0;
};

class EntityGMock : public EntityGMockInterface {
 public:
  virtual ~EntityGMock() {};
  MOCK_METHOD1(Destruct, void(EntityObject*));
  MOCK_METHOD1(GetAddress, const EntityAddressType*(const EntityObject*));
  MOCK_METHOD1(GetType, EntityTypeType(const EntityObject*));
  MOCK_METHOD1(GetDescription, const char*(const EntityObject*));
  MOCK_METHOD2(SetDescription, void(EntityObject*, const char*));
  MOCK_METHOD1(GetNextFeatureId, uint32_t(EntityObject*));
};

typedef struct EntityMock {
  /** Implements the Entity Interface */
  EntityObject obj;
  EntityGMock* gmock;
} EntityMock;

#define ENTITY_MOCK(obj) ((EntityMock*)(obj))

EntityMock* EntityMockCreate(void);

static inline void EntityMockDelete(EntityMock* self) {
  if (self != nullptr) {
    ENTITY_DESTRUCT(ENTITY_OBJECT(self));
    EEBUS_FREE(self);
  }
}

#endif  // TESTS_UNIT_TESTS_MOCKS_SPINE_ENTITY_ENTITY_MOCK_H_
