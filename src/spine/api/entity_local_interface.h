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
 * @brief Entity Local interface declarations
 */

#ifndef SRC_SPINE_API_ENTITY_LOCAL_INTERFACE_H_
#define SRC_SPINE_API_ENTITY_LOCAL_INTERFACE_H_

#include <stdbool.h>
#include <stdint.h>

#include "src/common/vector.h"
#include "src/spine/api/entity_interface.h"
#include "src/spine/api/feature_local_interface.h"
#include "src/spine/api/heartbeat_manager_interface.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct DeviceLocalObject DeviceLocalObject;

/**
 * @brief Entity Local Interface
 * (Entity Local "virtual functions table" declaration)
 */
typedef struct EntityLocalInterface EntityLocalInterface;

/**
 * @brief Entity Local Object type definition
 * ("abstract class", has no members but only pointer to
 * "virtual functions table")
 */
typedef struct EntityLocalObject EntityLocalObject;

/**
 * @brief EntityLocal Interface Structure
 */
struct EntityLocalInterface {
    /** Extends EntityInterface */
    EntityInterface entity_interface;

    DeviceLocalObject* (*get_device)(const EntityLocalObject* self);
    HeartbeatManagerObject* (*get_heartbeat_manager)(const EntityLocalObject* self);
    void (*add_feature)(EntityLocalObject* self, FeatureLocalObject* feature);
    FeatureLocalObject* (*get_feature_with_type_and_role)(
        const EntityLocalObject* self,
        FeatureTypeType feature_type,
        RoleType role
    );
    FeatureLocalObject* (*add_feature_with_type_and_role)(
        EntityLocalObject* self,
        FeatureTypeType feature_type,
        RoleType role
    );
    FeatureLocalObject* (*get_feature_with_id)(const EntityLocalObject* self, const uint32_t* feature_id);
    const Vector* (*get_features)(const EntityLocalObject* self);
    void (*add_use_case_support)(
        EntityLocalObject* self,
        UseCaseActorType actor,
        UseCaseNameType use_case_name_id,
        SpecificationVersionType version,
        const char* sub_revision,
        bool available,
        const UseCaseScenarioSupportType* scenarios,
        size_t scenarios_size
    );
    bool (*has_use_case_support)(const EntityLocalObject* self, const UseCaseFilterType* use_case_filter);
    void (*set_use_case_availability)(
        EntityLocalObject* self,
        const UseCaseFilterType* use_case_filter,
        bool available
    );
    void (*remove_use_case_supports)(
        EntityLocalObject* self,
        const UseCaseFilterType* use_case_filters,
        size_t use_case_filters_size
    );
    void (*remove_all_use_case_supports)(EntityLocalObject* self);
    void (*remove_all_subscriptions)(EntityLocalObject* self);
    void (*remove_all_bindings)(EntityLocalObject* self);
    NodeManagementDetailedDiscoveryEntityInformationType* (*create_information)(const EntityLocalObject* self);
    void (*tick)(EntityLocalObject* self);
};

/**
 * @brief Entity Local Object Structure
 */
struct EntityLocalObject {
    const EntityLocalInterface* interface_;
};

/**
 * @brief Entity Local pointer typecast
 */
#define ENTITY_LOCAL_OBJECT(obj) ((EntityLocalObject*)(obj))

/**
 * @brief Entity Local Interface class pointer typecast
 */
#define ENTITY_LOCAL_INTERFACE(obj) (ENTITY_LOCAL_OBJECT(obj)->interface_)

/**
 * @brief Entity Local Get Device caller definition
 */
#define ENTITY_LOCAL_GET_DEVICE(obj) (ENTITY_LOCAL_INTERFACE(obj)->get_device(obj))

/**
 * @brief Entity Local Get Heartbeat Manager caller definition
 */
#define ENTITY_LOCAL_GET_HEARTBEAT_MANAGER(obj) (ENTITY_LOCAL_INTERFACE(obj)->get_heartbeat_manager(obj))

/**
 * @brief Entity Local Add Feature caller definition
 */
#define ENTITY_LOCAL_ADD_FEATURE(obj, feature) (ENTITY_LOCAL_INTERFACE(obj)->add_feature(obj, feature))

