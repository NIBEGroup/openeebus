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
 * @brief SHIP 5.2 post-trust: leaving the "hello" PENDING phase on an
 * application trust decision
 */
#include <gtest/gtest.h>

#include <string_view>

#include "tests/src/json.h"
#include "tests/src/ship/ship_connection/ship_connection/ship_connection_test_suite.h"

using std::literals::string_view_literals::operator""sv;
using testing::_;
using testing::ReturnArg;

class ShipConnectionHelloStatePostTrustTests : public ShipConnectionTestSuite {};

/**
 * SHIP 13.4.4.1.2 a): the SME User switches into state READY and informs the
 * communication partner accordingly, stopping both prolongation timers.
 *
 * Timer Stop() counts include one per timer from the suite's TearDown().
 */
TEST_F(ShipConnectionHelloStatePostTrustTests, TrustGrantedLeavesPendingForReady) {
  // Arrange: a connection waiting for a decision about the peer
  SetShipConnectionState(kSmeHelloStatePendingListen);

  SHIP_CONNECTION_APPROVE_PENDING_HANDSHAKE(SHIP_CONNECTION_OBJECT(&sc));

  EXPECT_CALL(*wfr_timer_mock->gmock, Start(sc.wait_for_ready_timer, tHelloInit, false));
  EXPECT_CALL(*wfr_timer_mock->gmock, Stop(sc.wait_for_ready_timer)).Times(2);
  EXPECT_CALL(*spr_timer_mock->gmock, Stop(sc.send_prolongation_request_timer)).Times(2);
  EXPECT_CALL(*prr_timer_mock->gmock, Stop(sc.prolongation_request_reply_timer)).Times(2);
  EXPECT_CALL(*ifp_mock->gmock, HandleShipStateUpdate(sc.info_provider, _, kSmeHelloStateReadyInit, _));
  ExpectWebsocketClose("", false);

  // Act
  SmeHelloStatePendingListen(&sc);

  // Assert
  EXPECT_EQ(SHIP_CONNECTION_GET_SHIP_STATE(&sc, NULL), kSmeHelloStateReadyInit);
  EXPECT_EQ(sc.trust_decision, kShipConnectionTrustDecisionNone);
}

/**
 * SHIP 13.4.4.1.2 c): the SME User reports the abortion of the process, having
 * finally decided not to trust the communication partner.
 */
TEST_F(ShipConnectionHelloStatePostTrustTests, TrustDeniedAbortsTheHandshake) {
  // Arrange
  SetShipConnectionState(kSmeHelloStatePendingListen);

  SHIP_CONNECTION_ABORT_PENDING_HANDSHAKE(SHIP_CONNECTION_OBJECT(&sc));

  EXPECT_CALL(*wfr_timer_mock->gmock, Start(sc.wait_for_ready_timer, tHelloInit, false));
  EXPECT_CALL(*wfr_timer_mock->gmock, Stop(sc.wait_for_ready_timer)).Times(2);
  EXPECT_CALL(*spr_timer_mock->gmock, Stop(sc.send_prolongation_request_timer));
  EXPECT_CALL(*prr_timer_mock->gmock, Stop(sc.prolongation_request_reply_timer));
  EXPECT_CALL(*ifp_mock->gmock, HandleShipStateUpdate(sc.info_provider, _, kSmeHelloStateAbort, _));
  ExpectWebsocketClose("", false);

  // Act
  SmeHelloStatePendingListen(&sc);

  // Assert
  EXPECT_EQ(SHIP_CONNECTION_GET_SHIP_STATE(&sc, NULL), kSmeHelloStateAbort);
  EXPECT_EQ(sc.trust_decision, kShipConnectionTrustDecisionNone);
}

/**
 * A decision can race the handshake. Acting on it outside PENDING would tear
 * down a healthy connection, so it is dropped and the queue read resumes.
 */
