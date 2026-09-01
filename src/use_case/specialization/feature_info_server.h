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
 * @brief Feature Info Client
 */

#ifndef SRC_USE_CASE_FEATURE_INFO_FEATURE_INFO_SERVER_H_
#define SRC_USE_CASE_FEATURE_INFO_FEATURE_INFO_SERVER_H_

#include "src/spine/api/device_local_interface.h"
#include "src/spine/api/entity_local_interface.h"
#include "src/spine/api/feature_local_interface.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct FeatureInfoServer FeatureInfoServer;

struct FeatureInfoServer {
    FeatureTypeType feature_type;

    RoleType local_role;
    DeviceLocalObject* local_device;
    EntityLocalObject* local_entity;
    FeatureLocalObject* local_feature;
};

EebusError
FeatureInfoServerConstruct(FeatureInfoServer* self, FeatureTypeType feature_type, EntityLocalObject* local_entity);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_USE_CASE_FEATURE_INFO_FEATURE_INFO_SERVER_H_
