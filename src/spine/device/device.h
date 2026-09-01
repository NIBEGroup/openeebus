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
 * @brief Device implementation declarations
 */

#ifndef SRC_SPINE_DEVICE_DEVICE_H_
#define SRC_SPINE_DEVICE_DEVICE_H_

#include "src/spine/api/device_interface.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct Device Device;

struct Device {
    /** Implements the Device Interface */
    DeviceObject obj;

    const char* address;
    const DeviceTypeType* type;
    const NetworkManagementFeatureSetType* feature_set;
};

#define DEVICE(obj) ((Device*)(obj))

void DeviceConstruct(
    Device* self,
    const char* addr,
    const char* type,
    const NetworkManagementFeatureSetType* feature_set
);
void DeviceDestruct(DeviceObject* self);
const char* DeviceGetAddress(const DeviceObject* self);
const DeviceTypeType* DeviceGetDeviceType(const DeviceObject* self);
const NetworkManagementFeatureSetType* DeviceGetFeatureSet(const DeviceObject* self);
const NodeManagementDestinationDataType* DeviceCreateDestinationData(const DeviceObject* self);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SPINE_DEVICE_DEVICE_H_
