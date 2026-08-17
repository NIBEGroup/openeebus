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

#include "tests/src/spine/feature/write_approve_test_suite.h"
#include "src/common/api/eebus_timer_interface.h"
#include "src/common/eebus_arguments.h"
#include "src/spine/model/result_types.h"
#include "tests/src/memory_leak.inc"

constexpr uint8_t kMaxResponseTimeSec = TIME_MS_TO_S(kDefaultMaxResponseDelayMs);

void WriteApproveTestSuite::SetUp() {
  events_manager_.reset(EventsManagerCreate());
  device_local_mock_.reset(DeviceLocalMockCreate());
  entity_local_mock_.reset(EntityLocalMockCreate());
  device_remote_mock_.reset(DeviceRemoteMockCreate());
  entity_remote_mock_.reset(EntityRemoteMockCreate());
  feature_remote_mock_.reset(FeatureRemoteMockCreate());
  sender_mock_.reset(SenderMockCreate());

  const uint32_t entity_id = 1;
  entity_address_.reset(EntityAddressCreate("mock_address", &entity_id, 1));

  EXPECT_CALL(*entity_local_mock_->gmock, GetAddress(ENTITY_OBJECT(entity_local_mock_.get())))
      .WillRepeatedly(::testing::Return(entity_address_.get()));

  EXPECT_CALL(*device_remote_mock_->gmock, GetSki(DEVICE_REMOTE_OBJECT(device_remote_mock_.get())))
      .WillRepeatedly(::testing::Return("mock_remote_device_ski"));

  EXPECT_CALL(*device_remote_mock_->gmock, GetSender(DEVICE_REMOTE_OBJECT(device_remote_mock_.get())))
      .WillRepeatedly(::testing::Return(SENDER_OBJECT(sender_mock_.get())));

  EXPECT_CALL(*feature_remote_mock_->gmock, GetDevice(FEATURE_REMOTE_OBJECT(feature_remote_mock_.get())))
      .WillRepeatedly(::testing::Return(DEVICE_REMOTE_OBJECT(device_remote_mock_.get())));

  EXPECT_CALL(*entity_local_mock_->gmock, GetDevice(ENTITY_LOCAL_OBJECT(entity_local_mock_.get())))
      .WillRepeatedly(::testing::Return(DEVICE_LOCAL_OBJECT(device_local_mock_.get())));

  EXPECT_CALL(*device_local_mock_->gmock, GetEventsManager(DEVICE_LOCAL_OBJECT(device_local_mock_.get())))
      .WillRepeatedly(::testing::Return(events_manager_.get()));

  EXPECT_CALL(*device_local_mock_->gmock, NotifySubscribers(::testing::_, ::testing::_, ::testing::_))
      .WillRepeatedly(::testing::Return());

  feature_local_object_.reset(FeatureLocalCreate(
      0,
      ENTITY_LOCAL_OBJECT(entity_local_mock_.get()),
      kFeatureTypeTypeActuatorLevel,
      kRoleTypeServer
  ));
}

void WriteApproveTestSuite::TearDown() {
  EXPECT_CALL(*device_local_mock_->gmock, Destruct(DEVICE_OBJECT(device_local_mock_.get())));
  device_local_mock_.reset();

  EXPECT_CALL(*entity_local_mock_->gmock, Destruct(ENTITY_OBJECT(entity_local_mock_.get())));
  entity_local_mock_.reset();

  EXPECT_CALL(*device_remote_mock_->gmock, Destruct(DEVICE_OBJECT(device_remote_mock_.get())));
  device_remote_mock_.reset();

  EXPECT_CALL(*entity_remote_mock_->gmock, Destruct(ENTITY_OBJECT(entity_remote_mock_.get())));
  entity_remote_mock_.reset();

  EXPECT_CALL(*feature_remote_mock_->gmock, Destruct(FEATURE_OBJECT(feature_remote_mock_.get())));
  feature_remote_mock_.reset();

  EXPECT_CALL(*sender_mock_->gmock, Destruct(SENDER_OBJECT(sender_mock_.get())));
  sender_mock_.reset();

  feature_local_object_.reset();
  events_manager_.reset();
  entity_address_.reset();
  spine_data_.reset();
  cmd_mock_.reset();

  CheckForMemoryLeaks();
}

