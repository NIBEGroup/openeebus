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
 * @brief Device Local implementation declarations
 */

#ifndef SRC_SPINE_DEVICE_DEVICE_LOCAL_H_
#define SRC_SPINE_DEVICE_DEVICE_LOCAL_H_

#include <stddef.h>

#include "src/common/eebus_device_info.h"
#include "src/common/eebus_malloc.h"
#include "src/spine/api/device_local_interface.h"
#include "src/spine/model/entity_types.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

DeviceLocalObject*
DeviceLocalCreate(const EebusDeviceInfo* device_info, const NetworkManagementFeatureSetType* feature_set);

static inline void DeviceLocalDelete(DeviceLocalObject* device_local) {
    if (device_local != NULL) {
        DEVICE_DESTRUCT(DEVICE_OBJECT(device_local));
        EEBUS_FREE(device_local);
    }
}

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SPINE_DEVICE_DEVICE_LOCAL_H_
