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
 * @brief Node Management Usecase data handling implementation
 */

#include "src/common/array_util.h"
#include "src/common/eebus_arguments.h"
#include "src/spine/api/device_local_interface.h"
#include "src/spine/api/message.h"
#include "src/spine/events/events.h"
#include "src/spine/model/cmd.h"
#include "src/spine/node_management/node_management.h"
#include "src/spine/node_management/node_management_internal.h"

EebusError RequestUseCaseData(
    NodeManagementObject* self, const char* remote_device_ski, const char* remote_device_addr, SenderObject* sender) {
  FeatureLocalObject* fl = FEATURE_LOCAL_OBJECT(self);

  static const NodeManagementUseCaseDataType usecase_data = {0};

  const CmdType cmd = {
      .data_choice         = (NodeManagementUseCaseDataType*)&usecase_data,
      .data_choice_type_id = kFunctionTypeNodeManagementUseCaseData,
  };

  static const uint32_t entity_id        = DEVICE_INFORMATION_ENTITY_ID;
  static const uint32_t* const entity[1] = {&entity_id};
  static const uint32_t feature          = NODE_MANAGEMENT_FEATURE_ID;

  const FeatureAddressType addr = {
      .entity      = entity,
      .entity_size = ARRAY_SIZE(entity),
      .feature     = &feature,
      .device      = remote_device_addr,
  };

  return FEATURE_LOCAL_REQUEST_REMOTE_DATA_BY_SENDER_ADDRESS(
      fl, &cmd, sender, remote_device_ski, &addr, kDefaultMaxResponseDelayMs);
}

EebusError ProcessReadUseCaseData(NodeManagement* self, const Message* msg) {
  const FunctionObject* const function = FeatureGetFunction(FEATURE(self), kFunctionTypeNodeManagementUseCaseData);
  if (function == NULL) {
    return kEebusErrorNoChange;
  }

  const CmdType* const cmd = FUNCTION_CREATE_REPLY_CMD(function);
  if (cmd == NULL) {
    return kEebusErrorMemoryAllocate;
  }

  const FeatureAddressType* const addr = FEATURE_GET_ADDRESS(FEATURE_OBJECT(self));

  const EebusError err = SEND_REPLY(MessageGetSender(msg), msg->request_header, addr, cmd);
  CmdDelete((CmdType*)cmd);
  return err;
}

EebusError ProcessReplyUseCaseData(NodeManagement* self, const Message* msg) {
  const NodeManagementUseCaseDataType* const usecase_data = (const NodeManagementUseCaseDataType*)msg->cmd->data_choice;

  FeatureRemoteObject* const fr = msg->feature_remote;

  const EebusError err
      = FEATURE_REMOTE_UPDATE_DATA(fr, kFunctionTypeNodeManagementUseCaseData, usecase_data, NULL, NULL, true);
  if (err != kEebusErrorOk) {
    return err;
  }

  DeviceRemoteObject* const dr = FEATURE_REMOTE_GET_DEVICE(fr);

  // The data was updated, so send an event, other event handlers may watch out for this as well
  const EventPayload payload = {
      .ski            = DEVICE_REMOTE_GET_SKI(dr),
      .event_type     = kEventTypeDataChange,
      .change_type    = kElementChangeUpdate,
      .feature        = fr,
      .device         = dr,
      .entity         = FEATURE_REMOTE_GET_ENTITY(fr),
      .function_data  = usecase_data,
      .function_type  = kFunctionTypeNodeManagementUseCaseData,
      .cmd_classifier = &msg->cmd_classifier,
  };

  EVENTS_PUBLISH(DEVICE_LOCAL_GET_EVENTS_MANAGER(FEATURE_LOCAL_GET_DEVICE(FEATURE_LOCAL_OBJECT(self))), &payload);
  return kEebusErrorOk;
}

EebusError HandleMsgUseCaseData(NodeManagement* self, const Message* msg) {
  switch (msg->cmd_classifier) {
    case kCommandClassifierTypeRead: return ProcessReadUseCaseData(self, msg);
    case kCommandClassifierTypeReply: return ProcessReplyUseCaseData(self, msg);
    case kCommandClassifierTypeNotify: return ProcessReplyUseCaseData(self, msg);
    default: return kEebusErrorNotImplemented;
  }
}
