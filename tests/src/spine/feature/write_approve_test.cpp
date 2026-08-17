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

using std::literals::string_view_literals::operator""sv;

struct WriteApproveTestInput {
  std::string_view description = ""sv;
  bool send_err                = false;

  std::vector<WriteApprovalCallback> cbs = {};
};

std::ostream& operator<<(std::ostream& os, WriteApproveTestInput test_input) {
  return os << test_input.description;
}

class WriteApproveTests : public WriteApproveTestSuite, public ::testing::WithParamInterface<WriteApproveTestInput> {};

TEST_P(WriteApproveTests, WriteApproveTests) {
  FeatureLocal* feature_local = FEATURE_LOCAL(feature_local_object_.get());

  // Arrange: Add write approval callback to feature local
  for (const auto& cb : GetParam().cbs) {
    EebusError ret = FEATURE_LOCAL_ADD_WRITE_APPROVAL_CALLBACK(feature_local_object_.get(), cb, feature_local);
    EXPECT_EQ(ret, kEebusErrorOk);
  }

  EXPECT_EQ(VectorGetSize(&feature_local->wr_approval_cbs), GetParam().cbs.size());

  // Expect message parameters retrieved during handling
  EXPECT_CALL(*feature_remote_mock_->gmock, GetAddress(::testing::_))
      .Times(1)
      .WillOnce(::testing::Return(new FeatureAddressType()));

  EXPECT_CALL(*device_local_mock_->gmock, GetRemoteDeviceWithAddress(::testing::_, ::testing::_))
      .Times(1)
      .WillOnce(::testing::Return(DEVICE_REMOTE_OBJECT(device_remote_mock_.get())));

  EXPECT_CALL(*device_remote_mock_->gmock, GetFeatureWithAddress(::testing::_, ::testing::_))
      .Times(1)
      .WillOnce(::testing::Return(FEATURE_REMOTE_OBJECT(feature_remote_mock_.get())));

  // Setup expectations for sender error response or entity retrieval (process write request)
  if (GetParam().send_err) {
    EXPECT_CALL(*feature_remote_mock_->gmock, GetEntity(::testing::_))
        .Times(1)
        .WillOnce(::testing::Return(ENTITY_REMOTE_OBJECT(entity_remote_mock_.get())));
    EXPECT_CALL(*sender_mock_->gmock, ResultError(::testing::_, ::testing::_, ::testing::_, ::testing::_))
        .Times(1)
        .WillOnce(::testing::Return(kEebusErrorOk));
  } else {
    EXPECT_CALL(*feature_remote_mock_->gmock, GetEntity(FEATURE_REMOTE_OBJECT(feature_remote_mock_.get())))
        .WillRepeatedly(::testing::Return(ENTITY_REMOTE_OBJECT(entity_remote_mock_.get())));
  }

  // Create message to be approved or denied
  Message msg = CreateTestMessage(kFunctionTypeActuatorLevelData, 12345);

  // Verify no pending write requests before handling message
  EXPECT_EQ(PENDING_WRITE_REQUEST_CONTAINER_GET_SIZE(feature_local->pending_write_requests), 0);

  // Act: Handle messages
  EebusError ret = FEATURE_LOCAL_HANDLE_MESSAGE(feature_local_object_.get(), &msg);
  EXPECT_EQ(ret, kEebusErrorOk);

  // Assert: Verify with expected return value and number of pending write requests
  EXPECT_EQ(PENDING_WRITE_REQUEST_CONTAINER_GET_SIZE(feature_local->pending_write_requests), 0);
}

INSTANTIATE_TEST_SUITE_P(
    WriteApproveTests,
    WriteApproveTests,
    ::testing::Values(
        WriteApproveTestInput{
            .description = "Try to approve single write request before timeout"sv,
            .send_err    = false,
            .cbs         = {TryApproveWriteRequestBeforeTimeoutCallback},
},
        WriteApproveTestInput{
            .description = "Try to approve single write request after timeout"sv,
            .send_err    = true,
            .cbs         = {TryApproveWriteRequestAfterTimeoutCallback},
        },
        WriteApproveTestInput{
            .description = "Deny single write request"sv,
            .send_err    = true,
            .cbs         = {DenyWriteRequestCallback},
        },
        WriteApproveTestInput{
            .description = "Message approved multiple times"sv,
            .send_err    = false,
            .cbs         = {TryApproveShouldPass, TryApproveShouldPass},
        },
        WriteApproveTestInput{
            .description = "Message deny fails after successful message approve"sv,
            .send_err    = true,
            .cbs         = {TryApproveShouldPass, DenyShouldPass},
        },
        WriteApproveTestInput{
            .description = "Message approve fails after message is denied"sv,
            .send_err    = true,
            .cbs         = {DenyShouldPass, TryApproveShouldFail},
        },
        WriteApproveTestInput{
            .description = "Second message deny attempt fails"sv,
            .send_err    = true,
            .cbs         = {DenyShouldPass, DenyShouldFail},
        }
    )
);
