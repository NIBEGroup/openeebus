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
 * @brief SPINE Datagram Data Tunneling data types declarations
 */

#ifndef SRC_SPINE_MODEL_DATA_TUNNELING_TYPES_H_
#define SRC_SPINE_MODEL_DATA_TUNNELING_TYPES_H_

#include <stdbool.h>
#include <stdint.h>

#include "src/spine/model/common_data_types.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef const char* PurposeIdType;

typedef uint32_t ChannelIdType;

typedef struct DataTunnelingHeaderType DataTunnelingHeaderType;

struct DataTunnelingHeaderType {
    PurposeIdType purpose_id;        /**< "purposeId" */
    const ChannelIdType* channel_id; /**< "channelId" */
    const uint32_t* sequence_id;     /**< "sequenceId" */
};

typedef struct DataTunnelingHeaderElementsType DataTunnelingHeaderElementsType;

struct DataTunnelingHeaderElementsType {
    ElementTagType purpose_id;  /**< "purposeId" */
    ElementTagType channel_id;  /**< "channelId" */
    ElementTagType sequence_id; /**< "sequenceId" */
};

typedef struct DataTunnelingCallType DataTunnelingCallType;

struct DataTunnelingCallType {
    const DataTunnelingHeaderType* header; /**< "header" */
    const char* payload;                   /**< "payload" */
};

typedef struct DataTunnelingCallElementsType DataTunnelingCallElementsType;

struct DataTunnelingCallElementsType {
    const DataTunnelingHeaderElementsType* header; /**< "header" */
    ElementTagType payload;                        /**< "payload" */
};

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SPINE_MODEL_DATA_TUNNELING_TYPES_H_