TEST_F(ShipConnectionHelloStatePostTrustTests, TrustDecisionOutsidePendingIsIgnored) {
  // Arrange: a stale approval queued ahead of the cancellation, the connection
  // having already left PENDING
  SetShipConnectionState(kSmeHelloStateReadyListen);

  SHIP_CONNECTION_APPROVE_PENDING_HANDSHAKE(SHIP_CONNECTION_OBJECT(&sc));

  ShipConnectionQueueMessage queue_msg = {.type = kShipConnectionQueueMsgTypeCancel};
  MessageBufferInit(&queue_msg.msg_buf, NULL, 0);
  EEBUS_QUEUE_SEND(sc.msg_queue, &queue_msg, sizeof(queue_msg));

  EXPECT_CALL(*wfr_timer_mock->gmock, Start(sc.wait_for_ready_timer, tHelloInit, false));
  EXPECT_CALL(*wfr_timer_mock->gmock, Stop(sc.wait_for_ready_timer)).Times(2);
  EXPECT_CALL(*spr_timer_mock->gmock, Stop(sc.send_prolongation_request_timer));
  EXPECT_CALL(*prr_timer_mock->gmock, Stop(sc.prolongation_request_reply_timer));
  EXPECT_CALL(*ifp_mock->gmock, HandleShipStateUpdate(sc.info_provider, _, kSmeHelloStateAbort, _));
  ExpectWebsocketClose("", false);

  // Act: the message behind the stale decision is the one acted upon
  SmeHelloStateReadyListen(&sc);

  // Assert: the decision left no trace
  EXPECT_EQ(sc.trust_decision, kShipConnectionTrustDecisionNone);
  EXPECT_EQ(SHIP_CONNECTION_GET_SHIP_STATE(&sc, NULL), kSmeHelloStateAbort);
}

/**
 * The time an application has to decide is bounded by what the peer granted in
 * "connectionHello.waiting", not by a locally invented timeout.
 */
TEST_F(ShipConnectionHelloStatePostTrustTests, PendingWaitingMsReportsWhatThePeerGranted) {
  // Arrange
  MessageBuffer msg_buf = {0};
  const EebusError error
      = MessageBufferInitHelper(&msg_buf, R"({"connectionHello": [{"phase": "pending"}, {"waiting": 60000}]})"sv);
  ASSERT_EQ(error, kEebusErrorOk) << "Wrong test input!";
  ShipConnectionWebsocketCallback(kWebsocketCallbackTypeRead, msg_buf.data, msg_buf.data_size, &sc);
  MessageBufferRelease(&msg_buf);

  SetShipConnectionState(kSmeHelloStatePendingListen);

  EXPECT_CALL(*wfr_timer_mock->gmock, Start(sc.wait_for_ready_timer, tHelloInit, false));
  EXPECT_CALL(*wfr_timer_mock->gmock, Stop(sc.wait_for_ready_timer)).Times(2);
  EXPECT_CALL(*prr_timer_mock->gmock, Stop(sc.prolongation_request_reply_timer)).Times(2);
  EXPECT_CALL(*spr_timer_mock->gmock, Stop(sc.send_prolongation_request_timer));
  EXPECT_CALL(*spr_timer_mock->gmock, Start(sc.send_prolongation_request_timer, 45000, false));

  // Nothing granted before the peer says so
  EXPECT_EQ(SHIP_CONNECTION_GET_PENDING_WAITING_MS(SHIP_CONNECTION_OBJECT(&sc)), 0U);

  // Act
  SmeHelloStatePendingListen(&sc);

  // Assert
  ASSERT_EQ(SHIP_CONNECTION_GET_SHIP_STATE(&sc, NULL), kSmeHelloStatePendingListen);
  EXPECT_EQ(SHIP_CONNECTION_GET_PENDING_WAITING_MS(SHIP_CONNECTION_OBJECT(&sc)), 60000U);

  // ... and only while the connection is pending
  SetShipConnectionState(kSmeHelloStateReadyInit);
  EXPECT_EQ(SHIP_CONNECTION_GET_PENDING_WAITING_MS(SHIP_CONNECTION_OBJECT(&sc)), 0U);
}

/**
 * The peer announces "ready" once, while this side is still PENDING, so that it
 * has to be remembered.
 */
