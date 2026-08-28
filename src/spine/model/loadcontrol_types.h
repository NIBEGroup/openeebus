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
 * @brief SPINE Datagram Loadcontrol data types declarations
 */

#ifndef SRC_SPINE_MODEL_LOADCONTROL_TYPES_H_
#define SRC_SPINE_MODEL_LOADCONTROL_TYPES_H_

#include <stdbool.h>
#include <stdint.h>

#include "src/spine/model/common_data_types.h"
#include "src/spine/model/measurement_types.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef uint32_t LoadControlEventIdType;

typedef int32_t LoadControlEventActionType;

enum {
    kLoadControlEventActionTypePause,     /**< "pause" */
    kLoadControlEventActionTypeResume,    /**< "resume" */
    kLoadControlEventActionTypeReduce,    /**< "reduce" */
    kLoadControlEventActionTypeIncrease,  /**< "increase" */
    kLoadControlEventActionTypeEmergency, /**< "emergency" */
    kLoadControlEventActionTypeNormal,    /**< "normal" */
};

typedef int32_t LoadControlEventStateType;

enum {
    kLoadControlEventStateTypeEventAccepted,  /**< "eventAccepted" */
    kLoadControlEventStateTypeEventStarted,   /**< "eventStarted" */
    kLoadControlEventStateTypeEventStopped,   /**< "eventStopped" */
    kLoadControlEventStateTypeEventRejected,  /**< "eventRejected" */
    kLoadControlEventStateTypeEventCancelled, /**< "eventCancelled" */
    kLoadControlEventStateTypeEventError,     /**< "eventError" */
};

typedef uint32_t LoadControlLimitIdType;

typedef int32_t LoadControlLimitTypeType;

enum {
    kLoadControlLimitTypeTypeMinValueLimit,              /**< "minValueLimit" */
    kLoadControlLimitTypeTypeMaxValueLimit,              /**< "maxValueLimit" */
    kLoadControlLimitTypeTypeSignDependentAbsValueLimit, /**< "signDependentAbsValueLimit" */
};

typedef int32_t LoadControlCategoryType;

enum {
    kLoadControlCategoryTypeObligation,     /**< "obligation" */
    kLoadControlCategoryTypeRecommendation, /**< "recommendation" */
    kLoadControlCategoryTypeOptimization,   /**< "optimization" */
};

typedef struct LoadControlNodeDataType LoadControlNodeDataType;

struct LoadControlNodeDataType {
    const bool* is_node_remote_controllable; /**< "isNodeRemoteControllable" */
};

typedef struct LoadControlNodeDataElementsType LoadControlNodeDataElementsType;

struct LoadControlNodeDataElementsType {
    ElementTagType is_node_remote_controllable; /**< "isNodeRemoteControllable" */
};

typedef struct LoadControlEventDataType LoadControlEventDataType;

struct LoadControlEventDataType {
    const AbsoluteOrRelativeTimeType* timestamp;            /**< "timestamp" */
    const LoadControlEventIdType* event_id;                 /**< "eventId" */
    const LoadControlEventActionType* event_action_consume; /**< "eventActionConsume" */
    const LoadControlEventActionType* event_action_produce; /**< "eventActionProduce" */
    const TimePeriodType* time_period;                      /**< "timePeriod" */
};

typedef struct LoadControlEventDataElementsType LoadControlEventDataElementsType;

struct LoadControlEventDataElementsType {
    ElementTagType timestamp;                  /**< "timestamp" */
    ElementTagType event_id;                   /**< "eventId" */
    ElementTagType event_action_consume;       /**< "eventActionConsume" */
    ElementTagType event_action_produce;       /**< "eventActionProduce" */
    const TimePeriodElementsType* time_period; /**< "timePeriod" */
};

typedef struct LoadControlEventListDataType LoadControlEventListDataType;

struct LoadControlEventListDataType {
    const LoadControlEventDataType* const* load_control_event_data; /**< "loadControlEventData" */
    size_t load_control_event_data_size;
};

