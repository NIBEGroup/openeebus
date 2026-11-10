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
 * @brief Utility functions to simplify the message buffer related tests
 */

#ifndef TESTS_UNIT_TESTS_MESSAGE_BUFFER_H_
#define TESTS_UNIT_TESTS_MESSAGE_BUFFER_H_

#include <cstdint>
#include <string_view>

#include "src/common/eebus_malloc.h"
#include "src/common/message_buffer.h"

static inline void MessageBufferInitWithStringView(MessageBuffer* msg_buf, const std::string_view& s) {
  const size_t msg_size = s.size();
  uint8_t* const msg    = reinterpret_cast<uint8_t*>(EEBUS_MALLOC(msg_size));
  memcpy(msg, s.data(), msg_size);

  MessageBufferInit(msg_buf, msg, msg_size);
}

#endif  // TESTS_UNIT_TESTS_MESSAGE_BUFFER_H_
