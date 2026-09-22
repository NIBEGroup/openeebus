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
#include "src/spine/api/pending_reply_container_interface.h"
#include "src/spine/feature/feature.h"
#include "src/spine/feature/pending_reply_container.h"
#include "tests/src/memory_leak.inc"

static constexpr uint8_t kMaxResponseTimeSec = TIME_MS_TO_S(kDefaultMaxResponseDelayMs);

using PendingReplyContainerPtr = std::unique_ptr<PendingReplyContainerObject, decltype(&PendingReplyContainerDelete)>;

static PendingReplyContainerPtr MakePendingReplyContainer() {
  return PendingReplyContainerPtr(PendingReplyContainerCreate(), PendingReplyContainerDelete);
}

static constexpr uint32_t kDummyFeatureId         = 1;
static constexpr uint32_t kDummyEntityId          = 1;
static constexpr const uint32_t* kDummyEntity[1]  = {&kDummyEntityId};
static constexpr FeatureAddressType kDummyAddress = {
    .device      = nullptr,
    .entity      = kDummyEntity,
    .entity_size = 1,
    .feature     = &kDummyFeatureId,
};

struct CallbackRecord {
  int call_count{0};
  const ReplyMessage* received_msg{reinterpret_cast<const ReplyMessage*>(0x1)};
  const void* received_function_data{reinterpret_cast<const void*>(0x1)};
};

static void RecordingCallback(
    const ReplyMessage* reply_msg,
    const FeatureAddressType* remote_feature_addr,
    EebusError err,
    void* ctx
) {
  UNUSED(remote_feature_addr);
  UNUSED(err);

  auto* record = static_cast<CallbackRecord*>(ctx);

  record->received_msg           = reply_msg;
  record->received_function_data = reply_msg->function_data;
  ++record->call_count;
}

class PendingReplyContainerTest : public ::testing::Test {
 protected:
  void SetUp() override {
    container_ = MakePendingReplyContainer();
    ASSERT_NE(container_, nullptr);
  }

  void TearDown() override {
    container_.reset();
    CheckForMemoryLeaks();
  }

  PendingReplyContainerPtr container_{nullptr, PendingReplyContainerDelete};
};

TEST_F(PendingReplyContainerTest, CreateSucceeds) {
  EXPECT_NE(container_, nullptr);
}

TEST_F(PendingReplyContainerTest, AddReturnsOk) {
  CallbackRecord rec;
  const EebusError err = PENDING_REPLY_CONTAINER_ADD(
      container_.get(),
      1,
      &kDummyAddress,
      kFunctionTypeNodeManagementDetailedDiscoveryData,
      nullptr,
      RecordingCallback,
      &rec
  );
  EXPECT_EQ(err, kEebusErrorOk);
}

TEST_F(PendingReplyContainerTest, ProcessFiresMatchingCallback) {
  CallbackRecord rec;
  PENDING_REPLY_CONTAINER_ADD(
      container_.get(),
      10,
      &kDummyAddress,
      kFunctionTypeNodeManagementDetailedDiscoveryData,
      nullptr,
      RecordingCallback,
      &rec
  );

  ReplyMessage msg{.msg_cnt_ref = 10};
  PENDING_REPLY_CONTAINER_PROCESS(container_.get(), &msg, kEebusErrorOk);

  EXPECT_EQ(rec.call_count, 1);
  EXPECT_EQ(rec.received_msg, &msg);
}

TEST_F(PendingReplyContainerTest, ProcessIgnoresNonMatchingMsgCntRef) {
  CallbackRecord rec;
  PENDING_REPLY_CONTAINER_ADD(
      container_.get(),
      10,
      &kDummyAddress,
      kFunctionTypeNodeManagementDetailedDiscoveryData,
      nullptr,
      RecordingCallback,
      &rec
  );

  ReplyMessage msg{.msg_cnt_ref = 99};
  PENDING_REPLY_CONTAINER_PROCESS(container_.get(), &msg, kEebusErrorOk);

  EXPECT_EQ(rec.call_count, 0);
}

TEST_F(PendingReplyContainerTest, ProcessFiresOnlyMatchingEntry) {
  CallbackRecord rec_a;
  CallbackRecord rec_b;
  PENDING_REPLY_CONTAINER_ADD(
      container_.get(),
      1,
      &kDummyAddress,
      kFunctionTypeNodeManagementDetailedDiscoveryData,
      nullptr,
      RecordingCallback,
      &rec_a
  );
  PENDING_REPLY_CONTAINER_ADD(
      container_.get(),
      2,
      &kDummyAddress,
      kFunctionTypeNodeManagementDetailedDiscoveryData,
      nullptr,
      RecordingCallback,
      &rec_b
  );

  ReplyMessage msg{.msg_cnt_ref = 1};
  PENDING_REPLY_CONTAINER_PROCESS(container_.get(), &msg, kEebusErrorOk);

  EXPECT_EQ(rec_a.call_count, 1);
  EXPECT_EQ(rec_b.call_count, 0);
}

