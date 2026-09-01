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
 * @brief Binding Manager implementation declarations
 */

#ifndef SRC_SPINE_BINDING_BINDING_MANAGER_H_
#define SRC_SPINE_BINDING_BINDING_MANAGER_H_

#include <stddef.h>

#include "src/common/eebus_malloc.h"
#include "src/spine/api/binding_manager_interface.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

BindingManagerObject* BindingManagerCreate(const DeviceLocalObject* local_device);

static inline void BindingManagerDelete(BindingManagerObject* binding_manager) {
    if (binding_manager != NULL) {
        BINDING_MANAGER_DESTRUCT(binding_manager);
        EEBUS_FREE(binding_manager);
    }
}

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SPINE_BINDING_BINDING_MANAGER_H_
