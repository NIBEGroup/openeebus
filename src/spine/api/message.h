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
 * @brief SPINE Message declartions
 */

#ifndef SRC_EEBUS_SRC_SPINE_API_MESSAGE_H_
#define SRC_EEBUS_SRC_SPINE_API_MESSAGE_H_

#include "src/spine/api/device_remote_interface.h"
#include "src/spine/api/entity_remote_interface.h"
#include "src/spine/api/feature_local_interface.h"
#include "src/spine/api/feature_remote_interface.h"
#include "src/spine/model/command_frame_types.h"
#include "src/spine/model/function_types.h"
#include "src/spine/model/result_types.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct Message Message;

struct Message {
    const HeaderType* request_header;
    CommandClassifierType cmd_classifier;
    const CmdType* cmd;
    const FilterType* filter_partial;
    const FilterType* filter_delete;
    FeatureRemoteObject* feature_remote;
    EntityRemoteObject* entity_remote;
    DeviceRemoteObject* device_remote;
};

typedef struct FeatureLocalObject FeatureLocalObject;

typedef struct ReplyMessage ReplyMessage;

struct ReplyMessage {
    MsgCounterType msg_cnt_ref;
    const void* function_data;
    FunctionType function_type;
    FeatureLocalObject* feature_local;
    FeatureRemoteObject* feature_remote;
    EntityRemoteObject* entity_remote;
    DeviceRemoteObject* device_remote;
    const char* ski;
};

typedef struct ResultMessage ResultMessage;

struct ResultMessage {
    MsgCounterType msg_cnt_ref;
    FunctionType function_type;
    const ResultDataType* result_data;
    FeatureLocalObject* feature_local;
    FeatureRemoteObject* feature_remote;
    EntityRemoteObject* entity_remote;
    DeviceRemoteObject* device_remote;
};

static inline SenderObject* MessageGetSender(const Message* msg) {
    const DeviceRemoteObject* const dr = msg->device_remote;
    if (dr == NULL) {
        return NULL;
    }

    return DEVICE_REMOTE_GET_SENDER(dr);
}

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_EEBUS_SRC_SPINE_API_MESSAGE_H_
