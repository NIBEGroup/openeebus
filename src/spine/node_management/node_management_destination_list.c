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
 * @brief Node Management Destination List data handling implementation
 */

#include "src/common/array_util.h"
#include "src/common/eebus_arguments.h"
#include "src/spine/api/device_local_interface.h"
#include "src/spine/api/message.h"
#include "src/spine/node_management/node_management_internal.h"

EebusError RequestDestinationListData(NodeManagement* self, const char* remote_device_addr, SenderObject* sender) {
    UNUSED(self);
    UNUSED(remote_device_addr);
    UNUSED(sender);
    return kEebusErrorNotImplemented;
}

EebusError ProcessReadDestinationListData(NodeManagement* self, const Message* msg) {
    const DeviceLocalObject* const dl = FEATURE_LOCAL_GET_DEVICE(FEATURE_LOCAL_OBJECT(self));
    const DeviceObject* const device  = DEVICE_OBJECT(dl);

    const NodeManagementDestinationDataType* const detination_data[1] = {DEVICE_CREATE_DESTINATION_DATA(device)};

    const NodeManagementDestinationListDataType dest_list_data = {
        .node_management_destination_data      = detination_data,
        .node_management_destination_data_size = ARRAY_SIZE(detination_data),
    };

    const EebusError err
        = NodeManagementSendReply(self, &dest_list_data, kFunctionTypeNodeManagementDestinationListData, msg);
    NodeManagementDestinationDataDelete((NodeManagementDestinationDataType*)detination_data[0]);
    return err;
}

EebusError ProcessReplyDestinationListData(NodeManagement* self, const Message* msg) {
    UNUSED(self);
    UNUSED(msg);
    return kEebusErrorNotImplemented;
}

EebusError HandleMsgDestinationListData(NodeManagement* self, const Message* msg) {
    switch (msg->cmd_classifier) {
        case kCommandClassifierTypeRead: return ProcessReadDestinationListData(self, msg);
        case kCommandClassifierTypeReply: return ProcessReplyDestinationListData(self, msg);
        case kCommandClassifierTypeNotify: return ProcessReplyDestinationListData(self, msg);
        default: return kEebusErrorNotImplemented;
    }
}
