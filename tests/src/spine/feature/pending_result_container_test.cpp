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

#include <memory>

#include "src/common/api/eebus_timer_interface.h"
#include "src/common/eebus_arguments.h"
#include "src/spine/api/pending_result_container_interface.h"
#include "src/spine/feature/feature.h"
#include "src/spine/feature/pending_result_container.h"
#include "tests/src/memory_leak.inc"

static constexpr uint8_t kMaxResponseTimeSec = TIME_MS_TO_S(kDefaultMaxResponseDelayMs);

using PendingResultContainerPtr
    = std::unique_ptr<PendingResultContainerObject, decltype(&PendingResultContainerDelete)>;

static PendingResultContainerPtr MakePendingResultContainer() {
  return PendingResultContainerPtr(PendingResultContainerCreate(), PendingResultContainerDelete);
}

struct CallbackRecord {
  int call_count{0};
  std::unique_ptr<ResultMessage> received_msg;
  EebusError received_err{kEebusErrorOk};
};

static void RecordingCallback(
    const ResultMessage* result_msg,
    const FeatureAddressType* remote_feature_addr,
    EebusError err,
    void* ctx
) {
  UNUSED(remote_feature_addr);

  auto* record = static_cast<CallbackRecord*>(ctx);

  record->received_msg = (result_msg != nullptr) ? std::make_unique<ResultMessage>(*result_msg) : nullptr;
  record->received_err = err;
  ++record->call_count;
}

class PendingResultContainerTest : public ::testing::Test {
 protected:
  void SetUp() override {
    container_ = MakePendingResultContainer();
    ASSERT_NE(container_, nullptr);
  }

  void TearDown() override {
    container_.reset();
    CheckForMemoryLeaks();
  }

  PendingResultContainerPtr container_{nullptr, PendingResultContainerDelete};
};

TEST_F(PendingResultContainerTest, CreateSucceeds) {
  EXPECT_NE(container_, nullptr);
}

TEST_F(PendingResultContainerTest, AddReturnsOk) {
  CallbackRecord rec;
  const EebusError err = PENDING_RESULT_CONTAINER_ADD(
      container_.get(),
      1,
      kFunctionTypeLoadControlLimitListData,
      nullptr,
      RecordingCallback,
      &rec
  );
  EXPECT_EQ(err, kEebusErrorOk);
}

TEST_F(PendingResultContainerTest, ProcessFiresMatchingCallback) {
  CallbackRecord rec;
  PENDING_RESULT_CONTAINER_ADD(
      container_.get(),
      10,
      kFunctionTypeLoadControlLimitListData,
      nullptr,
      RecordingCallback,
      &rec
  );

  ResultMessage msg{.msg_cnt_ref = 10};
  PENDING_RESULT_CONTAINER_PROCESS(container_.get(), &msg);

  EXPECT_EQ(rec.call_count, 1);
  ASSERT_NE(rec.received_msg, nullptr);
  EXPECT_EQ(rec.received_msg->msg_cnt_ref, msg.msg_cnt_ref);
}

TEST_F(PendingResultContainerTest, ProcessIgnoresNonMatchingMsgCntRef) {
  CallbackRecord rec;
  PENDING_RESULT_CONTAINER_ADD(
      container_.get(),
      10,
      kFunctionTypeLoadControlLimitListData,
      nullptr,
      RecordingCallback,
      &rec
  );

  ResultMessage msg{.msg_cnt_ref = 99};
  PENDING_RESULT_CONTAINER_PROCESS(container_.get(), &msg);

  EXPECT_EQ(rec.call_count, 0);
}

TEST_F(PendingResultContainerTest, ProcessFiresOnlyMatchingEntry) {
  CallbackRecord rec_a;
  CallbackRecord rec_b;
  PENDING_RESULT_CONTAINER_ADD(
      container_.get(),
      1,
      kFunctionTypeLoadControlLimitListData,
      nullptr,
      RecordingCallback,
      &rec_a
  );
  PENDING_RESULT_CONTAINER_ADD(
      container_.get(),
      2,
      kFunctionTypeLoadControlLimitListData,
      nullptr,
      RecordingCallback,
      &rec_b
  );

  ResultMessage msg{.msg_cnt_ref = 1};
  PENDING_RESULT_CONTAINER_PROCESS(container_.get(), &msg);

  EXPECT_EQ(rec_a.call_count, 1);
  EXPECT_EQ(rec_b.call_count, 0);
}

TEST_F(PendingResultContainerTest, ProcessRemovesEntryAfterFiring) {
  CallbackRecord rec;
  PENDING_RESULT_CONTAINER_ADD(
      container_.get(),
      5,
      kFunctionTypeLoadControlLimitListData,
      nullptr,
      RecordingCallback,
      &rec
  );

  ResultMessage msg{.msg_cnt_ref = 5};
  PENDING_RESULT_CONTAINER_PROCESS(container_.get(), &msg);
  PENDING_RESULT_CONTAINER_PROCESS(container_.get(), &msg);

  EXPECT_EQ(rec.call_count, 1);
}

