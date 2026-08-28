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
 * @brief Entity Local implementation
 */

#include "src/common/eebus_assert.h"
#include "src/common/eebus_malloc.h"
#include "src/common/string_util.h"
#include "src/common/vector.h"
#include "src/spine/api/device_local_interface.h"
#include "src/spine/entity/entity.h"
#include "src/spine/feature/feature_local.h"
#include "src/spine/heartbeat/heartbeat_manager.h"
#include "src/spine/model/model.h"
#include "src/spine/model/node_management_types.h"

typedef struct EntityLocal EntityLocal;

struct EntityLocal {
    /** Inherits the Entity */
    Entity obj;

    DeviceLocalObject* device;
    Vector features;
    HeartbeatManagerObject* heartbeat_manager;
};

#define ENTITY_LOCAL(obj) ((EntityLocal*)(obj))

static void Destruct(EntityObject* self);
static DeviceLocalObject* GetDevice(const EntityLocalObject* self);
static HeartbeatManagerObject* GetHeartbeatManager(const EntityLocalObject* self);
static void AddFeature(EntityLocalObject* self, FeatureLocalObject* feature);
static FeatureLocalObject*
GetFeatureWithTypeAndRole(const EntityLocalObject* self, FeatureTypeType feature_type, RoleType role);
static FeatureLocalObject*
AddFeatureWithTypeAndRole(EntityLocalObject* self, FeatureTypeType feature_type, RoleType role);
static FeatureLocalObject* GetFeatureWithId(const EntityLocalObject* self, const uint32_t* feature_id);
static const Vector* GetFeatures(const EntityLocalObject* self);
static void AddUseCaseSupport(
    EntityLocalObject* self,
    UseCaseActorType actor,
    UseCaseNameType use_case_name_id,
    SpecificationVersionType version,
    const char* sub_revision,
    bool available,
    const UseCaseScenarioSupportType* scenarios,
    size_t scenarios_size
);
static bool HasUseCaseSupport(const EntityLocalObject* self, const UseCaseFilterType* use_case_filter);
static void SetUseCaseAvailability(EntityLocalObject* self, const UseCaseFilterType* use_case_filter, bool available);
static void
RemoveUseCaseSupports(EntityLocalObject* self, const UseCaseFilterType* use_case_filters, size_t use_case_filters_size);
static void RemoveAllUseCaseSupports(EntityLocalObject* self);
static void RemoveAllSubscriptions(EntityLocalObject* self);
static void RemoveAllBindings(EntityLocalObject* self);
static NodeManagementDetailedDiscoveryEntityInformationType* CreateInformation(const EntityLocalObject* self);
static void Tick(EntityLocalObject* self);

static const EntityLocalInterface entity_local_methods = {
    .entity_interface = {
        .destruct            = Destruct,
        .get_address         = EntityGetAddress,
        .get_type            = EntityGetType,
        .get_description     = EntityGetDescription,
        .set_description     = EntitySetDescription,
        .get_next_feature_id = EntityGetNextFeatureId,
    },

    .get_device                     = GetDevice,
    .get_heartbeat_manager          = GetHeartbeatManager,
    .add_feature                    = AddFeature,
    .get_feature_with_type_and_role = GetFeatureWithTypeAndRole,
    .add_feature_with_type_and_role = AddFeatureWithTypeAndRole,
    .get_feature_with_id            = GetFeatureWithId,
    .get_features                   = GetFeatures,
    .add_use_case_support           = AddUseCaseSupport,
    .has_use_case_support           = HasUseCaseSupport,
    .set_use_case_availability      = SetUseCaseAvailability,
    .remove_use_case_supports       = RemoveUseCaseSupports,
    .remove_all_use_case_supports   = RemoveAllUseCaseSupports,
    .remove_all_subscriptions       = RemoveAllSubscriptions,
    .remove_all_bindings            = RemoveAllBindings,
    .create_information             = CreateInformation,
    .tick                           = Tick,
};

static void EntityLocalConstruct(
    EntityLocal* self,
    DeviceLocalObject* device,
    EntityTypeType type,
    const uint32_t* entity_id,
    size_t entity_id_size,
    uint32_t heartbeat_period
);

