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
 * @brief SPINE Datagram Alarm data types declarations
 */

#ifndef SRC_SPINE_MODEL_ALARM_TYPES_H_
#define SRC_SPINE_MODEL_ALARM_TYPES_H_

#include <stdbool.h>
#include <stdint.h>

#include "src/spine/model/common_data_types.h"
#include "src/spine/model/threshold_types.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef uint32_t AlarmIdType;

typedef int32_t AlarmTypeType;

enum {
    kAlarmTypeTypeAlarmCancelled, /**< "alarmCancelled" */
    kAlarmTypeTypeUnderThreshold, /**< "underThreshold" */
    kAlarmTypeTypeOverThreshold,  /**< "overThreshold" */
};

typedef struct AlarmDataType AlarmDataType;

struct AlarmDataType {
    const AlarmIdType* alarm_id;                 /**< "alarmId" */
    const ThresholdIdType* threshold_id;         /**< "thresholdId" */
    const AbsoluteOrRelativeTimeType* timestamp; /**< "timestamp" */
    const AlarmTypeType* alarm_type;             /**< "alarmType" */
    const ScaledNumberType* measured_value;      /**< "measuredValue" */
    const TimePeriodType* evaluation_period;     /**< "evaluationPeriod" */
    const ScopeTypeType* scope_type;             /**< "scopeType" */
    LabelType label;                             /**< "label" */
    DescriptionType description;                 /**< "description" */
};

typedef struct AlarmDataElementsType AlarmDataElementsType;

struct AlarmDataElementsType {
    ElementTagType alarm_id;                         /**< "alarmId" */
    ElementTagType threshold_id;                     /**< "thresholdId" */
    ElementTagType timestamp;                        /**< "timestamp" */
    ElementTagType alarm_type;                       /**< "alarmType" */
    const ScaledNumberElementsType* measured_value;  /**< "measuredValue" */
    const TimePeriodElementsType* evaluation_period; /**< "evaluationPeriod" */
    ElementTagType scope_type;                       /**< "scopeType" */
    ElementTagType label;                            /**< "label" */
    ElementTagType description;                      /**< "description" */
};

typedef struct AlarmListDataType AlarmListDataType;

struct AlarmListDataType {
    const AlarmDataType* const* alarm_list_data; /**< "alarmListData" */
    size_t alarm_list_data_size;
};

typedef struct AlarmListDataSelectorsType AlarmListDataSelectorsType;

struct AlarmListDataSelectorsType {
    const AlarmIdType* alarm_id;     /**< "alarmId" */
    const ScopeTypeType* scope_type; /**< "scopeType" */
};

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SPINE_MODEL_ALARM_TYPES_H_
