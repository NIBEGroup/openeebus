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
 * @brief Feature Local implementation
 */

#include "src/spine/node_management/node_management.h"

#include "src/common/eebus_malloc.h"
#include "src/spine/api/device_interface.h"
#include "src/spine/feature/feature.h"
#include "src/spine/feature/feature_local_internal.h"
#include "src/spine/node_management/node_management_internal.h"

static EebusError HandleMessage(FeatureLocalObject* self, const Message* msg);
static void
NodeManagementCleanRemoteDeviceCaches(FeatureLocalObject* self, const DeviceAddressType* remote_addr, const char* ski);

static const FeatureLocalInterface node_management_methods = {
    .feature_interface = {
        .destruct                = FeatureLocalDestruct,
        .get_address             = FeatureGetAddress,
        .get_type                = FeatureGetType,
        .get_role                = FeatureGetRole,
        .get_function_operations = FeatureGetFunctionOperations,
        .get_description         = FeatureGetDescription,
        .set_description         = FeatureSetDescription,
        .to_string               = FeatureToString,
    },

    .get_device                            = FeatureLocalGetDevice,
    .get_entity                            = FeatureLocalGetEntity,
    .get_data                              = FeatureLocalGetData,
    .set_function_operations               = FeatureLocalSetFunctionOperations,
    .add_write_approval_callback           = FeatureLocalAddWriteApprovalCallback,
    .try_approve_write                     = FeatureLocalTryApproveWrite,
    .deny_write                            = FeatureLocalDenyWrite,
    .clean_remote_device_caches            = NodeManagementCleanRemoteDeviceCaches,
    .data_copy                             = FeatureLocalDataCopy,
    .update_data                           = FeatureLocalUpdateData,
    .set_data                              = FeatureLocalSetData,
    .has_subscription_to_remote            = FeatureLocalHasSubscriptionToRemote,
    .subscribe_to_remote                   = FeatureLocalSubscribeToRemote,
    .remove_remote_subscription            = FeatureLocalRemoveRemoteSubscription,
    .remove_all_remote_subscriptions       = FeatureLocalRemoveAllRemoteSubscriptions,
    .has_binding_to_remote                 = FeatureLocalHasBindingToRemote,
    .bind_to_remote                        = FeatureLocalBindToRemote,
    .remove_remote_binding                 = FeatureLocalRemoveRemoteBinding,
    .remove_all_remote_bindings            = FeatureLocalRemoveAllRemoteBindings,
    .handle_message                        = HandleMessage,
    .create_information                    = FeatureLocalCreateInformation,
    .tick                                  = FeatureLocalTick,
    .write_to_remote                       = FeatureLocalWriteToRemote,
    .read_from_remote                      = FeatureLocalReadFromRemote,
};

static void NodeManagementConstruct(NodeManagement* self, uint32_t id, EntityLocalObject* entity);

