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

#include "src/common/array_util.h"
#include "src/ship/ship_connection/client.h"
#include "tests/src/ship/ship_connection/ship_connection/ship_connection_test_suite.h"

using testing::Return;
constexpr size_t kShipInitMsgSize{ARRAY_SIZE(kShipInitMessage)};

class CmiClientSendInitMessageTests : public ShipConnectionTestSuite {};

TEST_F(CmiClientSendInitMessageTests, MessageSentSuccessfully) {
  // Arrange: Successfully send CMI init message
  EXPECT_CALL(*websocket_mock->gmock, Write(sc.websocket, MemEq(kShipInitMessage, kShipInitMsgSize), kShipInitMsgSize))
      .WillOnce(Return(static_cast<int32_t>(kShipInitMsgSize)));

  ExpectStateUpdate(kCmiStateClientWait);

  // Act: Try to send init message
  CmiStateClientSend(&sc);

  // Assert: SME is in kCmiStateClientWait
  EXPECT_EQ(SHIP_CONNECTION_GET_SHIP_STATE(&sc, nullptr), kCmiStateClientWait);
  ExpectConnectionClose("", false);
}

TEST_F(CmiClientSendInitMessageTests, MessageFailedToSend) {
  // Arrange: Fail to send CMI init message
  EXPECT_CALL(*websocket_mock->gmock, Write(sc.websocket, MemEq(kShipInitMessage, kShipInitMsgSize), kShipInitMsgSize))
      .WillOnce(Return(0));

  ExpectStateUpdate(kSmeStateError);
  ExpectConnectionClose("CMI client send failed", false);

  // Act: Try to send init message
  CmiStateClientSend(&sc);

  // Assert: SME is in kSmeStateError
  EXPECT_EQ(SHIP_CONNECTION_GET_SHIP_STATE(&sc, nullptr), kSmeStateError);
}
