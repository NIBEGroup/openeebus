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
 * @brief Device Remote implementation
 */

#include <string.h>

#include "device.h"
#include "src/common/array_util.h"
#include "src/common/eebus_malloc.h"
#include "src/common/num_ptr.h"
#include "src/common/string_util.h"
#include "src/spine/api/device_local_interface.h"
#include "src/spine/api/device_remote_interface.h"
#include "src/spine/api/sender_interface.h"
#include "src/spine/device/data_reader.h"
#include "src/spine/device/sender.h"
#include "src/spine/entity/entity_remote.h"
#include "src/spine/feature/feature_remote.h"
#include "src/spine/node_management/node_management_remote.h"

typedef struct DeviceRemote DeviceRemote;

struct DeviceRemote {
    /** Inherit Device */
    Device obj;

    const char* ski;
    Vector entities;
    NodeManagementRemoteObject* node_management;
    SenderObject* sender;
    DeviceLocalObject* local_device;
    DataReaderObject* data_reader;
};

#define DEVICE_REMOTE(obj) ((DeviceRemote*)(obj))

static void Destruct(DeviceObject* self);
static DeviceLocalObject* GetLocalDevice(const DeviceRemoteObject* self);
static const char* GetSki(const DeviceRemoteObject* self);
static DataReaderObject* GetDataReader(const DeviceRemoteObject* self);
static void AddEntity(DeviceRemoteObject* self, EntityRemoteObject* entity);
static EntityRemoteObject*
ReleaseEntity(DeviceRemoteObject* self, const uint32_t* const* entity_ids, size_t entity_ids_size);
static EntityRemoteObject*
GetEntity(const DeviceRemoteObject* self, const uint32_t* const* entity_ids, size_t entity_ids_size);
static const Vector* GetEntities(const DeviceRemoteObject* self);
static FeatureRemoteObject*
GetFeatureWithAddress(const DeviceRemoteObject* self, const FeatureAddressType* feature_addr);
static FeatureRemoteObject* GetFeatureWithTypeAndRole(
    const DeviceRemoteObject* self,
    const uint32_t* const* entity_ids,
    size_t entity_ids_size,
    FeatureTypeType feature_type,
    RoleType role
);
static EebusError HandleSpineMesssage(DeviceRemoteObject* self, MessageBuffer* msg);
NodeManagementRemoteObject* GetNodeManagement(const DeviceRemoteObject* self);
static SenderObject* GetSender(const DeviceRemoteObject* self);
static NodeManagementUseCaseDataType* UseCasesDataCopy(const DeviceRemoteObject* self);
static void UpdateDevice(DeviceRemoteObject* self, const NetworkManagementDeviceDescriptionDataType* description);
static const Vector*
AddEntityAndFeatures(DeviceRemoteObject* self, bool init, const NodeManagementDetailedDiscoveryDataType* data);
static EebusError CheckEntityInformation(
    const DeviceRemoteObject* self,
    bool init,
    const NodeManagementDetailedDiscoveryEntityInformationType* entity_info
);

static const DeviceRemoteInterface device_remote_methods = {
    .device_interface = {
        .destruct                = Destruct,
        .get_address             = DeviceGetAddress,
        .get_device_type         = DeviceGetDeviceType,
        .get_feature_set         = DeviceGetFeatureSet,
        .create_destination_data = DeviceCreateDestinationData,
    },

    .get_local_device               = GetLocalDevice,
    .get_ski                        = GetSki,
    .get_data_reader                = GetDataReader,
    .add_entity                     = AddEntity,
    .release_entity                 = ReleaseEntity,
    .get_entity                     = GetEntity,
    .get_entities                   = GetEntities,
    .get_feature_with_address       = GetFeatureWithAddress,
    .get_feature_with_type_and_role = GetFeatureWithTypeAndRole,
    .handle_spine_messsage          = HandleSpineMesssage,
    .get_node_management            = GetNodeManagement,
    .get_sender                     = GetSender,
    .use_cases_data_copy            = UseCasesDataCopy,
    .update_device                  = UpdateDevice,
    .add_entity_and_features        = AddEntityAndFeatures,
    .check_entity_information       = CheckEntityInformation,
};

