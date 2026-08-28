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
 * @brief SPINE Datagram Direct Control data types declarations
 */

#ifndef SRC_SPINE_MODEL_DIRECT_CONTROL_TYPES_H_
#define SRC_SPINE_MODEL_DIRECT_CONTROL_TYPES_H_

#include <stdbool.h>
#include <stdint.h>

#include "src/spine/model/common_data_types.h"
#include "src/spine/model/measurement_types.h"
#include "src/spine/model/power_sequences_types.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef const char* DirectControlActivityStateType;

typedef struct DirectControlActivityDataType DirectControlActivityDataType;

struct DirectControlActivityDataType {
    const AbsoluteOrRelativeTimeType* timestamp;   /**< "timestamp" */
    DirectControlActivityStateType activity_state; /**< "activityState" */
    const bool* is_activity_state_changeable;      /**< "isActivityStateChangeable" */
    const EnergyModeType* energy_mode;             /**< "energyMode" */
    const bool* is_energy_mode_changeable;         /**< "isEnergyModeChangeable" */
    const ScaledNumberType* power;                 /**< "power" */
    const bool* is_power_changeable;               /**< "isPowerChangeable" */
    const ScaledNumberType* energy;                /**< "energy" */
    const bool* is_energy_changeable;              /**< "isEnergyChangeable" */
    const PowerSequenceIdType* sequence_id;        /**< "sequence_id" */
};

typedef struct DirectControlActivityDataElementsType DirectControlActivityDataElementsType;

struct DirectControlActivityDataElementsType {
    ElementTagType timestamp;                    /**< "timestamp" */
    ElementTagType activity_state;               /**< "activityState" */
    ElementTagType is_activity_state_changeable; /**< "isActivityStateChangeable" */
    ElementTagType energy_mode;                  /**< "energyMode" */
    ElementTagType is_energy_mode_changeable;    /**< "isEnergyModeChangeable" */
    const ScaledNumberElementsType* power;       /**< "power" */
    ElementTagType is_power_changeable;          /**< "isPowerChangeable" */
    const ScaledNumberElementsType* energy;      /**< "energy" */
    ElementTagType is_energy_changeable;         /**< "isEnergyChangeable" */
    ElementTagType sequence_id;                  /**< "sequence_id" */
};

typedef struct DirectControlActivityListDataType DirectControlActivityListDataType;

struct DirectControlActivityListDataType {
    const DirectControlActivityDataType* const*
        direct_control_activity_data_elements; /**< "directControlActivityDataElements" */
    size_t direct_control_activity_data_elements_size;
};

typedef struct DirectControlActivityListDataSelectorsType DirectControlActivityListDataSelectorsType;

struct DirectControlActivityListDataSelectorsType {
    const TimestampIntervalType* timestamp_interval; /**< "timestampInterval" */
};

typedef struct DirectControlDescriptionDataType DirectControlDescriptionDataType;

struct DirectControlDescriptionDataType {
    const EnergyDirectionType* positive_energy_direction; /**< "positiveEnergyDirection" */
    const UnitOfMeasurementType* power_unit;              /**< "powerUnit" */
    const UnitOfMeasurementType* energy_unit;             /**< "energyUnit" */
};

typedef struct DirectControlDescriptionDataElementsType DirectControlDescriptionDataElementsType;

struct DirectControlDescriptionDataElementsType {
    ElementTagType positive_energy_direction; /**< "positiveEnergyDirection" */
    ElementTagType power_unit;                /**< "powerUnit" */
    ElementTagType energy_unit;               /**< "energyUnit" */
};

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SPINE_MODEL_DIRECT_CONTROL_TYPES_H_
