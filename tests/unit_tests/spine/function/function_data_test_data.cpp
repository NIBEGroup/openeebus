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
 * @brief Function Data Json print/parse functions implementation
 */

#include <memory>
#include <string_view>

#include "src/spine/model/model.h"
#include "tests/unit_tests/json.h"
#include "tests/unit_tests/spine/function_data.h"

static void FunctionDataParse(FunctionData* self, const char* s);
static char* FunctionDataPrintUnformatted(const FunctionData* self);

void FunctionDataParse(FunctionData* self, const char* s) {
  const EebusDataCfg* const cfg = ModelGetDataCfg(self->type);
  if (self->data != NULL) {
    EEBUS_DATA_DELETE(cfg, &self->data);
    self->data = NULL;
  }

  self->data = EEBUS_DATA_PARSE(cfg, s);
}

char* FunctionDataPrintUnformatted(const FunctionData* self) {
  const EebusDataCfg* const cfg = ModelGetDataCfg(self->type);
  return EEBUS_DATA_PRINT_UNFORMATTED(cfg, &self->data);
}

std::unique_ptr<FunctionData, decltype(&FunctionDataDelete)>
FunctionDataTestDataParse(FunctionType function_type, const std::string_view& s) {
  std::unique_ptr<char[], decltype(&JsonFree)> s_unformatted{JsonUnformat(s), JsonFree};
  if (s_unformatted == nullptr) {
    return {nullptr, nullptr};
  }

  std::unique_ptr<FunctionData, decltype(&FunctionDataDelete)> fd{
      FunctionDataCreate(function_type, NULL),
      FunctionDataDelete
  };
  if (fd == nullptr) {
    return {nullptr, nullptr};
  }

  FunctionDataParse(fd.get(), s_unformatted.get());
  return fd;
}

std::unique_ptr<char[], decltype(&JsonFree)> FunctionDataTestDataPrint(const FunctionData* fd) {
  std::unique_ptr<char[], decltype(&JsonFree)> s{FunctionDataPrintUnformatted(fd), JsonFree};
  return s;
}
