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
 * @brief Entity Remote interface declarations
 */

#ifndef SRC_SPINE_API_ENTITY_REMOTE_INTERFACE_H_
#define SRC_SPINE_API_ENTITY_REMOTE_INTERFACE_H_

#include "src/common/vector.h"
#include "src/spine/api/entity_interface.h"
#include "src/spine/api/feature_remote_interface.h"
#include "src/spine/model/node_management_types.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct DeviceRemoteObject DeviceRemoteObject;

/**
 * @brief Entity Remote Interface
 * (Entity Remote "virtual functions table" declaration)
 */
typedef struct EntityRemoteInterface EntityRemoteInterface;

/**
 * @brief Entity Remote Object type definition
 * ("abstract class", has no members but only pointer to
 * "virtual functions table")
 */
typedef struct EntityRemoteObject EntityRemoteObject;

/**
 * @brief EntityRemote Interface Structure
 */
struct EntityRemoteInterface {
    /** Extends EntityInterface */
    EntityInterface entity_interface;

    DeviceRemoteObject* (*get_device)(const EntityRemoteObject* self);
    void (*update_device_address)(EntityRemoteObject* self, const char* device_addr);
    void (*add_feature)(EntityRemoteObject* self, FeatureRemoteObject* feature);
    void (*remove_all_features)(EntityRemoteObject* self);
    FeatureRemoteObject* (*get_feature_with_type_and_role)(
        const EntityRemoteObject* self,
        FeatureTypeType feature_type,
        RoleType role
    );
    FeatureRemoteObject* (*get_feature_with_id)(const EntityRemoteObject* self, const uint32_t* feature_id);
    const Vector* (*get_features)(const EntityRemoteObject* self);
    bool (*has_use_case_support)(const EntityRemoteObject* self, const UseCaseFilterType* use_case_filter);
};

/**
 * @brief Entity Remote Object Structure
 */
struct EntityRemoteObject {
    const EntityRemoteInterface* interface_;
};

/**
 * @brief Entity Remote pointer typecast
 */
#define ENTITY_REMOTE_OBJECT(obj) ((EntityRemoteObject*)(obj))

/**
 * @brief Entity Remote Interface class pointer typecast
 */
#define ENTITY_REMOTE_INTERFACE(obj) (ENTITY_REMOTE_OBJECT(obj)->interface_)

/**
 * @brief Entity Remote Get Device caller definition
 */
#define ENTITY_REMOTE_GET_DEVICE(obj) (ENTITY_REMOTE_INTERFACE(obj)->get_device(obj))

/**
 * @brief Entity Remote Update Device Address caller definition
 */
#define ENTITY_REMOTE_UPDATE_DEVICE_ADDRESS(obj, device_addr) \
    (ENTITY_REMOTE_INTERFACE(obj)->update_device_address(obj, device_addr))

/**
 * @brief Entity Remote Add Feature caller definition
 */
#define ENTITY_REMOTE_ADD_FEATURE(obj, feature) (ENTITY_REMOTE_INTERFACE(obj)->add_feature(obj, feature))

/**
 * @brief Entity Remote Remove All Features caller definition
 */
#define ENTITY_REMOTE_REMOVE_ALL_FEATURES(obj) (ENTITY_REMOTE_INTERFACE(obj)->remove_all_features(obj))

/**
 * @brief Entity Remote Get Feature With Type And Role caller definition
 */
#define ENTITY_REMOTE_GET_FEATURE_WITH_TYPE_AND_ROLE(obj, feature_type, role) \
    (ENTITY_REMOTE_INTERFACE(obj)->get_feature_with_type_and_role(obj, feature_type, role))

/**
 * @brief Entity Remote Get Feature With Id caller definition
 */
#define ENTITY_REMOTE_GET_FEATURE_WITH_ID(obj, feature_id) \
    (ENTITY_REMOTE_INTERFACE(obj)->get_feature_with_id(obj, feature_id))

/**
 * @brief Entity Remote Get Features caller definition
 */
#define ENTITY_REMOTE_GET_FEATURES(obj) (ENTITY_REMOTE_INTERFACE(obj)->get_features(obj))

/**
 * @brief Entity Remote Has Use Case Support caller definition
 */
#define ENTITY_REMOTE_HAS_USE_CASE_SUPPORT(obj, use_case_filter) \
    (ENTITY_REMOTE_INTERFACE(obj)->has_use_case_support(obj, use_case_filter))

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SPINE_API_ENTITY_REMOTE_INTERFACE_H_
