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
 * @brief SPINE Datagram Messaging data types declarations
 */

#ifndef SRC_SPINE_MODEL_MESSAGING_TYPES_H_
#define SRC_SPINE_MODEL_MESSAGING_TYPES_H_

#include <stdbool.h>
#include <stdint.h>

#include "src/spine/model/common_data_types.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef uint32_t MessagingNumberType;

typedef const char* MessagingDataTextType;

typedef int32_t MessagingTypeType;

enum {
    kMessagingTypeTypeLogging,     /**< "logging" */
    kMessagingTypeTypeInformation, /**< "information" */
    kMessagingTypeTypeWarning,     /**< "warning" */
    kMessagingTypeTypeAlarm,       /**< "alarm" */
    kMessagingTypeTypeEmergency,   /**< "emergency" */
    kMessagingTypeTypeObsolete,    /**< "obsolete" */
};

typedef struct MessagingDataType MessagingDataType;

struct MessagingDataType {
    const AbsoluteOrRelativeTimeType* timestamp; /**< "timestamp" */
    const MessagingNumberType* messaging_number; /**< "messagingNumber" */
    const MessagingTypeType* messaging_type;     /**< "type" */
    MessagingDataTextType text;                  /**< "text" */
};

typedef struct MessagingDataElementsType MessagingDataElementsType;

struct MessagingDataElementsType {
    ElementTagType timestamp;        /**< "timestamp" */
    ElementTagType messaging_number; /**< "messagingNumber" */
    ElementTagType messaging_type;   /**< "type" */
    ElementTagType text;             /**< "text" */
};

typedef struct MessagingListDataType MessagingListDataType;

struct MessagingListDataType {
    const MessagingDataType* const* messaging_data; /**< "messagingData" */
    size_t messaging_data_size;
};

typedef struct MessagingListDataSelectorsType MessagingListDataSelectorsType;

struct MessagingListDataSelectorsType {
    const TimestampIntervalType* timestamp_interval; /**< "timestampInterval" */
    const MessagingNumberType* messaging_number;     /**< "messagingNumber" */
};

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SPINE_MODEL_MESSAGING_TYPES_H_
