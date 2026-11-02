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

#ifndef TESTS_UNIT_TESTS_SPINE_FUNCTION_FILTER_TEST_DATA_H_
#define TESTS_UNIT_TESTS_SPINE_FUNCTION_FILTER_TEST_DATA_H_

#include <memory>
#include <string_view>

#include "src/spine/model/command_frame_types.h"

FilterType* FilterParse(const char* s);
char* FilterPrintUnformatted(const FilterType* self);
void FilterDelete(FilterType* self);
std::unique_ptr<FilterType, decltype(&FilterDelete)> FilterTestDataParse(const std::string_view& s);

#endif  // TESTS_UNIT_TESTS_SPINE_FUNCTION_FILTER_TEST_DATA_H_
