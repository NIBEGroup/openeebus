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

#include <chrono>
#include <iomanip>

#include "src/common/array_util.h"
#include "src/common/eebus_arguments.h"
#include "src/spine/device/device_local.h"
#include "src/spine/device/device_local_internal.h"
#include "src/spine/entity/entity_local.h"
#include "tests/src/json.h"
#include "tests/src/memory_leak.inc"
#include "tests/src/use_case/use_case_test_fixture.h"

using testing::_;
using testing::Args;
using testing::Invoke;
using testing::Return;
using testing::WithArgs;

UseCaseTestFixture::UseCaseTestFixture(const char* type, const char* vendor, const char* serial_num, bool log_messages)
    : log_messages_{log_messages} {
  device_info_.reset(EebusDeviceInfoCreate(type, vendor, "TestBrand", "TestModel", serial_num, "TestShipId"));
}

void LogMessage(const char* direction, const uint8_t* msg, size_t msg_size) {
  UNUSED(msg_size);
  auto now  = std::chrono::system_clock::now();
  auto time = std::chrono::system_clock::to_time_t(now);
  auto ms   = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
  std::cout << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S") << "." << std::setfill('0') << std::setw(3)
            << ms.count() << " " << direction << " ";
  std::string_view s(reinterpret_cast<const char*>(msg), msg_size);
  std::cout << s << std::endl;
}

void UseCaseTestFixture::LogMessageSend(const uint8_t* msg, size_t msg_size) {
  LogMessage("Send message", msg, msg_size);
}

void UseCaseTestFixture::SetUp() {
  data_write_mock_.reset(DataWriterMockCreate());
  device_local_.reset(DeviceLocalCreate(device_info_.get(), &kFeatureSet));

  // Create entities, setup use cases and entities to the SPINE device
  SetUpUseCase();

  data_reader_
      = DEVICE_LOCAL_SETUP_REMOTE_DEVICE(device_local_.get(), kRemoteSki, DATA_WRITER_OBJECT(data_write_mock_.get()));
}

void UseCaseTestFixture::TearDown() {
  DEVICE_LOCAL_REMOVE_REMOTE_DEVICE(device_local_.get(), kRemoteSki);
  TearDownUseCase();

  device_local_.reset();

  EXPECT_CALL(*data_write_mock_->gmock, Destruct(_)).WillOnce(Return());
  data_write_mock_.reset();

  device_info_.reset();

  EXPECT_EQ(heap_used, 0);
  CheckForMemoryLeaks();
}

void UseCaseTestFixture::ExpectSendMessage(const char* expected_json) {
  if (log_messages_ == kUseCaseLogMessagesEnabled) {
    EXPECT_CALL(*data_write_mock_->gmock, WriteMessage(_, _, _))
        .With(Args<1, 2>(JsonMsgEq(expected_json)))
        .WillOnce(WithArgs<1, 2>(Invoke(LogMessageSend)));
  } else {
    EXPECT_CALL(*data_write_mock_->gmock, WriteMessage(_, _, _))
        .With(Args<1, 2>(JsonMsgEq(expected_json)))
        .WillOnce(Return());
  }
}

void UseCaseTestFixture::HandleTick() {
  DeviceLocal1sTickCallback(device_local_.get());
  EXPECT_EQ(HandleQueueMessage(device_local_.get()), kEebusErrorOk);
}

void UseCaseTestFixture::HandleMessage(const char* msg_string) {
  MessageBuffer msg_buf;

  const char* const s = JsonUnformat(msg_string);

  uint8_t* const msg    = reinterpret_cast<uint8_t*>(const_cast<char*>(s));
  const size_t msg_size = strlen(s) + 1;

  if (log_messages_ == kUseCaseLogMessagesEnabled) {
    LogMessage("Received message", msg, msg_size);
  }

  MessageBufferInitWithDeallocator(&msg_buf, msg, msg_size, JsonFree);

  DATA_READER_HANDLE_MESSAGE(data_reader_, &msg_buf);
  MessageBufferRelease(&msg_buf);
  EXPECT_EQ(HandleQueueMessage(device_local_.get()), kEebusErrorOk);
}
