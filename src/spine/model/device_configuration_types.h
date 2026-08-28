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
 * @brief SPINE Datagram Device Configuration data types declarations
 */

#ifndef SRC_SPINE_MODEL_DEVICE_CONFIGURATION_TYPES_H_
#define SRC_SPINE_MODEL_DEVICE_CONFIGURATION_TYPES_H_

#include <stdbool.h>
#include <stdint.h>

#include "src/common/eebus_date_time/eebus_date_time.h"
#include "src/common/eebus_errors.h"
#include "src/spine/model/common_data_types.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef uint32_t DeviceConfigurationKeyIdType;

typedef int32_t DeviceConfigurationKeyValueStringType;

enum {
    kDeviceConfigurationKeyValueStringTypeISO151182ED1, /**< "iso15118-2ed1" */
    kDeviceConfigurationKeyValueStringTypeISO151182ED2, /**< "iso15118-2ed2" */
    kDeviceConfigurationKeyValueStringTypeIEC61851,     /**< "iec61851" */
};

typedef int32_t DeviceConfigurationKeyNameType;

enum {
    kDeviceConfigurationKeyNameTypePeakPowerOfPVSystem,                 /**< "peakPowerOfPvSystem" */
    kDeviceConfigurationKeyNameTypePvCurtailmentLimitFactor,            /**< "pvCurtailmentLimitFactor" */
    kDeviceConfigurationKeyNameTypeAsymmetricChargingSupported,         /**< "asymmetricChargingSupported" */
    kDeviceConfigurationKeyNameTypeCommunicationsStandard,              /**< "communicationsStandard" */
    kDeviceConfigurationKeyNameTypeInverterGridCode,                    /**< "inverterGridCode" */
    kDeviceConfigurationKeyNameTypePvStringAvailabilityStatus,          /**< "pvStringAvailabilityStatus" */
    kDeviceConfigurationKeyNameTypeBatteryAvailabilityStatus,           /**< "batteryAvailabilityStatus" */
    kDeviceConfigurationKeyNameTypeGridConnectionStatus,                /**< "gridConnectionStatus" */
    kDeviceConfigurationKeyNameTypeTimeToAcChargePowerMax,              /**< "timeToAcChargePowerMax" */
    kDeviceConfigurationKeyNameTypeTimeToAcDischargePowerMax,           /**< "timeToAcDischargePowerMax" */
    kDeviceConfigurationKeyNameTypeTilt,                                /**< "tilt" */
    kDeviceConfigurationKeyNameTypeAzimuth,                             /**< "azimuth" */
    kDeviceConfigurationKeyNameTypeBatteryType,                         /**< "batteryType" */
    kDeviceConfigurationKeyNameTypeMaxCycleCountPerDay,                 /**< "maxCycleCountPerDay" */
    kDeviceConfigurationKeyNameTypeFailsafeConsumptionActivePowerLimit, /**< "failsafeConsumptionActivePowerLimit" */
    kDeviceConfigurationKeyNameTypeFailsafeProductionActivePowerLimit,  /**< "failsafeProductionActivePowerLimit" */
    kDeviceConfigurationKeyNameTypeFailsafePositiveReactivePowerLimit,  /**< "failsafePositiveReactivePowerLimit" */
    kDeviceConfigurationKeyNameTypeFailsafeNegativeReactivePowerLimit,  /**< "failsafeNegativeReactivePowerLimit" */
    kDeviceConfigurationKeyNameTypeFailsafePositiveCosPhiLimit,         /**< "failsafePositiveCosPhiLimit" */
    kDeviceConfigurationKeyNameTypeFailsafeNegativeCosPhiLimit,         /**< "failsafeNegativeCosPhiLimit" */
    kDeviceConfigurationKeyNameTypeMaxAcChargePower,                    /**< "maxAcChargePower" */
    kDeviceConfigurationKeyNameTypeMaxAcDischargePower,                 /**< "maxAcDischargePower" */
    kDeviceConfigurationKeyNameTypeMaxDcChargePower,                    /**< "maxDcChargePower" */
    kDeviceConfigurationKeyNameTypeMaxDcDischargePower,                 /**< "maxDcDischargePower" */
    kDeviceConfigurationKeyNameTypeBatteryActiveControlMode,            /**< "batteryActiveControlMode" */
    kDeviceConfigurationKeyNameTypeDefaultAcPower,                      /**< "defaultAcPower" */
    kDeviceConfigurationKeyNameTypeDefaultDcPower,                      /**< "defaultDcPower" */
    kDeviceConfigurationKeyNameTypeDefaultPccPower,                     /**< "defaultPccPower" */
    kDeviceConfigurationKeyNameTypeFailsafeAcPowerSetpoint,             /**< "failsafeAcPowerSetpoint" */
    kDeviceConfigurationKeyNameTypeFailsafeDcPowerSetpoint,             /**< "failsafeDcPowerSetpoint" */
    kDeviceConfigurationKeyNameTypeFailsafePccPowerSetpoint,            /**< "failsafePccPowerSetpoint" */
    kDeviceConfigurationKeyNameTypeFailsafeDurationMinimum,             /**< "failsafeDurationMinimum" */
    kDeviceConfigurationKeyNameTypeDischargingBelowTargetEnergyRequestPermitted, /**<
                                                                                    "dischargingBelowTargetEnergyRequestPermitted"
                                                                                  */
    kDeviceConfigurationKeyNameTypeIncentivesSimulationCyclesMax,                /**< "incentivesSimulationCyclesMax" */
    kDeviceConfigurationKeyNameTypeIncentivesSimulationConcurrent,    /**< "incentivesSimulationConcurrent" */
    kDeviceConfigurationKeyNameTypeIncentivesTimeoutIncentiveRequest, /**< "incentivesTimeoutIncentiveRequest" */
    kDeviceConfigurationKeyNameTypeIncentivesWaitIncentiveWriteable,  /**< "incentivesWaitIncentiveWriteable" */
};

