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
 * @brief Device Remote interface declarations
 */

#ifndef SRC_SPINE_API_DEVICE_REMOTE_INTERFACE_H_
#define SRC_SPINE_API_DEVICE_REMOTE_INTERFACE_H_

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

#include "src/common/eebus_errors.h"
#include "src/common/message_buffer.h"
#include "src/ship/api/data_reader_interface.h"
#include "src/spine/api/device_interface.h"
#include "src/spine/api/entity_remote_interface.h"
#include "src/spine/api/feature_remote_interface.h"
#include "src/spine/api/sender_interface.h"
#include "src/spine/model/network_management_types.h"
#include "src/spine/model/node_management_types.h"
#include "src/spine/node_management/node_management_remote.h"

/**
 * @brief Device Remote Interface
 * (Device Remote "virtual functions table" declaration)
 */
typedef struct DeviceRemoteInterface DeviceRemoteInterface;
typedef struct DeviceLocalObject DeviceLocalObject;

/**
 * @brief Device Remote Object type definition
 * ("abstract class", has no members but only pointer to
 * "virtual functions table")
 */
typedef struct DeviceRemoteObject DeviceRemoteObject;

/**
 * @brief DeviceRemote Interface Structure
 */
struct DeviceRemoteInterface {
  /** Extends DeviceInterface */
  DeviceInterface device_interface;

  DeviceLocalObject* (*get_local_device)(const DeviceRemoteObject* self);
  const char* (*get_ski)(const DeviceRemoteObject* self);
  DataReaderObject* (*get_data_reader)(const DeviceRemoteObject* self);
  void (*add_entity)(DeviceRemoteObject* self, EntityRemoteObject* entity);
  EntityRemoteObject* (*release_entity)(
      DeviceRemoteObject* self,
      const uint32_t* const* entity_ids,
      size_t entity_ids_size
  );
  EntityRemoteObject* (*get_entity)(
      const DeviceRemoteObject* self,
      const uint32_t* const* entity_ids,
      size_t entity_ids_size
  );
  const Vector* (*get_entities)(const DeviceRemoteObject* self);
  FeatureRemoteObject* (*get_feature_with_address)(
      const DeviceRemoteObject* self, const FeatureAddressType* feature_addr);
  FeatureRemoteObject* (*get_feature_with_type_and_role)(
      const DeviceRemoteObject* self,
      const uint32_t* const* entity_ids,
      size_t entity_ids_size,
      FeatureTypeType feature_type,
      RoleType role
  );
  EebusError (*handle_spine_messsage)(DeviceRemoteObject* self, MessageBuffer* msg);
  NodeManagementRemoteObject* (*get_node_management)(const DeviceRemoteObject* self);
  SenderObject* (*get_sender)(const DeviceRemoteObject* self);
  NodeManagementUseCaseDataType* (*use_cases_data_copy)(const DeviceRemoteObject* self);
  void (*update_device)(DeviceRemoteObject* self, const NetworkManagementDeviceDescriptionDataType* description);
  const Vector* (*add_entity_and_features)(
      DeviceRemoteObject* self, bool init, const NodeManagementDetailedDiscoveryDataType* data);
  EebusError (*check_entity_information)(const DeviceRemoteObject* self, bool init,
      const NodeManagementDetailedDiscoveryEntityInformationType* entity_info);
};

/**
 * @brief Device Remote Object Structure
 */
struct DeviceRemoteObject {
  const DeviceRemoteInterface* interface_;
};

/**
 * @brief Device Remote pointer typecast
 */
#define DEVICE_REMOTE_OBJECT(obj) ((DeviceRemoteObject*)(obj))

/**
 * @brief Device Remote Interface class pointer typecast
 */
#define DEVICE_REMOTE_INTERFACE(obj) (DEVICE_REMOTE_OBJECT(obj)->interface_)

/**
 * @brief Device Remote Get Local Device caller definition
 */
#define DEVICE_REMOTE_GET_LOCAL_DEVICE(obj) (DEVICE_REMOTE_INTERFACE(obj)->get_local_device(obj))

