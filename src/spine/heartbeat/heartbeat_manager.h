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
 * @brief Heartbeat Manager implementation declarations
 */

#ifndef SRC_SPINE_HEARTBEAT_HEARTBEAT_MANAGER_H_
#define SRC_SPINE_HEARTBEAT_HEARTBEAT_MANAGER_H_

#include <stddef.h>

#include "src/spine/api/heartbeat_manager_interface.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

// period: interval in seconds between two heartbeat transmissions.
// Choose heartbeat period <= the remote node's heartbeat timeout (its
// deadline for receiving a heartbeat) so the remote always sees a fresh one.
HeartbeatManagerObject* HeartbeatManagerCreate(EntityLocalObject* local_entity, uint32_t period);

static inline void HeartbeatManagerDelete(HeartbeatManagerObject* heartbeat_manager) {
    if (heartbeat_manager != NULL) {
        HEARTBEAT_MANAGER_DESTRUCT(heartbeat_manager);
        EEBUS_FREE(heartbeat_manager);
    }
}

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SPINE_HEARTBEAT_HEARTBEAT_MANAGER_H_
