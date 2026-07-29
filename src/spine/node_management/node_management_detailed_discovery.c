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
 * @brief Node Management Detailed Discovery data handling
 */

#include "src/common/array_util.h"
#include "src/common/eebus_arguments.h"
#include "src/spine/api/device_local_interface.h"
#include "src/spine/entity/entity.h"
#include "src/spine/events/events.h"
#include "src/spine/feature/feature.h"
#include "src/spine/model/command_frame_types.h"
#include "src/spine/model/node_management_types.h"
#include "src/spine/model/specification_version.h"
#include "src/spine/node_management/node_management.h"
#include "src/spine/node_management/node_management_internal.h"

EebusError RequestDetailedDiscovery(
    NodeManagementObject* self,
    const char* remote_device_ski,
    const char* remote_device_addr,
    SenderObject* sender
) {
  static const uint32_t entity_id        = DEVICE_INFORMATION_ADDRESS_ENTITY_ID;
  static const uint32_t* const entity[1] = {&entity_id};
  static const uint32_t feature          = NODE_MANAGEMENT_FEATURE_ID;

  const FeatureAddressType remote_feature_addr = {
      .device      = remote_device_addr,
      .entity      = entity,
      .entity_size = ARRAY_SIZE(entity),
      .feature     = &feature,
  };

  static const NodeManagementDetailedDiscoveryDataType discovery_data = {0};

  const CmdType cmd = {
      .data_choice         = &discovery_data,
      .data_choice_type_id = kFunctionTypeNodeManagementDetailedDiscoveryData,
  };

  FeatureLocalObject* const fl = FEATURE_LOCAL_OBJECT(self);
  return FEATURE_LOCAL_REQUEST_REMOTE_DATA_BY_SENDER_ADDRESS(
      fl,
      &cmd,
      sender,
      remote_device_ski,
      &remote_feature_addr,
      kDefaultMaxResponseDelayMs
  );
}

EebusError AddDeviceInfo(NodeManagement* self, NodeManagementDetailedDiscoveryDataType* discovery_data) {
  const DeviceLocalObject* const dl  = FEATURE_LOCAL_GET_DEVICE(FEATURE_LOCAL_OBJECT(self));
  discovery_data->device_information = DEVICE_LOCAL_CREATE_INFORMATION(dl);
  if (discovery_data->device_information == NULL) {
    return kEebusErrorMemoryAllocate;
  }

  return kEebusErrorOk;
}

EebusError AddEntityInfo(NodeManagement* self, NodeManagementDetailedDiscoveryDataType* discovery_data) {
  const DeviceLocalObject* const dl = FEATURE_LOCAL_GET_DEVICE(FEATURE_LOCAL_OBJECT(self));
  const Vector* const entities      = DEVICE_LOCAL_GET_ENTITIES(dl);

  const size_t entity_info_size = sizeof(discovery_data->entity_information[0]) * VectorGetSize(entities);

  const NodeManagementDetailedDiscoveryEntityInformationType** const entity_information
      = (const NodeManagementDetailedDiscoveryEntityInformationType**)EEBUS_MALLOC(entity_info_size);

  discovery_data->entity_information = entity_information;

  if (discovery_data->entity_information == NULL) {
    return kEebusErrorMemoryAllocate;
  }

  discovery_data->entity_information_size = 0;
  for (size_t i = 0; i < VectorGetSize(entities); ++i) {
    const EntityLocalObject* const entity_local = VectorGetElement(entities, i);

    entity_information[i] = ENTITY_LOCAL_CREATE_INFORMATION(entity_local);
    if (entity_information[i] == NULL) {
      return kEebusErrorMemoryAllocate;
    }

    ++discovery_data->entity_information_size;
  }

  return kEebusErrorOk;
}

EebusError AddFeatureInfo(NodeManagement* self, NodeManagementDetailedDiscoveryDataType* discovery_data) {
  const DeviceLocalObject* const dl = FEATURE_LOCAL_GET_DEVICE(FEATURE_LOCAL_OBJECT(self));
  const Vector* const entities      = DEVICE_LOCAL_GET_ENTITIES(dl);

  size_t n = 0;
  for (size_t i = 0; i < VectorGetSize(entities); ++i) {
    const EntityLocalObject* const entity_local = VectorGetElement(entities, i);

    const Vector* const features = ENTITY_LOCAL_GET_FEATURES(entity_local);
    n += VectorGetSize(features);
  }

  const size_t buf_size = sizeof(discovery_data->feature_information[0]) * n;

  const NodeManagementDetailedDiscoveryFeatureInformationType** const feature_information
      = (const NodeManagementDetailedDiscoveryFeatureInformationType**)EEBUS_MALLOC(buf_size);

  discovery_data->feature_information = feature_information;

  if (discovery_data->feature_information == NULL) {
    return kEebusErrorMemoryAllocate;
  }

  discovery_data->feature_information_size = 0;
  for (size_t i = 0; i < VectorGetSize(entities); ++i) {
    const EntityLocalObject* const entity_local = VectorGetElement(entities, i);

    const Vector* const features = ENTITY_LOCAL_GET_FEATURES(entity_local);
    for (size_t j = 0; j < VectorGetSize(features); ++j) {
      const FeatureLocalObject* const feature_local = VectorGetElement(features, j);

      feature_information[discovery_data->feature_information_size] = FEATURE_LOCAL_CREATE_INFORMATION(feature_local);
      if (feature_information[discovery_data->feature_information_size] == NULL) {
        return kEebusErrorMemoryAllocate;
      }

      discovery_data->feature_information_size++;
    }
  }

  return kEebusErrorOk;
}