void NodeManagementConstruct(NodeManagement* self, uint32_t id, EntityLocalObject* entity) {
  FeatureLocalConstruct(FEATURE_LOCAL(self), id, entity, kFeatureTypeTypeNodeManagement, kRoleTypeSpecial);

  //  Override "virtual functions table"
  FEATURE_LOCAL_INTERFACE(self) = &node_management_methods;

  FeatureLocalObject* const fl = FEATURE_LOCAL_OBJECT(self);
  FEATURE_LOCAL_SET_FUNCTION_OPERATIONS(fl, kFunctionTypeNodeManagementDetailedDiscoveryData, true, false);
  FEATURE_LOCAL_SET_FUNCTION_OPERATIONS(fl, kFunctionTypeNodeManagementUseCaseData, true, false);
  FEATURE_LOCAL_SET_FUNCTION_OPERATIONS(fl, kFunctionTypeNodeManagementSubscriptionData, true, false);
  FEATURE_LOCAL_SET_FUNCTION_OPERATIONS(fl, kFunctionTypeNodeManagementSubscriptionRequestCall, false, false);
  FEATURE_LOCAL_SET_FUNCTION_OPERATIONS(fl, kFunctionTypeNodeManagementSubscriptionDeleteCall, false, false);
  FEATURE_LOCAL_SET_FUNCTION_OPERATIONS(fl, kFunctionTypeNodeManagementBindingData, true, false);
  FEATURE_LOCAL_SET_FUNCTION_OPERATIONS(fl, kFunctionTypeNodeManagementBindingRequestCall, false, false);
  FEATURE_LOCAL_SET_FUNCTION_OPERATIONS(fl, kFunctionTypeNodeManagementBindingDeleteCall, false, false);

  DeviceObject* const device = DEVICE_OBJECT(FEATURE_LOCAL_GET_DEVICE(fl));

  const NetworkManagementFeatureSetType* const feature_set = DEVICE_GET_FEATURE_SET(device);
  if ((feature_set != NULL) && (*feature_set != kNetworkManagementFeatureSetTypeSimple)) {
    FEATURE_LOCAL_SET_FUNCTION_OPERATIONS(fl, kFunctionTypeNodeManagementDestinationListData, true, false);
  }
}

NodeManagementObject* NodeManagementCreate(uint32_t id, EntityLocalObject* entity) {
  NodeManagement* const node_management = (NodeManagement*)EEBUS_MALLOC(sizeof(NodeManagement));

  NodeManagementConstruct(node_management, id, entity);

  return NODE_MANAGEMENT_OBJECT(node_management);
}

EebusError
NodeManagementSendReply(const NodeManagement* self, const void* data, FunctionType data_type, const Message* msg) {
  CmdType cmd = {
      .data_choice         = (void*)data,
      .data_choice_type_id = data_type,
  };

  const FeatureAddressType* const addr = FEATURE_GET_ADDRESS(FEATURE_OBJECT(self));

  return SEND_REPLY(MessageGetSender(msg), msg->request_header, addr, &cmd);
}

EebusError HandleMessage(FeatureLocalObject* self, const Message* msg) {
  if ((msg == NULL) || (msg->cmd == NULL) || (msg->cmd->data_choice == NULL)) {
    return kEebusErrorInputArgumentNull;
  }

  NodeManagement* const nm = NODE_MANAGEMENT(self);

  switch (msg->cmd->data_choice_type_id) {
    case kFunctionTypeResultData: return FeatureLocalProcessResult(FEATURE_LOCAL(self), msg);
    case kFunctionTypeNodeManagementDetailedDiscoveryData: return HandleMsgDetailedDiscoveryData(nm, msg);
    case kFunctionTypeNodeManagementSubscriptionRequestCall: return HandleMsgSubscriptionRequestCall(nm, msg);
    case kFunctionTypeNodeManagementSubscriptionDeleteCall: return HandleMsgSubscriptionDeleteCall(nm, msg);
    case kFunctionTypeNodeManagementSubscriptionData: return HandleMsgSubscriptionData(nm, msg);
    case kFunctionTypeNodeManagementBindingRequestCall: return HandleMsgBindingRequestCall(nm, msg);
    case kFunctionTypeNodeManagementBindingDeleteCall: return HandleMsgBindingDeleteCall(nm, msg);
    case kFunctionTypeNodeManagementBindingData: return HandleMsgBindingData(nm, msg);
    case kFunctionTypeNodeManagementUseCaseData: return HandleMsgUseCaseData(nm, msg);
    case kFunctionTypeNodeManagementDestinationListData: return HandleMsgDestinationListData(nm, msg);
    default: return kEebusErrorNotImplemented;
  }
}

void NodeManagementCleanRemoteDeviceCaches(
    FeatureLocalObject* self,
    const DeviceAddressType* remote_addr,
    const char* ski
) {
  FeatureLocalCleanRemoteDeviceCaches(self, remote_addr, ski);
  PENDING_REPLY_CONTAINER_REMOVE_FOR_SKI(FEATURE_LOCAL(self)->pending_replies, ski);
}