void EntityLocalConstruct(
    EntityLocal* self,
    DeviceLocalObject* device,
    EntityTypeType type,
    const uint32_t* entity_id,
    size_t entity_id_size,
    uint32_t heartbeat_period
) {
    EntityConstruct(ENTITY(self), type, DEVICE_GET_ADDRESS(DEVICE_OBJECT(device)), entity_id, entity_id_size);
    // Override "virtual functions table"
    ENTITY_LOCAL_INTERFACE(self) = &entity_local_methods;

    self->device = device;
    VectorConstruct(&self->features);

    // Only needed if the entity address is not DeviceInformationEntityId
    if ((entity_id != NULL) && (entity_id[0] != DEVICE_INFORMATION_ENTITY_ID)) {
        self->heartbeat_manager = HeartbeatManagerCreate(ENTITY_LOCAL_OBJECT(self), heartbeat_period);
    } else {
        self->heartbeat_manager = NULL;
    }
}

EntityLocalObject* EntityLocalCreate(
    DeviceLocalObject* device,
    EntityTypeType type,
    const uint32_t* entity_id,
    size_t entity_id_size,
    uint32_t heartbeat_period
) {
    EntityLocal* const entity_local = (EntityLocal*)EEBUS_MALLOC(sizeof(EntityLocal));
    if (entity_local == NULL) {
        return NULL;
    }

    EntityLocalConstruct(entity_local, device, type, entity_id, entity_id_size, heartbeat_period);

    return ENTITY_LOCAL_OBJECT(entity_local);
}

void Destruct(EntityObject* self) {
    EntityLocal* const enl = ENTITY_LOCAL(self);

    HeartbeatManagerDelete(enl->heartbeat_manager);
    enl->heartbeat_manager = NULL;

    for (size_t i = 0; i < VectorGetSize(&enl->features); ++i) {
        FeatureLocalObject* const fl = (FeatureLocalObject*)VectorGetElement(&enl->features, i);
        FeatureLocalDelete(fl);
    }

    VectorDestruct(&enl->features);

    EntityDestruct(self);
}

DeviceLocalObject* GetDevice(const EntityLocalObject* self) {
    return ENTITY_LOCAL(self)->device;
}

HeartbeatManagerObject* GetHeartbeatManager(const EntityLocalObject* self) {
    return ENTITY_LOCAL(self)->heartbeat_manager;
}

void AddFeature(EntityLocalObject* self, FeatureLocalObject* feature) {
    EntityLocal* const enl = ENTITY_LOCAL(self);
    VectorPushBack(&enl->features, feature);
}

FeatureLocalObject*
GetFeatureWithTypeAndRole(const EntityLocalObject* self, FeatureTypeType feature_type, RoleType role) {
    const EntityLocal* const enl = ENTITY_LOCAL(self);

    for (size_t i = 0; i < VectorGetSize(&enl->features); ++i) {
        FeatureLocalObject* const fl = (FeatureLocalObject*)VectorGetElement(&enl->features, i);
        FeatureObject* const feature = FEATURE_OBJECT(fl);

        if ((feature_type == FEATURE_GET_TYPE(feature)) && (role == FEATURE_GET_ROLE(feature))) {
            return fl;
        }
    }

    return NULL;
}

FeatureLocalObject* AddFeatureWithTypeAndRole(EntityLocalObject* self, FeatureTypeType feature_type, RoleType role) {
    FeatureLocalObject* const f = ENTITY_LOCAL_GET_FEATURE_WITH_TYPE_AND_ROLE(self, feature_type, role);
    if (f != NULL) {
        return f;
    }

    const uint32_t feature_id = ENTITY_GET_NEXT_FEATURE_ID(ENTITY_OBJECT(self));

    FeatureLocalObject* const f_new = FeatureLocalCreate(feature_id, self, feature_type, role);

    const char* const description
        = StringFmtSprintf("%s %s", ModelFeatureTypeToString(feature_type), ModelRoleToString(role));
    FEATURE_SET_DESCRIPTION(FEATURE_OBJECT(f_new), description);
    StringDelete((char*)description);

    ENTITY_LOCAL_ADD_FEATURE(self, f_new);
    return f_new;
}

