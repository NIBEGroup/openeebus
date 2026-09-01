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
 * @brief EEBUS Device Info related functions implementation
 */

#include "eebus_device_info.h"

#include "src/common/eebus_errors.h"
#include "src/common/eebus_malloc.h"
#include "src/common/string_util.h"

static EebusError EebusDeviceInfoConstruct(
    EebusDeviceInfo* device_info,
    const char* type,
    const char* vendor,
    const char* brand,
    const char* model,
    const char* serial_num,
    const char* ship_id
);

void EebusDeviceInfoDestruct(EebusDeviceInfo* device_info);

EebusError EebusDeviceInfoConstruct(
    EebusDeviceInfo* device_info,
    const char* type,
    const char* vendor,
    const char* brand,
    const char* model,
    const char* serial_num,
    const char* ship_id
) {
    memset(device_info, 0, sizeof(*device_info));

    if (StringIsEmpty(type) || StringIsEmpty(vendor) || StringIsEmpty(brand) || StringIsEmpty(model)
        || StringIsEmpty(serial_num) || StringIsEmpty(ship_id)) {
        return kEebusErrorInputArgument;
    }

    device_info->type = StringCopy(type);
    if (device_info->type == NULL) {
        return kEebusErrorMemoryAllocate;
    }

    device_info->vendor = StringCopy(vendor);
    if (device_info->vendor == NULL) {
        return kEebusErrorMemoryAllocate;
    }

    device_info->brand = StringCopy(brand);
    if (device_info->brand == NULL) {
        return kEebusErrorMemoryAllocate;
    }

    device_info->model = StringCopy(model);
    if (device_info->model == NULL) {
        return kEebusErrorMemoryAllocate;
    }

    device_info->serial_num = StringCopy(serial_num);
    if (device_info->serial_num == NULL) {
        return kEebusErrorMemoryAllocate;
    }

    device_info->ship_id = StringCopy(ship_id);
    if (device_info->ship_id == NULL) {
        return kEebusErrorMemoryAllocate;
    }

    // Create the SPINE device address, according to Protocol Specification 7.1.1.2
    device_info->address = StringFmtSprintf("d:_n:%s_%s", vendor, serial_num);
    if (device_info->address == NULL) {
        return kEebusErrorMemoryAllocate;
    }

    return kEebusErrorOk;
}

EebusDeviceInfo* EebusDeviceInfoCreate(
    const char* type,
    const char* vendor,
    const char* brand,
    const char* model,
    const char* serial_num,
    const char* ship_id
) {
    EebusDeviceInfo* device_info = (EebusDeviceInfo*)EEBUS_MALLOC(sizeof(EebusDeviceInfo));
    if (device_info == NULL) {
        return NULL;
    }

    if (EebusDeviceInfoConstruct(device_info, type, vendor, brand, model, serial_num, ship_id) != kEebusErrorOk) {
        EEBUS_FREE(device_info);
        return NULL;
    }

    return device_info;
}

EebusDeviceInfo* EebusDeviceInfoCopy(const EebusDeviceInfo* src) {
    return EebusDeviceInfoCreate(src->type, src->vendor, src->brand, src->model, src->serial_num, src->ship_id);
}

void EebusDeviceInfoDestruct(EebusDeviceInfo* device_info) {
    StringDelete((char*)device_info->type);
    device_info->type = NULL;

    StringDelete((char*)device_info->vendor);
    device_info->vendor = NULL;

    StringDelete((char*)device_info->brand);
    device_info->brand = NULL;

    StringDelete((char*)device_info->model);
    device_info->model = NULL;

    StringDelete((char*)device_info->serial_num);
    device_info->serial_num = NULL;

    StringDelete((char*)device_info->ship_id);
    device_info->ship_id = NULL;

    StringDelete((char*)device_info->address);
    device_info->address = NULL;
}

void EebusDeviceInfoDelete(EebusDeviceInfo* device_info) {
    if (device_info != NULL) {
        EebusDeviceInfoDestruct(device_info);
        EEBUS_FREE(device_info);
    }
}