TEST_F(PendingResultContainerTest, TickDoesNotFireBeforeTimeout) {
  CallbackRecord rec;
  PENDING_RESULT_CONTAINER_ADD(
      container_.get(),
      1,
      kFunctionTypeLoadControlLimitListData,
      nullptr,
      RecordingCallback,
      &rec
  );

  for (size_t i = 0; i < kMaxResponseTimeSec - 1; ++i) {
    PENDING_RESULT_CONTAINER_TICK(container_.get());
  }

  EXPECT_EQ(rec.call_count, 0);
}

TEST_F(PendingResultContainerTest, TickFiresWithNullOnTimeout) {
  CallbackRecord rec;
  PENDING_RESULT_CONTAINER_ADD(
      container_.get(),
      1,
      kFunctionTypeLoadControlLimitListData,
      nullptr,
      RecordingCallback,
      &rec
  );

  for (size_t i = 0; i < kMaxResponseTimeSec + 1; ++i) {
    PENDING_RESULT_CONTAINER_TICK(container_.get());
  }

  EXPECT_EQ(rec.call_count, 1);
  ASSERT_NE(rec.received_msg, nullptr);
  EXPECT_EQ(rec.received_err, kEebusErrorTime);
  EXPECT_EQ(rec.received_msg->function_type, kFunctionTypeLoadControlLimitListData);
}

TEST_F(PendingResultContainerTest, TickRemovesExpiredEntry) {
  CallbackRecord rec;
  PENDING_RESULT_CONTAINER_ADD(
      container_.get(),
      1,
      kFunctionTypeLoadControlLimitListData,
      nullptr,
      RecordingCallback,
      &rec
  );

  for (size_t i = 0; i < kMaxResponseTimeSec; ++i) {
    PENDING_RESULT_CONTAINER_TICK(container_.get());
  }

  // Tick again — expired entry is already gone, so callback is not called again
  for (size_t i = 0; i < 5; ++i) {
    PENDING_RESULT_CONTAINER_TICK(container_.get());
  }

  EXPECT_EQ(rec.call_count, 1);
}

TEST_F(PendingResultContainerTest, TickExpiresAllEntriesIndependently) {
  CallbackRecord rec_a;
  CallbackRecord rec_b;
  PENDING_RESULT_CONTAINER_ADD(
      container_.get(),
      1,
      kFunctionTypeLoadControlLimitListData,
      nullptr,
      RecordingCallback,
      &rec_a
  );
  PENDING_RESULT_CONTAINER_ADD(
      container_.get(),
      2,
      kFunctionTypeLoadControlLimitListData,
      nullptr,
      RecordingCallback,
      &rec_b
  );

  for (size_t i = 0; i < kMaxResponseTimeSec + 1; ++i) {
    PENDING_RESULT_CONTAINER_TICK(container_.get());
  }

  EXPECT_EQ(rec_a.call_count, 1);
  EXPECT_EQ(rec_b.call_count, 1);
  ASSERT_NE(rec_a.received_msg, nullptr);
  ASSERT_NE(rec_b.received_msg, nullptr);
  EXPECT_EQ(rec_a.received_err, kEebusErrorTime);
  EXPECT_EQ(rec_b.received_err, kEebusErrorTime);
  EXPECT_EQ(rec_a.received_msg->function_type, kFunctionTypeLoadControlLimitListData);
  EXPECT_EQ(rec_b.received_msg->function_type, kFunctionTypeLoadControlLimitListData);
}

TEST_F(PendingResultContainerTest, ProcessAfterTickDoesNotFireExpired) {
  CallbackRecord rec;
  PENDING_RESULT_CONTAINER_ADD(
      container_.get(),
      7,
      kFunctionTypeLoadControlLimitListData,
      nullptr,
      RecordingCallback,
      &rec
  );

  for (size_t i = 0; i < kMaxResponseTimeSec; ++i) {
    PENDING_RESULT_CONTAINER_TICK(container_.get());
  }

  ResultMessage msg{.msg_cnt_ref = 7};
  PENDING_RESULT_CONTAINER_PROCESS(container_.get(), &msg);

  EXPECT_EQ(rec.call_count, 1);  // only the timeout fire, not the process fire
}

TEST_F(PendingResultContainerTest, DeleteWithPendingItemsReleasesMemory) {
  CallbackRecord rec;
  PENDING_RESULT_CONTAINER_ADD(
      container_.get(),
      1,
      kFunctionTypeLoadControlLimitListData,
      nullptr,
      RecordingCallback,
      &rec
  );
  PENDING_RESULT_CONTAINER_ADD(
      container_.get(),
      2,
      kFunctionTypeLoadControlLimitListData,
      nullptr,
      RecordingCallback,
      &rec
  );
}
