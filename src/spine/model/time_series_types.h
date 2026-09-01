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
 * @brief SPINE Datagram Time Series data types declarations
 */

#ifndef SRC_SPINE_MODEL_TIME_SERIES_TYPES_H_
#define SRC_SPINE_MODEL_TIME_SERIES_TYPES_H_

#include <stdbool.h>
#include <stdint.h>

#include "src/spine/model/common_data_types.h"
#include "src/spine/model/measurement_types.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef uint32_t TimeSeriesIdType;

typedef uint32_t TimeSeriesSlotIdType;

typedef TimeSeriesSlotIdType TimeSeriesSlotCountType;

typedef int32_t TimeSeriesTypeType;

enum {
    kTimeSeriesTypeTypePlan,                     /**< "plan" */
    kTimeSeriesTypeTypeSingleDemand,             /**< "singleDemand" */
    kTimeSeriesTypeTypeConstraints,              /**< "constraints" */
    kTimeSeriesTypeTypeEnergyRequest,            /**< "energyRequest" */
    kTimeSeriesTypeTypeDischargingEnergyRequest, /**< "dischargingEnergyRequest" */
    kTimeSeriesTypeTypeConsumptionLimitCurve,    /**< "consumptionLimitCurve" */
    kTimeSeriesTypeTypeProductionLimitCurve,     /**< "productionLimitCurve" */
};

typedef struct TimeSeriesSlotType TimeSeriesSlotType;

struct TimeSeriesSlotType {
    const TimeSeriesSlotIdType* time_series_slot_id;           /**< "timeSeriesSlotId" */
    const TimePeriodType* time_period;                         /**< "timePeriod" */
    const DurationType* duration;                              /**< "duration" */
    const AbsoluteOrRecurringTimeType* recurrence_information; /**< "recurrenceInformation" */
    const ScaledNumberType* value;                             /**< "value" */
    const ScaledNumberType* min_value;                         /**< "minValue" */
    const ScaledNumberType* max_value;                         /**< "maxValue" */
};

typedef struct TimeSeriesSlotElementsType TimeSeriesSlotElementsType;

struct TimeSeriesSlotElementsType {
    ElementTagType time_series_slot_id;                                /**< "timeSeriesSlotId" */
    ElementTagType time_period;                                        /**< "timePeriod" */
    ElementTagType duration;                                           /**< "duration" */
    const AbsoluteOrRecurringTimeElementsType* recurrence_information; /**< "recurrenceInformation" */
    const ScaledNumberElementsType* value;                             /**< "value" */
    const ScaledNumberElementsType* min_value;                         /**< "minValue" */
    const ScaledNumberElementsType* max_value;                         /**< "maxValue" */
};

typedef struct TimeSeriesDataType TimeSeriesDataType;

struct TimeSeriesDataType {
    const TimeSeriesIdType* time_series_id;            /**< "timeSeriesId" */
    const TimePeriodType* time_period;                 /**< "timePeriod" */
    const TimeSeriesSlotType* const* time_series_slot; /**< "timeSeriesSlot" */
    size_t time_series_slot_size;
};

typedef struct TimeSeriesDataElementsType TimeSeriesDataElementsType;

struct TimeSeriesDataElementsType {
    ElementTagType time_series_id;                      /**< "timeSeriesId" */
    const TimePeriodElementsType* time_period;          /**< "timePeriod" */
    const TimeSeriesSlotElementsType* time_series_slot; /**< "timeSeriesSlot" */
};

typedef struct TimeSeriesListDataType TimeSeriesListDataType;

struct TimeSeriesListDataType {
    const TimeSeriesDataType* const* time_series_data; /**< "timeSeriesData" */
    size_t time_series_data_size;
};

typedef struct TimeSeriesListDataSelectorsType TimeSeriesListDataSelectorsType;

struct TimeSeriesListDataSelectorsType {
    const TimeSeriesIdType* time_series_id;          /**< "timeSeriesId" */
    const TimeSeriesSlotIdType* time_series_slot_id; /**< "timeSeriesSlotId" */
};

typedef struct TimeSeriesDescriptionDataType TimeSeriesDescriptionDataType;

struct TimeSeriesDescriptionDataType {
    const TimeSeriesIdType* time_series_id;     /**< "timeSeriesId" */
    const TimeSeriesTypeType* time_series_type; /**< "timeSeriesType" */
    const bool* time_series_writeable;          /**< "timeSeriesWriteable" */
    const bool* update_required;                /**< "updateRequired" */
    const MeasurementIdType* measurement_id;    /**< "measurementId" */
    const CurrencyType* currency;               /**< "currency" */
    const UnitOfMeasurementType* unit;          /**< "unit" */
    LabelType label;                            /**< "label" */
    DescriptionType description;                /**< "description" */
    ScopeTypeType* scope_type;                  /**< "scopeType" */
};

