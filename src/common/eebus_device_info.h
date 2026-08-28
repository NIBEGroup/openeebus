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
 * @brief EEBUS Device Information declaration
 */
#ifndef SRC_COMMON_DEVICE_INFO_H_
#define SRC_COMMON_DEVICE_INFO_H_

#include "src/spine/model/device_types.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct EebusDeviceInfo EebusDeviceInfo;

struct EebusDeviceInfo {
    const char* type;
    const char* vendor;
    const char* brand;
    const char* model;
    const char* serial_num;
    const char* ship_id;
    const char* address;
};

/**
 * @brief Dynamiclally allocates and constructs the EEBUS Device Info instance
 * @param type Device type string
 * @param vendor Device vendor string
 * @param brand Device brand string
 * @param model Device model string
 * @param serial_num Device serial number string
 * @param ship_id Device SHIP ID string
 * @return EEBUS Device Info instance on successful, NULL otherwise
 */
EebusDeviceInfo* EebusDeviceInfoCreate(
    const char* type,
    const char* vendor,
    const char* brand,
    const char* model,
    const char* serial_num,
    const char* ship_id
);

/**
 * @brief Allocate memory for Device Info fields and copy related data
 * from source to destination specified
 * @param src EEBUS Device Info instance to copy from
 */
EebusDeviceInfo* EebusDeviceInfoCopy(const EebusDeviceInfo* src);

/**
 * @brief Release the EEBUS Device Info fields and deallocate the instance
 * @param device_info EEBUS Device Info instance to be deleted
 */
void EebusDeviceInfoDelete(EebusDeviceInfo* device_info);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_COMMON_DEVICE_INFO_H_
