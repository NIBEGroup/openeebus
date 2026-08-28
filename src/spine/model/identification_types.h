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
 * @brief SPINE Datagram Identification data types declarations
 */

#ifndef SRC_SPINE_MODEL_IDENTIFICATION_TYPES_H_
#define SRC_SPINE_MODEL_IDENTIFICATION_TYPES_H_

#include <stdbool.h>
#include <stdint.h>

#include "src/spine/model/common_data_types.h"
#include "src/spine/model/measurement_types.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef uint32_t IdentificationIdType;

typedef int32_t IdentificationTypeType;

enum {
    kIdentificationTypeTypeEui48,       /**< "eui48" */
    kIdentificationTypeTypeEui64,       /**< "eui64" */
    kIdentificationTypeTypeUserrfidtag, /**< "userRfidTag" */
};

typedef const char* IdentificationValueType;

typedef uint32_t SessionIdType;

typedef struct IdentificationDataType IdentificationDataType;

struct IdentificationDataType {
    const IdentificationIdType* identification_id;      /**< "identificationId" */
    const IdentificationTypeType* identification_type;  /**< "identificationType" */
    const IdentificationValueType identification_value; /**< "identificationValue" */
    const bool* authorized;                             /**< "authorized" */
};

typedef struct IdentificationDataElementsType IdentificationDataElementsType;

struct IdentificationDataElementsType {
    ElementTagType identification_id;    /**< "identificationId" */
    ElementTagType identification_type;  /**< "identificationType" */
    ElementTagType identification_value; /**< "identificationValue" */
    ElementTagType authorized;           /**< "authorized" */
};

typedef struct IdentificationListDataType IdentificationListDataType;

struct IdentificationListDataType {
    const IdentificationDataType* const* identification_data; /**< "identificationData" */
    size_t identification_data_size;
};

typedef struct IdentificationListDataSelectorsType IdentificationListDataSelectorsType;

struct IdentificationListDataSelectorsType {
    const IdentificationIdType* identification_id;     /**< "identificationId" */
    const IdentificationTypeType* identification_type; /**< "identificationType" */
};

typedef struct SessionIdentificationDataType SessionIdentificationDataType;

struct SessionIdentificationDataType {
    const SessionIdType* session_id;               /**< "sessionId" */
    const IdentificationIdType* identification_id; /**< "identificationId" */
    const bool* is_latest_session;                 /**< "isLatestSession" */
    const TimePeriodType* time_period;             /**< "timePeriod" */
};

typedef struct SessionIdentificationDataElementsType SessionIdentificationDataElementsType;

struct SessionIdentificationDataElementsType {
    ElementTagType session_id;                 /**< "sessionId" */
    ElementTagType identification_id;          /**< "identificationId" */
    ElementTagType is_latest_session;          /**< "isLatestSession" */
    const TimePeriodElementsType* time_period; /**< "timePeriod" */
};

typedef struct SessionIdentificationListDataType SessionIdentificationListDataType;

struct SessionIdentificationListDataType {
    const SessionIdentificationDataType* const* session_identification_data; /**< "sessionIdentificationData" */
    size_t session_identification_data_size;
};

typedef struct SessionIdentificationListDataSelectorsType SessionIdentificationListDataSelectorsType;

struct SessionIdentificationListDataSelectorsType {
    const SessionIdType* session_id;               /**< "sessionId" */
    const IdentificationIdType* identification_id; /**< "identificationId" */
    const bool* is_latest_session;                 /**< "isLatestSession" */
    const TimePeriodType* time_period;             /**< "timePeriod" */
};

typedef struct SessionMeasurementRelationDataType SessionMeasurementRelationDataType;

struct SessionMeasurementRelationDataType {
    const SessionIdType* session_id;                /**< "sessionId" */
    const MeasurementIdType* const* measurement_id; /**< "measurementId" */
    size_t measurement_id_size;
};

typedef struct SessionMeasurementRelationDataElementsType SessionMeasurementRelationDataElementsType;

struct SessionMeasurementRelationDataElementsType {
    ElementTagType session_id;     /**< "sessionId" */
    ElementTagType measurement_id; /**< "measurementId" */
};

typedef struct SessionMeasurementRelationListDataType SessionMeasurementRelationListDataType;

struct SessionMeasurementRelationListDataType {
    const SessionMeasurementRelationDataType* const*
        session_measurement_relation_data; /**< "sessionMeasurementRelationData" */
    size_t session_measurement_relation_data_size;
};

typedef struct SessionMeasurementRelationListDataSelectorsType SessionMeasurementRelationListDataSelectorsType;

struct SessionMeasurementRelationListDataSelectorsType {
    const SessionIdType* session_id;         /**< "sessionId" */
    const MeasurementIdType* measurement_id; /**< "measurementId" */
};

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SPINE_MODEL_IDENTIFICATION_TYPES_H_
