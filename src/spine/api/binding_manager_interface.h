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
 * @brief Binding Manager interface declarations
 */

#ifndef SRC_SPINE_API_BINDING_MANAGER_INTERFACE_H_
#define SRC_SPINE_API_BINDING_MANAGER_INTERFACE_H_

#include "src/common/eebus_errors.h"
#include "src/spine/api/device_remote_interface.h"
#include "src/spine/api/feature_local_interface.h"
#include "src/spine/api/feature_remote_interface.h"
#include "src/spine/model/binding_management_types.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @brief Binding Manager Interface
 * (Binding Manager "virtual functions table" declaration)
 */
typedef struct BindingManagerInterface BindingManagerInterface;

/**
 * @brief Binding Manager Object type definition
 * ("abstract class", has no members but only pointer to
 * "virtual functions table")
 */
typedef struct BindingManagerObject BindingManagerObject;

/**
 * @brief BindingManager Interface Structure
 */
struct BindingManagerInterface {
    void (*destruct)(BindingManagerObject* self);
    EebusError (*add_binding)(
        BindingManagerObject* self,
        DeviceRemoteObject* remote_device,
        const BindingManagementRequestCallType* data
    );
    EebusError (*remove_binding)(
        BindingManagerObject* self,
        const BindingManagementDeleteCallType* data,
        DeviceRemoteObject* remote_device
    );
    void (*remove_device_bindings)(BindingManagerObject* self, DeviceRemoteObject* remote_device);
    void (*remove_entity_bindings)(BindingManagerObject* self, EntityRemoteObject* remote_entity);
    bool (*has_binding)(
        const BindingManagerObject* self,
        const FeatureAddressType* local_addr,
        const FeatureAddressType* remote_addr
    );
    NodeManagementBindingDataType* (*create_binding_data)(
        const BindingManagerObject* self,
        const DeviceRemoteObject* remote_device
    );
};

/**
 * @brief Binding Manager Object Structure
 */
struct BindingManagerObject {
    const BindingManagerInterface* interface_;
};

/**
 * @brief Binding Manager pointer typecast
 */
#define BINDING_MANAGER_OBJECT(obj) ((BindingManagerObject*)(obj))

/**
 * @brief Binding Manager Interface class pointer typecast
 */
#define BINDING_MANAGER_INTERFACE(obj) (BINDING_MANAGER_OBJECT(obj)->interface_)

/**
 * @brief Binding Manager Destruct caller definition
 */
#define BINDING_MANAGER_DESTRUCT(obj) (BINDING_MANAGER_INTERFACE(obj)->destruct(obj))

/**
 * @brief Binding Manager Add Binding caller definition
 */
#define BINDING_MANAGER_ADD_BINDING(obj, remote_device, data) \
    (BINDING_MANAGER_INTERFACE(obj)->add_binding(obj, remote_device, data))

/**
 * @brief Binding Manager Remove Binding caller definition
 */
#define BINDING_MANAGER_REMOVE_BINDING(obj, data, remote_device) \
    (BINDING_MANAGER_INTERFACE(obj)->remove_binding(obj, data, remote_device))

/**
 * @brief Binding Manager Remove Device Bindings caller definition
 */
#define BINDING_MANAGER_REMOVE_DEVICE_BINDINGS(obj, remote_device) \
    (BINDING_MANAGER_INTERFACE(obj)->remove_device_bindings(obj, remote_device))

/**
 * @brief Binding Manager Remove Entity Bindings caller definition
 */
#define BINDING_MANAGER_REMOVE_ENTITY_BINDINGS(obj, remote_entity) \
    (BINDING_MANAGER_INTERFACE(obj)->remove_entity_bindings(obj, remote_entity))

/**
 * @brief Binding Manager Has Binding caller definition
 */
#define BINDING_MANAGER_HAS_BINDING(obj, local_addr, remote_addr) \
    (BINDING_MANAGER_INTERFACE(obj)->has_binding(obj, local_addr, remote_addr))

/**
 * @brief Binding Manager Create Binding Data caller definition
 */
#define BINDING_MANAGER_CREATE_BINDING_DATA(obj, remote_device) \
    (BINDING_MANAGER_INTERFACE(obj)->create_binding_data(obj, remote_device))

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SPINE_API_BINDING_MANAGER_INTERFACE_H_