typedef struct TimeSeriesDescriptionDataElementsType TimeSeriesDescriptionDataElementsType;

struct TimeSeriesDescriptionDataElementsType {
    ElementTagType time_series_id;        /**< "timeSeriesId" */
    ElementTagType time_series_type;      /**< "timeSeriesType" */
    ElementTagType time_series_writeable; /**< "timeSeriesWriteable" */
    ElementTagType update_required;       /**< "updateRequired" */
    ElementTagType measurement_id;        /**< "measurementId" */
    ElementTagType currency;              /**< "currency" */
    ElementTagType unit;                  /**< "unit" */
    ElementTagType label;                 /**< "label" */
    ElementTagType description;           /**< "description" */
    ElementTagType scope_type;            /**< "scopeType" */
};

typedef struct TimeSeriesDescriptionListDataType TimeSeriesDescriptionListDataType;

struct TimeSeriesDescriptionListDataType {
    const TimeSeriesDescriptionDataType* const* time_series_description_data; /**< "timeSeriesDescriptionData" */
    size_t time_series_description_data_size;
};

typedef struct TimeSeriesDescriptionListDataSelectorsType TimeSeriesDescriptionListDataSelectorsType;

struct TimeSeriesDescriptionListDataSelectorsType {
    const TimeSeriesIdType* time_series_id;     /**< "timeSeriesId" */
    const TimeSeriesTypeType* time_series_type; /**< "timeSeriesType" */
    const MeasurementIdType* measurement_id;    /**< "measurementId" */
    const ScopeTypeType* scope_type;            /**< "scopeType" */
};

typedef struct TimeSeriesConstraintsDataType TimeSeriesConstraintsDataType;

struct TimeSeriesConstraintsDataType {
    const TimeSeriesIdType* time_series_id;                            /**< "timeSeriesId" */
    const TimeSeriesSlotCountType* slot_count_min;                     /**< "slotCountMin" */
    const TimeSeriesSlotCountType* slot_count_max;                     /**< "slotCountMax" */
    const DurationType* slot_duration_min;                             /**< "slotDurationMin" */
    const DurationType* slot_duration_max;                             /**< "slotDurationMax" */
    const DurationType* slot_duration_step_size;                       /**< "slotDurationStepSize" */
    const AbsoluteOrRelativeTimeType* earliest_time_series_start_time; /**< "earliestTimeSeriesStartTime" */
    const AbsoluteOrRelativeTimeType* latest_time_series_end_time;     /**< "latestTimeSeriesEndTime" */
    const ScaledNumberType* slot_value_min;                            /**< "slotValueMin" */
    const ScaledNumberType* slot_value_max;                            /**< "slotValueMax" */
    const ScaledNumberType* slot_value_step_size;                      /**< "slotValueStepSize" */
};

typedef struct TimeSeriesConstraintsDataElementsType TimeSeriesConstraintsDataElementsType;

struct TimeSeriesConstraintsDataElementsType {
    ElementTagType time_series_id;                  /**< "timeSeriesId" */
    ElementTagType slot_count_min;                  /**< "slotCountMin" */
    ElementTagType slot_count_max;                  /**< "slotCountMax" */
    ElementTagType slot_duration_min;               /**< "slotDurationMin" */
    ElementTagType slot_duration_max;               /**< "slotDurationMax" */
    ElementTagType slot_duration_step_size;         /**< "slotDurationStepSize" */
    ElementTagType earliest_time_series_start_time; /**< "earliestTimeSeriesStartTime" */
    ElementTagType latest_time_series_end_time;     /**< "latestTimeSeriesEndTime" */
    ElementTagType slot_value_min;                  /**< "slotValueMin" */
    ElementTagType slot_value_max;                  /**< "slotValueMax" */
    ElementTagType slot_value_step_size;            /**< "slotValueStepSize" */
};

typedef struct TimeSeriesConstraintsListDataType TimeSeriesConstraintsListDataType;

struct TimeSeriesConstraintsListDataType {
    const TimeSeriesConstraintsDataType* const* time_series_constraints_data; /**< "timeSeriesConstraintsData" */
    size_t time_series_constraints_data_size;
};

typedef struct TimeSeriesConstraintsListDataSelectorsType TimeSeriesConstraintsListDataSelectorsType;

struct TimeSeriesConstraintsListDataSelectorsType {
    const TimeSeriesIdType* time_series_id; /**< "timeSeriesId" */
};

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SPINE_MODEL_TIME_SERIES_TYPES_H_