TEST_F(ShipConnectionHelloStatePostTrustTests, PeerReadyDuringPendingIsRemembered) {
  // Arrange
  MessageBuffer msg_buf = {0};
  const EebusError error
      = MessageBufferInitHelper(&msg_buf, R"({"connectionHello": [{"phase": "ready"}, {"waiting": 60000}]})"sv);
  ASSERT_EQ(error, kEebusErrorOk) << "Wrong test input!";
  ShipConnectionWebsocketCallback(kWebsocketCallbackTypeRead, msg_buf.data, msg_buf.data_size, &sc);
  MessageBufferRelease(&msg_buf);

  SetShipConnectionState(kSmeHelloStatePendingListen);

  EXPECT_CALL(*wfr_timer_mock->gmock, Start(sc.wait_for_ready_timer, tHelloInit, false));
  EXPECT_CALL(*wfr_timer_mock->gmock, Stop(sc.wait_for_ready_timer)).Times(3);
  EXPECT_CALL(*prr_timer_mock->gmock, Stop(sc.prolongation_request_reply_timer)).Times(2);
  EXPECT_CALL(*spr_timer_mock->gmock, Stop(sc.send_prolongation_request_timer));
  EXPECT_CALL(*spr_timer_mock->gmock, Start(sc.send_prolongation_request_timer, 45000, false));
  ExpectWebsocketClose("", false);

  ASSERT_FALSE(sc.remote_hello_ready);

  // Act
  SmeHelloStatePendingListen(&sc);

  // Assert: still waiting on the user, but the peer's readiness is recorded
  EXPECT_EQ(SHIP_CONNECTION_GET_SHIP_STATE(&sc, NULL), kSmeHelloStatePendingListen);
  EXPECT_TRUE(sc.remote_hello_ready);
}

/**
 * SHIP 13.4.4.1.2: HELLO_OK needs both SME Users ready, and both are. Listening
 * for a "ready" already received consumes the protocol handshake that arrives
 * instead, aborting the connection just accepted.
 */
TEST_F(ShipConnectionHelloStatePostTrustTests, ReadyInitWithThePeerAlreadyReadyReachesHelloOk) {
  // Arrange: as PENDING left it, the peer having announced ready
  sc.remote_hello_ready = true;

  EXPECT_CALL(*spr_timer_mock->gmock, Stop(sc.send_prolongation_request_timer)).Times(2);
  EXPECT_CALL(*prr_timer_mock->gmock, Stop(sc.prolongation_request_reply_timer)).Times(2);
  EXPECT_CALL(*wfr_timer_mock->gmock, Stop(sc.wait_for_ready_timer));
  EXPECT_CALL(*websocket_mock->gmock, Write(sc.websocket, _, _)).WillOnce(ReturnArg<2>());
  EXPECT_CALL(*ifp_mock->gmock, HandleShipStateUpdate(sc.info_provider, _, kSmeHelloStateOk, _));
  ExpectWebsocketClose("", false);

  // Act
  SmeHelloStateReadyInit(&sc);

  // Assert: straight to HELLO_OK rather than listening for a second "ready"
  EXPECT_EQ(SHIP_CONNECTION_GET_SHIP_STATE(&sc, NULL), kSmeHelloStateOk);
}

/**
 * The mirror case: nothing heard from the peer yet, so becoming READY still
 * waits for its "ready". Every pre-trust connection takes this path out of CMI.
 */
TEST_F(ShipConnectionHelloStatePostTrustTests, ReadyInitWithoutAPeerReadyStillListens) {
  // Arrange: nothing received from the peer
  ASSERT_FALSE(sc.remote_hello_ready);

  EXPECT_CALL(*spr_timer_mock->gmock, Stop(sc.send_prolongation_request_timer)).Times(2);
  EXPECT_CALL(*prr_timer_mock->gmock, Stop(sc.prolongation_request_reply_timer)).Times(2);
  EXPECT_CALL(*wfr_timer_mock->gmock, Stop(sc.wait_for_ready_timer));
  EXPECT_CALL(*websocket_mock->gmock, Write(sc.websocket, _, _)).WillOnce(ReturnArg<2>());
  EXPECT_CALL(*ifp_mock->gmock, HandleShipStateUpdate(sc.info_provider, _, kSmeHelloStateReadyListen, _));
  ExpectWebsocketClose("", false);

  // Act
  SmeHelloStateReadyInit(&sc);

  // Assert
  EXPECT_EQ(SHIP_CONNECTION_GET_SHIP_STATE(&sc, NULL), kSmeHelloStateReadyListen);
}
