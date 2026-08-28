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
 * @brief Feature Remote internal declarations (to be used only by unit tests or FeatureLocal subclasses)
 */

#ifndef SRC_SPINE_FEATURE_FEATURE_REMOTE_INTERNAL_H_
#define SRC_SPINE_FEATURE_FEATURE_REMOTE_INTERNAL_H_

#include "src/spine/api/feature_remote_interface.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct FeatureRemote FeatureRemote;

struct FeatureRemote {
    /** Inherits the Feature */
    Feature obj;

    EntityRemoteObject* entity;
    uint32_t max_response_delay;
};

#define FEATURE_REMOTE(obj) ((FeatureRemote*)(obj))

EebusError FeatureRemoteConstruct(
    FeatureRemote* self,
    uint32_t id,
    EntityRemoteObject* entity,
    FeatureTypeType type,
    RoleType role
);
void FeatureRemoteDestruct(FeatureObject* self);
DeviceRemoteObject* FeatureRemoteGetDevice(const FeatureRemoteObject* self);
EntityRemoteObject* FeatureRemoteGetEntity(const FeatureRemoteObject* self);
const void* FeatureRemoteGetData(const FeatureRemoteObject* self, FunctionType function_type);
void* FeatureRemoteDataCopy(const FeatureRemoteObject* self, FunctionType fcn_type);
EebusError FeatureRemoteUpdateData(
    FeatureRemoteObject* self,
    FunctionType function_type,
    const void* new_data,
    const FilterType* filter_partial,
    const FilterType* filter_delete,
    bool persist
);
void FeatureRemoteSetFunctionOperations(
    FeatureRemoteObject* self,
    const FunctionPropertyType* const* supported_functions,
    size_t supported_functions_size
);
void FeatureRemoteSetMaxResponseDelay(FeatureRemoteObject* self, uint32_t max_delay);
uint32_t FeatureRemoteGetMaxResponseDelay(const FeatureRemoteObject* self);

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  // SRC_SPINE_FEATURE_FEATURE_REMOTE_INTERNAL_H_
