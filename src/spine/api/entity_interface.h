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
 * @brief Entity interface declarations
 */

#ifndef SRC_SPINE_API_ENTITY_INTERFACE_H_
#define SRC_SPINE_API_ENTITY_INTERFACE_H_

#include <stdint.h>

#include "src/spine/model/entity_types.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @brief Entity Interface
 * (Entity "virtual functions table" declaration)
 */
typedef struct EntityInterface EntityInterface;

/**
 * @brief Entity Object type definition
 * ("abstract class", has no members but only pointer to
 * "virtual functions table")
 */
typedef struct EntityObject EntityObject;

/**
 * @brief Entity Interface Structure
 */
struct EntityInterface {
    void (*destruct)(EntityObject* self);
    const EntityAddressType* (*get_address)(const EntityObject* self);
    EntityTypeType (*get_type)(const EntityObject* self);
    const char* (*get_description)(const EntityObject* self);
    void (*set_description)(EntityObject* self, const char* description);
    uint32_t (*get_next_feature_id)(EntityObject* self);
};

/**
 * @brief Entity Object Structure
 */
struct EntityObject {
    const EntityInterface* interface_;
};

/**
 * @brief Entity pointer typecast
 */
#define ENTITY_OBJECT(obj) ((EntityObject*)(obj))

/**
 * @brief Entity Interface class pointer typecast
 */
#define ENTITY_INTERFACE(obj) (ENTITY_OBJECT(obj)->interface_)

/**
 * @brief Entity Destruct caller definition
 */
#define ENTITY_DESTRUCT(obj) (ENTITY_INTERFACE(obj)->destruct(obj))

/**
 * @brief Entity Get Address caller definition
 */
#define ENTITY_GET_ADDRESS(obj) (ENTITY_INTERFACE(obj)->get_address(obj))

/**
 * @brief Entity Get Type caller definition
 */
#define ENTITY_GET_TYPE(obj) (ENTITY_INTERFACE(obj)->get_type(obj))

/**
 * @brief Entity Get Description caller definition
 */
#define ENTITY_GET_DESCRIPTION(obj) (ENTITY_INTERFACE(obj)->get_description(obj))

/**
 * @brief Entity Set Description caller definition
 */
#define ENTITY_SET_DESCRIPTION(obj, description) (ENTITY_INTERFACE(obj)->set_description(obj, description))

/**
 * @brief Entity Get Next Feature Id caller definition
 */
#define ENTITY_GET_NEXT_FEATURE_ID(obj) (ENTITY_INTERFACE(obj)->get_next_feature_id(obj))

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SPINE_API_ENTITY_INTERFACE_H_
