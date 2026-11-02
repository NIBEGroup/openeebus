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

#ifndef TESTS_UNIT_TESTS_USE_CASE_USE_CASE_TEST_FIXTURE_H
#define TESTS_UNIT_TESTS_USE_CASE_USE_CASE_TEST_FIXTURE_H

#include <memory>

#include <gtest/gtest.h>

#include "mocks/common/eebus_timer/eebus_timer_mock.h"
#include "mocks/ship/ship_connection/data_writer_mock.h"
#include "src/spine/device/device_local.h"
#include "tests/unit_tests/use_case/matchers.h"

class UseCaseTestFixture : public ::testing::Test {
 public:
  static constexpr bool kUseCaseLogMessagesEnabled  = true;
  static constexpr bool kUseCaseLogMessagesDisabled = false;

  UseCaseTestFixture(
      const char* type,
      const char* vendor,
      const char* serial_num,
      bool log_messages = kUseCaseLogMessagesDisabled
  );

 protected:
  static constexpr uint32_t kHeartbeatTimeout = 60;  // Timeout 60 seconds

  std::unique_ptr<DataWriterMock, decltype(&DataWriterMockDelete)> data_write_mock_{nullptr, DataWriterMockDelete};
  std::unique_ptr<DeviceLocalObject, decltype(&DeviceLocalDelete)> device_local_{nullptr, DeviceLocalDelete};
  DataReaderObject* data_reader_{nullptr};

  void SetUp() override;
  void TearDown() override;

  virtual void SetUpUseCase()    = 0;
  virtual void TearDownUseCase() = 0;
  bool IsLogMessagesEnabled() const {
    return log_messages_;
  }
  static void LogMessageSend(const uint8_t* msg, size_t msg_size);
  void ExpectSendMessage(const char* expected_json);
  void HandleTick();
  void HandleMessage(const char* msg);

 private:
  static constexpr NetworkManagementFeatureSetType kFeatureSet = kNetworkManagementFeatureSetTypeSmart;

  static constexpr char kRemoteSki[] = "0123456789abcdefedcb0123456789abcdefedcb";

  bool log_messages_{false};
  std::unique_ptr<EebusDeviceInfo, decltype(&EebusDeviceInfoDelete)> device_info_{nullptr, EebusDeviceInfoDelete};
};

#endif  // TESTS_UNIT_TESTS_USE_CASE_USE_CASE_TEST_FIXTURE_H
