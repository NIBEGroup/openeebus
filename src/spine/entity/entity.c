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
 * @brief Entity implementation
 */

#include "entity.h"

#include <string.h>

#include "src/common/eebus_malloc.h"
#include "src/common/string_util.h"
#include "src/spine/api/entity_interface.h"

void EntityConstruct(
    Entity* self,
    EntityTypeType type,
    const char* device_addr,
    const uint32_t* entity_id,
    size_t entity_id_size
) {
    self->type            = type;
    self->address         = EntityAddressCreate(device_addr, entity_id, entity_id_size);
    self->description     = NULL;
    self->next_feature_id = (entity_id[0] == 0) ? 0 : 1;
}

void EntityDestruct(EntityObject* self) {
    Entity* const entity = ENTITY(self);

    EntityAddressDelete(entity->address);
    entity->address = NULL;

    StringDelete(entity->description);
    entity->description = NULL;
}

const EntityAddressType* EntityGetAddress(const EntityObject* self) {
    return ENTITY(self)->address;
}

EntityTypeType EntityGetType(const EntityObject* self) {
    return ENTITY(self)->type;
}

const char* EntityGetDescription(const EntityObject* self) {
    return ENTITY(self)->description;
}

void EntitySetDescription(EntityObject* self, const char* description) {
    Entity* const entity = ENTITY(self);

    StringDelete(entity->description);
    entity->description = StringCopy(description);
}

uint32_t EntityGetNextFeatureId(EntityObject* self) {
    return ENTITY(self)->next_feature_id++;
}
