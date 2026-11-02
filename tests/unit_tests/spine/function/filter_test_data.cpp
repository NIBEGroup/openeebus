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
 * @brief Filter Json print/parse functions for test purpose
 */

#include <memory>
#include <string_view>

#include "src/spine/model/command_frame_types.h"
#include "src/spine/model/model.h"
#include "tests/unit_tests/json.h"

FilterType* FilterParse(const char* s) {
  const EebusDataCfg* const cfg = GetFilterCfg();
  return (FilterType*)EEBUS_DATA_PARSE(cfg, s);
}

char* FilterPrintUnformatted(const FilterType* self) {
  const EebusDataCfg* const cfg = GetFilterCfg();
  return EEBUS_DATA_PRINT_UNFORMATTED(cfg, &self);
}

void FilterDelete(FilterType* self) {
  const EebusDataCfg* const cfg = GetFilterCfg();
  EEBUS_DATA_DELETE(cfg, &self);
}

std::unique_ptr<FilterType, decltype(&FilterDelete)> FilterTestDataParse(const std::string_view& s) {
  std::unique_ptr<char[], decltype(&JsonFree)> s_filter_delete{JsonUnformat(s), JsonFree};
  if (s_filter_delete == nullptr) {
    return {nullptr, nullptr};
  }

  std::unique_ptr<FilterType, decltype(&FilterDelete)> filter{FilterParse(s_filter_delete.get()), FilterDelete};
  return filter;
}
