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
 * @brief Node Management private declarations
 */

#ifndef SRC_EEBUS_SRC_SPINE_NODE_MANAGEMENT_NODE_MANAGEMENT_INTERNAL_H_
#define SRC_EEBUS_SRC_SPINE_NODE_MANAGEMENT_NODE_MANAGEMENT_INTERNAL_H_

#include "src/common/eebus_errors.h"
#include "src/spine/feature/feature_local_internal.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct NodeManagement NodeManagement;

struct NodeManagement {
    /** Inherits the Feature Local class */
    FeatureLocal obj;
};

#define NODE_MANAGEMENT(obj) ((NodeManagement*)(obj))

EebusError
NodeManagementSendReply(const NodeManagement* self, const void* data, FunctionType data_type, const Message* msg);

/**
 * @brief Handle detailed discovery data message received from a remote device
 */
EebusError HandleMsgDetailedDiscoveryData(NodeManagement* self, const Message* msg);

EebusError HandleMsgSubscriptionData(NodeManagement* self, const Message* msg);
EebusError HandleMsgSubscriptionRequestCall(NodeManagement* self, const Message* msg);
EebusError HandleMsgSubscriptionDeleteCall(NodeManagement* self, const Message* msg);

EebusError HandleMsgBindingData(NodeManagement* self, const Message* msg);
EebusError HandleMsgBindingRequestCall(NodeManagement* self, const Message* msg);
EebusError HandleMsgBindingDeleteCall(NodeManagement* self, const Message* msg);

EebusError HandleMsgUseCaseData(NodeManagement* self, const Message* msg);

EebusError RequestDestinationListData(NodeManagement* self, const char* remote_device_addr, SenderObject* sender);
EebusError HandleMsgDestinationListData(NodeManagement* self, const Message* msg);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_EEBUS_SRC_SPINE_NODE_MANAGEMENT_NODE_MANAGEMENT_INTERNAL_H_
