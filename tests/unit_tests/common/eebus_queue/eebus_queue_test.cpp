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
#include <gtest/gtest.h>
#include <cstdint>
#include <memory>

#include "src/common/eebus_queue/eebus_queue.h"

#include "tests/unit_tests/memory_leak.inc"

TEST(EebusQueueTest, EebusQueueTestGeneric) {
  static const size_t kQueueMaxSize = 5;
  std::unique_ptr<EebusQueueObject, decltype(&EebusQueueDelete)> queue{
      EebusQueueCreate(kQueueMaxSize, sizeof(int32_t), nullptr),
      &EebusQueueDelete
  };

  ASSERT_NE(queue, nullptr) << "Failed to create EebusQueue";
  ASSERT_TRUE(EEBUS_QUEUE_IS_EMPTY(queue.get()));
  ASSERT_FALSE(EEBUS_QUEUE_IS_FULL(queue.get()));

  int32_t msg_wr = 1;
  for (size_t i = 0; i < kQueueMaxSize; ++i) {
    EXPECT_FALSE(EEBUS_QUEUE_IS_FULL(queue.get()));
    EXPECT_EQ(EEBUS_QUEUE_SEND(queue.get(), &msg_wr, 0), kEebusErrorOk);
    ++msg_wr;
  }

  ASSERT_TRUE(EEBUS_QUEUE_IS_FULL(queue.get()));

  msg_wr         = 1;
  int32_t msg_rd = 0;
  for (size_t i = 0; i < kQueueMaxSize; ++i) {
    EXPECT_FALSE(EEBUS_QUEUE_IS_EMPTY(queue.get()));
    EXPECT_EQ(EEBUS_QUEUE_RECEIVE(queue.get(), &msg_rd, 0), kEebusErrorOk);
    EXPECT_EQ(msg_rd, msg_wr);
    ++msg_wr;
  }

  ASSERT_TRUE(EEBUS_QUEUE_IS_EMPTY(queue.get()));

  msg_wr = 0;
  EXPECT_EQ(EEBUS_QUEUE_SEND(queue.get(), &msg_wr, 0), kEebusErrorOk);
  EXPECT_EQ(EEBUS_QUEUE_SEND(queue.get(), &msg_wr, 0), kEebusErrorOk);
  EXPECT_EQ(EEBUS_QUEUE_RECEIVE(queue.get(), &msg_rd, 0), kEebusErrorOk);
  EXPECT_EQ(EEBUS_QUEUE_RECEIVE(queue.get(), &msg_rd, 0), kEebusErrorOk);
  msg_rd = 0;

  msg_wr = 6;
  for (size_t i = 0; i < kQueueMaxSize; ++i) {
    EXPECT_EQ(EEBUS_QUEUE_SEND(queue.get(), &msg_wr, 0), kEebusErrorOk);
    ++msg_wr;
  }

  ASSERT_TRUE(EEBUS_QUEUE_IS_FULL(queue.get()));
  EXPECT_EQ(EEBUS_QUEUE_SEND(queue.get(), &msg_wr, 1), kEebusErrorTime);

  msg_wr = 6;
  msg_rd = 0;
  for (size_t i = 0; i < kQueueMaxSize; ++i) {
    EXPECT_FALSE(EEBUS_QUEUE_IS_EMPTY(queue.get()));
    EXPECT_EQ(EEBUS_QUEUE_RECEIVE(queue.get(), &msg_rd, 0), kEebusErrorOk);
    EXPECT_EQ(msg_rd, msg_wr);
    ++msg_wr;
  }

  ASSERT_TRUE(EEBUS_QUEUE_IS_EMPTY(queue.get()));
  EXPECT_EQ(EEBUS_QUEUE_RECEIVE(queue.get(), &msg_rd, 1), kEebusErrorTime);

  queue.reset();

  EXPECT_EQ(heap_used, 0);
  CheckForMemoryLeaks();
}
