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
 * @brief Node Management Remote implementation declarations
 */

#ifndef SRC_SPINE_NODE_MANAGEMENT_NODE_MANAGEMENT_REMOTE_H_
#define SRC_SPINE_NODE_MANAGEMENT_NODE_MANAGEMENT_REMOTE_H_

#include "src/common/eebus_malloc.h"
#include "src/spine/api/feature_remote_interface.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct NodeManagementRemoteObject NodeManagementRemoteObject;

struct NodeManagementRemoteObject {
    /** Inherits the Feature Remote class */
    FeatureRemoteObject obj;
};

#define NODE_MANAGEMENT_REMOTE_OBJECT(obj) ((NodeManagementRemoteObject*)(obj))

NodeManagementRemoteObject* NodeManagementRemoteCreate(uint32_t id, EntityRemoteObject* entity);

static inline void NodeManagementRemoteDelete(NodeManagementRemoteObject* node_management_remote) {
    if (node_management_remote != NULL) {
        FEATURE_DESTRUCT(FEATURE_OBJECT(node_management_remote));
        EEBUS_FREE(node_management_remote);
    }
}

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SPINE_NODE_MANAGEMENT_NODE_MANAGEMENT_REMOTE_H_
