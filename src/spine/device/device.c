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
 * @brief Device implementation
 */

#include "device.h"

#include <string.h>

#include "src/common/eebus_malloc.h"
#include "src/common/num_ptr.h"
#include "src/common/string_util.h"
#include "src/spine/api/device_interface.h"
#include "src/spine/model/model.h"
#include "src/spine/model/node_management_types.h"

void DeviceConstruct(
    Device* self,
    const char* addr,
    const char* type,
    const NetworkManagementFeatureSetType* feature_set
) {
    self->address = StringCopy(addr);

    const DeviceTypeType* device_type = ModelStringToDeviceType(type);
    if (device_type != NULL) {
        self->type = Int32Create(*device_type);
    } else {
        self->type = NULL;
    }

    if (feature_set != NULL) {
        self->feature_set = Int32Create(*feature_set);
    } else {
        self->feature_set = NULL;
    }
}

void DeviceDestruct(DeviceObject* self) {
    Device* const device = DEVICE(self);

    Int32Delete((int32_t*)device->feature_set);
    device->feature_set = NULL;

    Int32Delete((int32_t*)device->type);
    device->type = NULL;

    StringDelete((char*)device->address);
    device->address = NULL;
}

const char* DeviceGetAddress(const DeviceObject* self) {
    return DEVICE(self)->address;
}

const DeviceTypeType* DeviceGetDeviceType(const DeviceObject* self) {
    return DEVICE(self)->type;
}

const NetworkManagementFeatureSetType* DeviceGetFeatureSet(const DeviceObject* self) {
    return DEVICE(self)->feature_set;
}

const NodeManagementDestinationDataType* DeviceCreateDestinationData(const DeviceObject* self) {
    Device* const device = DEVICE(self);

    return NodeManagementDestinationDataCreate(device->address, device->type, device->feature_set);
}