typedef int32_t DeviceConfigurationKeyValueTypeType;

enum {
    kDeviceConfigurationKeyValueTypeTypeBoolean,      /**< "boolean" */
    kDeviceConfigurationKeyValueTypeTypeDate,         /**< "date" */
    kDeviceConfigurationKeyValueTypeTypeDateTime,     /**< "dateTime" */
    kDeviceConfigurationKeyValueTypeTypeDuration,     /**< "duration" */
    kDeviceConfigurationKeyValueTypeTypeString,       /**< "string" */
    kDeviceConfigurationKeyValueTypeTypeTime,         /**< "time" */
    kDeviceConfigurationKeyValueTypeTypeScaledNumber, /**< "scaledNumber" */
    kDeviceConfigurationKeyValueTypeTypeInteger,      /**< "integer" */
};

typedef struct DeviceConfigurationKeyValueValueType DeviceConfigurationKeyValueValueType;

struct DeviceConfigurationKeyValueValueType {
    const bool* boolean;                                 /**< "boolean" */
    const DateType* date;                                /**< "date" */
    const DateTimeType* date_time;                       /**< "dateTime" */
    const DurationType* duration;                        /**< "duration" */
    const DeviceConfigurationKeyValueStringType* string; /**< "string" */
    const TimeType* time;                                /**< "time" */
    const ScaledNumberType* scaled_number;               /**< "scaledNumber" */
    const int64_t* integer;                              /**< "integer" */
};

typedef struct DeviceConfigurationKeyValueValueElementsType DeviceConfigurationKeyValueValueElementsType;

struct DeviceConfigurationKeyValueValueElementsType {
    ElementTagType boolean;                        /**< "boolean" */
    ElementTagType date;                           /**< "date" */
    ElementTagType date_time;                      /**< "dateTime" */
    ElementTagType duration;                       /**< "duration" */
    ElementTagType string;                         /**< "string" */
    ElementTagType time;                           /**< "time" */
    const ScaledNumberElementsType* scaled_number; /**< "scaledNumber" */
};

typedef struct DeviceConfigurationKeyValueDataType DeviceConfigurationKeyValueDataType;

struct DeviceConfigurationKeyValueDataType {
    const DeviceConfigurationKeyIdType* key_id;        /**< "keyId" */
    const DeviceConfigurationKeyValueValueType* value; /**< "value" */
    const bool* is_value_changeable;                   /**< "isValueChangeable" */
};

typedef struct DeviceConfigurationKeyValueDataElementsType DeviceConfigurationKeyValueDataElementsType;

struct DeviceConfigurationKeyValueDataElementsType {
    ElementTagType key_id;                                     /**< "keyId" */
    const DeviceConfigurationKeyValueValueElementsType* value; /**< "value" */
    ElementTagType is_value_changeable;                        /**< "isValueChangeable" */
};

typedef struct DeviceConfigurationKeyValueListDataType DeviceConfigurationKeyValueListDataType;

struct DeviceConfigurationKeyValueListDataType {
    const DeviceConfigurationKeyValueDataType* const*
        device_configuration_key_value_data; /**< "deviceConfigurationKeyValueData" */
    size_t device_configuration_key_value_data_size;
};

typedef struct DeviceConfigurationKeyValueListDataSelectorsType DeviceConfigurationKeyValueListDataSelectorsType;

