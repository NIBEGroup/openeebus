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
 * @brief SPINE Sender test suite implementation
 */

#include "tests/unit_tests/spine/device/sender/sender_test_suite.h"

#include "src/common/json.h"
#include "tests/unit_tests/json.h"

using testing::_;
using testing::Invoke;
using testing::WithArgs;

std::unique_ptr<DataWriterMock, decltype(&DataWriterMockDelete)> SenderTestSuite::writer_mock_{nullptr, nullptr};
std::unique_ptr<SenderObject, decltype(&SenderDelete)> SenderTestSuite::sender_{nullptr, nullptr};

void SenderTestSuite::SetUp() {
  writer_mock_ = decltype(writer_mock_){DataWriterMockCreate(), DataWriterMockDelete};
  sender_      = decltype(sender_){SenderCreate(DATA_WRITER_OBJECT(writer_mock_.get())), SenderDelete};
}

void SenderTestSuite::ExpectMessageWrite(const std::string_view& msg_expected) {
  EXPECT_CALL(*writer_mock_->gmock, WriteMessage(_, _, _))
      .WillOnce(WithArgs<1, 2>(Invoke([&msg_expected](const uint8_t* msg, size_t msg_size) {
        std::unique_ptr<char[], decltype(&JsonFree)> s{JsonUnformat(msg_expected), JsonFree};
        EXPECT_STREQ(s.get(), reinterpret_cast<const char*>(msg));
        const size_t msg_size_expected = (s != nullptr) ? strlen(s.get()) + 1 : 0;
        EXPECT_EQ(msg_size, msg_size_expected);
      })));
}

void SenderTestSuite::TearDown() {
  EXPECT_CALL(*writer_mock_->gmock, Destruct(DATA_WRITER_OBJECT(writer_mock_.get())));
  SenderTestSuite::sender_.reset();
  SenderTestSuite::writer_mock_.reset();
}
