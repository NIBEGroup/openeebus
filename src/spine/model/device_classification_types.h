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
 * @brief SPINE Datagram Device Classification data types declarations
 */

#ifndef SRC_SPINE_MODEL_DEVICE_CLASSIFICATION_TYPES_H_
#define SRC_SPINE_MODEL_DEVICE_CLASSIFICATION_TYPES_H_

#include <stdbool.h>
#include <stdint.h>

#include "src/spine/model/common_data_types.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef const char* DeviceClassificationStringType;

typedef int32_t PowerSourceType;

enum {
    kPowerSourceTypeUnknown,          /**< "unknown" */
    kPowerSourceTypeMainssinglephase, /**< "mainsSinglePhase" */
    kPowerSourceTypeMains3Phase,      /**< "mains3Phase" */
    kPowerSourceTypeBattery,          /**< "battery" */
    kPowerSourceTypeDc,               /**< "dc" */
};

typedef struct DeviceClassificationManufacturerDataType DeviceClassificationManufacturerDataType;

struct DeviceClassificationManufacturerDataType {
    DeviceClassificationStringType device_name;                      /**< "deviceName" */
    DeviceClassificationStringType device_code;                      /**< "deviceCode" */
    DeviceClassificationStringType serial_number;                    /**< "serialNumber" */
    DeviceClassificationStringType software_revision;                /**< "softwareRevision" */
    DeviceClassificationStringType hardware_revision;                /**< "hardwareRevision" */
    DeviceClassificationStringType vendor_name;                      /**< "vendorName" */
    DeviceClassificationStringType vendor_code;                      /**< "vendorCode" */
    DeviceClassificationStringType brand_name;                       /**< "brandName" */
    const PowerSourceType* power_source;                             /**< "powerSource" */
    DeviceClassificationStringType manufacturer_node_identification; /**< "manufacturerNodeIdentification" */
    LabelType manufacturer_label;                                    /**< "manufacturerLabel" */
    DescriptionType manufacturer_description;                        /**< "manufacturerDescription" */
};

typedef struct DeviceClassificationManufacturerDataElementsType DeviceClassificationManufacturerDataElementsType;

struct DeviceClassificationManufacturerDataElementsType {
    ElementTagType device_name;                      /**< "deviceName" */
    ElementTagType device_code;                      /**< "deviceCode" */
    ElementTagType serial_number;                    /**< "serialNumber" */
    ElementTagType software_revision;                /**< "softwareRevision" */
    ElementTagType hardware_revision;                /**< "hardwareRevision" */
    ElementTagType vendor_name;                      /**< "vendorName" */
    ElementTagType vendor_code;                      /**< "vendorCode" */
    ElementTagType brand_name;                       /**< "brandName" */
    ElementTagType power_source;                     /**< "powerSource" */
    ElementTagType manufacturer_node_identification; /**< "manufacturerNodeIdentification" */
    ElementTagType manufacturer_label;               /**< "manufacturerLabel" */
    ElementTagType manufacturer_description;         /**< "manufacturerDescription" */
};

typedef struct DeviceClassificationUserDataType DeviceClassificationUserDataType;

struct DeviceClassificationUserDataType {
    DeviceClassificationStringType user_node_identification; /**< "userNodeIdentification" */
    LabelType user_label;                                    /**< "userLabel" */
    DescriptionType user_description;                        /**< "userDescription" */
};

typedef struct DeviceClassificationUserDataElementsType DeviceClassificationUserDataElementsType;

struct DeviceClassificationUserDataElementsType {
    ElementTagType user_node_identification; /**< "userNodeIdentification" */
    ElementTagType user_label;               /**< "userLabel" */
    ElementTagType user_description;         /**< "userDescription" */
};

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SPINE_MODEL_DEVICE_CLASSIFICATION_TYPES_H_