/**
 * @brief Entity Local Get Feature With Type And Role caller definition
 */
#define ENTITY_LOCAL_GET_FEATURE_WITH_TYPE_AND_ROLE(obj, feature_type, role) \
    (ENTITY_LOCAL_INTERFACE(obj)->get_feature_with_type_and_role(obj, feature_type, role))

/**
 * @brief Entity Local Add Feature With Type And Role caller definition
 */
#define ENTITY_LOCAL_ADD_FEATURE_WITH_TYPE_AND_ROLE(obj, feature_type, role) \
    (ENTITY_LOCAL_INTERFACE(obj)->add_feature_with_type_and_role(obj, feature_type, role))

/**
 * @brief Entity Local Get Feature With Id caller definition
 */
#define ENTITY_LOCAL_GET_FEATURE_WITH_ID(obj, feature_id) \
    (ENTITY_LOCAL_INTERFACE(obj)->get_feature_with_id(obj, feature_id))

/**
 * @brief Entity Local Get Features caller definition
 */
#define ENTITY_LOCAL_GET_FEATURES(obj) (ENTITY_LOCAL_INTERFACE(obj)->get_features(obj))

/**
 * @brief Entity Local Add Use Case Support caller definition
 */
#define ENTITY_LOCAL_ADD_USE_CASE_SUPPORT(              \
    obj,                                                \
    actor,                                              \
    use_case_name_id,                                   \
    version,                                            \
    sub_revision,                                       \
    available,                                          \
    scenarios,                                          \
    scenarios_size                                      \
)                                                       \
    (ENTITY_LOCAL_INTERFACE(obj)->add_use_case_support( \
        obj,                                            \
        actor,                                          \
        use_case_name_id,                               \
        version,                                        \
        sub_revision,                                   \
        available,                                      \
        scenarios,                                      \
        scenarios_size                                  \
    ))

/**
 * @brief Entity Local Has Use Case Support caller definition
 */
#define ENTITY_LOCAL_HAS_USE_CASE_SUPPORT(obj, use_case_filter) \
    (ENTITY_LOCAL_INTERFACE(obj)->has_use_case_support(obj, use_case_filter))

/**
 * @brief Entity Local Set Use Case Availability caller definition
 */
#define ENTITY_LOCAL_SET_USE_CASE_AVAILABILITY(obj, use_case_filter, available) \
    (ENTITY_LOCAL_INTERFACE(obj)->set_use_case_availability(obj, use_case_filter, available))

/**
 * @brief Entity Local Remove Use Case Supports caller definition
 */
#define ENTITY_LOCAL_REMOVE_USE_CASE_SUPPORTS(obj, use_case_filters, use_case_filters_size) \
    (ENTITY_LOCAL_INTERFACE(obj)->remove_use_case_supports(obj, use_case_filters, use_case_filters_size))

/**
 * @brief Entity Local Remove All Use Case Supports caller definition
 */
#define ENTITY_LOCAL_REMOVE_ALL_USE_CASE_SUPPORTS(obj) (ENTITY_LOCAL_INTERFACE(obj)->remove_all_use_case_supports(obj))

/**
 * @brief Entity Local Remove All Subscriptions caller definition
 */
#define ENTITY_LOCAL_REMOVE_ALL_SUBSCRIPTIONS(obj) (ENTITY_LOCAL_INTERFACE(obj)->remove_all_subscriptions(obj))

/**
 * @brief Entity Local Remove All Bindings caller definition
 */
#define ENTITY_LOCAL_REMOVE_ALL_BINDINGS(obj) (ENTITY_LOCAL_INTERFACE(obj)->remove_all_bindings(obj))

/**
 * @brief Entity Local Create Information caller definition
 */
#define ENTITY_LOCAL_CREATE_INFORMATION(obj) (ENTITY_LOCAL_INTERFACE(obj)->create_information(obj))

/**
 * @brief Entity Local Tick caller definition
 */
#define ENTITY_LOCAL_TICK(obj) (ENTITY_LOCAL_INTERFACE(obj)->tick(obj))

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SPINE_API_ENTITY_LOCAL_INTERFACE_H_