Message WriteApproveTestSuite::CreateTestMessage(FunctionType data_type_id, uint64_t msg_counter) {
  // Create spine data with deleter
  spine_data_ = std::unique_ptr<void, std::function<void(void*)>>(
      ModelFunctionDataCreateEmpty(data_type_id),
      [data_type_id](void* p) -> void { ModelFunctionDataDelete(data_type_id, p); }
  );

  // Create mock command with spine data
  cmd_mock_ = std::make_unique<CmdType>(CmdType{
      .data_choice         = spine_data_.get(),
      .data_choice_type_id = data_type_id,
  });

  // Create mock request header with required fields (as member variables)
  msg_counter_ = msg_counter;
  header_mock_ = HeaderType{
      .msg_cnt = &msg_counter_,
  };

  // Get mock objects
  DeviceRemoteObject* device_remote   = DEVICE_REMOTE_OBJECT(device_remote_mock_.get());
  EntityRemoteObject* entity_remote   = ENTITY_REMOTE_OBJECT(entity_remote_mock_.get());
  FeatureRemoteObject* feature_remote = FEATURE_REMOTE_OBJECT(feature_remote_mock_.get());

  // Create and return the message
  return Message{
      .request_header = &header_mock_,
      .cmd_classifier = kCommandClassifierTypeWrite,
      .cmd            = cmd_mock_.get(),
      .filter_partial = nullptr,
      .filter_delete  = nullptr,
      .feature_remote = feature_remote,
      .entity_remote  = entity_remote,
      .device_remote  = device_remote,
  };
}

// Test callbacks

void TryApproveWriteRequestBeforeTimeoutCallback(const Message* msg, void* ctx) {
  FeatureLocal* feature_local = FEATURE_LOCAL(ctx);

  // Get the SKI from the device remote
  const char* ski = DEVICE_REMOTE_GET_SKI(msg->device_remote);

  // Get the message counter from the request header
  MsgCounterType msg_cnt = *msg->request_header->msg_cnt;

  // There should be one pending write request
  EXPECT_EQ(PENDING_WRITE_REQUEST_CONTAINER_GET_SIZE(feature_local->pending_write_requests), 1);

  // User waits just before the maximum response time to pass
  for (int i = 0; i < (kMaxResponseTimeSec - 1); ++i) {
    FEATURE_LOCAL_TICK(FEATURE_LOCAL_OBJECT(ctx));
  }

  // Check that all pending write requests are not expired yet
  EXPECT_EQ(PENDING_WRITE_REQUEST_CONTAINER_GET_SIZE(feature_local->pending_write_requests), 1);

  // Check remaining time for the pending write request
  PendingWriteRequestObject* pwr_before
      = PENDING_WRITE_REQUEST_CONTAINER_FIND(feature_local->pending_write_requests, ski, msg_cnt);
  ASSERT_NE(pwr_before, nullptr);
  EXPECT_EQ(PENDING_WRITE_REQUEST_GET_REMAINING_TIME(pwr_before), 1);

  // Approve write request before timeout
  EebusError ret = FEATURE_LOCAL_TRY_APPROVE_WRITE(FEATURE_LOCAL_OBJECT(ctx), ski, msg_cnt);
  EXPECT_EQ(ret, kEebusErrorOk);
}