struct DeviceConfigurationKeyValueListDataSelectorsType {
    const DeviceConfigurationKeyIdType* key_id; /**< "keyId" */
};

typedef struct DeviceConfigurationKeyValueDescriptionDataType DeviceConfigurationKeyValueDescriptionDataType;

struct DeviceConfigurationKeyValueDescriptionDataType {
    const DeviceConfigurationKeyIdType* key_id;            /**< "keyId" */
    const DeviceConfigurationKeyNameType* key_name;        /**< "keyName" */
    const DeviceConfigurationKeyValueTypeType* value_type; /**< "valueType" */
    const UnitOfMeasurementType* unit;                     /**< "unit" */
    LabelType label;                                       /**< "label" */
    DescriptionType description;                           /**< "description" */
};

typedef struct DeviceConfigurationKeyValueDescriptionDataElementsType
    DeviceConfigurationKeyValueDescriptionDataElementsType;

struct DeviceConfigurationKeyValueDescriptionDataElementsType {
    ElementTagType key_id;      /**< "keyId" */
    ElementTagType key_name;    /**< "keyName" */
    ElementTagType value_type;  /**< "valueType" */
    ElementTagType unit;        /**< "unit" */
    ElementTagType label;       /**< "label" */
    ElementTagType description; /**< "description" */
};

typedef struct DeviceConfigurationKeyValueDescriptionListDataType DeviceConfigurationKeyValueDescriptionListDataType;

struct DeviceConfigurationKeyValueDescriptionListDataType {
    const DeviceConfigurationKeyValueDescriptionDataType* const*
        device_configuration_key_value_description_data; /**< "deviceConfigurationKeyValueDescriptionData" */
    size_t device_configuration_key_value_description_data_size;
};

typedef struct DeviceConfigurationKeyValueDescriptionListDataSelectorsType
    DeviceConfigurationKeyValueDescriptionListDataSelectorsType;

struct DeviceConfigurationKeyValueDescriptionListDataSelectorsType {
    const DeviceConfigurationKeyIdType* key_id;     /**< "keyId" */
    const DeviceConfigurationKeyNameType* key_name; /**< "keyName" */
};

typedef struct DeviceConfigurationKeyValueConstraintsDataType DeviceConfigurationKeyValueConstraintsDataType;

struct DeviceConfigurationKeyValueConstraintsDataType {
    const DeviceConfigurationKeyIdType* key_id;                  /**< "keyId" */
    const DeviceConfigurationKeyValueValueType* value_range_min; /**< "valueRangeMin" */
    const DeviceConfigurationKeyValueValueType* value_range_max; /**< "valueRangeMax" */
    const DeviceConfigurationKeyValueValueType* value_step_size; /**< "valueStepSize" */
};

typedef struct DeviceConfigurationKeyValueConstraintsDataElementsType
    DeviceConfigurationKeyValueConstraintsDataElementsType;

struct DeviceConfigurationKeyValueConstraintsDataElementsType {
    ElementTagType key_id;                                               /**< "keyId" */
    const DeviceConfigurationKeyValueValueElementsType* value_range_min; /**< "valueRangeMin" */
    const DeviceConfigurationKeyValueValueElementsType* value_range_max; /**< "valueRangeMax" */
    const DeviceConfigurationKeyValueValueElementsType* value_step_size; /**< "valueStepSize" */
};

typedef struct DeviceConfigurationKeyValueConstraintsListDataType DeviceConfigurationKeyValueConstraintsListDataType;

struct DeviceConfigurationKeyValueConstraintsListDataType {
    const DeviceConfigurationKeyValueConstraintsDataType* const*
        device_configuration_key_value_constraints_data; /**< "deviceConfigurationKeyValueConstraintsData" */
    size_t device_configuration_key_value_constraints_data_size;
};

typedef struct DeviceConfigurationKeyValueConstraintsListDataSelectorsType
    DeviceConfigurationKeyValueConstraintsListDataSelectorsType;

struct DeviceConfigurationKeyValueConstraintsListDataSelectorsType {
    const DeviceConfigurationKeyIdType* key_id; /**< "keyId" */
};

EebusError
DeviceConfigurationKeyValueGetDuration(const DeviceConfigurationKeyValueDataType* key_value, DurationType* duration);
bool DeviceConfigurationKeyValueIsValid(const DeviceConfigurationKeyValueDataType* key_value);
const ScaledNumberType* DeviceConfigurationKeyValueGetScaledNumber(
    const DeviceConfigurationKeyValueDataType* key_value
);
bool DeviceConfigurationKeyValueIsChangeable(const DeviceConfigurationKeyValueDataType* key_value);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SPINE_MODEL_DEVICE_CONFIGURATION_TYPES_H_
