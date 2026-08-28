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
 * @brief SPINE Datagram Version data types declarations
 */

#ifndef SRC_SPINE_MODEL_VERSION_TYPES_H_
#define SRC_SPINE_MODEL_VERSION_TYPES_H_

#include <stdbool.h>
#include <stdint.h>

#include "src/spine/model/common_data_types.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef SpecificationVersionType SpecificationVersionDataType;

typedef TagType SpecificationVersionDataElementsType;

typedef struct SpecificationVersionListDataType SpecificationVersionListDataType;

struct SpecificationVersionListDataType {
    const SpecificationVersionDataType* specification_version_data; /**< "specificationVersionData" */
    size_t specification_version_data_size;
};

typedef TagType SpecificationVersionListDataSelectorsType;

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SPINE_MODEL_VERSION_TYPES_H_