void TryApproveWriteRequestAfterTimeoutCallback(const Message* msg, void* ctx) {
  FeatureLocal* feature_local = FEATURE_LOCAL(FEATURE_LOCAL_OBJECT(ctx));

  // Get the SKI from the device remote
  const char* ski = DEVICE_REMOTE_GET_SKI(msg->device_remote);

  // Get the message counter from the request header
  MsgCounterType msg_cnt = *msg->request_header->msg_cnt;

  // Check that the pending write request is not expired yet
  PendingWriteRequestObject* pwr_after
      = PENDING_WRITE_REQUEST_CONTAINER_FIND(feature_local->pending_write_requests, ski, msg_cnt);
  ASSERT_NE(pwr_after, nullptr);
  EXPECT_EQ(PENDING_WRITE_REQUEST_HAS_EXPIRED(pwr_after), false);
  EXPECT_EQ(PENDING_WRITE_REQUEST_GET_REMAINING_TIME(pwr_after), kMaxResponseTimeSec);

  // There should be one pending write request
  EXPECT_EQ(PENDING_WRITE_REQUEST_CONTAINER_GET_SIZE(feature_local->pending_write_requests), 1);

  // User waits for the maximum response time to pass
  for (int i = 0; i < (kMaxResponseTimeSec + 1); ++i) {
    FEATURE_LOCAL_TICK(FEATURE_LOCAL_OBJECT(ctx));
  }

  // After waiting for the maximum response time to pass, the write request cannot be approved anymore
  EebusError ret = FEATURE_LOCAL_TRY_APPROVE_WRITE(FEATURE_LOCAL_OBJECT(ctx), ski, msg_cnt);
  EXPECT_EQ(ret, kEebusErrorNoChange);
}

void DenyWriteRequestCallback(const Message* msg, void* ctx) {
  FeatureLocal* feature_local = FEATURE_LOCAL(ctx);

  // Get the SKI from the device remote
  const char* ski = DEVICE_REMOTE_GET_SKI(msg->device_remote);

  // Get the message counter from the request header
  MsgCounterType msg_cnt = *msg->request_header->msg_cnt;

  // Verify that there is one pending write request
  EXPECT_EQ(PENDING_WRITE_REQUEST_CONTAINER_GET_SIZE(feature_local->pending_write_requests), 1);

  // Deny write request
  const ErrorType err = {
      .error_number = kErrorNumberTypeCommandRejected,
      .description  = "Write request denied by user",
  };
  EebusError ret = FEATURE_LOCAL_DENY_WRITE(FEATURE_LOCAL_OBJECT(ctx), ski, msg_cnt, &err);
  EXPECT_EQ(ret, kEebusErrorOk);
}

void TryApproveShouldPass(const Message* msg, void* ctx) {
  const char* ski = DEVICE_REMOTE_GET_SKI(msg->device_remote);

  MsgCounterType msg_cnt = *msg->request_header->msg_cnt;

  EebusError ret = FEATURE_LOCAL_TRY_APPROVE_WRITE(FEATURE_LOCAL_OBJECT(ctx), ski, msg_cnt);
  EXPECT_EQ(ret, kEebusErrorOk);
}

void TryApproveShouldFail(const Message* msg, void* ctx) {
  const char* ski = DEVICE_REMOTE_GET_SKI(msg->device_remote);

  MsgCounterType msg_cnt = *msg->request_header->msg_cnt;

  EebusError ret = FEATURE_LOCAL_TRY_APPROVE_WRITE(FEATURE_LOCAL_OBJECT(ctx), ski, msg_cnt);
  EXPECT_EQ(ret, kEebusErrorNoChange);
}

void DenyShouldPass(const Message* msg, void* ctx) {
  const char* ski = DEVICE_REMOTE_GET_SKI(msg->device_remote);

  MsgCounterType msg_cnt = *msg->request_header->msg_cnt;

  const ErrorType err = {
      .error_number = kErrorNumberTypeCommandRejected,
      .description  = "Write request denied by user",
  };
  EebusError ret = FEATURE_LOCAL_DENY_WRITE(FEATURE_LOCAL_OBJECT(ctx), ski, msg_cnt, &err);
  EXPECT_EQ(ret, kEebusErrorOk);
}

void DenyShouldFail(const Message* msg, void* ctx) {
  const char* ski = DEVICE_REMOTE_GET_SKI(msg->device_remote);

  MsgCounterType msg_cnt = *msg->request_header->msg_cnt;

  const ErrorType err = {
      .error_number = kErrorNumberTypeCommandRejected,
      .description  = "Write request denied by user",
  };
  EebusError ret = FEATURE_LOCAL_DENY_WRITE(FEATURE_LOCAL_OBJECT(ctx), ski, msg_cnt, &err);
  EXPECT_EQ(ret, kEebusErrorNoChange);
}
