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
 * @brief Device interface declarations
 */

#ifndef SRC_SPINE_API_DEVICE_INTERFACE_H_
#define SRC_SPINE_API_DEVICE_INTERFACE_H_

#include "src/spine/model/device_types.h"
#include "src/spine/model/network_management_types.h"
#include "src/spine/model/node_management_types.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @brief Device Interface
 * (Device "virtual functions table" declaration)
 */
typedef struct DeviceInterface DeviceInterface;

/**
 * @brief Device Object type definition
 * ("abstract class", has no members but only pointer to
 * "virtual functions table")
 */
typedef struct DeviceObject DeviceObject;

/**
 * @brief Device Interface Structure
 */
struct DeviceInterface {
    void (*destruct)(DeviceObject* self);
    const char* (*get_address)(const DeviceObject* self);
    const DeviceTypeType* (*get_device_type)(const DeviceObject* self);
    const NetworkManagementFeatureSetType* (*get_feature_set)(const DeviceObject* self);
    const NodeManagementDestinationDataType* (*create_destination_data)(const DeviceObject* self);
};

/**
 * @brief Device Object Structure
 */
struct DeviceObject {
    const DeviceInterface* interface_;
};

/**
 * @brief Device pointer typecast
 */
#define DEVICE_OBJECT(obj) ((DeviceObject*)(obj))

/**
 * @brief Device Interface class pointer typecast
 */
#define DEVICE_INTERFACE(obj) (DEVICE_OBJECT(obj)->interface_)

/**
 * @brief Device Destruct caller definition
 */
#define DEVICE_DESTRUCT(obj) (DEVICE_INTERFACE(obj)->destruct(obj))

/**
 * @brief Device Get Address caller definition
 */
#define DEVICE_GET_ADDRESS(obj) (DEVICE_INTERFACE(obj)->get_address(obj))

/**
 * @brief Device Get Device Type caller definition
 */
#define DEVICE_GET_DEVICE_TYPE(obj) (DEVICE_INTERFACE(obj)->get_device_type(obj))

/**
 * @brief Device Get Feature Set caller definition
 */
#define DEVICE_GET_FEATURE_SET(obj) (DEVICE_INTERFACE(obj)->get_feature_set(obj))

/**
 * @brief Device Create Destination Data caller definition
 */
#define DEVICE_CREATE_DESTINATION_DATA(obj) (DEVICE_INTERFACE(obj)->create_destination_data(obj))

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SPINE_API_DEVICE_INTERFACE_H_
