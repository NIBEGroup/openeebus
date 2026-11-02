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
#include "tests/unit_tests/spine/function_data.h"

#include "src/common/eebus_data/eebus_data_list.h"
#include "src/common/eebus_data/eebus_data_sequence.h"
#include "src/common/eebus_data/eebus_data_util.h"
#include "src/spine/model/model.h"

EebusError FunctionDataConstruct(FunctionData* self, FunctionType function_type, const void* data) {
  self->type = function_type;
  self->data = NULL;

  const EebusDataCfg* const cfg = ModelGetDataCfg(function_type);
  if (cfg == NULL) {
    return kEebusErrorInputArgumentOutOfRange;
  }

  return EEBUS_DATA_COPY(cfg, &data, &self->data);
}

void FunctionDataDestruct(FunctionData* self) {
  const EebusDataCfg* const cfg = ModelGetDataCfg(self->type);
  EEBUS_DATA_DELETE(cfg, &self->data);
  self->data = NULL;
}

FunctionData* FunctionDataCreate(FunctionType function_type, const void* data) {
  FunctionData* const function_data = (FunctionData*)EEBUS_MALLOC(sizeof(FunctionData));
  if (function_data == NULL) {
    return NULL;
  }

  const EebusError ret = FunctionDataConstruct(function_data, function_type, data);
  if (ret != kEebusErrorOk) {
    FunctionDataDelete(function_data);
    return NULL;
  }

  return function_data;
}
