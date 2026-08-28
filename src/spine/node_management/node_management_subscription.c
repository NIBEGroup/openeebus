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
 * @brief Node Management Subscription data handling implementation
 */

#include "src/common/eebus_errors.h"
#include "src/spine/api/device_local_interface.h"
#include "src/spine/api/message.h"
#include "src/spine/node_management/node_management_internal.h"

SubscriptionManagerObject* GetSubscriptionManager(NodeManagement* self) {
    const FeatureLocalObject* const fl = FEATURE_LOCAL_OBJECT(self);
    const DeviceLocalObject* const dl  = FEATURE_LOCAL_GET_DEVICE(fl);
    return DEVICE_LOCAL_GET_SUBSCRIPTION_MANAGER(dl);
}

EebusError HandleMsgSubscriptionData(NodeManagement* self, const Message* msg) {
    if (msg->cmd_classifier != kCommandClassifierTypeCall) {
        return kEebusErrorNotImplemented;
    }

    const SubscriptionManagerObject* sm = GetSubscriptionManager(self);

    NodeManagementSubscriptionDataType* const subscription_data
        = SUBSCRIPTION_MANAGER_CREATE_SUBSCRIPTION_DATA(sm, msg->device_remote);
    if (subscription_data == NULL) {
        return kEebusErrorMemoryAllocate;
    }

    const EebusError err
        = NodeManagementSendReply(self, subscription_data, kFunctionTypeNodeManagementSubscriptionData, msg);
    NodeManagementSubscriptionDataDelete(subscription_data);
    return err;
}

EebusError HandleMsgSubscriptionRequestCall(NodeManagement* self, const Message* msg) {
    if (msg->cmd_classifier != kCommandClassifierTypeCall) {
        return kEebusErrorNotImplemented;
    }

    const NodeManagementSubscriptionRequestCallType* const data
        = (const NodeManagementSubscriptionRequestCallType*)msg->cmd->data_choice;

    return SUBSCRIPTION_MANAGER_ADD_SUBSCRIPTION(
        GetSubscriptionManager(self),
        msg->device_remote,
        data->subscription_request
    );
}

EebusError HandleMsgSubscriptionDeleteCall(NodeManagement* self, const Message* msg) {
    if (msg->cmd_classifier != kCommandClassifierTypeCall) {
        return kEebusErrorNotImplemented;
    }

    const NodeManagementSubscriptionDeleteCallType* const data
        = (const NodeManagementSubscriptionDeleteCallType*)msg->cmd->data_choice;
    return SUBSCRIPTION_MANAGER_REMOVE_SUBSCRIPTION(
        GetSubscriptionManager(self),
        msg->device_remote,
        data->subscription_delete
    );
}