static void
DeviceRemoteConstruct(DeviceRemote* self, DeviceLocalObject* local_device, const char* ski, SenderObject* sender);
static EntityAddressType DeviceInfoEntityAddress(const DeviceRemoteObject* self);
static EntityRemoteObject*
AddEntityWithAddressAndType(DeviceRemote* self, const EntityAddressType* addr, EntityTypeType entity_type);
static const char* DeviceInfoGetDeviceAddress(const NodeManagementDetailedDiscoveryDeviceInformationType* device_info);
static bool EntityIdsMatch(const EntityAddressType* entity_addr, const FeatureAddressType* feature_addr);
static void EntityRemoteAddFeaturesWithInfo(
    DeviceRemote* self,
    EntityRemoteObject* entity,
    const NodeManagementDetailedDiscoveryFeatureInformationType* const* feature_info,
    size_t feature_info_size
);

EntityAddressType DeviceInfoEntityAddress(const DeviceRemoteObject* self) {
    static const uint32_t device_information_entity_id      = DEVICE_INFORMATION_ENTITY_ID;
    static const uint32_t* device_information_entity_ids[1] = {&device_information_entity_id};

    return (EntityAddressType){
        .device      = DEVICE_GET_ADDRESS(DEVICE_OBJECT(self)),
        .entity      = device_information_entity_ids,
        .entity_size = ARRAY_SIZE(device_information_entity_ids),
    };
}

EntityRemoteObject*
AddEntityWithAddressAndType(DeviceRemote* self, const EntityAddressType* addr, EntityTypeType entity_type) {
    uint32_t* entity_ids = (uint32_t*)EEBUS_MALLOC(addr->entity_size * sizeof(uint32_t));
    if (entity_ids == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < addr->entity_size; ++i) {
        if (addr->entity[i] == NULL) {
            EEBUS_FREE(entity_ids);
            return NULL;
        }

        entity_ids[i] = *addr->entity[i];
    }

    EntityRemoteObject* const new_entity
        = EntityRemoteCreate(DEVICE_REMOTE_OBJECT(self), entity_type, entity_ids, addr->entity_size);
    DEVICE_REMOTE_ADD_ENTITY(DEVICE_REMOTE_OBJECT(self), new_entity);

    EEBUS_FREE(entity_ids);
    return new_entity;
}

void DeviceRemoteConstruct(DeviceRemote* self, DeviceLocalObject* local_device, const char* ski, SenderObject* sender) {
    DeviceConstruct(DEVICE(self), NULL, NULL, NULL);
    // Override "virtual functions table"
    DEVICE_REMOTE_INTERFACE(self) = &device_remote_methods;

    VectorConstruct(&self->entities);

    self->ski          = StringCopy(ski);
    self->sender       = sender;
    self->local_device = local_device;
    self->data_reader  = DataReaderCreate(DEVICE_REMOTE_OBJECT(self));

    // Add Node Management
    const EntityAddressType device_info_entity_addr = DeviceInfoEntityAddress(DEVICE_REMOTE_OBJECT(self));

    EntityRemoteObject* const device_info_entity
        = AddEntityWithAddressAndType(self, &device_info_entity_addr, kEntityTypeTypeDeviceInformation);

    const uint32_t nm_feature_id = ENTITY_GET_NEXT_FEATURE_ID(ENTITY_OBJECT(device_info_entity));

    self->node_management = NodeManagementRemoteCreate(nm_feature_id, device_info_entity);
    ENTITY_REMOTE_ADD_FEATURE(device_info_entity, FEATURE_REMOTE_OBJECT(self->node_management));
}

DeviceRemoteObject* DeviceRemoteCreate(DeviceLocalObject* local_device, const char* ski, SenderObject* sender) {
    DeviceRemote* const device_remote = (DeviceRemote*)EEBUS_MALLOC(sizeof(DeviceRemote));
    if (device_remote == NULL) {
        return NULL;
    }

    DeviceRemoteConstruct(device_remote, local_device, ski, sender);

    return DEVICE_REMOTE_OBJECT(device_remote);
}

