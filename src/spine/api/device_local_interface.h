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
 * @brief Device Local interface declarations
 */

#ifndef SRC_SPINE_API_DEVICE_LOCAL_INTERFACE_H_
#define SRC_SPINE_API_DEVICE_LOCAL_INTERFACE_H_

#include "src/common/eebus_errors.h"
#include "src/common/vector.h"
#include "src/ship/api/data_reader_interface.h"
#include "src/ship/api/data_writer_interface.h"
#include "src/spine/api/binding_manager_interface.h"
#include "src/spine/api/device_interface.h"
#include "src/spine/api/device_remote_interface.h"
#include "src/spine/api/entity_local_interface.h"
#include "src/spine/api/events_manager_interface.h"
#include "src/spine/api/feature_local_interface.h"
#include "src/spine/api/heartbeat_manager_interface.h"
#include "src/spine/api/subscription_manager_interface.h"
#include "src/spine/node_management/node_management.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @brief Device Local Interface
 * (Device Local "virtual functions table" declaration)
 */
typedef struct DeviceLocalInterface DeviceLocalInterface;

/**
 * @brief Device Local Object type definition
 * ("abstract class", has no members but only pointer to
 * "virtual functions table")
 */
typedef struct DeviceLocalObject DeviceLocalObject;

/**
 * @brief DeviceLocal Interface Structure
 */
struct DeviceLocalInterface {
    /** Extends DeviceInterface */
    DeviceInterface device_interface;

    EebusError (*start)(DeviceLocalObject* self);
    void (*stop)(DeviceLocalObject* self);
    DataReaderObject* (*setup_remote_device)(DeviceLocalObject* self, const char* ski, DataWriterObject* writer);
    void (*add_remote_device_for_ski)(DeviceLocalObject* self, const char* ski, DeviceRemoteObject* remote_device);
    EebusError (*request_remote_detailed_discovery_data)(
        DeviceLocalObject* self,
        const DeviceRemoteObject* remote_device
    );
    void (*remove_remote_device_connection)(DeviceLocalObject* self, const char* ski);
    void (*remove_remote_device)(DeviceLocalObject* self, const char* ski);
    DeviceRemoteObject* (*get_remote_device_with_address)(const DeviceLocalObject* self, const char* device_addr);
    DeviceRemoteObject* (*get_remote_device_with_ski)(const DeviceLocalObject* self, const char* ski);
    void (*add_entity)(DeviceLocalObject* self, EntityLocalObject* entity);
    void (*remove_entity)(DeviceLocalObject* self, EntityLocalObject* entity);
    EntityLocalObject* (*get_entity)(
        const DeviceLocalObject* self,
        const uint32_t* const* entity_ids,
        size_t entity_ids_size
    );
    EntityLocalObject* (*get_entity_with_type)(const DeviceLocalObject* self, EntityTypeType entity_type);
    const Vector* (*get_entities)(const DeviceLocalObject* self);
    FeatureLocalObject* (*get_feature_with_address)(
        const DeviceLocalObject* self,
        const FeatureAddressType* feature_addr
    );
    EebusError (*handle_message)(DeviceLocalObject* self, MessageBuffer* msg, DeviceRemoteObject* remote_device);
    NodeManagementObject* (*get_node_management)(const DeviceLocalObject* self);
    BindingManagerObject* (*get_binding_manager)(const DeviceLocalObject* self);
    SubscriptionManagerObject* (*get_subscription_manager)(const DeviceLocalObject* self);
    EventsManagerObject* (*get_events_manager)(const DeviceLocalObject* self);
    void (*notify_subscribers)(
        const DeviceLocalObject* self,
        const FeatureAddressType* feature_addr,
        const CmdType* cmd
    );
    NodeManagementDetailedDiscoveryDeviceInformationType* (*create_information)(const DeviceLocalObject* self);
    void (*lock)(DeviceLocalObject* self);
    void (*unlock)(DeviceLocalObject* self);
};

/**
 * @brief Device Local Object Structure
 */
struct DeviceLocalObject {
    const DeviceLocalInterface* interface_;
};

/**
 * @brief Device Local pointer typecast
 */
#define DEVICE_LOCAL_OBJECT(obj) ((DeviceLocalObject*)(obj))

/**
 * @brief Device Local Interface class pointer typecast
 */
#define DEVICE_LOCAL_INTERFACE(obj) (DEVICE_LOCAL_OBJECT(obj)->interface_)

/**
 * @brief Device Local Start caller definition
 */
#define DEVICE_LOCAL_START(obj) (DEVICE_LOCAL_INTERFACE(obj)->start(obj))

/**
 * @brief Device Local Stop caller definition
 */
#define DEVICE_LOCAL_STOP(obj) (DEVICE_LOCAL_INTERFACE(obj)->stop(obj))

/**
 * @brief Device Local Setup Remote Device caller definition
 */
#define DEVICE_LOCAL_SETUP_REMOTE_DEVICE(obj, ski, writer) \
    (DEVICE_LOCAL_INTERFACE(obj)->setup_remote_device(obj, ski, writer))

/**
 * @brief Device Local Add Remote Device For Ski caller definition
 */
#define DEVICE_LOCAL_ADD_REMOTE_DEVICE_FOR_SKI(obj, ski, remote_device) \
    (DEVICE_LOCAL_INTERFACE(obj)->add_remote_device_for_ski(obj, ski, remote_device))

