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
 * @brief Feature Local implementation declarations
 */

#ifndef SRC_SPINE_FEATURE_FEATURE_LOCAL_H_
#define SRC_SPINE_FEATURE_FEATURE_LOCAL_H_

#include <stddef.h>

#include "src/common/eebus_malloc.h"
#include "src/spine/api/feature_local_interface.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

FeatureLocalObject* FeatureLocalCreate(uint32_t id, EntityLocalObject* entity, FeatureTypeType type, RoleType role);

static inline void FeatureLocalDelete(FeatureLocalObject* feature_local) {
    if (feature_local != NULL) {
        FEATURE_DESTRUCT(FEATURE_OBJECT(feature_local));
        EEBUS_FREE(feature_local);
    }
}

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SPINE_FEATURE_FEATURE_LOCAL_H_