DeviceLocalObject* GetLocalDevice(const DeviceRemoteObject* self) {
    if (self == NULL) {
        return NULL;
    }
    return DEVICE_REMOTE(self)->local_device;
}

void Destruct(DeviceObject* self) {
    DeviceRemote* const dr = DEVICE_REMOTE(self);

    for (size_t i = 0; i < VectorGetSize(&dr->entities); ++i) {
        EntityRemoteObject* const entity = (EntityRemoteObject*)VectorGetElement(&dr->entities, i);
        EntityRemoteDelete(entity);
    }

    VectorDestruct(&dr->entities);

    StringDelete((char*)dr->ski);
    dr->ski = NULL;

    SenderDelete(dr->sender);
    dr->sender = NULL;

    DataReaderDelete(dr->data_reader);
    dr->data_reader = NULL;

    DeviceDestruct(DEVICE_OBJECT(self));
}

const char* GetSki(const DeviceRemoteObject* self) {
    return DEVICE_REMOTE(self)->ski;
}

DataReaderObject* GetDataReader(const DeviceRemoteObject* self) {
    return DEVICE_REMOTE(self)->data_reader;
}

void AddEntity(DeviceRemoteObject* self, EntityRemoteObject* entity) {
    DeviceRemote* const dr = DEVICE_REMOTE(self);
    VectorPushBack(&dr->entities, entity);
}

EntityRemoteObject* ReleaseEntity(DeviceRemoteObject* self, const uint32_t* const* entity_ids, size_t entity_ids_size) {
    DeviceRemote* const dr = DEVICE_REMOTE(self);

    EntityRemoteObject* const entity = DEVICE_REMOTE_GET_ENTITY(self, entity_ids, entity_ids_size);
    if (entity == NULL) {
        return NULL;
    }

    VectorRemove(&dr->entities, entity);
    return entity;
}

EntityRemoteObject*
GetEntity(const DeviceRemoteObject* self, const uint32_t* const* entity_ids, size_t entity_ids_size) {
    DeviceRemote* const dr = DEVICE_REMOTE(self);

    for (size_t i = 0; i < VectorGetSize(&dr->entities); ++i) {
        EntityRemoteObject* const entity = (EntityRemoteObject*)VectorGetElement(&dr->entities, i);
        if (EntityAddressMatchIds(ENTITY_GET_ADDRESS(ENTITY_OBJECT(entity)), entity_ids, entity_ids_size)) {
            return entity;
        }
    }

    return NULL;
}

const Vector* GetEntities(const DeviceRemoteObject* self) {
    return &DEVICE_REMOTE(self)->entities;
}

FeatureRemoteObject* GetFeatureWithAddress(const DeviceRemoteObject* self, const FeatureAddressType* feature_addr) {
    const EntityRemoteObject* const entity
        = DEVICE_REMOTE_GET_ENTITY(self, feature_addr->entity, feature_addr->entity_size);

    if (entity == NULL) {
        return NULL;
    }

    return ENTITY_REMOTE_GET_FEATURE_WITH_ID(entity, feature_addr->feature);
}

FeatureRemoteObject* GetFeatureWithTypeAndRole(
    const DeviceRemoteObject* self,
    const uint32_t* const* entity_ids,
    size_t entity_ids_size,
    FeatureTypeType feature_type,
    RoleType role
) {
    const EntityRemoteObject* entity = DEVICE_REMOTE_GET_ENTITY(self, entity_ids, entity_ids_size);
    if (entity == NULL) {
        return NULL;
    }

    return ENTITY_REMOTE_GET_FEATURE_WITH_TYPE_AND_ROLE(entity, feature_type, role);
}

EebusError HandleSpineMesssage(DeviceRemoteObject* self, MessageBuffer* msg) {
    DeviceRemote* const dr = DEVICE_REMOTE(self);
    return DEVICE_LOCAL_HANDLE_MESSAGE(dr->local_device, msg, self);
}

