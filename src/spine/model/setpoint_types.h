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
 * @brief SPINE Datagram Setpoint data types declarations
 */

#ifndef SRC_SPINE_MODEL_SETPOINT_TYPES_H_
#define SRC_SPINE_MODEL_SETPOINT_TYPES_H_

#include <stdbool.h>
#include <stdint.h>

#include "src/spine/model/common_data_types.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef uint32_t SetpointIdType;

typedef int32_t SetpointTypeType;

enum {
    kSetpointTypeTypeValueAbsolute, /**< "valueAbsolute" */
    kSetpointTypeTypeValueRelative, /**< "valueRelative" */
};

typedef struct SetpointDataType SetpointDataType;

struct SetpointDataType {
    const SetpointIdType* setpoint_id;                  /**< "setpointId" */
    const ScaledNumberType* value;                      /**< "value" */
    const ScaledNumberType* value_min;                  /**< "valueMin" */
    const ScaledNumberType* value_max;                  /**< "valueMax" */
    const ScaledNumberType* value_tolerance_absolute;   /**< "valueToleranceAbsolute" */
    const ScaledNumberType* value_tolerance_percentage; /**< "valueTolerancePercentage" */
    const bool* is_setpoint_changeable;                 /**< "isSetpointChangeable" */
    const bool* is_setpoint_active;                     /**< "isSetpointActive" */
    const TimePeriodType* time_period;                  /**< "timePeriod" */
};

typedef struct SetpointDataElementsType SetpointDataElementsType;

struct SetpointDataElementsType {
    ElementTagType setpoint_id;                                 /**< "setpointId" */
    const ScaledNumberElementsType* value;                      /**< "value" */
    const ScaledNumberElementsType* value_min;                  /**< "valueMin" */
    const ScaledNumberElementsType* value_max;                  /**< "valueMax" */
    const ScaledNumberElementsType* value_tolerance_absolute;   /**< "valueToleranceAbsolute" */
    const ScaledNumberElementsType* value_tolerance_percentage; /**< "valueTolerancePercentage" */
    ElementTagType is_setpoint_changeable;                      /**< "isSetpointChangeable" */
    ElementTagType is_setpoint_active;                          /**< "isSetpointActive" */
    const TimePeriodElementsType* time_period;                  /**< "timePeriod" */
};

typedef struct SetpointListDataType SetpointListDataType;

struct SetpointListDataType {
    const SetpointDataType* const* setpoint_data; /**< "setpointData" */
    size_t setpoint_data_size;
};

typedef struct SetpointListDataSelectorsType SetpointListDataSelectorsType;

struct SetpointListDataSelectorsType {
    const SetpointIdType* setpoint_id; /**< "setpointId" */
};

typedef struct SetpointConstraintsDataType SetpointConstraintsDataType;

struct SetpointConstraintsDataType {
    const SetpointIdType* setpoint_id;          /**< "setpointId" */
    const ScaledNumberType* setpoint_range_min; /**< "setpointRangeMin" */
    const ScaledNumberType* setpoint_range_max; /**< "setpointRangeMax" */
    const ScaledNumberType* setpoint_step_size; /**< "setpointStepSize" */
};

typedef struct SetpointConstraintsDataElementsType SetpointConstraintsDataElementsType;

struct SetpointConstraintsDataElementsType {
    ElementTagType setpoint_id;                         /**< "setpointId" */
    const ScaledNumberElementsType* setpoint_range_min; /**< "setpointRangeMin" */
    const ScaledNumberElementsType* setpoint_range_max; /**< "setpointRangeMax" */
    const ScaledNumberElementsType* setpoint_step_size; /**< "setpointStepSize" */
};

typedef struct SetpointConstraintsListDataType SetpointConstraintsListDataType;

struct SetpointConstraintsListDataType {
    const SetpointConstraintsDataType* const* setpoint_constraints_data; /**< "setpointConstraintsData" */
    size_t setpoint_constraints_data_size;
};

typedef struct SetpointConstraintsListDataSelectorsType SetpointConstraintsListDataSelectorsType;

struct SetpointConstraintsListDataSelectorsType {
    const SetpointIdType* setpoint_id; /**< "setpointId" */
};

typedef struct SetpointDescriptionDataType SetpointDescriptionDataType;

struct SetpointDescriptionDataType {
    const SetpointIdType* setpoint_id;     /**< "setpointId" */
    const SetpointIdType* measurement_id;  /**< "measurementId" */
    const SetpointIdType* time_table_id;   /**< "timeTableId" */
    const SetpointTypeType* setpoint_type; /**< "setpointType" */
    const ScaledNumberType* unit;          /**< "unit" */
    const ScaledNumberType* scope_type;    /**< "scopeType" */
    LabelType label;                       /**< "label" */
    DescriptionType description;           /**< "description" */
};

typedef struct SetpointDescriptionDataElementsType SetpointDescriptionDataElementsType;

struct SetpointDescriptionDataElementsType {
    ElementTagType setpoint_id;    /**< "setpointId" */
    ElementTagType measurement_id; /**< "measurementId" */
    ElementTagType time_table_id;  /**< "timeTableId" */
    ElementTagType setpoint_type;  /**< "setpointType" */
    ElementTagType unit;           /**< "unit" */
    ElementTagType scope_type;     /**< "scopeType" */
    ElementTagType label;          /**< "label" */
    ElementTagType description;    /**< "description" */
};

typedef struct SetpointDescriptionListDataType SetpointDescriptionListDataType;

struct SetpointDescriptionListDataType {
    const SetpointDescriptionDataType* const* setpoint_description_data; /**< "setpointDescriptionData" */
    size_t setpoint_description_data_size;
};

typedef struct SetpointDescriptionListDataSelectorsType SetpointDescriptionListDataSelectorsType;

struct SetpointDescriptionListDataSelectorsType {
    const SetpointIdType* setpoint_id;    /**< "setpointId" */
    const SetpointIdType* measurement_id; /**< "measurementId" */
    const SetpointIdType* time_table_id;  /**< "timeTableId" */
    const SetpointIdType* setpoint_type;  /**< "setpointType" */
    const ScaledNumberType* scope_type;   /**< "scopeType" */
};

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SPINE_MODEL_SETPOINT_TYPES_H_
