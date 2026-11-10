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
 * @file
 * @brief SPINE Sender test suite
 */

#ifndef SRC_SPINE_DEVICE_SENDER_SENDER_TEST_SUITE_H_
#define SRC_SPINE_DEVICE_SENDER_SENDER_TEST_SUITE_H_

#include <gtest/gtest.h>

#include <memory>

#include "mocks/ship/ship_connection/data_writer_mock.h"
#include "src/spine/device/sender.h"
#include "src/spine/model/node_management_types.h"

class SenderTestSuite : public testing::Test {
 public:
  void SetUp();
  void TearDown();
  static inline SenderObject* GetSender() {
    return sender_.get();
  }

 protected:
  static void ExpectMessageWrite(const std::string_view& msg);

 private:
  static std::unique_ptr<DataWriterMock, decltype(&DataWriterMockDelete)> writer_mock_;
  static std::unique_ptr<SenderObject, decltype(&SenderDelete)> sender_;
};

#endif  // SRC_SPINE_DEVICE_SENDER_SENDER_TEST_SUITE_H_