/**
 * @brief Device Remote Get Ski caller definition
 */
#define DEVICE_REMOTE_GET_SKI(obj) (DEVICE_REMOTE_INTERFACE(obj)->get_ski(obj))

/**
 * @brief Device Remote Get Data Reader caller definition
 */
#define DEVICE_REMOTE_GET_DATA_READER(obj) (DEVICE_REMOTE_INTERFACE(obj)->get_data_reader(obj))

/**
 * @brief Device Remote Add Entity caller definition
 */
#define DEVICE_REMOTE_ADD_ENTITY(obj, entity) (DEVICE_REMOTE_INTERFACE(obj)->add_entity(obj, entity))

/**
 * @brief Device Remote Release Entity caller definition
 */
#define DEVICE_REMOTE_RELEASE_ENTITY(obj, entity_ids, entity_ids_size) \
  (DEVICE_REMOTE_INTERFACE(obj)->release_entity(obj, entity_ids, entity_ids_size))

/**
 * @brief Device Remote Get Entity caller definition
 */
#define DEVICE_REMOTE_GET_ENTITY(obj, entity_ids, entity_ids_size) \
  (DEVICE_REMOTE_INTERFACE(obj)->get_entity(obj, entity_ids, entity_ids_size))

/**
 * @brief Device Remote Get Entities caller definition
 */
#define DEVICE_REMOTE_GET_ENTITIES(obj) (DEVICE_REMOTE_INTERFACE(obj)->get_entities(obj))

/**
 * @brief Device Remote Get Feature With Address caller definition
 */
#define DEVICE_REMOTE_GET_FEATURE_WITH_ADDRESS(obj, feature_addr) \
  (DEVICE_REMOTE_INTERFACE(obj)->get_feature_with_address(obj, feature_addr))

/**
 * @brief Device Remote Get Feature With Type And Role caller definition
 */
#define DEVICE_REMOTE_GET_FEATURE_WITH_TYPE_AND_ROLE(obj, entity_ids, entity_ids_size, feature_type, role) \
  (DEVICE_REMOTE_INTERFACE(obj)->get_feature_with_type_and_role(obj, entity_ids, entity_ids_size, feature_type, role))

/**
 * @brief Device Remote Handle Spine Messsage caller definition
 */
#define DEVICE_REMOTE_HANDLE_SPINE_MESSSAGE(obj, msg) (DEVICE_REMOTE_INTERFACE(obj)->handle_spine_messsage(obj, msg))

/**
 * @brief Device Remote Get Node Management caller definition
 */
#define DEVICE_REMOTE_GET_NODE_MANAGEMENT(obj) (DEVICE_REMOTE_INTERFACE(obj)->get_node_management(obj))

/**
 * @brief Device Remote Get Sender caller definition
 */
#define DEVICE_REMOTE_GET_SENDER(obj) (DEVICE_REMOTE_INTERFACE(obj)->get_sender(obj))

/**
 * @brief Device Remote Use Cases Data Copy caller definition
 */
#define DEVICE_REMOTE_USE_CASES_DATA_COPY(obj) (DEVICE_REMOTE_INTERFACE(obj)->use_cases_data_copy(obj))

/**
 * @brief Device Remote Update Device caller definition
 */
#define DEVICE_REMOTE_UPDATE_DEVICE(obj, description) (DEVICE_REMOTE_INTERFACE(obj)->update_device(obj, description))

/**
 * @brief Device Remote Add Entity And Features caller definition
 */
#define DEVICE_REMOTE_ADD_ENTITY_AND_FEATURES(obj, init, data) \
  (DEVICE_REMOTE_INTERFACE(obj)->add_entity_and_features(obj, init, data))

/**
 * @brief Device Remote Check Entity Information caller definition
 */
#define DEVICE_REMOTE_CHECK_ENTITY_INFORMATION(obj, init, entity_info) \
  (DEVICE_REMOTE_INTERFACE(obj)->check_entity_information(obj, init, entity_info))

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SPINE_API_DEVICE_REMOTE_INTERFACE_H_
