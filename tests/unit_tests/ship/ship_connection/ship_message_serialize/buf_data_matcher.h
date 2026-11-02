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
 * @brief SHIP message buffer data matcher
 */
#ifndef TESTS_UNIT_TESTS_SHIP_SHIP_CONNECTION_SHIP_MESSAGE_SERIALIZE_BUF_DATA_MATCHER_H
#define TESTS_UNIT_TESTS_SHIP_SHIP_CONNECTION_SHIP_MESSAGE_SERIALIZE_BUF_DATA_MATCHER_H

#include <gmock/gmock.h>

MATCHER_P(BufDataEq, msg, "") {
  std::string_view data_sv(reinterpret_cast<const char*>(arg->data), arg->data_size);
  *result_listener << data_sv << "\n";

  bool match = (arg->data_size >= 2);

  match = match && (arg->data_size == msg.length());
  match = match && (arg->data[0] == msg.data()[0]);
  match = match && (!strncmp(reinterpret_cast<const char*>(&arg->data[1]), &msg.data()[1], arg->data_size - 1));

  return match;
}

#endif  // TESTS_UNIT_TESTS_SHIP_SHIP_CONNECTION_SHIP_MESSAGE_SERIALIZE_BUF_DATA_MATCHER_H
