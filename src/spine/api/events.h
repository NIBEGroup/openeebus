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
 * @brief Events types declarations
 */

#ifndef SRC_EEBUS_SRC_SPINE_API_EVENTS_H_
#define SRC_EEBUS_SRC_SPINE_API_EVENTS_H_

#include <stdint.h>

#include "src/spine/api/device_remote_interface.h"
#include "src/spine/api/entity_remote_interface.h"
#include "src/spine/api/feature_local_interface.h"
#include "src/spine/api/feature_remote_interface.h"
#include "src/spine/model/command_frame_types.h"
#include "src/spine/model/function_types.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef uint32_t EventHandlerLevel;

enum {
    kEventHandlerLevelCore,         // Shall only be used by the core stack
    kEventHandlerLevelApplication,  // Shall only be used by applications
};

typedef uint16_t ElementChangeType;

enum {
    kElementChangeAdd,
    kElementChangeUpdate,
    kElementChangeRemove,
};

typedef uint16_t EventType;

enum {
    kEventTypeDeviceChange,        // Sent after successful response of NodeManagementDetailedDiscovery
    kEventTypeEntityChange,        // Sent after successful response of NodeManagementDetailedDiscovery
    kEventTypeSubscriptionChange,  // Sent after successful subscription request from remote
    kEventTypeBindingChange,       // Sent after successful binding request from remote
    kEventTypeDataChange,          // Sent after remote provided new data items for a function
    kEventTypeUseCaseChange,       // Sent after successful response of NodeManagementUseCaseData
};

typedef struct EventPayload EventPayload;

struct EventPayload {
    const char* ski;                // Required
    EventType event_type;           // Required
    ElementChangeType change_type;  // Required
    DeviceRemoteObject* device;     // Required for DetailedDiscovery Call
    EntityRemoteObject* entity;     // Required for DetailedDiscovery Call and Notify
    FeatureRemoteObject* feature;
    FeatureLocalObject* local_feature;  // Required for write commands
    FunctionType function_type;         // Required for write commands
    const void* function_data;
    const CommandClassifierType* cmd_classifier;  // Optional, used together with EventType EventTypeDataChange
    const UseCaseFilterType* use_case_filter;     // Optional, used together with EventType EventTypeUseCaseChange
};

typedef void (*EventHandler)(const EventPayload* payload, void* ctx);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_EEBUS_SRC_SPINE_API_EVENTS_H_
