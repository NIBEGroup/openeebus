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
#include "src/spine/api/pending_result_interface.h"
#include "src/spine/feature/feature.h"
#include "src/spine/feature/pending_result.h"
#include "tests/src/memory_leak.inc"

static constexpr uint8_t kMaxResponseTimeSec = TIME_MS_TO_S(kDefaultMaxResponseDelayMs);

using PendingResultPtr = std::unique_ptr<PendingResultObject, decltype(&PendingResultDelete)>;

static PendingResultPtr MakePendingResult(MsgCounterType msg_cnt, ResultMessageCallback cb, void* ctx) {
  return PendingResultPtr(
      PendingResultCreate(msg_cnt, kFunctionTypeLoadControlLimitListData, nullptr, cb, ctx),
      PendingResultDelete
  );
}

class PendingResultTest : public ::testing::Test {
 protected:
  int call_count_ = 0;

  std::unique_ptr<ResultMessage> received_msg_;
  EebusError received_err_ = kEebusErrorOk;

  static void TestCallback(
      const ResultMessage* result_msg,
      const FeatureAddressType* remote_feature_addr,
      EebusError err,
      void* ctx
  ) {
    UNUSED(remote_feature_addr);

    auto* const self = static_cast<PendingResultTest*>(ctx);

    self->received_msg_ = (result_msg != nullptr) ? std::make_unique<ResultMessage>(*result_msg) : nullptr;
    self->received_err_ = err;
    ++self->call_count_;
  }

  void TearDown() override {
    CheckForMemoryLeaks();
  }
};

TEST_F(PendingResultTest, CreateStoresCorrectMsgCntRef) {
  constexpr MsgCounterType kMsgCnt = 42;

  auto pending_result = MakePendingResult(kMsgCnt, TestCallback, this);
  ASSERT_NE(pending_result, nullptr);

  EXPECT_EQ(PENDING_RESULT_GET_MSG_CNT_REF(pending_result.get()), kMsgCnt);
}

TEST_F(PendingResultTest, CreateNotExpiredInitially) {
  auto pending_result = MakePendingResult(1, TestCallback, this);
  ASSERT_NE(pending_result, nullptr);
  EXPECT_FALSE(PENDING_RESULT_HAS_EXPIRED(pending_result.get()));
}

TEST_F(PendingResultTest, UpdateTimeNotExpiredBeforeTimeout) {
  auto pending_result = MakePendingResult(1, TestCallback, this);
  ASSERT_NE(pending_result, nullptr);

  for (size_t i = 0; i < kMaxResponseTimeSec - 1; ++i) {
    PENDING_RESULT_UPDATE_TIME(pending_result.get());
  }

  EXPECT_FALSE(PENDING_RESULT_HAS_EXPIRED(pending_result.get()));
}

TEST_F(PendingResultTest, UpdateTimeExpiresAtTimeout) {
  auto pending_result = MakePendingResult(1, TestCallback, this);
  ASSERT_NE(pending_result, nullptr);

  for (size_t i = 0; i < kMaxResponseTimeSec; ++i) {
    PENDING_RESULT_UPDATE_TIME(pending_result.get());
  }

  EXPECT_TRUE(PENDING_RESULT_HAS_EXPIRED(pending_result.get()));
}

TEST_F(PendingResultTest, UpdateTimeSafeAfterExpiry) {
  auto pending_result = MakePendingResult(1, TestCallback, this);
  ASSERT_NE(pending_result, nullptr);

  for (size_t i = 0; i < kMaxResponseTimeSec + 5; ++i) {
    PENDING_RESULT_UPDATE_TIME(pending_result.get());
  }

  EXPECT_TRUE(PENDING_RESULT_HAS_EXPIRED(pending_result.get()));
}

TEST_F(PendingResultTest, FireInvokesCallbackWithResultMsg) {
  auto pending_result = MakePendingResult(1, TestCallback, this);
  ASSERT_NE(pending_result, nullptr);

  ResultMessage result_msg = {};
  PENDING_RESULT_FIRE(pending_result.get(), &result_msg, kEebusErrorOk);

  EXPECT_EQ(call_count_, 1);
  ASSERT_NE(received_msg_, nullptr);
  EXPECT_EQ(received_err_, kEebusErrorOk);
  EXPECT_EQ(received_msg_->function_type, kFunctionTypeLoadControlLimitListData);
}

TEST_F(PendingResultTest, FireWithNullSignalsTimeout) {
  auto pending_result = MakePendingResult(1, TestCallback, this);
  ASSERT_NE(pending_result, nullptr);

  PENDING_RESULT_FIRE(pending_result.get(), nullptr, kEebusErrorTime);

  EXPECT_EQ(call_count_, 1);
  ASSERT_NE(received_msg_, nullptr);
  EXPECT_EQ(received_err_, kEebusErrorTime);
  EXPECT_EQ(received_msg_->function_type, kFunctionTypeLoadControlLimitListData);
}

TEST_F(PendingResultTest, DeleteReleasesMemory) {
  auto pending_result = MakePendingResult(1, TestCallback, this);
  ASSERT_NE(pending_result, nullptr);
}
