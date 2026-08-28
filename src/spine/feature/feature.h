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
 * @brief Feature implementation declarations
 *
 * Transformed from spine-go/spine/feature.go
 */

#ifndef SRC_SPINE_FEATURE_FEATURE_H_
#define SRC_SPINE_FEATURE_FEATURE_H_

#include "src/common/vector.h"
#include "src/spine/api/feature_interface.h"
#include "src/spine/api/function_interface.h"
#include "src/spine/model/entity_types.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

static const uint32_t kDefaultMaxResponseDelayMs = 10000;

typedef struct Feature Feature;

struct Feature {
    /** Implements the (Feature) Interface */
    FeatureObject obj;

    FeatureTypeType type;
    FeatureAddressType* address;
    char* description;
    RoleType role;
    Vector functions;
};

#define FEATURE(obj) ((Feature*)(obj))

void FeatureConstruct(
    Feature* self,
    FeatureTypeType type,
    const EntityAddressType* entity_addr,
    uint32_t feature_id,
    RoleType role
);
void FeatureDestruct(FeatureObject* self);
const FeatureAddressType* FeatureGetAddress(const FeatureObject* self);
FeatureTypeType FeatureGetType(const FeatureObject* self);
RoleType FeatureGetRole(const FeatureObject* self);
const OperationsObject* FeatureGetFunctionOperations(const FeatureObject* self, FunctionType fcn_type);
const char* FeatureGetDescription(const FeatureObject* self);
void FeatureSetDescription(FeatureObject* self, const char* description);
const char* FeatureToString(const FeatureObject* self);

FunctionObject* FeatureGetFunction(const Feature* self, FeatureTypeType type);
bool FeatureParametersMatch(const Feature* feature, RoleType role, FeatureTypeType type);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SPINE_FEATURE_FEATURE_H_