TEST_F(PendingReplyContainerTest, ProcessRemovesEntryAfterFiring) {
  CallbackRecord rec;
  PENDING_REPLY_CONTAINER_ADD(
      container_.get(),
      5,
      &kDummyAddress,
      kFunctionTypeNodeManagementDetailedDiscoveryData,
      nullptr,
      RecordingCallback,
      &rec
  );

  ReplyMessage msg{.msg_cnt_ref = 5};
  PENDING_REPLY_CONTAINER_PROCESS(container_.get(), &msg, kEebusErrorOk);
  PENDING_REPLY_CONTAINER_PROCESS(container_.get(), &msg, kEebusErrorOk);

  EXPECT_EQ(rec.call_count, 1);
}

TEST_F(PendingReplyContainerTest, TickDoesNotFireBeforeTimeout) {
  CallbackRecord rec;
  PENDING_REPLY_CONTAINER_ADD(
      container_.get(),
      1,
      &kDummyAddress,
      kFunctionTypeNodeManagementDetailedDiscoveryData,
      nullptr,
      RecordingCallback,
      &rec
  );

  for (size_t i = 0; i < kMaxResponseTimeSec - 1; ++i) {
    PENDING_REPLY_CONTAINER_TICK(container_.get());
  }

  EXPECT_EQ(rec.call_count, 0);
}

TEST_F(PendingReplyContainerTest, TickFiresWithNullOnTimeout) {
  CallbackRecord rec;
  PENDING_REPLY_CONTAINER_ADD(
      container_.get(),
      1,
      &kDummyAddress,
      kFunctionTypeNodeManagementDetailedDiscoveryData,
      nullptr,
      RecordingCallback,
      &rec
  );

  for (size_t i = 0; i < kMaxResponseTimeSec + 1; ++i) {
    PENDING_REPLY_CONTAINER_TICK(container_.get());
  }

  EXPECT_EQ(rec.call_count, 1);
  EXPECT_NE(rec.received_msg, nullptr);
  EXPECT_EQ(rec.received_function_data, nullptr);
}

TEST_F(PendingReplyContainerTest, TickRemovesExpiredEntry) {
  CallbackRecord rec;
  PENDING_REPLY_CONTAINER_ADD(
      container_.get(),
      1,
      &kDummyAddress,
      kFunctionTypeNodeManagementDetailedDiscoveryData,
      nullptr,
      RecordingCallback,
      &rec
  );

  for (size_t i = 0; i < kMaxResponseTimeSec; ++i) {
    PENDING_REPLY_CONTAINER_TICK(container_.get());
  }

  // Tick again — expired entry is already gone, so callback is not called again
  for (size_t i = 0; i < 5; ++i) {
    PENDING_REPLY_CONTAINER_TICK(container_.get());
  }

  EXPECT_EQ(rec.call_count, 1);
}

TEST_F(PendingReplyContainerTest, TickExpiresAllEntriesIndependently) {
  CallbackRecord rec_a;
  CallbackRecord rec_b;
  PENDING_REPLY_CONTAINER_ADD(
      container_.get(),
      1,
      &kDummyAddress,
      kFunctionTypeNodeManagementDetailedDiscoveryData,
      nullptr,
      RecordingCallback,
      &rec_a
  );
  PENDING_REPLY_CONTAINER_ADD(
      container_.get(),
      2,
      &kDummyAddress,
      kFunctionTypeNodeManagementDetailedDiscoveryData,
      nullptr,
      RecordingCallback,
      &rec_b
  );

  for (size_t i = 0; i < kMaxResponseTimeSec + 1; ++i) {
    PENDING_REPLY_CONTAINER_TICK(container_.get());
  }

  EXPECT_EQ(rec_a.call_count, 1);
  EXPECT_EQ(rec_b.call_count, 1);
  EXPECT_EQ(rec_a.received_function_data, nullptr);
  EXPECT_EQ(rec_b.received_function_data, nullptr);
}

TEST_F(PendingReplyContainerTest, ProcessAfterTickDoesNotFireExpired) {
  CallbackRecord rec;
  PENDING_REPLY_CONTAINER_ADD(
      container_.get(),
      7,
      &kDummyAddress,
      kFunctionTypeNodeManagementDetailedDiscoveryData,
      nullptr,
      RecordingCallback,
      &rec
  );

  for (size_t i = 0; i < kMaxResponseTimeSec; ++i) {
    PENDING_REPLY_CONTAINER_TICK(container_.get());
  }

  ReplyMessage msg{.msg_cnt_ref = 7};
  PENDING_REPLY_CONTAINER_PROCESS(container_.get(), &msg, kEebusErrorOk);

  EXPECT_EQ(rec.call_count, 1);  // only the timeout fire, not the process fire
}

TEST_F(PendingReplyContainerTest, DeleteWithPendingItemsReleasesMemory) {
  CallbackRecord rec;
  PENDING_REPLY_CONTAINER_ADD(
      container_.get(),
      1,
      &kDummyAddress,
      kFunctionTypeNodeManagementDetailedDiscoveryData,
      nullptr,
      RecordingCallback,
      &rec
  );
  PENDING_REPLY_CONTAINER_ADD(
      container_.get(),
      2,
      &kDummyAddress,
      kFunctionTypeNodeManagementDetailedDiscoveryData,
      nullptr,
      RecordingCallback,
      &rec
  );
}
