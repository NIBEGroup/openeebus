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
 * @brief SPINE Datagram State Information data types declarations
 */

#ifndef SRC_SPINE_MODEL_STATE_INFORMATION_TYPES_H_
#define SRC_SPINE_MODEL_STATE_INFORMATION_TYPES_H_

#include <stdbool.h>
#include <stdint.h>

#include "src/spine/model/common_data_types.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef uint32_t StateInformationIdType;

typedef int32_t StateInformationType;

enum {
    kStateInformationTypeExternalOverrideFromGrid,      /**< "externalOverrideFromGrid" */
    kStateInformationTypeAutonomousGridSupport,         /**< "autonomousGridSupport" */
    kStateInformationTypeIslandingMode,                 /**< "islandingMode" */
    kStateInformationTypeBalancing,                     /**< "balancing" */
    kStateInformationTypeTrickleCharging,               /**< "trickleCharging" */
    kStateInformationTypeCalibration,                   /**< "calibration" */
    kStateInformationTypeCommissioningMissing,          /**< "commissioningMissing" */
    kStateInformationTypeSleeping,                      /**< "sleeping" */
    kStateInformationTypeStarting,                      /**< "starting" */
    kStateInformationTypeMppt,                          /**< "mppt" */
    kStateInformationTypeThrottled,                     /**< "throttled" */
    kStateInformationTypeShuttingDown,                  /**< "shuttingDown" */
    kStateInformationTypeManualShutdown,                /**< "manualShutdown" */
    kStateInformationTypeInverterDefective,             /**< "inverterDefective" */
    kStateInformationTypeBatteryOvercurrentProtection,  /**< "batteryOvercurrentProtection" */
    kStateInformationTypePvStringOvercurrentProtection, /**< "pvStringOvercurrentProtection" */
    kStateInformationTypeGridFault,                     /**< "gridFault" */
    kStateInformationTypeGroundFault,                   /**< "groundFault" */
    kStateInformationTypeAcDisconnected,                /**< "acDisconnected" */
    kStateInformationTypeDcDisconnected,                /**< "dcDisconnected" */
    kStateInformationTypeCabinetOpen,                   /**< "cabinetOpen" */
    kStateInformationTypeOverTemperature,               /**< "overTemperature" */
    kStateInformationTypeUnderTemperature,              /**< "underTemperature" */
    kStateInformationTypeFrequencyAboveLimit,           /**< "frequencyAboveLimit" */
    kStateInformationTypeFrequencyBelowLimit,           /**< "frequencyBelowLimit" */
    kStateInformationTypeAcVoltageAboveLimit,           /**< "acVoltageAboveLimit" */
    kStateInformationTypeAcVoltageBelowLimit,           /**< "acVoltageBelowLimit" */
    kStateInformationTypeDcVoltageAboveLimit,           /**< "dcVoltageAboveLimit" */
    kStateInformationTypeDcVoltageBelowLimit,           /**< "dcVoltageBelowLimit" */
    kStateInformationTypeHardwareTestFailure,           /**< "hardwareTestFailure" */
    kStateInformationTypeGenericInternalError,          /**< "genericInternalError" */
};

typedef int32_t StateInformationFunctionalityType;

enum {
    kStateInformationFunctionalityTypeExternalOverrideFromGrid, /**< "externalOverrideFromGrid" */
    kStateInformationFunctionalityTypeAutonomousGridSupport,    /**< "autonomousGridSupport" */
    kStateInformationFunctionalityTypeIslandingMode,            /**< "islandingMode" */
    kStateInformationFunctionalityTypeBalancing,                /**< "balancing" */
    kStateInformationFunctionalityTypeTrickleCharging,          /**< "trickleCharging" */
    kStateInformationFunctionalityTypeCalibration,              /**< "calibration" */
    kStateInformationFunctionalityTypeCommissioningMissing,     /**< "commissioningMissing" */
    kStateInformationFunctionalityTypeSleeping,                 /**< "sleeping" */
    kStateInformationFunctionalityTypeStarting,                 /**< "starting" */
    kStateInformationFunctionalityTypeMppt,                     /**< "mppt" */
    kStateInformationFunctionalityTypeThrottled,                /**< "throttled" */
    kStateInformationFunctionalityTypeShuttingDown,             /**< "shuttingDown" */
    kStateInformationFunctionalityTypeManualShutdown,           /**< "manualShutdown" */
};