EebusError AddInfo(NodeManagement* self, NodeManagementDetailedDiscoveryDataType* discovery_data) {
  const EebusError device_info_ret = AddDeviceInfo(self, discovery_data);
  if (device_info_ret != kEebusErrorOk) {
    return device_info_ret;
  }

  const EebusError entity_info_ret = AddEntityInfo(self, discovery_data);
  if (entity_info_ret != kEebusErrorOk) {
    return entity_info_ret;
  }

  const EebusError feature_info_ret = AddFeatureInfo(self, discovery_data);
  if (feature_info_ret != kEebusErrorOk) {
    return feature_info_ret;
  }

  return kEebusErrorOk;
}

// Handle incoming detailed discovery read call
EebusError ProcessReadDetailedDiscoveryData(NodeManagement* self, const Message* msg) {
  NodeManagementDetailedDiscoveryDataType* const discovery_data
      = NodeManagementDetailedDiscoveryDataCreate((const SpecificationVersionType*)&specification_version, 1);
  if (discovery_data == NULL) {
    return kEebusErrorMemoryAllocate;
  }

  if (AddInfo(self, discovery_data) != kEebusErrorOk) {
    NodeManagementDetailedDiscoveryDataDelete(discovery_data);
    return kEebusErrorMemoryAllocate;
  }

  const EebusError err
      = NodeManagementSendReply(self, discovery_data, kFunctionTypeNodeManagementDetailedDiscoveryData, msg);
  NodeManagementDetailedDiscoveryDataDelete(discovery_data);
  return err;
}

EebusError ProcessReplyDetailedDiscoveryData(NodeManagement* self, const Message* msg) {
  DeviceRemoteObject* const dr = msg->device_remote;
  EventsManagerObject* const events_manager
      = DEVICE_LOCAL_GET_EVENTS_MANAGER(FEATURE_LOCAL_GET_DEVICE(FEATURE_LOCAL_OBJECT(self)));

  const NodeManagementDetailedDiscoveryDataType* const discovery_data
      = (const NodeManagementDetailedDiscoveryDataType*)msg->cmd->data_choice;

  if (discovery_data->device_information == NULL) {
    return kEebusErrorInputArgument;
  }

  const NetworkManagementDeviceDescriptionDataType* const device_description
      = discovery_data->device_information->description;
  if (device_description == NULL) {
    return kEebusErrorInputArgument;
  }

  DEVICE_REMOTE_UPDATE_DEVICE(dr, device_description);

  // Temporary work around
  // Store the feature remote address as all the features are removed from entities
  // within the DEVICE_REMOTE_ADD_ENTITY_AND_FEATURES() call
  FeatureAddressType* feature_remote_addr
      = FeatureAddressCopy(FEATURE_GET_ADDRESS(FEATURE_OBJECT(msg->feature_remote)));
  if (feature_remote_addr == NULL) {
    return kEebusErrorInputArgumentNull;
  }

  const Vector* const entities = DEVICE_REMOTE_ADD_ENTITY_AND_FEATURES(dr, true, discovery_data);
  if (entities == NULL) {
    FeatureAddressDelete(feature_remote_addr);
    return kEebusErrorMemoryAllocate;
  }

  // Publish event for remote device added
  const EventPayload payload = {
      .ski           = DEVICE_REMOTE_GET_SKI(dr),
      .event_type    = kEventTypeDeviceChange,
      .change_type   = kElementChangeAdd,
      .device        = dr,
      .feature       = DEVICE_REMOTE_GET_FEATURE_WITH_ADDRESS(dr, feature_remote_addr),
      .function_data = discovery_data,
      .function_type = kFunctionTypeNodeManagementDetailedDiscoveryData,
  };

  EVENTS_PUBLISH(events_manager, &payload);

  // Publish event for each added remote entity
  for (size_t i = 0; i < VectorGetSize(entities); ++i) {
    EntityRemoteObject* const entity = VectorGetElement(entities, i);

    const EventPayload payload = {
        .ski           = DEVICE_REMOTE_GET_SKI(dr),
        .event_type    = kEventTypeEntityChange,
        .change_type   = kElementChangeAdd,
        .device        = dr,
        .entity        = entity,
        .function_data = discovery_data,
        .function_type = kFunctionTypeNodeManagementDetailedDiscoveryData,
    };

    EVENTS_PUBLISH(events_manager, &payload);
  }

  FeatureAddressDelete(feature_remote_addr);
  VectorDestruct((Vector*)entities);
  EEBUS_FREE((void*)entities);
  return kEebusErrorOk;
}

EebusError ProcessNotifyDetailedDiscoveryData(NodeManagement* self, const Message* msg) {
  UNUSED(self);
  UNUSED(msg);
  // TODO: Implement method
  return kEebusErrorNotImplemented;
}

EebusError HandleMsgDetailedDiscoveryData(NodeManagement* self, const Message* msg) {
  const DeviceRemoteObject* const dr = msg->device_remote;
  if ((dr == NULL) || (DEVICE_REMOTE_GET_SENDER(dr) == NULL)) {
    return kEebusErrorInputArgument;
  }

  switch (msg->cmd_classifier) {
    case kCommandClassifierTypeRead: return ProcessReadDetailedDiscoveryData(self, msg);
    case kCommandClassifierTypeReply: return ProcessReplyDetailedDiscoveryData(self, msg);
    case kCommandClassifierTypeNotify: return ProcessNotifyDetailedDiscoveryData(self, msg);
    default: return kEebusErrorNotImplemented;
  }
}
