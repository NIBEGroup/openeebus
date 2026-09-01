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
 * @brief SPINE Datagram Entity payload functions implementation
 */

#include "src/spine/model/entity_types.h"

#include <stddef.h>
#include <stdio.h>

#include "src/common/bool_ptr.h"
#include "src/common/eebus_malloc.h"
#include "src/common/num_ptr.h"
#include "src/common/string_util.h"
#include "src/spine/model/model.h"

EntityAddressType* EntityAddressCreate(const char* device, const uint32_t* entity_ids, size_t entity_ids_size) {
    EntityAddressType* const addr = (EntityAddressType*)EEBUS_MALLOC(sizeof(EntityAddressType));
    if (addr == NULL) {
        return NULL;
    }

    addr->device = StringCopy(device);

    const uint32_t** const entity_ids_tmp = (const uint32_t**)EEBUS_MALLOC(sizeof(uint32_t*) * entity_ids_size);

    addr->entity = entity_ids_tmp;
    for (size_t i = 0; i < entity_ids_size; ++i) {
        entity_ids_tmp[i] = Uint32Create(entity_ids[i]);
    }

    addr->entity_size = entity_ids_size;
    return addr;
}

void EntityAddressDelete(EntityAddressType* self) {
    if (self == NULL) {
        return;
    }

    StringDelete((char*)self->device);

    for (size_t i = 0; i < self->entity_size; ++i) {
        Uint32Delete((uint32_t*)self->entity[i]);
    }

    EEBUS_FREE((void*)self->entity);

    EEBUS_FREE(self);
}

EntityAddressType* EntityAddressCopy(const EntityAddressType* self) {
    const EebusDataCfg* const cfg = ModelGetEntityAddressCfg();
    return (EntityAddressType*)ModelDataCopy(cfg, self);
}

bool EntityAddressCompare(const EntityAddressType* addr_a, const EntityAddressType* addr_b) {
    const EebusDataCfg* const cfg = ModelGetEntityAddressCfg();
    return EEBUS_DATA_COMPARE(cfg, &addr_a, cfg, &addr_b);
}

bool EntityAddressMatchIds(const EntityAddressType* self, const uint32_t* const* entity_ids, size_t entity_ids_size) {
    if ((self == NULL) || (self->entity == NULL) || (self->entity_size == 0)) {
        return false;
    }

    if ((entity_ids == NULL) || (entity_ids_size == 0)) {
        return false;
    }

    const EntityAddressType entity_addr_a = {
        .entity      = self->entity,
        .entity_size = self->entity_size,
    };

    const EntityAddressType entity_addr_b = {
        .entity      = entity_ids,
        .entity_size = entity_ids_size,
    };

    return EntityAddressCompare(&entity_addr_a, &entity_addr_b);
}

void EntityAddressPrint(const char* fmt, const EntityAddressType* addr) {
    if (addr == NULL) {
        printf(fmt, "NULL");
        return;
    }

    const char* entity_addr_string = EEBUS_DATA_PRINT_UNFORMATTED(ModelGetEntityAddressCfg(), &addr);
    if (entity_addr_string != NULL) {
        printf(fmt, entity_addr_string);
    } else {
        printf(fmt, "<error converting to string>");
    }

    StringDelete((char*)entity_addr_string);
}
