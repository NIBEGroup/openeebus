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
 * @brief SPINE Datagram Timetable data types declarations
 */

#ifndef SRC_SPINE_MODEL_TIMETABLE_TYPES_H_
#define SRC_SPINE_MODEL_TIMETABLE_TYPES_H_

#include <stdbool.h>
#include <stdint.h>

#include "src/spine/model/common_data_types.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef uint32_t TimeTableIdType;

typedef uint32_t TimeSlotIdType;

typedef TimeSlotIdType TimeSlotCountType;

typedef int32_t TimeSlotTimeModeType;

enum {
    kTimeSlotTimeModeTypeAbsolute,  /**< "absolute" */
    kTimeSlotTimeModeTypeRecurring, /**< "recurring" */
    kTimeSlotTimeModeTypeBoth,      /**< "both" */
};

typedef struct TimeTableDataType TimeTableDataType;

struct TimeTableDataType {
    const TimeTableIdType* time_table_id;                    /**< "timeTableId" */
    const TimeSlotIdType* time_slot_id;                      /**< "timeSlotId" */
    const RecurrenceInformationType* recurrence_information; /**< "recurrenceInformation" */
    const AbsoluteOrRecurringTimeType* start_time;           /**< "startTime" */
    const AbsoluteOrRecurringTimeType* end_time;             /**< "endTime" */
};

typedef struct TimeTableDataElementsType TimeTableDataElementsType;

struct TimeTableDataElementsType {
    ElementTagType time_table_id;                                    /**< "timeTableId" */
    ElementTagType time_slot_id;                                     /**< "timeSlotId" */
    const RecurrenceInformationElementsType* recurrence_information; /**< "recurrenceInformation" */
    const AbsoluteOrRecurringTimeElementsType* start_time;           /**< "startTime" */
    const AbsoluteOrRecurringTimeElementsType* end_time;             /**< "endTime" */
};

typedef struct TimeTableListDataType TimeTableListDataType;

struct TimeTableListDataType {
    const TimeTableDataType* const* time_table_data; /**< "timeTableData" */
    size_t time_table_data_size;
};

typedef struct TimeTableListDataSelectorsType TimeTableListDataSelectorsType;

struct TimeTableListDataSelectorsType {
    const TimeTableIdType* time_table_id; /**< "timeTableId" */
    const TimeSlotIdType* time_slot_id;   /**< "timeSlotId" */
};

typedef struct TimeTableConstraintsDataType TimeTableConstraintsDataType;

struct TimeTableConstraintsDataType {
    const TimeTableIdType* time_table_id;        /**< "timeTableId" */
    const TimeSlotCountType* slot_count_min;     /**< "slotCountMin" */
    const TimeSlotCountType* slot_count_max;     /**< "slotCountMax" */
    const DurationType* slot_duration_min;       /**< "slotDurationMin" */
    const DurationType* slot_duration_max;       /**< "slotDurationMax" */
    const DurationType* slot_duration_step_size; /**< "slotDurationStepSize" */
    const DurationType* slot_shift_step_size;    /**< "slotShiftStepSize" */
    const TimeType* first_slot_begins_at;        /**< "firstSlotBeginsAt" */
};

typedef struct TimeTableConstraintsDataElementsType TimeTableConstraintsDataElementsType;

struct TimeTableConstraintsDataElementsType {
    ElementTagType time_table_id;           /**< "timeTableId" */
    ElementTagType slot_count_min;          /**< "slotCountMin" */
    ElementTagType slot_count_max;          /**< "slotCountMax" */
    ElementTagType slot_duration_min;       /**< "slotDurationMin" */
    ElementTagType slot_duration_max;       /**< "slotDurationMax" */
    ElementTagType slot_duration_step_size; /**< "slotDurationStepSize" */
    ElementTagType slot_shift_step_size;    /**< "slotShiftStepSize" */
    ElementTagType first_slot_begins_at;    /**< "firstSlotBeginsAt" */
};

typedef struct TimeTableConstraintsListDataType TimeTableConstraintsListDataType;

struct TimeTableConstraintsListDataType {
    const TimeTableConstraintsDataType* const* time_table_constraints_data; /**< "timeTableConstraintsData" */
    size_t time_table_constraints_data_size;
};

typedef struct TimeTableConstraintsListDataSelectorsType TimeTableConstraintsListDataSelectorsType;

struct TimeTableConstraintsListDataSelectorsType {
    const TimeTableIdType* time_table_id; /**< "timeTableId" */
};

typedef struct TimeTableDescriptionDataType TimeTableDescriptionDataType;

struct TimeTableDescriptionDataType {
    const TimeTableIdType* time_table_id;            /**< "timeTableId" */
    const bool* time_slot_count_changeable;          /**< "timeSlotCountChangeable" */
    const bool* time_slot_times_changeable;          /**< "timeSlotTimesChangeable" */
    const TimeSlotTimeModeType* time_slot_time_mode; /**< "timeSlotTimeMode" */
    LabelType label;                                 /**< "label" */
    DescriptionType description;                     /**< "description" */
};

typedef struct TimeTableDescriptionDataElementsType TimeTableDescriptionDataElementsType;

struct TimeTableDescriptionDataElementsType {
    ElementTagType time_table_id;              /**< "timeTableId" */
    ElementTagType time_slot_count_changeable; /**< "timeSlotCountChangeable" */
    ElementTagType time_slot_times_changeable; /**< "timeSlotTimesChangeable" */
    ElementTagType time_slot_time_mode;        /**< "timeSlotTimeMode" */
    ElementTagType label;                      /**< "label" */
    ElementTagType description;                /**< "description" */
};

typedef struct TimeTableDescriptionListDataType TimeTableDescriptionListDataType;

struct TimeTableDescriptionListDataType {
    const TimeTableDescriptionDataType* const* time_table_description_data; /**< "timeTableDescriptionData" */
    size_t time_table_description_data_size;
};

typedef struct TimeTableDescriptionListDataSelectorsType TimeTableDescriptionListDataSelectorsType;

struct TimeTableDescriptionListDataSelectorsType {
    const TimeTableIdType* time_table_id; /**< "timeTableId" */
};

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SPINE_MODEL_TIMETABLE_TYPES_H_
