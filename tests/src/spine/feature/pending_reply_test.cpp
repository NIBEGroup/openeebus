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
#include "src/spine/api/pending_reply_interface.h"
#include "src/spine/feature/feature.h"
#include "src/spine/feature/pending_reply.h"
#include "tests/src/memory_leak.inc"

static constexpr uint8_t kMaxResponseTimeSec = TIME_MS_TO_S(kDefaultMaxResponseDelayMs);

using PendingReplyPtr = std::unique_ptr<PendingReplyObject, decltype(&PendingReplyDelete)>;

static constexpr uint32_t kDummyFeatureId         = 1;
static constexpr uint32_t kDummyEntityId          = 1;
static constexpr const uint32_t* kDummyEntity[1]  = {&kDummyEntityId};
static constexpr FeatureAddressType kDummyAddress = {
    .device      = nullptr,
    .entity      = kDummyEntity,
    .entity_size = 1,
    .feature     = &kDummyFeatureId,
};

static PendingReplyPtr MakePendingReply(MsgCounterType msg_cnt, ReplyMessageCallback cb, void* ctx) {
  return PendingReplyPtr(
      PendingReplyCreate(msg_cnt, &kDummyAddress, kFunctionTypeNodeManagementDetailedDiscoveryData, nullptr, cb, ctx),
      PendingReplyDelete
  );
}

class PendingReplyTest : public ::testing::Test {
 protected:
  int call_count_ = 0;

  const ReplyMessage* received_msg_   = reinterpret_cast<const ReplyMessage*>(0x1);
  bool received_msg_non_null_         = false;
  const void* received_function_data_ = reinterpret_cast<const void*>(0x1);

  static void TestCallback(
      const ReplyMessage* reply_msg,
      const FeatureAddressType* remote_feature_addr,
      EebusError err,
      void* ctx
  ) {
    UNUSED(remote_feature_addr);
    UNUSED(err);

    auto* const self = static_cast<PendingReplyTest*>(ctx);

    self->received_msg_          = reply_msg;
    self->received_msg_non_null_ = (reply_msg != nullptr);
    if (reply_msg != nullptr) {
      self->received_function_data_ = reply_msg->function_data;
    }
    ++self->call_count_;
  }

  void TearDown() override {
    CheckForMemoryLeaks();
  }
};

TEST_F(PendingReplyTest, CreateStoresCorrectMsgCntRef) {
  constexpr MsgCounterType kMsgCnt = 42;

  auto pending_reply = MakePendingReply(kMsgCnt, TestCallback, this);
  ASSERT_NE(pending_reply, nullptr);

  EXPECT_EQ(PENDING_REPLY_GET_MSG_CNT_REF(pending_reply.get()), kMsgCnt);
}

TEST_F(PendingReplyTest, CreateNotExpiredInitially) {
  auto pending_reply = MakePendingReply(1, TestCallback, this);
  ASSERT_NE(pending_reply, nullptr);
  EXPECT_FALSE(PENDING_REPLY_HAS_EXPIRED(pending_reply.get()));
}

TEST_F(PendingReplyTest, UpdateTimeNotExpiredBeforeTimeout) {
  auto pending_reply = MakePendingReply(1, TestCallback, this);
  ASSERT_NE(pending_reply, nullptr);

  for (size_t i = 0; i < kMaxResponseTimeSec - 1; ++i) {
    PENDING_REPLY_UPDATE_TIME(pending_reply.get());
  }

  EXPECT_FALSE(PENDING_REPLY_HAS_EXPIRED(pending_reply.get()));
}

TEST_F(PendingReplyTest, UpdateTimeExpiresAtTimeout) {
  auto pending_reply = MakePendingReply(1, TestCallback, this);
  ASSERT_NE(pending_reply, nullptr);

  for (size_t i = 0; i < kMaxResponseTimeSec; ++i) {
    PENDING_REPLY_UPDATE_TIME(pending_reply.get());
  }

  EXPECT_TRUE(PENDING_REPLY_HAS_EXPIRED(pending_reply.get()));
}

TEST_F(PendingReplyTest, UpdateTimeSafeAfterExpiry) {
  auto pending_reply = MakePendingReply(1, TestCallback, this);
  ASSERT_NE(pending_reply, nullptr);

  for (size_t i = 0; i < kMaxResponseTimeSec + 5; ++i) {
    PENDING_REPLY_UPDATE_TIME(pending_reply.get());
  }

  EXPECT_TRUE(PENDING_REPLY_HAS_EXPIRED(pending_reply.get()));
}

TEST_F(PendingReplyTest, FireInvokesCallbackWithReplyMsg) {
  auto pending_reply = MakePendingReply(1, TestCallback, this);
  ASSERT_NE(pending_reply, nullptr);

  ReplyMessage reply_msg = {};
  PENDING_REPLY_FIRE(pending_reply.get(), &reply_msg, kEebusErrorOk);

  EXPECT_EQ(call_count_, 1);
  EXPECT_EQ(received_msg_, &reply_msg);
}

TEST_F(PendingReplyTest, FireWithNullSignalsTimeout) {
  auto pending_reply = MakePendingReply(1, TestCallback, this);
  ASSERT_NE(pending_reply, nullptr);

  PENDING_REPLY_FIRE(pending_reply.get(), nullptr, kEebusErrorTime);

  EXPECT_EQ(call_count_, 1);
  EXPECT_TRUE(received_msg_non_null_);
  EXPECT_EQ(received_function_data_, nullptr);
}

TEST_F(PendingReplyTest, DeleteReleasesMemory) {
  auto pending_reply = MakePendingReply(1, TestCallback, this);
  ASSERT_NE(pending_reply, nullptr);
}
