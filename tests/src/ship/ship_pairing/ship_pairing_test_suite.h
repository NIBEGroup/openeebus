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
 * @file ship_pairing_test_suite.h
 * @brief shippairing test suite
 *
 * The fixtures are the worked example of the SHIP Pairing Service TS 1.0.0,
 * Annex A. Using the specification's own values means these tests fail if an
 * implementation is self-consistent but does not interoperate.
 */
#ifndef TESTS_SRC_SHIP_SHIP_PAIRING_SHIP_PAIRING_TEST_SUITE_H
#define TESTS_SRC_SHIP_SHIP_PAIRING_SHIP_PAIRING_TEST_SUITE_H

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "mocks/common/eebus_timer/eebus_timer_mock.h"
#include "mocks/ship/tls_certificate/tls_certificate_mock.h"

extern "C" {
#include "src/common/string_util.h"
#include "src/ship/api/ship_pairing_entry.h"
#include "src/ship/ship_pairing/ship_pairing.h"
#include "src/ship/ship_pairing/ship_pairing_internal.h"
}

#include "tests/src/ship/annex_a.h"

#define TEST_INSTANCE_NAME "Control Unit ExampleCompany C8277H008F#1"
#define TEST_DOMAIN "local."

/** @brief One key-value pair of a TXT record */
using TxtPair = std::pair<std::string, std::string>;

class ShipPairingTestSuite : public testing::Test {
 public:
  using EntryPtr = std::unique_ptr<ShipPairingEntry, decltype(&ShipPairingEntryDelete)>;

  void TearDown() override;

  /**
   * @brief The eleven key-value pairs of the Annex A request, in specified order
   */
  static std::vector<TxtPair> AnnexAPairs();

  /**
   * @brief Encodes key-value pairs into the DNS-SD wire form of a TXT record
   *
   * Each pair becomes "key=value" preceded by one octet holding its length.
   */
  static std::string EncodeTxtRecord(const std::vector<TxtPair>& pairs);

  /**
   * @brief Creates an entry and parses a TXT record built from @p pairs into it
   */
  static EntryPtr ParseEntry(const std::vector<TxtPair>& pairs);

  /**
   * @brief Creates an entry holding the unmodified Annex A request
   */
  static EntryPtr AnnexAEntry();
};

/**
 * @brief An evaluator built on the devA certificate of Annex A
 *
 * The fingerprint the evaluator matches "forPar" against is calculated from
 * that certificate rather than supplied, so these tests also cover that the two
 * agree.
 */
class Evaluator {
 public:
  Evaluator() : cert_mock_(TlsCertificateMockCreate()) {
    EXPECT_CALL(*cert_mock_->gmock, GetCertificate(testing::_)).WillRepeatedly(testing::Return(kDevACertDer));
    EXPECT_CALL(*cert_mock_->gmock, GetCertificateSize(testing::_))
        .WillRepeatedly(testing::Return(sizeof(kDevACertDer)));
    EXPECT_CALL(*cert_mock_->gmock, Destruct(testing::_)).Times(testing::AnyNumber());

    obj_ = ShipPairingCreate(ANNEX_A_FOR_ID, TLS_CERTIFICATE_OBJECT(cert_mock_));

    if (obj_ != nullptr) {
      timer_ = EEBUS_TIMER_MOCK(SHIP_PAIRING(obj_)->reactivation_timer);
      EXPECT_CALL(*timer_->gmock, Start(testing::_, testing::_, testing::_)).Times(testing::AnyNumber());
      EXPECT_CALL(*timer_->gmock, Stop(testing::_)).Times(testing::AnyNumber());
      EXPECT_CALL(*timer_->gmock, Destruct(testing::_)).Times(testing::AnyNumber());
    }
  }

  ~Evaluator() {
    ShipPairingDelete(obj_);
    TlsCertificateMockDelete(cert_mock_);
  }

  Evaluator(const Evaluator&)            = delete;
  Evaluator& operator=(const Evaluator&) = delete;

  ShipPairingObject* Get() const {
    return obj_;
  }
  EebusTimerMock* Timer() const {
    return timer_;
  }

  void SetAnnexASecret() {
    uint8_t secret[SHIP_PAIRING_SECRET_SIZE];
    ASSERT_TRUE(StringHexToBytes(ANNEX_A_SECRET, secret, sizeof(secret)));
    SHIP_PAIRING_SET_SECRET(obj_, secret, sizeof(secret));
  }

 private:
  TlsCertificateMock* cert_mock_;
  ShipPairingObject* obj_{nullptr};
  EebusTimerMock* timer_{nullptr};
};

#endif  // TESTS_SRC_SHIP_SHIP_PAIRING_SHIP_PAIRING_TEST_SUITE_H
