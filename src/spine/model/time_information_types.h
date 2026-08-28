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
 * @brief SPINE Datagram Time Information data types declarations
 */

#ifndef SRC_SPINE_MODEL_TIME_INFORMATION_TYPES_H_
#define SRC_SPINE_MODEL_TIME_INFORMATION_TYPES_H_

#include <stdbool.h>
#include <stdint.h>

#include "src/spine/model/common_data_types.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct TimeInformationDataType TimeInformationDataType;

struct TimeInformationDataType {
    const DateTimeType* utc;               /**< "utc" */
    const DurationType* utc_offset;        /**< "utcOffset" */
    const DayOfWeekType* day_of_week;      /**< "dayOfWeek" */
    const CalendarWeekType* calendar_week; /**< "calendarWeek" */
};

typedef struct TimeInformationDataElementsType TimeInformationDataElementsType;

struct TimeInformationDataElementsType {
    ElementTagType utc;           /**< "utc" */
    ElementTagType utc_offset;    /**< "utcOffset" */
    ElementTagType day_of_week;   /**< "dayOfWeek" */
    ElementTagType calendar_week; /**< "calendarWeek" */
};

typedef struct TimeDistributorDataType TimeDistributorDataType;

struct TimeDistributorDataType {
    const bool* is_time_distributor;      /**< "isTimeDistributor" */
    const uint32_t* distributor_priority; /**< "distributorPriority" */
};

typedef struct TimeDistributorDataElementsType TimeDistributorDataElementsType;

struct TimeDistributorDataElementsType {
    ElementTagType is_time_distributor;  /**< "isTimeDistributor" */
    ElementTagType distributor_priority; /**< "distributorPriority" */
};

typedef struct TimePrecisionDataType TimePrecisionDataType;

struct TimePrecisionDataType {
    const bool* is_synchronised;      /**< "isSynchronised" */
    const DateTimeType* last_sync_at; /**< "lastSyncAt" */
    const int32_t* clock_drift;       /**< "clockDrift" */
};

typedef struct TimePrecisionDataElementsType TimePrecisionDataElementsType;

struct TimePrecisionDataElementsType {
    ElementTagType is_synchronised; /**< "isSynchronised" */
    ElementTagType last_sync_at;    /**< "lastSyncAt" */
    ElementTagType clock_drift;     /**< "clockDrift" */
};

typedef TagType TimeDistributorEnquiryCallType;

typedef TagType TimeDistributorEnquiryCallElementsType;

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SPINE_MODEL_TIME_INFORMATION_TYPES_H_
