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
 * @file ship_connection_test_suite.h
 * @brief SHIP connection test suite
 */
#ifndef TESTS_SRC_SHIP_SHIP_CONNECTION_SHIP_CONNECTION_SHIP_CONNECTION_TEST_SUITE_H
#define TESTS_SRC_SHIP_SHIP_CONNECTION_SHIP_CONNECTION_SHIP_CONNECTION_TEST_SUITE_H

#include <gtest/gtest.h>

#include <cstring>
#include <string_view>
#include "mocks/common/eebus_timer/eebus_timer_mock.h"
#include "mocks/ship/api/info_provider_mock.h"
#include "mocks/ship/tls_certificate/tls_certificate_mock.h"
#include "mocks/ship/websocket/websocket_creator_mock.h"
#include "mocks/ship/websocket/websocket_mock.h"
#include "src/ship/ship_connection/ship_connection.h"
#include "src/ship/ship_connection/ship_connection_internal.h"

constexpr std::string_view kShipRemoteSki{"RemoteSKI"};

MATCHER_P2(MemEq, expected, size, "") {
  return memcmp(arg, expected, size) == 0;
}

MATCHER_P(MsgBufEq, expected, "") {
  const size_t cmp_size = expected.data_size - 1;
  *result_listener << "\n  actual  : " << std::string_view(reinterpret_cast<const char*>(arg), cmp_size)
                   << "\n  expected: " << std::string_view(reinterpret_cast<const char*>(expected.data), cmp_size);
  return memcmp(arg, expected.data, cmp_size) == 0;
}

class ShipConnectionTestSuite : public testing::Test {
 public:
  void SetUp() override;
  void TearDown() override;

 protected:
  /** Mock for the wait-for-ready timer */
  EebusTimerMock* wfr_timer_mock;

  /** Mock for the send-prolongation-request timer */
  EebusTimerMock* spr_timer_mock;

  /** Mock for the prolongation-request-reply timer */
  EebusTimerMock* prr_timer_mock;

  TlsCertificateMock* tls_cert_mock;
  InfoProviderMock* ifp_mock;
  WebsocketMock* websocket_mock;
  WebsocketCreatorMock* websocket_creator_mock;
  ShipConnection sc;

 protected:
  EebusError MessageBufferInitHelper(MessageBuffer* msg_buf, const std::string_view& msg);
  void ExpectWebsocketWrite(const MessageBuffer& expected_msg, bool should_succeed);
  void ExpectWebsocketClose(const char* error_msg, bool had_error);
  void ExpectConnectionClose(const char* error_msg, bool had_error);
  void ExpectStateUpdate(SmeState state);
  void SetShipConnectionState(SmeState state);
};

#endif  // TESTS_SRC_SHIP_SHIP_CONNECTION_SHIP_CONNECTION_SHIP_CONNECTION_TEST_SUITE_H
