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
 * @brief Function Data type declaration
 */

#ifndef SRC_SPINE_MODEL_FUNCTION_DATA_H_
#define SRC_SPINE_MODEL_FUNCTION_DATA_H_

#include "src/common/api/eebus_data_interface.h"
#include "src/common/eebus_errors.h"
#include "src/common/eebus_malloc.h"
#include "src/spine/model/command_frame_types.h"
#include "src/spine/model/function_types.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct {
  FunctionType type;
  void* data;
} FunctionData;

EebusError FunctionDataConstruct(FunctionData* self, FunctionType function_type, const void* data);
void FunctionDataDestruct(FunctionData* self);

FunctionData* FunctionDataCreate(FunctionType function_type, const void* data);

static inline void FunctionDataDelete(FunctionData* self) {
  if (self != NULL) {
    FunctionDataDestruct(self);
    EEBUS_FREE(self);
  }
}

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // FunctionData
