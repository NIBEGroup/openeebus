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
 * @file write_approve_test_suite.h
 * @brief Write approval test suite
 */

#ifndef TESTS_SRC_SPINE_FEATURE_WRITE_APPROVE_TEST_SUITE_H
#define TESTS_SRC_SPINE_FEATURE_WRITE_APPROVE_TEST_SUITE_H

#include <gtest/gtest.h>

#include "mocks/spine/device/device_local_mock.h"
#include "mocks/spine/device/device_remote_mock.h"
#include "mocks/spine/device/sender_mock.h"
#include "mocks/spine/entity/entity_local_mock.h"
#include "mocks/spine/entity/entity_remote_mock.h"
#include "mocks/spine/feature/feature_remote_mock.h"
#include "src/spine/api/feature_local_interface.h"
#include "src/spine/api/message.h"
#include "src/spine/api/pending_write_request_interface.h"
#include "src/spine/events/events.h"
#include "src/spine/feature/feature_local.h"
#include "src/spine/model/model.h"

class WriteApproveTestSuite : public testing::Test {
 public:
  void SetUp() override;
  void TearDown() override;

  // Helper method to create a test message
  Message CreateTestMessage(FunctionType data_type_id, uint64_t msg_counter);

 protected:
  std::unique_ptr<EventsManagerObject, decltype(&EventsManagerDelete)> events_manager_{nullptr, EventsManagerDelete};
  std::unique_ptr<DeviceLocalMock, decltype(&DeviceLocalMockDelete)> device_local_mock_{nullptr, DeviceLocalMockDelete};
  std::unique_ptr<EntityLocalMock, decltype(&EntityLocalMockDelete)> entity_local_mock_{nullptr, EntityLocalMockDelete};

  std::unique_ptr<DeviceRemoteMock, decltype(&DeviceRemoteMockDelete)> device_remote_mock_{
      nullptr,
      DeviceRemoteMockDelete
  };

  std::unique_ptr<EntityRemoteMock, decltype(&EntityRemoteMockDelete)> entity_remote_mock_{
      nullptr,
      EntityRemoteMockDelete
  };

  std::unique_ptr<FeatureRemoteMock, decltype(&FeatureRemoteMockDelete)> feature_remote_mock_{
      nullptr,
      FeatureRemoteMockDelete
  };

  std::unique_ptr<SenderMock, decltype(&SenderMockDelete)> sender_mock_{nullptr, SenderMockDelete};
  std::unique_ptr<EntityAddressType, decltype(&EntityAddressDelete)> entity_address_{nullptr, EntityAddressDelete};
  std::unique_ptr<FeatureLocalObject, decltype(&FeatureLocalDelete)> feature_local_object_{nullptr, FeatureLocalDelete};
  std::unique_ptr<void, std::function<void(void*)>> spine_data_;
  std::unique_ptr<CmdType> cmd_mock_;
  uint64_t msg_counter_;
  HeaderType header_mock_;
};

void TryApproveWriteRequestBeforeTimeoutCallback(const Message* msg, void* ctx);
void TryApproveWriteRequestAfterTimeoutCallback(const Message* msg, void* ctx);
void DenyWriteRequestCallback(const Message* msg, void* ctx);

void TryApproveShouldPass(const Message* msg, void* ctx);
void TryApproveShouldFail(const Message* msg, void* ctx);
void DenyShouldPass(const Message* msg, void* ctx);
void DenyShouldFail(const Message* msg, void* ctx);

#endif  // TESTS_SRC_SPINE_FEATURE_WRITE_APPROVE_TEST_SUITE_H