typedef int32_t StateInformationFailureType;

enum {
    kStateInformationFailureTypeInverterDefective,             /**< "inverterDefective" */
    kStateInformationFailureTypeBatteryOvercurrentProtection,  /**< "batteryOvercurrentProtection" */
    kStateInformationFailureTypePvStringOvercurrentProtection, /**< "pvStringOvercurrentProtection" */
    kStateInformationFailureTypeGridFault,                     /**< "gridFault" */
    kStateInformationFailureTypeGroundFault,                   /**< "groundFault" */
    kStateInformationFailureTypeAcDisconnected,                /**< "acDisconnected" */
    kStateInformationFailureTypeDcDisconnected,                /**< "dcDisconnected" */
    kStateInformationFailureTypeCabinetOpen,                   /**< "cabinetOpen" */
    kStateInformationFailureTypeOverTemperature,               /**< "overTemperature" */
    kStateInformationFailureTypeUnderTemperature,              /**< "underTemperature" */
    kStateInformationFailureTypeFrequencyAboveLimit,           /**< "frequencyAboveLimit" */
    kStateInformationFailureTypeFrequencyBelowLimit,           /**< "frequencyBelowLimit" */
    kStateInformationFailureTypeAcVoltageAboveLimit,           /**< "acVoltageAboveLimit" */
    kStateInformationFailureTypeAcVoltageBelowLimit,           /**< "acVoltageBelowLimit" */
    kStateInformationFailureTypeDcVoltageAboveLimit,           /**< "dcVoltageAboveLimit" */
    kStateInformationFailureTypeDcVoltageBelowLimit,           /**< "dcVoltageBelowLimit" */
    kStateInformationFailureTypeHardwareTestFailure,           /**< "hardwareTestFailure" */
    kStateInformationFailureTypeGenericInternalError,          /**< "genericInternalError" */
};

typedef int32_t StateInformationCategoryType;

enum {
    kStateInformationCategoryTypeFunctionality, /**< "functionality" */
    kStateInformationCategoryTypeFailure,       /**< "failure" */
};

typedef struct StateInformationDataType StateInformationDataType;

struct StateInformationDataType {
    const StateInformationIdType* state_information_id;    /**< "stateInformationId" */
    const StateInformationType* state_information;         /**< "stateInformation" */
    const bool* is_active;                                 /**< "isActive" */
    const StateInformationCategoryType* category;          /**< "category" */
    const AbsoluteOrRelativeTimeType* time_of_last_change; /**< "timeOfLastChange" */
};

typedef struct StateInformationDataElementsType StateInformationDataElementsType;

struct StateInformationDataElementsType {
    ElementTagType state_information_id; /**< "stateInformationId" */
    ElementTagType state_information;    /**< "stateInformation" */
    ElementTagType is_active;            /**< "isActive" */
    ElementTagType category;             /**< "category" */
    ElementTagType time_of_last_change;  /**< "timeOfLastChange" */
};

typedef struct StateInformationListDataType StateInformationListDataType;

struct StateInformationListDataType {
    const StateInformationDataType* const* state_information_data; /**< "stateInformationData" */
    size_t state_information_data_size;
};

typedef struct StateInformationListDataSelectorsType StateInformationListDataSelectorsType;

struct StateInformationListDataSelectorsType {
    const StateInformationIdType* state_information_id; /**< "stateInformationId" */
    const StateInformationType* state_information;      /**< "stateInformation" */
    const bool* is_active;                              /**< "isActive" */
    const StateInformationCategoryType* category;       /**< "category" */
};

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SPINE_MODEL_STATE_INFORMATION_TYPES_H_