FeatureLocalObject* GetFeatureWithId(const EntityLocalObject* self, const uint32_t* feature_id) {
    const EntityLocal* const enl = ENTITY_LOCAL(self);

    if (feature_id == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < VectorGetSize(&enl->features); ++i) {
        FeatureLocalObject* const fl = (FeatureLocalObject*)VectorGetElement(&enl->features, i);

        const FeatureAddressType* const feature_addr = FEATURE_GET_ADDRESS(FEATURE_OBJECT(fl));

        if ((feature_addr->feature != NULL) && (*feature_addr->feature == *feature_id)) {
            return fl;
        }
    }

    return NULL;
}

const Vector* GetFeatures(const EntityLocalObject* self) {
    return &ENTITY_LOCAL(self)->features;
}

void AddUseCaseSupport(
    EntityLocalObject* self,
    UseCaseActorType actor,
    UseCaseNameType use_case_name_id,
    SpecificationVersionType version,
    const char* sub_revision,
    bool available,
    const UseCaseScenarioSupportType* scenarios,
    size_t scenarios_size
) {
    EntityLocal* const enl = ENTITY_LOCAL(self);

    NodeManagementObject* const nm = DEVICE_LOCAL_GET_NODE_MANAGEMENT(enl->device);

    NodeManagementUseCaseDataType* use_case_data
        = FEATURE_LOCAL_DATA_COPY(FEATURE_LOCAL_OBJECT(nm), kFunctionTypeNodeManagementUseCaseData);

    if (use_case_data == NULL) {
        use_case_data = NodeManagementUseCaseDataCreateEmpty();
    }

    const EntityAddressType* entity_addr = ENTITY_GET_ADDRESS(ENTITY_OBJECT(self));

    const FeatureAddressType addr = {
        .device      = entity_addr->device,
        .entity      = entity_addr->entity,
        .entity_size = entity_addr->entity_size,
    };

    NodeManagementUseCaseDataAddUseCaseSupport(
        use_case_data,
        &addr,
        actor,
        use_case_name_id,
        version,
        sub_revision,
        available,
        scenarios,
        scenarios_size
    );

    FEATURE_LOCAL_SET_DATA(FEATURE_LOCAL_OBJECT(nm), kFunctionTypeNodeManagementUseCaseData, use_case_data);
    ModelFunctionDataDelete(kFunctionTypeNodeManagementUseCaseData, use_case_data);
}

