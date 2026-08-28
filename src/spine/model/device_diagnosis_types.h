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
 * @brief SPINE Datagram Device Diagnosis data types declarations
 */

#ifndef SRC_SPINE_MODEL_DEVICE_DIAGNOSIS_TYPES_H_
#define SRC_SPINE_MODEL_DEVICE_DIAGNOSIS_TYPES_H_

#include <stdbool.h>
#include <stdint.h>

#include "src/spine/model/common_data_types.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef const char* VendorStateCodeType;

typedef const char* LastErrorCodeType;

typedef int32_t DeviceDiagnosisOperatingStateType;

enum {
    kDeviceDiagnosisOperatingStateTypeNormalOperation,     /**< "normalOperation" */
    kDeviceDiagnosisOperatingStateTypeStandby,             /**< "standby" */
    kDeviceDiagnosisOperatingStateTypeFailure,             /**< "failure" */
    kDeviceDiagnosisOperatingStateTypeServiceNeeded,       /**< "serviceNeeded" */
    kDeviceDiagnosisOperatingStateTypeOverrideDetected,    /**< "overrideDetected" */
    kDeviceDiagnosisOperatingStateTypeInAlarm,             /**< "inAlarm" */
    kDeviceDiagnosisOperatingStateTypeNotReachable,        /**< "notReachable" */
    kDeviceDiagnosisOperatingStateTypeFinished,            /**< "finished" */
    kDeviceDiagnosisOperatingStateTypeTemporarilyNotReady, /**< "temporarilyNotReady" */
    kDeviceDiagnosisOperatingStateTypeOff,                 /**< "off" */
};

typedef int32_t PowerSupplyConditionType;

enum {
    kPowerSupplyConditionTypeGood,     /**< "good" */
    kPowerSupplyConditionTypeLow,      /**< "low" */
    kPowerSupplyConditionTypeCritical, /**< "critical" */
    kPowerSupplyConditionTypeUnknown,  /**< "unknown" */
    kPowerSupplyConditionTypeError,    /**< "error" */
};

typedef struct DeviceDiagnosisStateDataType DeviceDiagnosisStateDataType;

struct DeviceDiagnosisStateDataType {
    const AbsoluteOrRelativeTimeTypeType* timestamp;          /**< "timestamp" */
    const DeviceDiagnosisOperatingStateType* operating_state; /**< "operatingState" */
    VendorStateCodeType vendor_state_code;                    /**< "vendorStateCode" */
    LastErrorCodeType last_error_code;                        /**< "lastErrorCode" */
    const DurationType* up_time;                              /**< "upTime" */
    const DurationType* total_up_time;                        /**< "totalUpTime" */
    const PowerSupplyConditionType* power_supply_condition;   /**< "powerSupplyCondition" */
};

typedef struct DeviceDiagnosisStateDataElementsType DeviceDiagnosisStateDataElementsType;

struct DeviceDiagnosisStateDataElementsType {
    ElementTagType timestamp;              /**< "timestamp" */
    ElementTagType operating_state;        /**< "operatingState" */
    ElementTagType vendor_state_code;      /**< "vendorStateCode" */
    ElementTagType last_error_code;        /**< "lastErrorCode" */
    ElementTagType up_time;                /**< "upTime" */
    ElementTagType total_up_time;          /**< "totalUpTime" */
    ElementTagType power_supply_condition; /**< "powerSupplyCondition" */
};

typedef struct DeviceDiagnosisHeartbeatDataType DeviceDiagnosisHeartbeatDataType;

struct DeviceDiagnosisHeartbeatDataType {
    const AbsoluteOrRelativeTimeType* timestamp; /**< "timestamp" */
    const uint64_t* heartbeat_counter;           /**< "heartbeatCounter" */
    const DurationType* heartbeat_timeout;       /**< "heartbeatTimeout" */
};

typedef struct DeviceDiagnosisHeartbeatDataElementsType DeviceDiagnosisHeartbeatDataElementsType;

struct DeviceDiagnosisHeartbeatDataElementsType {
    ElementTagType timestamp;         /**< "timestamp" */
    ElementTagType heartbeat_counter; /**< "heartbeatCounter" */
    ElementTagType heartbeat_timeout; /**< "heartbeatTimeout" */
};

typedef struct DeviceDiagnosisServiceDataType DeviceDiagnosisServiceDataType;

struct DeviceDiagnosisServiceDataType {
    const AbsoluteOrRelativeTimeType* timestamp;         /**< "timestamp" */
    const AbsoluteOrRelativeTimeType* installation_time; /**< "installationTime" */
    const uint64_t* boot_counter;                        /**< "bootCounter" */
    const char* next_service;                            /**< "nextService" */
};

typedef struct DeviceDiagnosisServiceDataElementsType DeviceDiagnosisServiceDataElementsType;

struct DeviceDiagnosisServiceDataElementsType {
    ElementTagType timestamp;         /**< "timestamp" */
    ElementTagType installation_time; /**< "installationTime" */
    ElementTagType boot_counter;      /**< "bootCounter" */
    ElementTagType next_service;      /**< "nextService" */
};

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SPINE_MODEL_DEVICE_DIAGNOSIS_TYPES_H_
