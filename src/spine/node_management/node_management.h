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
 * @brief Node Management implementation declarations
 */

#ifndef SRC_SPINE_NODE_MANAGEMENT_NODE_MANAGEMENT_H_
#define SRC_SPINE_NODE_MANAGEMENT_NODE_MANAGEMENT_H_

#include "src/common/eebus_malloc.h"
#include "src/spine/feature/feature_local_internal.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct NodeManagementObject NodeManagementObject;

struct NodeManagementObject {
  /** Inherits the Feature Local class */
  FeatureLocalObject obj;
};

#define NODE_MANAGEMENT_OBJECT(obj) ((NodeManagementObject*)(obj))

NodeManagementObject* NodeManagementCreate(uint32_t id, EntityLocalObject* entity);

static inline void NodeMnagementDelete(NodeManagementObject* node_management) {
  if (node_management != NULL) {
    FEATURE_DESTRUCT(FEATURE_OBJECT(node_management));
    EEBUS_FREE(node_management);
  }
}

/**
 * @brief Request detailed discovery data from a remote device
 */
EebusError RequestDetailedDiscovery(
    NodeManagementObject* self,
    const char* remote_device_addr,
    const char* ski,
    SenderObject* sender,
    ReplyMessageCallback cb,
    void* ctx
);

EebusError RequestUseCaseData(
    NodeManagementObject* self,
    const char* remote_device_addr,
    const char* ski,
    SenderObject* sender,
    ReplyMessageCallback cb,
    void* ctx
);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SPINE_NODE_MANAGEMENT_NODE_MANAGEMENT_H_
