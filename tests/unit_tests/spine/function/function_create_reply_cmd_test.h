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
 * @brief SPINE Function Create Reply Command generic test declarations
 */

#ifndef TESTS_UNIT_TESTS_SPINE_FUNCTION_FUNCTION_CREATE_REPLY_CMD_TEST_H_
#define TESTS_UNIT_TESTS_SPINE_FUNCTION_FUNCTION_CREATE_REPLY_CMD_TEST_H_

#include <gtest/gtest.h>

#include <string_view>

#include "src/spine/model/function_types.h"

struct FunctionCreateReplyCmdTestInput {
  std::string_view description;
  FunctionType function_type = static_cast<FunctionType>(0);
  std::string_view data_txt;
  std::string_view cmd_txt;
};

std::ostream& operator<<(std::ostream& os, FunctionCreateReplyCmdTestInput test_input);

class FunctionCreateReplyCmdTests : public ::testing::TestWithParam<FunctionCreateReplyCmdTestInput> {};

#endif  // TESTS_UNIT_TESTS_SPINE_FUNCTION_FUNCTION_CREATE_REPLY_CMD_TEST_H_
