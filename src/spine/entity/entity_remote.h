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
 * @brief Entity Remote implementation declarations
 */

#ifndef SRC_SPINE_ENTITY_ENTITY_REMOTE_H_
#define SRC_SPINE_ENTITY_ENTITY_REMOTE_H_

#include <stddef.h>

#include "src/common/eebus_malloc.h"
#include "src/spine/api/entity_remote_interface.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

EntityRemoteObject*
EntityRemoteCreate(DeviceRemoteObject* device, EntityTypeType type, const uint32_t* entity_id, size_t entity_id_size);

static inline void EntityRemoteDelete(EntityRemoteObject* entity_remote) {
    if (entity_remote != NULL) {
        ENTITY_DESTRUCT(ENTITY_OBJECT(entity_remote));
        EEBUS_FREE(entity_remote);
    }
}

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SPINE_ENTITY_ENTITY_REMOTE_H_