SenderObject* GetSender(const DeviceRemoteObject* self) {
    return DEVICE_REMOTE(self)->sender;
}

NodeManagementRemoteObject* GetNodeManagement(const DeviceRemoteObject* self) {
    return DEVICE_REMOTE(self)->node_management;
}

NodeManagementUseCaseDataType* UseCasesDataCopy(const DeviceRemoteObject* self) {
    const EntityAddressType entity_addr = DeviceInfoEntityAddress(self);
    const FeatureRemoteObject* const nm = DEVICE_REMOTE_GET_FEATURE_WITH_TYPE_AND_ROLE(
        self,
        entity_addr.entity,
        entity_addr.entity_size,
        kFeatureTypeTypeNodeManagement,
        kRoleTypeSpecial
    );

    return (NodeManagementUseCaseDataType*)FEATURE_REMOTE_DATA_COPY(nm, kFunctionTypeNodeManagementUseCaseData);
}

void UpdateDevice(DeviceRemoteObject* self, const NetworkManagementDeviceDescriptionDataType* description) {
    Device* const device = DEVICE(self);

    if (description == NULL) {
        return;
    }

    if ((description->device_address != NULL) && (description->device_address->device != NULL)) {
        StringDelete((char*)device->address);
        device->address = StringCopy(description->device_address->device);
    }

    if (description->device_type != NULL) {
        Int32Delete((int32_t*)device->feature_set);
        device->type = Int32Create(*description->device_type);
    }

    if (description->network_feature_set != NULL) {
        Int32Delete((int32_t*)device->feature_set);
        device->feature_set = Int32Create(*description->network_feature_set);
    }
}

const char* DeviceInfoGetDeviceAddress(const NodeManagementDetailedDiscoveryDeviceInformationType* device_info) {
    if (device_info == NULL) {
        return NULL;
    }

    const NetworkManagementDeviceDescriptionDataType* description = device_info->description;
    if ((description != NULL) && (description->device_address != NULL)) {
        return description->device_address->device;
    }

    return NULL;
}

FeatureRemoteObject* FeatureRemoteCreateWithInfo(
    DeviceRemote* self,
    EntityRemoteObject* entity,
    const NodeManagementDetailedDiscoveryFeatureInformationType* feature_info
) {
    const NetworkManagementFeatureDescriptionDataType* const feature_description = feature_info->description;
    if (feature_description == NULL) {
        return NULL;
    }

    if ((feature_description->feature_address == NULL) || (feature_description->feature_address->feature == NULL)) {
        return NULL;
    }

    const uint32_t feature_id = *feature_description->feature_address->feature;

    if (feature_description->feature_type == NULL) {
        return NULL;
    }

    const FeatureTypeType feature_type = *feature_description->feature_type;

    if (feature_description->role == NULL) {
        return NULL;
    }

    const RoleType role = *feature_description->role;

    FeatureRemoteObject* fr = NULL;
    if ((feature_type == kFeatureTypeTypeNodeManagement) && (role == kRoleTypeSpecial)) {
        self->node_management = NodeManagementRemoteCreate(feature_id, entity);

        fr = FEATURE_REMOTE_OBJECT(self->node_management);
    } else {
        fr = FeatureRemoteCreate(feature_id, entity, feature_type, role);
    }

    FEATURE_SET_DESCRIPTION(FEATURE_OBJECT(fr), feature_description->description);

    if (feature_description->max_response_delay != NULL) {
        FEATURE_REMOTE_SET_MAX_RESPONSE_DELAY(
            fr,
            (uint32_t)EebusDurationToSeconds(feature_description->max_response_delay)
        );
    }

    if (feature_description->supported_function != NULL) {
        FEATURE_REMOTE_SET_FUNCTION_OPERATIONS(
            fr,
            feature_description->supported_function,
            feature_description->supported_function_size
        );
    }

    return fr;
}