typedef struct LoadControlEventListDataSelectorsType LoadControlEventListDataSelectorsType;

struct LoadControlEventListDataSelectorsType {
    const TimestampIntervalType* timestamp_interval; /**< "timestampInterval" */
    const LoadControlEventIdType* event_id;          /**< "eventId" */
};

typedef struct LoadControlStateDataType LoadControlStateDataType;

struct LoadControlStateDataType {
    const AbsoluteOrRelativeTimeType* timestamp;                    /**< "timestamp" */
    const LoadControlEventIdType* event_id;                         /**< "eventId" */
    const LoadControlEventStateType* event_state_consume;           /**< "eventStateConsume" */
    const LoadControlEventActionType* applied_event_action_consume; /**< "appliedEventActionConsume" */
    const LoadControlEventStateType* event_state_produce;           /**< "eventStateProduce" */
    const LoadControlEventActionType* applied_event_action_produce; /**< "appliedEventActionProduce" */
};

typedef struct LoadControlStateDataElementsType LoadControlStateDataElementsType;

struct LoadControlStateDataElementsType {
    ElementTagType timestamp;                    /**< "timestamp" */
    ElementTagType event_id;                     /**< "eventId" */
    ElementTagType event_state_consume;          /**< "eventStateConsume" */
    ElementTagType applied_event_action_consume; /**< "appliedEventActionConsume" */
    ElementTagType event_state_produce;          /**< "eventStateProduce" */
    ElementTagType applied_event_action_produce; /**< "appliedEventActionProduce" */
};

typedef struct LoadControlStateListDataType LoadControlStateListDataType;

struct LoadControlStateListDataType {
    const LoadControlStateDataType* const* load_control_state_data; /**< "loadControlStateData" */
    size_t load_control_state_data_size;
};

typedef struct LoadControlStateListDataSelectorsType LoadControlStateListDataSelectorsType;

struct LoadControlStateListDataSelectorsType {
    const TimestampIntervalType* timestamp_interval; /**< "timestampInterval" */
    const LoadControlEventIdType* event_id;          /**< "eventId" */
};

typedef struct LoadControlLimitDataType LoadControlLimitDataType;

struct LoadControlLimitDataType {
    const LoadControlLimitIdType* limit_id; /**< "limitId" */
    const bool* is_limit_changeable;        /**< "isLimitChangeable" */
    const bool* is_limit_active;            /**< "isLimitActive" */
    const TimePeriodType* time_period;      /**< "timePeriod" */
    const ScaledNumberType* value;          /**< "value" */
};

typedef struct LoadControlLimitDataElementsType LoadControlLimitDataElementsType;

struct LoadControlLimitDataElementsType {
    ElementTagType limit_id;                   /**< "limitId" */
    ElementTagType is_limit_changeable;        /**< "isLimitChangeable" */
    ElementTagType is_limit_active;            /**< "isLimitActive" */
    const TimePeriodElementsType* time_period; /**< "timePeriod" */
    const ScaledNumberElementsType* value;     /**< "value" */
};

typedef struct LoadControlLimitListDataType LoadControlLimitListDataType;

struct LoadControlLimitListDataType {
    const LoadControlLimitDataType* const* load_control_limit_data; /**< "loadControlLimitData" */
    size_t load_control_limit_data_size;
};

typedef struct LoadControlLimitListDataSelectorsType LoadControlLimitListDataSelectorsType;

struct LoadControlLimitListDataSelectorsType {
    const LoadControlLimitIdType* limit_id; /**< "limitId" */
};

typedef struct LoadControlLimitConstraintsDataType LoadControlLimitConstraintsDataType;

struct LoadControlLimitConstraintsDataType {
    const LoadControlLimitIdType* limit_id;  /**< "limitId" */
    const ScaledNumberType* value_range_min; /**< "valueRangeMin" */
    const ScaledNumberType* value_range_max; /**< "valueRangeMax" */
    const ScaledNumberType* value_step_size; /**< "valueStepSize" */
};

