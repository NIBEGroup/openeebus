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
 * @brief Entity implementation declarations
 */

#ifndef SRC_SPINE_ENTITY_ENTITY_H_
#define SRC_SPINE_ENTITY_ENTITY_H_

#include "src/spine/api/entity_interface.h"
#include "src/spine/model/device_types.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

#define DEVICE_INFORMATION_ADDRESS_ENTITY_ID ((uint32_t)0)

typedef struct Entity Entity;

struct Entity {
    /** Implements the Entity Interface */
    EntityObject obj;

    EntityTypeType type;
    EntityAddressType* address;
    char* description;
    uint32_t next_feature_id;
};

#define ENTITY(obj) ((Entity*)(obj))

void EntityConstruct(
    Entity* self,
    EntityTypeType type,
    const char* device_addr,
    const uint32_t* entity_id,
    size_t entity_id_size
);
void EntityDestruct(EntityObject* self);
const EntityAddressType* EntityGetAddress(const EntityObject* self);
EntityTypeType EntityGetType(const EntityObject* self);
const char* EntityGetDescription(const EntityObject* self);
void EntitySetDescription(EntityObject* self, const char* description);
uint32_t EntityGetNextFeatureId(EntityObject* self);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SPINE_ENTITY_ENTITY_H_
