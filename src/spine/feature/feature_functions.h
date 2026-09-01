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
 * @brief SPINE Feature related functions table
 */

#ifndef SRC_SPINE_FEATURE_FEATURE_FUNCTIONS_H_
#define SRC_SPINE_FEATURE_FEATURE_FUNCTIONS_H_

#include <stddef.h>

#include "src/spine/model/feature_types.h"
#include "src/spine/model/function_types.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct FeatureFunctions FeatureFunctions;

struct FeatureFunctions {
    FeatureTypeType feature_type;
    const FunctionType* functions_list;
    size_t functions_list_size;
};

const FeatureFunctions* GetFeatureFunctions(FeatureTypeType feature_type);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SPINE_FEATURE_FEATURE_FUNCTIONS_H_