typedef struct LoadControlLimitConstraintsDataElementsType LoadControlLimitConstraintsDataElementsType;

struct LoadControlLimitConstraintsDataElementsType {
    ElementTagType limit_id;                         /**< "limitId" */
    const ScaledNumberElementsType* value_range_min; /**< "valueRangeMin" */
    const ScaledNumberElementsType* value_range_max; /**< "valueRangeMax" */
    const ScaledNumberElementsType* value_step_size; /**< "valueStepSize" */
};

typedef struct LoadControlLimitConstraintsListDataType LoadControlLimitConstraintsListDataType;

struct LoadControlLimitConstraintsListDataType {
    const LoadControlLimitConstraintsDataType* const*
        load_control_limit_constraints_data; /**< "loadControlLimitConstraintsData" */
    size_t load_control_limit_constraints_data_size;
};

typedef struct LoadControlLimitConstraintsListDataSelectorsType LoadControlLimitConstraintsListDataSelectorsType;

struct LoadControlLimitConstraintsListDataSelectorsType {
    const LoadControlLimitIdType* limit_id; /**< "limitId" */
};

typedef struct LoadControlLimitDescriptionDataType LoadControlLimitDescriptionDataType;

struct LoadControlLimitDescriptionDataType {
    const LoadControlLimitIdType* limit_id;        /**< "limitId" */
    const LoadControlLimitTypeType* limit_type;    /**< "limitType" */
    const LoadControlCategoryType* limit_category; /**< "limitCategory" */
    const EnergyDirectionType* limit_direction;    /**< "limitDirection" */
    const MeasurementIdType* measurement_id;       /**< "measurementId" */
    const UnitOfMeasurementType* unit;             /**< "unit" */
    const ScopeTypeType* scope_type;               /**< "scopeType" */
    LabelType label;                               /**< "label" */
    DescriptionType description;                   /**< "description" */
};

typedef struct LoadControlLimitDescriptionDataElementsType LoadControlLimitDescriptionDataElementsType;

struct LoadControlLimitDescriptionDataElementsType {
    ElementTagType limit_id;        /**< "limitId" */
    ElementTagType limit_type;      /**< "limitType" */
    ElementTagType limit_category;  /**< "limitCategory" */
    ElementTagType limit_direction; /**< "limitDirection" */
    ElementTagType measurement_id;  /**< "measurementId" */
    ElementTagType unit;            /**< "unit" */
    ElementTagType scope_type;      /**< "scopeType" */
    ElementTagType label;           /**< "label" */
    ElementTagType description;     /**< "description" */
};

typedef struct LoadControlLimitDescriptionListDataType LoadControlLimitDescriptionListDataType;

struct LoadControlLimitDescriptionListDataType {
    const LoadControlLimitDescriptionDataType* const*
        load_control_limit_description_data; /**< "loadControlLimitDescriptionData" */
    size_t load_control_limit_description_data_size;
};

typedef struct LoadControlLimitDescriptionListDataSelectorsType LoadControlLimitDescriptionListDataSelectorsType;

struct LoadControlLimitDescriptionListDataSelectorsType {
    const LoadControlLimitIdType* limit_id;     /**< "limitId" */
    const LoadControlLimitTypeType* limit_type; /**< "limitType" */
    const EnergyDirectionType* limit_direction; /**< "limitDirection" */
    const MeasurementIdType* measurement_id;    /**< "measurementId" */
    const ScopeTypeType* scope_type;            /**< "scopeType" */
};

bool LoadControlLimitIsValid(const LoadControlLimitDataType* limit);
bool LoadControlLimitIsLimitChangeable(const LoadControlLimitDataType* limit);
bool LoadControlLimitIsActive(const LoadControlLimitDataType* limit);
EebusError LoadControlLimitGetDuration(const LoadControlLimitDataType* limit, DurationType* duration, bool* is_null);
const ScaledNumberType* LoadControlLimitGetValue(const LoadControlLimitDataType* limit);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SPINE_MODEL_LOADCONTROL_TYPES_H_
