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
 * @brief SPINE Datagram declaration
 */

#ifndef SRC_SPINE_MODEL_DATAGRAM_H_
#define SRC_SPINE_MODEL_DATAGRAM_H_

#include "src/spine/model/command_frame_types.h"
#include "src/spine/model/common_data_types.h"
#include "src/spine/model/feature_types.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct {
    SpecificationVersionType spec_version;       /**< "specificationVersion" */
    const FeatureAddressType* src_addr;          /**< "addressSource" */
    const FeatureAddressType* dest_addr;         /**< "addressDestination" */
    const FeatureAddressType* originator_addr;   /**< "addressOriginator" */
    const uint64_t* msg_cnt;                     /**< "msgCounter" */
    const uint64_t* msg_cnt_ref;                 /**< "msgCounterReference" */
    const CommandClassifierType* cmd_classifier; /**< "cmdClassifier" */
    const bool* ack_request;                     /**< "ackRequest" */
    const AbsoluteOrRelativeTimeType* timestamp; /**< "timestamp" */
} HeaderType;

typedef struct {
    const CmdType* const* cmd; /**< "cmd" */
    size_t cmd_size;
} PayloadType;

typedef struct {
    const HeaderType* header;   /**< "header" */
    const PayloadType* payload; /**< "payload" */
} DatagramType;                 /**< "datagram" */

bool DatagramIsValid(const DatagramType* datagram);
void DatagramDelete(DatagramType* datagram);

DatagramType* DatagramParse(const char* s);
char* DatagramPrintUnformatted(const DatagramType* datagram);
DatagramType* DatagramCopy(const DatagramType* datagram);
bool DatagramHeaderCompare(const HeaderType* header_a, const HeaderType* header_b);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SPINE_MODEL_DATAGRAM_H_
