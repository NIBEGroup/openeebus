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
 * @brief SPINE Possible Operations type declarations
 */

#ifndef SRC_SPINE_MODEL_POSSIBLE_OPERATIONS_TYPES_H_
#define SRC_SPINE_MODEL_POSSIBLE_OPERATIONS_TYPES_H_

#include "src/spine/model/common_data_types.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct PossibleOperationsClassifierType PossibleOperationsClassifierType;
struct PossibleOperationsClassifierType {
    ElementTagType partial; /**< "partial" */
};

typedef struct PossibleOperationsReadType PossibleOperationsReadType;

struct PossibleOperationsReadType {
    ElementTagType partial; /**< "partial" */
};

typedef struct PossibleOperationsWriteType PossibleOperationsWriteType;

struct PossibleOperationsWriteType {
    ElementTagType partial; /**< "partial" */
};

typedef struct PossibleOperationsType PossibleOperationsType;

struct PossibleOperationsType {
    const PossibleOperationsReadType* read;   /**< "read" */
    const PossibleOperationsWriteType* write; /**< "write" */
};

typedef struct PossibleOperationsElementsType PossibleOperationsElementsType;

struct PossibleOperationsElementsType {
    ElementTagType read_enabled;  /**< "read" */
    ElementTagType write_enabled; /**< "write" */
};

PossibleOperationsType* PossibleOperationsCreate(bool read, bool read_partial, bool write, bool write_partial);
void PossibleOperationsDelete(PossibleOperationsType* self);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SPINE_MODEL_POSSIBLE_OPERATIONS_TYPES_H_