/**
 * @brief Device Local Request Remote Detailed Discovery Data caller definition
 */
#define DEVICE_LOCAL_REQUEST_REMOTE_DETAILED_DISCOVERY_DATA(obj, remote_device) \
    (DEVICE_LOCAL_INTERFACE(obj)->request_remote_detailed_discovery_data(obj, remote_device))

/**
 * @brief Device Local Remove Remote Device Connection caller definition
 */
#define DEVICE_LOCAL_REMOVE_REMOTE_DEVICE_CONNECTION(obj, ski) \
    (DEVICE_LOCAL_INTERFACE(obj)->remove_remote_device_connection(obj, ski))

/**
 * @brief Device Local Remove Remote Device caller definition
 */
#define DEVICE_LOCAL_REMOVE_REMOTE_DEVICE(obj, ski) (DEVICE_LOCAL_INTERFACE(obj)->remove_remote_device(obj, ski))

/**
 * @brief Device Local Get Remote Device With Address caller definition
 */
#define DEVICE_LOCAL_GET_REMOTE_DEVICE_WITH_ADDRESS(obj, device_addr) \
    (DEVICE_LOCAL_INTERFACE(obj)->get_remote_device_with_address(obj, device_addr))

/**
 * @brief Device Local Get Remote Device With Ski caller definition
 */
#define DEVICE_LOCAL_GET_REMOTE_DEVICE_WITH_SKI(obj, ski) \
    (DEVICE_LOCAL_INTERFACE(obj)->get_remote_device_with_ski(obj, ski))

/**
 * @brief Device Local Add Entity caller definition
 */
#define DEVICE_LOCAL_ADD_ENTITY(obj, entity) (DEVICE_LOCAL_INTERFACE(obj)->add_entity(obj, entity))

/**
 * @brief Device Local Remove Entity caller definition
 */
#define DEVICE_LOCAL_REMOVE_ENTITY(obj, entity) (DEVICE_LOCAL_INTERFACE(obj)->remove_entity(obj, entity))

/**
 * @brief Device Local Get Entity caller definition
 */
#define DEVICE_LOCAL_GET_ENTITY(obj, entity_ids, entity_ids_size) \
    (DEVICE_LOCAL_INTERFACE(obj)->get_entity(obj, entity_ids, entity_ids_size))

/**
 * @brief Device Local Get Entity With Type caller definition
 */
#define DEVICE_LOCAL_GET_ENTITY_WITH_TYPE(obj, entity_type) \
    (DEVICE_LOCAL_INTERFACE(obj)->get_entity_with_type(obj, entity_type))

/**
 * @brief Device Local Get Entities caller definition
 */
#define DEVICE_LOCAL_GET_ENTITIES(obj) (DEVICE_LOCAL_INTERFACE(obj)->get_entities(obj))

/**
 * @brief Device Local Get Feature With Address caller definition
 */
#define DEVICE_LOCAL_GET_FEATURE_WITH_ADDRESS(obj, feature_addr) \
    (DEVICE_LOCAL_INTERFACE(obj)->get_feature_with_address(obj, feature_addr))

/**
 * @brief Device Local Handle Message caller definition
 */
#define DEVICE_LOCAL_HANDLE_MESSAGE(obj, msg, remote_device) \
    (DEVICE_LOCAL_INTERFACE(obj)->handle_message(obj, msg, remote_device))

/**
 * @brief Device Local Get Node Management caller definition
 */
#define DEVICE_LOCAL_GET_NODE_MANAGEMENT(obj) (DEVICE_LOCAL_INTERFACE(obj)->get_node_management(obj))

/**
 * @brief Device Local Get Binding Manager caller definition
 */
#define DEVICE_LOCAL_GET_BINDING_MANAGER(obj) (DEVICE_LOCAL_INTERFACE(obj)->get_binding_manager(obj))

/**
 * @brief Device Local Get Subscription Manager caller definition
 */
#define DEVICE_LOCAL_GET_SUBSCRIPTION_MANAGER(obj) (DEVICE_LOCAL_INTERFACE(obj)->get_subscription_manager(obj))

/**
 * @brief Device Local Get Events Manager caller definition
 */
#define DEVICE_LOCAL_GET_EVENTS_MANAGER(obj) (DEVICE_LOCAL_INTERFACE(obj)->get_events_manager(obj))

/**
 * @brief Device Local Notify Subscribers caller definition
 */
#define DEVICE_LOCAL_NOTIFY_SUBSCRIBERS(obj, feature_addr, cmd) \
    (DEVICE_LOCAL_INTERFACE(obj)->notify_subscribers(obj, feature_addr, cmd))

/**
 * @brief Device Local Create Information caller definition
 */
#define DEVICE_LOCAL_CREATE_INFORMATION(obj) (DEVICE_LOCAL_INTERFACE(obj)->create_information(obj))

/**
 * @brief Device Local Lock caller definition
 */
#define DEVICE_LOCAL_LOCK(obj) (DEVICE_LOCAL_INTERFACE(obj)->lock(obj))

/**
 * @brief Device Local Unlock caller definition
 */
#define DEVICE_LOCAL_UNLOCK(obj) (DEVICE_LOCAL_INTERFACE(obj)->unlock(obj))

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SPINE_API_DEVICE_LOCAL_INTERFACE_H_
