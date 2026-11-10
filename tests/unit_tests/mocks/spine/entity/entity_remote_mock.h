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
 * @brief Entity Remote Mock "class"
 */

#ifndef TESTS_UNIT_TESTS_MOCKS_SPINE_ENTITY_ENTITY_REMOTE_MOCK_H_
#define TESTS_UNIT_TESTS_MOCKS_SPINE_ENTITY_ENTITY_REMOTE_MOCK_H_

#include <gmock/gmock.h>

#include <memory>

#include "entity_mock.h"
#include "src/spine/api/entity_remote_interface.h"

class EntityRemoteGMockInterface : public EntityGMockInterface {
 public:
  virtual ~EntityRemoteGMockInterface() {};
  virtual DeviceRemoteObject* GetDevice(const EntityRemoteObject* self)               = 0;
  virtual void UpdateDeviceAddress(EntityRemoteObject* self, const char* device_addr) = 0;
  virtual void AddFeature(EntityRemoteObject* self, FeatureRemoteObject* feature)     = 0;
  virtual void RemoveAllFeatures(EntityRemoteObject* self)                            = 0;
  virtual FeatureRemoteObject*
  GetFeatureWithTypeAndRole(const EntityRemoteObject* self, FeatureTypeType feature_type, RoleType role)
      = 0;
  virtual FeatureRemoteObject* GetFeatureWithId(const EntityRemoteObject* self, const uint32_t* feature_id) = 0;
  virtual const Vector* GetFeatures(const EntityRemoteObject* self)                                         = 0;
  virtual bool HasUseCaseSupport(const EntityRemoteObject* self, const UseCaseFilterType* use_case_filter)  = 0;
};

class EntityRemoteGMock : public EntityRemoteGMockInterface {
 public:
  virtual ~EntityRemoteGMock() {};
  MOCK_METHOD1(Destruct, void(EntityObject*));
  MOCK_METHOD1(GetAddress, const EntityAddressType*(const EntityObject*));
  MOCK_METHOD1(GetType, EntityTypeType(const EntityObject*));
  MOCK_METHOD1(GetDescription, const char*(const EntityObject*));
  MOCK_METHOD2(SetDescription, void(EntityObject*, const char*));
  MOCK_METHOD1(GetNextFeatureId, uint32_t(EntityObject*));
  MOCK_METHOD1(GetDevice, DeviceRemoteObject*(const EntityRemoteObject*));
  MOCK_METHOD2(UpdateDeviceAddress, void(EntityRemoteObject*, const char*));
  MOCK_METHOD2(AddFeature, void(EntityRemoteObject*, FeatureRemoteObject*));
  MOCK_METHOD1(RemoveAllFeatures, void(EntityRemoteObject*));
  MOCK_METHOD3(GetFeatureWithTypeAndRole, FeatureRemoteObject*(const EntityRemoteObject*, FeatureTypeType, RoleType));
  MOCK_METHOD2(GetFeatureWithId, FeatureRemoteObject*(const EntityRemoteObject*, const uint32_t*));
  MOCK_METHOD1(GetFeatures, const Vector*(const EntityRemoteObject*));
  MOCK_METHOD2(HasUseCaseSupport, bool(const EntityRemoteObject*, const UseCaseFilterType*));
};

typedef struct EntityRemoteMock {
  /** Implements the Entity Remote Interface */
  EntityRemoteObject obj;
  EntityRemoteGMock* gmock;
} EntityRemoteMock;

#define ENTITY_REMOTE_MOCK(obj) ((EntityRemoteMock*)(obj))

EntityRemoteMock* EntityRemoteMockCreate(void);

static inline void EntityRemoteMockDelete(EntityRemoteMock* self) {
  if (self != nullptr) {
    ENTITY_DESTRUCT(ENTITY_OBJECT(self));
    EEBUS_FREE(self);
  }
}

#endif  // TESTS_UNIT_TESTS_MOCKS_SPINE_ENTITY_ENTITY_REMOTE_MOCK_H_