bool EntityIdsMatch(const EntityAddressType* entity_addr, const FeatureAddressType* feature_addr) {
    if ((entity_addr == NULL) || (feature_addr == NULL)) {
        return false;
    }

    return EntityAddressMatchIds(entity_addr, feature_addr->entity, feature_addr->entity_size);
}

void EntityRemoteAddFeaturesWithInfo(
    DeviceRemote* self,
    EntityRemoteObject* entity,
    const NodeManagementDetailedDiscoveryFeatureInformationType* const* feature_info,
    size_t feature_info_size
) {
    for (size_t i = 0; i < feature_info_size; ++i) {
        const FeatureAddressType* const feature_addr = feature_info[i]->description->feature_address;
        const EntityAddressType* const entity_addr   = ENTITY_GET_ADDRESS(ENTITY_OBJECT(entity));

        if (EntityIdsMatch(entity_addr, feature_addr)) {
            FeatureRemoteObject* const fr = FeatureRemoteCreateWithInfo(self, entity, feature_info[i]);
            if (fr != NULL) {
                ENTITY_REMOTE_ADD_FEATURE(entity, fr);
            }
        }
    }
}

const Vector*
AddEntityAndFeatures(DeviceRemoteObject* self, bool init, const NodeManagementDetailedDiscoveryDataType* data) {
    DeviceRemote* const dr = DEVICE_REMOTE(self);
    Vector* const new_data = VectorCreate();

    for (size_t i = 0; i < data->entity_information_size; ++i) {
        const NodeManagementDetailedDiscoveryEntityInformationType* const ei = data->entity_information[i];

        const EebusError err = CheckEntityInformation(self, init, ei);
        if (err != kEebusErrorOk) {
            VectorDestruct(new_data);
            EEBUS_FREE(new_data);
            return NULL;
        }

        const EntityAddressType* const entity_addr = ei->description->entity_address;

        EntityRemoteObject* entity = DEVICE_REMOTE_GET_ENTITY(self, entity_addr->entity, entity_addr->entity_size);
        if (entity == NULL) {
            entity = AddEntityWithAddressAndType(dr, entity_addr, *ei->description->entity_type);
            VectorPushBack(new_data, entity);
        }

        // Make sure the device address is set, which is not on entity 0 on startup!
        const EntityAddressType* const actual_entity_addr = ENTITY_GET_ADDRESS(ENTITY_OBJECT(entity));
        if ((actual_entity_addr->device == NULL) || (strlen(actual_entity_addr->device) == 0)) {
            const char* const device_addr = DeviceInfoGetDeviceAddress(data->device_information);
            if (device_addr != NULL) {
                ENTITY_REMOTE_UPDATE_DEVICE_ADDRESS(entity, device_addr);
            }
        }

        ENTITY_SET_DESCRIPTION(ENTITY_OBJECT(entity), ei->description->description);
        ENTITY_REMOTE_REMOVE_ALL_FEATURES(entity);
        EntityRemoteAddFeaturesWithInfo(dr, entity, data->feature_information, data->feature_information_size);
    }

    return new_data;
}

EebusError CheckEntityInformation(
    const DeviceRemoteObject* self,
    bool init,
    const NodeManagementDetailedDiscoveryEntityInformationType* entity_info
) {
    const NetworkManagementEntityDescriptionDataType* const description = entity_info->description;
    if (description == NULL) {
        return kEebusErrorInputArgumentNull;
    }

    const EntityAddressType* const entity_addr = description->entity_address;
    if ((entity_addr == NULL) || (entity_addr->entity == NULL)) {
        return kEebusErrorInputArgument;
    }

    // Consider on initial NodeManagement Detailed Discovery, the device being empty as it is not yet known
    if (init) {
        return kEebusErrorOk;
    }

    const char* const addr = DEVICE_GET_ADDRESS(DEVICE_OBJECT(self));
    if ((entity_addr->device != NULL) && (addr != NULL) && (strcmp(entity_addr->device, addr) != 0)) {
        return kEebusErrorInputArgument;
    }

    return kEebusErrorOk;
}
