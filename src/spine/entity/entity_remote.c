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
 * @brief Entity Remote implementation
 */

#include "src/common/array_util.h"
#include "src/common/eebus_malloc.h"
#include "src/common/string_util.h"
#include "src/spine/api/device_remote_interface.h"
#include "src/spine/api/entity_remote_interface.h"
#include "src/spine/entity/entity.h"
#include "src/spine/feature/feature_remote.h"

typedef struct EntityRemote EntityRemote;

struct EntityRemote {
    /** Inherits the Entity */
    Entity obj;

    DeviceRemoteObject* device;
    Vector features;
};

#define ENTITY_REMOTE(obj) ((EntityRemote*)(obj))

static void Destruct(EntityObject* self);
static DeviceRemoteObject* GetDevice(const EntityRemoteObject* self);
static void UpdateDeviceAddress(EntityRemoteObject* self, const char* device_addr);
static void AddFeature(EntityRemoteObject* self, FeatureRemoteObject* feature);
static void RemoveAllFeatures(EntityRemoteObject* self);
static FeatureRemoteObject*
GetFeatureWithTypeAndRole(const EntityRemoteObject* self, FeatureTypeType feature_type, RoleType role);
static FeatureRemoteObject* GetFeatureWithId(const EntityRemoteObject* self, const uint32_t* feature_id);
static const Vector* GetFeatures(const EntityRemoteObject* self);
static bool HasUseCaseSupport(const EntityRemoteObject* self, const UseCaseFilterType* use_case_filter);

static const EntityRemoteInterface entity_remote_methods = {
    .entity_interface = {
        .destruct            = Destruct,
        .get_address         = EntityGetAddress,
        .get_type            = EntityGetType,
        .get_description     = EntityGetDescription,
        .set_description     = EntitySetDescription,
        .get_next_feature_id = EntityGetNextFeatureId,
    },

    .get_device                     = GetDevice,
    .update_device_address          = UpdateDeviceAddress,
    .add_feature                    = AddFeature,
    .remove_all_features            = RemoveAllFeatures,
    .get_feature_with_type_and_role = GetFeatureWithTypeAndRole,
    .get_feature_with_id            = GetFeatureWithId,
    .get_features                   = GetFeatures,
    .has_use_case_support           = HasUseCaseSupport,
};

static void EntityRemoteConstruct(
    EntityRemote* self,
    DeviceRemoteObject* device,
    EntityTypeType type,
    const uint32_t* entity_id,
    size_t entity_id_size
);

void EntityRemoteConstruct(
    EntityRemote* self,
    DeviceRemoteObject* device,
    EntityTypeType type,
    const uint32_t* entity_id,
    size_t entity_id_size
) {
    EntityConstruct(ENTITY(self), type, DEVICE_GET_ADDRESS(DEVICE_OBJECT(device)), entity_id, entity_id_size);
    // Override "virtual functions table"
    ENTITY_REMOTE_INTERFACE(self) = &entity_remote_methods;

    self->device = device;
    VectorConstruct(&self->features);
}

EntityRemoteObject*
EntityRemoteCreate(DeviceRemoteObject* device, EntityTypeType type, const uint32_t* entity_id, size_t entity_id_size) {
    EntityRemote* const entity_remote = (EntityRemote*)EEBUS_MALLOC(sizeof(EntityRemote));

    EntityRemoteConstruct(entity_remote, device, type, entity_id, entity_id_size);

    return ENTITY_REMOTE_OBJECT(entity_remote);
}

void Destruct(EntityObject* self) {
    RemoveAllFeatures(ENTITY_REMOTE_OBJECT(self));
    VectorDestruct(&ENTITY_REMOTE(self)->features);

    EntityDestruct(self);
}

DeviceRemoteObject* GetDevice(const EntityRemoteObject* self) {
    return ENTITY_REMOTE(self)->device;
}

void UpdateDeviceAddress(EntityRemoteObject* self, const char* device_addr) {
    Entity* const entity = ENTITY(self);

    StringDelete((char*)entity->address->device);
    entity->address->device = StringCopy(device_addr);
}

void AddFeature(EntityRemoteObject* self, FeatureRemoteObject* feature) {
    EntityRemote* const enr = ENTITY_REMOTE(self);
    VectorPushBack(&enr->features, feature);
}

void RemoveAllFeatures(EntityRemoteObject* self) {
    EntityRemote* const enr = ENTITY_REMOTE(self);

    for (size_t i = 0; i < VectorGetSize(&enr->features); ++i) {
        FeatureRemoteDelete((FeatureRemoteObject*)VectorGetElement(&enr->features, i));
    }

    VectorClear(&enr->features);
}

FeatureRemoteObject*
GetFeatureWithTypeAndRole(const EntityRemoteObject* self, FeatureTypeType feature_type, RoleType role) {
    EntityRemote* const enr = ENTITY_REMOTE(self);

    for (size_t i = 0; i < VectorGetSize(&enr->features); ++i) {
        FeatureRemoteObject* const fr = (FeatureRemoteObject*)VectorGetElement(&enr->features, i);

        FeatureObject* const f = FEATURE_OBJECT(fr);
        if ((FEATURE_GET_TYPE(f) == feature_type) && (FEATURE_GET_ROLE(f) == role)) {
            return fr;
        }
    }

    return NULL;
}

FeatureRemoteObject* GetFeatureWithId(const EntityRemoteObject* self, const uint32_t* feature_id) {
    const EntityRemote* const enr = ENTITY_REMOTE(self);

    if (feature_id == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < VectorGetSize(&enr->features); ++i) {
        FeatureRemoteObject* const fr = (FeatureRemoteObject*)VectorGetElement(&enr->features, i);

        const FeatureAddressType* const feature_addr = FEATURE_GET_ADDRESS(FEATURE_OBJECT(fr));

        if ((feature_addr->feature != NULL) && (*feature_addr->feature == *feature_id)) {
            return fr;
        }
    }

    return NULL;
}

const Vector* GetFeatures(const EntityRemoteObject* self) {
    return &ENTITY_REMOTE(self)->features;
}

bool HasUseCaseSupport(const EntityRemoteObject* self, const UseCaseFilterType* use_case_filter) {
    EntityRemote* const enr = ENTITY_REMOTE(self);

    NodeManagementRemoteObject* nm = DEVICE_REMOTE_GET_NODE_MANAGEMENT(enr->device);
    if (nm == NULL) {
        return false;
    }

    const NodeManagementUseCaseDataType* const use_case_data
        = FEATURE_REMOTE_GET_DATA(FEATURE_REMOTE_OBJECT(nm), kFunctionTypeNodeManagementUseCaseData);
    if (use_case_data == NULL) {
        return false;
    }

    const EntityAddressType* entity_addr = ENTITY_GET_ADDRESS(ENTITY_OBJECT(self));

    const FeatureAddressType addr = {
        .device      = entity_addr->device,
        .entity      = entity_addr->entity,
        .entity_size = entity_addr->entity_size,
    };

    return NodeManagementUseCaseDataHasUseCaseSupport(
        use_case_data,
        &addr,
        use_case_filter->actor,
        use_case_filter->use_case_name_id
    );
}