bool HasUseCaseSupport(const EntityLocalObject* self, const UseCaseFilterType* use_case_filter) {
    EntityLocal* const enl = ENTITY_LOCAL(self);

    NodeManagementObject* const nm = DEVICE_LOCAL_GET_NODE_MANAGEMENT(enl->device);

    const NodeManagementUseCaseDataType* const use_case_data
        = FEATURE_LOCAL_GET_DATA(FEATURE_LOCAL_OBJECT(nm), kFunctionTypeNodeManagementUseCaseData);
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

void SetUseCaseAvailability(EntityLocalObject* self, const UseCaseFilterType* use_case_filter, bool available) {
    EntityLocal* const enl = ENTITY_LOCAL(self);

    NodeManagementObject* const nm = DEVICE_LOCAL_GET_NODE_MANAGEMENT(enl->device);

    NodeManagementUseCaseDataType* const use_case_data
        = FEATURE_LOCAL_DATA_COPY(FEATURE_LOCAL_OBJECT(nm), kFunctionTypeNodeManagementUseCaseData);
    if (use_case_data == NULL) {
        return;
    }

    const EntityAddressType* entity_addr = ENTITY_GET_ADDRESS(ENTITY_OBJECT(self));

    const FeatureAddressType addr = {
        .device      = entity_addr->device,
        .entity      = entity_addr->entity,
        .entity_size = entity_addr->entity_size,
    };

    NodeManagementUseCaseDataSetAvailability(
        use_case_data,
        &addr,
        use_case_filter->actor,
        use_case_filter->use_case_name_id,
        available
    );
    FEATURE_LOCAL_SET_DATA(FEATURE_LOCAL_OBJECT(nm), kFunctionTypeNodeManagementUseCaseData, use_case_data);
    ModelFunctionDataDelete(kFunctionTypeNodeManagementUseCaseData, use_case_data);
}

void RemoveUseCaseSupports(
    EntityLocalObject* self,
    const UseCaseFilterType* use_case_filters,
    size_t use_case_filters_size
) {
    EntityLocal* const enl = ENTITY_LOCAL(self);

    if ((use_case_filters == NULL) || (use_case_filters_size == 0)) {
        return;
    }

    NodeManagementObject* const nm = DEVICE_LOCAL_GET_NODE_MANAGEMENT(enl->device);

    NodeManagementUseCaseDataType* const use_case_data
        = FEATURE_LOCAL_DATA_COPY(FEATURE_LOCAL_OBJECT(nm), kFunctionTypeNodeManagementUseCaseData);
    if (use_case_data == NULL) {
        return;
    }

    const EntityAddressType* entity_addr = ENTITY_GET_ADDRESS(ENTITY_OBJECT(self));

    const FeatureAddressType addr = {
        .device      = entity_addr->device,
        .entity      = entity_addr->entity,
        .entity_size = entity_addr->entity_size,
    };

    NodeManagementUseCaseDataRemoveWithFilters(use_case_data, &addr, use_case_filters, use_case_filters_size);
    FEATURE_LOCAL_SET_DATA(FEATURE_LOCAL_OBJECT(nm), kFunctionTypeNodeManagementUseCaseData, use_case_data);
    ModelFunctionDataDelete(kFunctionTypeNodeManagementUseCaseData, use_case_data);
}

void RemoveAllUseCaseSupports(EntityLocalObject* self) {
    EntityLocal* const enl = ENTITY_LOCAL(self);

    NodeManagementObject* const nm = DEVICE_LOCAL_GET_NODE_MANAGEMENT(enl->device);

    NodeManagementUseCaseDataType* const use_case_data
        = FEATURE_LOCAL_DATA_COPY(FEATURE_LOCAL_OBJECT(nm), kFunctionTypeNodeManagementUseCaseData);
    if (use_case_data == NULL) {
        return;
    }

    const EntityAddressType* entity_addr = ENTITY_GET_ADDRESS(ENTITY_OBJECT(self));

    const FeatureAddressType addr = {
        .device      = entity_addr->device,
        .entity      = entity_addr->entity,
        .entity_size = entity_addr->entity_size,
    };

    NodeManagementUseCaseDataRemoveWithAddress(use_case_data, &addr);
    FEATURE_LOCAL_SET_DATA(FEATURE_LOCAL_OBJECT(nm), kFunctionTypeNodeManagementUseCaseData, use_case_data);
    ModelFunctionDataDelete(kFunctionTypeNodeManagementUseCaseData, use_case_data);
}

void RemoveAllSubscriptions(EntityLocalObject* self) {
    const EntityLocal* const enl = ENTITY_LOCAL(self);

    for (size_t i = 0; i < VectorGetSize(&enl->features); ++i) {
        FeatureLocalObject* const fl = (FeatureLocalObject*)VectorGetElement(&enl->features, i);
        FEATURE_LOCAL_REMOVE_ALL_REMOTE_SUBSCRIPTIONS(fl);
    }
}

void RemoveAllBindings(EntityLocalObject* self) {
    const EntityLocal* const enl = ENTITY_LOCAL(self);

    for (size_t i = 0; i < VectorGetSize(&enl->features); ++i) {
        FeatureLocalObject* const fl = (FeatureLocalObject*)VectorGetElement(&enl->features, i);
        FEATURE_LOCAL_REMOVE_ALL_REMOTE_BINDINGS(fl);
    }
}

NodeManagementDetailedDiscoveryEntityInformationType* CreateInformation(const EntityLocalObject* self) {
    const EntityObject* const entity = ENTITY_OBJECT(self);

    const EntityAddressType* entity_addr = ENTITY_GET_ADDRESS(entity);
    const EntityTypeType entity_type     = ENTITY_GET_TYPE(entity);
    return NodeManagementDetailedDiscoveryEntityInformationCreate(entity_addr, entity_type);
}

void Tick(EntityLocalObject* self) {
    const Vector* features = ENTITY_LOCAL_GET_FEATURES(self);

    for (size_t i = 0; i < VectorGetSize(features); ++i) {
        FeatureLocalObject* const fl = (FeatureLocalObject*)VectorGetElement(features, i);
        FEATURE_LOCAL_TICK(fl);
    }
}
