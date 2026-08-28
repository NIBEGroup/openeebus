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
 * @brief SPINE Device related types declarations
 */

#ifndef SRC_SPINE_MODEL_DEVICE_TYPES_H_
#define SRC_SPINE_MODEL_DEVICE_TYPES_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct DeviceAddressType DeviceAddressType;
struct DeviceAddressType {
    const char* device; /**< "device" */
};

typedef int32_t DeviceTypeType;

enum {
    kDeviceTypeTypeDishwasher,              /**< "Dishwasher" */
    kDeviceTypeTypeDryer,                   /**< "Dryer" */
    kDeviceTypeTypeEnvironmentSensor,       /**< "EnvironmentSensor" */
    kDeviceTypeTypeGeneric,                 /**< "Generic" */
    kDeviceTypeTypeHeatgenerationSystem,    /**< "HeatGenerationSystem" */
    kDeviceTypeTypeHeatsinkSystem,          /**< "HeatSinkSystem" */
    kDeviceTypeTypeHeatstorageSystem,       /**< "HeatStorageSystem" */
    kDeviceTypeTypeHVACController,          /**< "HVACController" */
    kDeviceTypeTypeSubmeter,                /**< "SubMeter" */
    kDeviceTypeTypeWasher,                  /**< "Washer" */
    kDeviceTypeTypeElectricitySupplySystem, /**< "ElectricitySupplySystem" */
    kDeviceTypeTypeEnergyManagementSystem,  /**< "EnergyManagementSystem" */
    kDeviceTypeTypeInverter,                /**< "Inverter" */
    kDeviceTypeTypeChargingStation,         /**< "ChargingStation" */
};

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SPINE_MODEL_DEVICE_TYPES_H_
