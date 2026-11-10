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
 * @brief Entity Local Mock "class"
 */

#ifndef TESTS_UNIT_TESTS_MOCKS_SPINE_ENTITY_ENTITY_LOCAL_MOCK_H_
#define TESTS_UNIT_TESTS_MOCKS_SPINE_ENTITY_ENTITY_LOCAL_MOCK_H_

#include <gmock/gmock.h>

#include <memory>

#include "entity_mock.h"
#include "src/spine/api/entity_local_interface.h"

class EntityLocalGMockInterface : public EntityGMockInterface {
 public:
  virtual ~EntityLocalGMockInterface() {};
  virtual DeviceLocalObject* GetDevice(const EntityLocalObject* self)                = 0;
  virtual HeartbeatManagerObject* GetHeartbeatManager(const EntityLocalObject* self) = 0;
  virtual void AddFeature(EntityLocalObject* self, FeatureLocalObject* feature)      = 0;
  virtual FeatureLocalObject*
  GetFeatureWithTypeAndRole(const EntityLocalObject* self, FeatureTypeType feature_type, RoleType role)
      = 0;
  virtual FeatureLocalObject*
  AddFeatureWithTypeAndRole(EntityLocalObject* self, FeatureTypeType feature_type, RoleType role)
      = 0;
  virtual FeatureLocalObject* GetFeatureWithId(const EntityLocalObject* self, const uint32_t* feature_id) = 0;
  virtual const Vector* GetFeatures(const EntityLocalObject* self)                                        = 0;
  virtual void AddUseCaseSupport(
      EntityLocalObject* self,
      UseCaseActorType actor,
      UseCaseNameType use_case_name_id,
      SpecificationVersionType version,
      const char* sub_revision,
      bool available,
      const UseCaseScenarioSupportType* scenarios,
      size_t scenarios_size
  )                                                                                                       = 0;
  virtual bool HasUseCaseSupport(const EntityLocalObject* self, const UseCaseFilterType* use_case_filter) = 0;
  virtual void SetUseCaseAvailability(EntityLocalObject* self, const UseCaseFilterType* use_case_filter, bool available)
      = 0;
  virtual void RemoveUseCaseSupports(
      EntityLocalObject* self,
      const UseCaseFilterType* use_case_filters,
      size_t use_case_filters_size
  )                                                                                                              = 0;
  virtual void RemoveAllUseCaseSupports(EntityLocalObject* self)                                                 = 0;
  virtual void RemoveAllSubscriptions(EntityLocalObject* self)                                                   = 0;
  virtual void RemoveAllBindings(EntityLocalObject* self)                                                        = 0;
  virtual NodeManagementDetailedDiscoveryEntityInformationType* CreateInformation(const EntityLocalObject* self) = 0;
  virtual void Tick(EntityLocalObject* self)                                                                     = 0;
};

class EntityLocalGMock : public EntityLocalGMockInterface {
 public:
  virtual ~EntityLocalGMock() {};
  MOCK_METHOD1(Destruct, void(EntityObject*));
  MOCK_METHOD1(GetAddress, const EntityAddressType*(const EntityObject*));
  MOCK_METHOD1(GetType, EntityTypeType(const EntityObject*));
  MOCK_METHOD1(GetDescription, const char*(const EntityObject*));
  MOCK_METHOD2(SetDescription, void(EntityObject*, const char*));
  MOCK_METHOD1(GetNextFeatureId, uint32_t(EntityObject*));
  MOCK_METHOD1(GetDevice, DeviceLocalObject*(const EntityLocalObject*));
  MOCK_METHOD1(GetHeartbeatManager, HeartbeatManagerObject*(const EntityLocalObject*));
  MOCK_METHOD2(AddFeature, void(EntityLocalObject*, FeatureLocalObject*));
  MOCK_METHOD3(GetFeatureWithTypeAndRole, FeatureLocalObject*(const EntityLocalObject*, FeatureTypeType, RoleType));
  MOCK_METHOD3(AddFeatureWithTypeAndRole, FeatureLocalObject*(EntityLocalObject*, FeatureTypeType, RoleType));
  MOCK_METHOD2(GetFeatureWithId, FeatureLocalObject*(const EntityLocalObject*, const uint32_t*));
  MOCK_METHOD1(GetFeatures, const Vector*(const EntityLocalObject*));
  MOCK_METHOD8(
      AddUseCaseSupport,
      void(
          EntityLocalObject*,
          UseCaseActorType,
          UseCaseNameType,
          SpecificationVersionType,
          const char*,
          bool,
          const UseCaseScenarioSupportType*,
          size_t
      )
  );
  MOCK_METHOD2(HasUseCaseSupport, bool(const EntityLocalObject*, const UseCaseFilterType*));
  MOCK_METHOD3(SetUseCaseAvailability, void(EntityLocalObject*, const UseCaseFilterType*, bool));
  MOCK_METHOD3(RemoveUseCaseSupports, void(EntityLocalObject*, const UseCaseFilterType*, size_t));
  MOCK_METHOD1(RemoveAllUseCaseSupports, void(EntityLocalObject*));
  MOCK_METHOD1(RemoveAllSubscriptions, void(EntityLocalObject*));
  MOCK_METHOD1(RemoveAllBindings, void(EntityLocalObject*));
  MOCK_METHOD1(CreateInformation, NodeManagementDetailedDiscoveryEntityInformationType*(const EntityLocalObject*));
  MOCK_METHOD1(Tick, void(EntityLocalObject*));
};

typedef struct EntityLocalMock {
  /** Implements the Entity Local Interface */
  EntityLocalObject obj;
  EntityLocalGMock* gmock;
} EntityLocalMock;

#define ENTITY_LOCAL_MOCK(obj) ((EntityLocalMock*)(obj))

EntityLocalMock* EntityLocalMockCreate(void);

static inline void EntityLocalMockDelete(EntityLocalMock* self) {
  if (self != nullptr) {
    ENTITY_DESTRUCT(ENTITY_OBJECT(self));
    EEBUS_FREE(self);
  }
}

#endif  // TESTS_UNIT_TESTS_MOCKS_SPINE_ENTITY_ENTITY_LOCAL_MOCK_H_
