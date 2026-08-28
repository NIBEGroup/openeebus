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
 * @brief Node Management Binding data handling implementation
 */

#include "src/common/eebus_errors.h"
#include "src/spine/api/device_local_interface.h"
#include "src/spine/node_management/node_management_internal.h"

BindingManagerObject* GetBindingManager(NodeManagement* self) {
    const FeatureLocalObject* const fl = FEATURE_LOCAL_OBJECT(self);
    const DeviceLocalObject* const dl  = FEATURE_LOCAL_GET_DEVICE(fl);
    return DEVICE_LOCAL_GET_BINDING_MANAGER(dl);
}

EebusError HandleMsgBindingData(NodeManagement* self, const Message* msg) {
    if (msg->cmd_classifier != kCommandClassifierTypeCall) {
        return kEebusErrorNotImplemented;
    }

    const BindingManagerObject* const bm = GetBindingManager(self);

    NodeManagementBindingDataType* const binding_data = BINDING_MANAGER_CREATE_BINDING_DATA(bm, msg->device_remote);
    if (binding_data == NULL) {
        return kEebusErrorMemoryAllocate;
    }

    const EebusError err = NodeManagementSendReply(self, binding_data, kFunctionTypeNodeManagementBindingData, msg);
    NodeManagementBindingDataDelete(binding_data);
    return err;
}

EebusError HandleMsgBindingRequestCall(NodeManagement* self, const Message* msg) {
    if (msg->cmd_classifier != kCommandClassifierTypeCall) {
        return kEebusErrorNotImplemented;
    }

    const NodeManagementBindingRequestCallType* const data
        = (const NodeManagementBindingRequestCallType*)msg->cmd->data_choice;

    BindingManagerObject* const bm = GetBindingManager(self);
    return BINDING_MANAGER_ADD_BINDING(bm, msg->device_remote, data->binding_request);
}

EebusError HandleMsgBindingDeleteCall(NodeManagement* self, const Message* msg) {
    if (msg->cmd_classifier != kCommandClassifierTypeCall) {
        return kEebusErrorNotImplemented;
    }

    const NodeManagementBindingDeleteCallType* const data
        = (const NodeManagementBindingDeleteCallType*)msg->cmd->data_choice;

    BindingManagerObject* const bm = GetBindingManager(self);
    return BINDING_MANAGER_REMOVE_BINDING(bm, data->binding_delete, msg->device_remote);
}
