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
 * @brief SPINE Feature specific type declarations
 */

#ifndef SRC_SPINE_MODEL_FEATURE_TYPES_H_
#define SRC_SPINE_MODEL_FEATURE_TYPES_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "src/spine/model/common_data_types.h"
#include "src/spine/model/entity_types.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct FeatureAddressType FeatureAddressType;

struct FeatureAddressType {
    const char* device;            /**< "device" */
    const uint32_t* const* entity; /**< "entity" */
    size_t entity_size;
    const uint32_t* feature; /**< "feature" */
};

typedef struct FeatureAddressElementsType FeatureAddressElementsType;

struct FeatureAddressElementsType {
    ElementTagType device;  /**< "device" */
    ElementTagType entity;  /**< "entity" */
    ElementTagType feature; /**< "feature" */
};

typedef int32_t RoleType;

enum {
    kRoleTypeClient,  /**< "client" */
    kRoleTypeServer,  /**< "server" */
    kRoleTypeSpecial, /**< "special" */
};

typedef char* FeatureGroupType;

typedef int32_t FeatureTypeType;

enum {
    kFeatureTypeTypeActuatorLevel,           /**< "ActuatorLevel" */
    kFeatureTypeTypeActuatorSwitch,          /**< "ActuatorSwitch" */
    kFeatureTypeTypeAlarm,                   /**< "Alarm" */
    kFeatureTypeTypeDataTunneling,           /**< "DataTunneling" */
    kFeatureTypeTypeDeviceClassification,    /**< "DeviceClassification" */
    kFeatureTypeTypeDeviceDiagnosis,         /**< "DeviceDiagnosis" */
    kFeatureTypeTypeDirectControl,           /**< "DirectControl" */
    kFeatureTypeTypeElectricalConnection,    /**< "ElectricalConnection" */
    kFeatureTypeTypeGeneric,                 /**< "Generic" */
    kFeatureTypeTypeHvac,                    /**< "HVAC" */
    kFeatureTypeTypeLoadControl,             /**< "LoadControl" */
    kFeatureTypeTypeMeasurement,             /**< "Measurement" */
    kFeatureTypeTypeMessaging,               /**< "Messaging" */
    kFeatureTypeTypeNetworkManagement,       /**< "NetworkManagement" */
    kFeatureTypeTypeNodeManagement,          /**< "NodeManagement" */
    kFeatureTypeTypeOperatingConstraints,    /**< "OperatingConstraints" */
    kFeatureTypeTypePowerSequences,          /**< "PowerSequences" */
    kFeatureTypeTypeSensing,                 /**< "Sensing" */
    kFeatureTypeTypeSetpoint,                /**< "Setpoint" */
    kFeatureTypeTypeSmartEnergyManagementPs, /**< "SmartEnergyManagementPs" */
    kFeatureTypeTypeTaskManagement,          /**< "TaskManagement" */
    kFeatureTypeTypeThreshold,               /**< "Threshold" */
    kFeatureTypeTypeTimeInformation,         /**< "TimeInformation" */
    kFeatureTypeTypeTimeTable,               /**< "TimeTable" */
    kFeatureTypeTypeDeviceConfiguration,     /**< "DeviceConfiguration" */
    kFeatureTypeTypeSupplyCondition,         /**< "SupplyCondition" */
    kFeatureTypeTypeTimeSeries,              /**< "TimeSeries" */
    kFeatureTypeTypeTariffInformation,       /**< "TariffInformation" */
    kFeatureTypeTypeIncentiveTable,          /**< "IncentiveTable" */
    kFeatureTypeTypeBill,                    /**< "Bill" */
    kFeatureTypeTypeIdentification,          /**< "Identification" */
    kFeatureTypeTypeStateInformation,        /**< "StateInformation" */
};

typedef int32_t FeatureSpecificUsageType;

enum {
    FeatureSpecificUsageTypeHistory,                               /**< "History" */
    FeatureSpecificUsageTypeRealtime,                              /**< "RealTime" */
    FeatureSpecificUsageTypeOperationmode,                         /**< "OperationMode" */
    FeatureSpecificUsageTypeOverrun,                               /**< "Overrun" */
    FeatureSpecificUsageTypeContact,                               /**< "Contact" */
    FeatureSpecificUsageTypeElectrical,                            /**< "Electrical" */
    FeatureSpecificUsageTypeHeat,                                  /**< "Heat" */
    FeatureSpecificUsageTypeLevel,                                 /**< "Level" */
    FeatureSpecificUsageTypePressure,                              /**< "Pressure" */
    FeatureSpecificUsageTypeTemperature,                           /**< "Temperature" */
    FeatureSpecificUsageTypeFixedForecast,                         /**< "FixedForecast" */
    FeatureSpecificUsageTypeFlexibleChosenForecast,                /**< "FlexibleChosenForecast" */
    FeatureSpecificUsageTypeFlexibleOptionalForecast,              /**< "FlexibleOptionalForecast" */
    FeatureSpecificUsageTypeOptionalSequenceBasedImmediateControl, /**< "OptionalSequenceBasedImmediateControl" */
};

FeatureAddressType* FeatureAddressCreate(const EntityAddressType* entity_addr, uint32_t feature_id);
bool FeatureAddressIsValid(const FeatureAddressType* addr);
FeatureAddressType* FeatureAddressCopy(const FeatureAddressType* self);
bool FeatureAddressCompare(const FeatureAddressType* addr_a, const FeatureAddressType* addr_b);
bool FeatureAddressMatchDevice(const FeatureAddressType* addr, const char* device);
void FeatureAddressPrint(const char* fmt, const FeatureAddressType* addr);
void FeatureAddressDelete(FeatureAddressType* addr);
void FeatureAddressElementsDelete(FeatureAddressElementsType* self);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SPINE_MODEL_FEATURE_TYPES_H_
