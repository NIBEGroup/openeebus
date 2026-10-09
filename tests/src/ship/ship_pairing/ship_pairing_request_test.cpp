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
 * @brief Tests for creating and announcing a request, chapter 8 and section 4.2
 */
#include "tests/src/ship/ship_pairing/ship_pairing_test_suite.h"

extern "C" {
#include "src/common/string_util.h"
#include "src/ship/ship_pairing/ship_pairing_digest.h"
#include "src/ship/ship_pairing/ship_pairing_request.h"
#include "src/ship/ship_pairing/ship_pairing_request_internal.h"
}

namespace {

constexpr uint32_t kSettledMs = SHIP_PAIRING_REQUEST_SETTLED_MINUTES * 60 * 1000;

/**
 * @brief A request built on the devZ certificate of Annex A
 *
 * The counterpart of the Evaluator: together they are the two nodes of the
 * worked example.
 */
class Announcer {
 public:
  Announcer() : cert_mock_(TlsCertificateMockCreate()) {
    EXPECT_CALL(*cert_mock_->gmock, GetCertificate(testing::_)).WillRepeatedly(testing::Return(kDevZCertDer));
    EXPECT_CALL(*cert_mock_->gmock, GetCertificateSize(testing::_))
        .WillRepeatedly(testing::Return(sizeof(kDevZCertDer)));
    EXPECT_CALL(*cert_mock_->gmock, Destruct(testing::_)).Times(testing::AnyNumber());

    EXPECT_TRUE(StringHexToBytes(ANNEX_A_SECRET, secret_, sizeof(secret_)));

    const ShipPairingRequestConfig config = {
        .instance_name   = TEST_INSTANCE_NAME,
        .for_id          = ANNEX_A_FOR_ID,
        .for_par         = ANNEX_A_FOR_PAR,
        .secret          = secret_,
        .secret_size     = sizeof(secret_),
        .trust_id        = ANNEX_A_TRUST_ID,
        .tls_certificate = TLS_CERTIFICATE_OBJECT(cert_mock_),
    };

    request_ = ShipPairingRequestCreate(&config);

    if (request_ != nullptr) {
      timer_ = EEBUS_TIMER_MOCK(request_->settled_timer);
      EXPECT_CALL(*timer_->gmock, Start(testing::_, testing::_, testing::_)).Times(testing::AnyNumber());
      EXPECT_CALL(*timer_->gmock, Stop(testing::_)).Times(testing::AnyNumber());
      EXPECT_CALL(*timer_->gmock, Destruct(testing::_)).Times(testing::AnyNumber());
    }
  }

  ~Announcer() {
    ShipPairingRequestDelete(request_);
    TlsCertificateMockDelete(cert_mock_);
  }

  Announcer(const Announcer&)            = delete;
  Announcer& operator=(const Announcer&) = delete;

  ShipPairingRequest* Get() const {
    return request_;
  }
  EebusTimerMock* TimerMock() const {
    return timer_;
  }
  const uint8_t* Secret() const {
    return secret_;
  }
  static size_t SecretSize() {
    return SHIP_PAIRING_SECRET_SIZE;
  }

 private:
  TlsCertificateMock* cert_mock_;
  uint8_t secret_[SHIP_PAIRING_SECRET_SIZE] = {};
  ShipPairingRequest* request_              = nullptr;
  EebusTimerMock* timer_                    = nullptr;
};

}  // namespace

TEST_F(ShipPairingTestSuite, BuildsARequestThatConformsToTheSpecification) {
  Announcer announcer;
  ASSERT_NE(announcer.Get(), nullptr);

  const ShipPairingEntry* const entry = ShipPairingRequestGetEntry(announcer.Get());
  ASSERT_NE(entry, nullptr);

  EXPECT_TRUE(ShipPairingEntryIsValid(entry));

  EXPECT_STREQ(ShipPairingEntryGetTxtVers(entry), SHIP_PAIRING_TXTVERS);
  EXPECT_STREQ(ShipPairingEntryGetParType(entry), SHIP_PAIRING_PAR_TYPE_FP_SHA256);
  EXPECT_STREQ(ShipPairingEntryGetForId(entry), ANNEX_A_FOR_ID);
  EXPECT_STREQ(ShipPairingEntryGetForPar(entry), ANNEX_A_FOR_PAR);
  EXPECT_STREQ(ShipPairingEntryGetTrustId(entry), ANNEX_A_TRUST_ID);
  EXPECT_STREQ(ShipPairingEntryGetType(entry), SHIP_PAIRING_TYPE_ADD_CU);
  EXPECT_STREQ(ShipPairingEntryGetAlg(entry), SHIP_PAIRING_ALG_HMAC_SHA256);

  // Both are derived from this node's own certificate rather than configured.
  EXPECT_STREQ(ShipPairingEntryGetTrustPar(entry), ANNEX_A_TRUST_PAR);
  EXPECT_STREQ(ShipPairingEntryGetTrustCurve(entry), ANNEX_A_CURVE);

  EXPECT_STREQ(ShipPairingEntryGetDomain(entry), SHIP_PAIRING_DOMAIN);
  EXPECT_STREQ(ShipPairingEntryGetName(entry), TEST_INSTANCE_NAME);
}

TEST_F(ShipPairingTestSuite, AuthenticatesItsRequestWithTheAddressedNodesSecret) {
  Announcer announcer;
  ASSERT_NE(announcer.Get(), nullptr);

  const ShipPairingEntry* const entry = ShipPairingRequestGetEntry(announcer.Get());
  ASSERT_NE(entry, nullptr);

  EXPECT_TRUE(ShipPairingVerifyDigest(entry, announcer.Secret(), Announcer::SecretSize()));
}

TEST_F(ShipPairingTestSuite, TheEvaluatorAcceptsARequestThisLibraryBuilt) {
  // The whole point of both roles living here: what one builds, the other
  // accepts, without either being written against the other's fixtures.
  Announcer announcer;
  Evaluator evaluator;
  ASSERT_NE(announcer.Get(), nullptr);
  ASSERT_NE(evaluator.Get(), nullptr);
  evaluator.SetAnnexASecret();

  const ShipPairingEntry* const entry = ShipPairingRequestGetEntry(announcer.Get());
  ASSERT_NE(entry, nullptr);

  EXPECT_EQ(SHIP_PAIRING_EVALUATE(evaluator.Get(), entry), kShipPairingResultAccepted);
  EXPECT_TRUE(SHIP_PAIRING_HAS_TRUSTED_PEER(evaluator.Get()));
}

TEST_F(ShipPairingTestSuite, TheEvaluatorRejectsASecondRequestAsAReplay) {
  Announcer first;
  Evaluator evaluator;
  ASSERT_NE(first.Get(), nullptr);
  ASSERT_NE(evaluator.Get(), nullptr);
  evaluator.SetAnnexASecret();

  ASSERT_EQ(
      SHIP_PAIRING_EVALUATE(evaluator.Get(), ShipPairingRequestGetEntry(first.Get())),
      kShipPairingResultAccepted
  );

  SHIP_PAIRING_ACTIVATE_ADD_CU(evaluator.Get());
  EXPECT_EQ(
      SHIP_PAIRING_EVALUATE(evaluator.Get(), ShipPairingRequestGetEntry(first.Get())),
      kShipPairingResultAlreadySeen
  );
}

TEST_F(ShipPairingTestSuite, GivesEveryRequestItsOwnNonce) {
  // Section 6.2: a nonce is never reused, including when the process is run
  // again with the same node.
  std::set<std::string> nonces;
  std::set<std::string> digests;

  for (int i = 0; i < 8; ++i) {
    Announcer announcer;
    ASSERT_NE(announcer.Get(), nullptr);

    const ShipPairingEntry* const entry = ShipPairingRequestGetEntry(announcer.Get());
    ASSERT_NE(entry, nullptr);

    nonces.insert(ShipPairingEntryGetTrustNonce(entry));
    digests.insert(ShipPairingEntryGetDigest(entry));
  }

  EXPECT_EQ(nonces.size(), 8u);
  EXPECT_EQ(digests.size(), 8u);
}

TEST_F(ShipPairingTestSuite, AnnouncesFromTheMomentItIsBuilt) {
  Announcer announcer;
  ASSERT_NE(announcer.Get(), nullptr);

  EXPECT_TRUE(ShipPairingRequestIsAnnouncing(announcer.Get()));
}

TEST_F(ShipPairingTestSuite, StopsAnnouncingAfterFifteenUninterruptedMinutes) {
  Announcer announcer;
  ASSERT_NE(announcer.Get(), nullptr);

  EXPECT_CALL(*announcer.TimerMock()->gmock, Start(testing::_, kSettledMs, false)).Times(1);

  ShipPairingRequestNotifyConnected(announcer.Get());
  EXPECT_TRUE(ShipPairingRequestIsAnnouncing(announcer.Get()));

  EebusTimerMockExpire(announcer.TimerMock());

  EXPECT_FALSE(ShipPairingRequestIsAnnouncing(announcer.Get()));
}

TEST_F(ShipPairingTestSuite, StartsTheFifteenMinutesAgainAfterAnInterruption) {
  Announcer announcer;
  ASSERT_NE(announcer.Get(), nullptr);

  // An interruption discards what had been counted, so the timer is stopped and
  // then started again rather than resumed.
  {
    testing::InSequence sequence;
    EXPECT_CALL(*announcer.TimerMock()->gmock, Stop(testing::_));
    EXPECT_CALL(*announcer.TimerMock()->gmock, Start(testing::_, kSettledMs, false));
    EXPECT_CALL(*announcer.TimerMock()->gmock, Stop(testing::_));
    EXPECT_CALL(*announcer.TimerMock()->gmock, Stop(testing::_));
    EXPECT_CALL(*announcer.TimerMock()->gmock, Start(testing::_, kSettledMs, false));
  }

  ShipPairingRequestNotifyConnected(announcer.Get());
  ShipPairingRequestNotifyDisconnected(announcer.Get());
  ShipPairingRequestNotifyConnected(announcer.Get());

  EXPECT_TRUE(ShipPairingRequestIsAnnouncing(announcer.Get()));
}

TEST_F(ShipPairingTestSuite, StopsAnnouncingImmediatelyWhenTrustIsWithdrawn) {
  Announcer announcer;
  ASSERT_NE(announcer.Get(), nullptr);

  ShipPairingRequestNotifyConnected(announcer.Get());

  // Section 4.2: an administrator who recognises a mistake withdraws trust, and
  // the announcement has to go at once rather than after fifteen minutes.
  ShipPairingRequestStop(announcer.Get());

  EXPECT_FALSE(ShipPairingRequestIsAnnouncing(announcer.Get()));

  // A stopped request stays stopped: a corrected one is a new request under a
  // new instance name (section 5.5).
  ShipPairingRequestNotifyConnected(announcer.Get());
  EXPECT_FALSE(ShipPairingRequestIsAnnouncing(announcer.Get()));
}

TEST_F(ShipPairingTestSuite, ExposesItsKeysInTheSpecifiedOrderForAnnouncement) {
  Announcer announcer;
  ASSERT_NE(announcer.Get(), nullptr);

  const ShipPairingEntry* const entry = ShipPairingRequestGetEntry(announcer.Get());
  ASSERT_NE(entry, nullptr);

  ASSERT_EQ(ShipPairingEntryGetTxtPairCount(), 11u);

  // Section 5.4 requires "txtvers" to come first on the wire.
  EXPECT_STREQ(ShipPairingEntryGetTxtKey(0), "txtvers");
  EXPECT_STREQ(ShipPairingEntryGetTxtValue(entry, 0), SHIP_PAIRING_TXTVERS);

  for (size_t i = 0; i < ShipPairingEntryGetTxtPairCount(); ++i) {
    EXPECT_NE(ShipPairingEntryGetTxtKey(i), nullptr) << "key " << i;
    EXPECT_NE(ShipPairingEntryGetTxtValue(entry, i), nullptr) << "value " << i;
  }

  EXPECT_EQ(ShipPairingEntryGetTxtKey(ShipPairingEntryGetTxtPairCount()), nullptr);
  EXPECT_EQ(ShipPairingEntryGetTxtValue(entry, ShipPairingEntryGetTxtPairCount()), nullptr);
}

TEST_F(ShipPairingTestSuite, RefusesToBuildARequestFromAnIncompleteConfiguration) {
  TlsCertificateMock* const cert_mock = TlsCertificateMockCreate();
  ASSERT_NE(cert_mock, nullptr);
  EXPECT_CALL(*cert_mock->gmock, GetCertificate(testing::_)).WillRepeatedly(testing::Return(kDevZCertDer));
  EXPECT_CALL(*cert_mock->gmock, GetCertificateSize(testing::_)).WillRepeatedly(testing::Return(sizeof(kDevZCertDer)));
  EXPECT_CALL(*cert_mock->gmock, Destruct(testing::_)).Times(testing::AnyNumber());

  uint8_t secret[SHIP_PAIRING_SECRET_SIZE];
  ASSERT_TRUE(StringHexToBytes(ANNEX_A_SECRET, secret, sizeof(secret)));

  const ShipPairingRequestConfig complete = {
      .instance_name   = TEST_INSTANCE_NAME,
      .for_id          = ANNEX_A_FOR_ID,
      .for_par         = ANNEX_A_FOR_PAR,
      .secret          = secret,
      .secret_size     = sizeof(secret),
      .trust_id        = ANNEX_A_TRUST_ID,
      .tls_certificate = TLS_CERTIFICATE_OBJECT(cert_mock),
  };

  EXPECT_EQ(ShipPairingRequestCreate(nullptr), nullptr);

  ShipPairingRequestConfig config = complete;
  config.for_id                   = nullptr;
  EXPECT_EQ(ShipPairingRequestCreate(&config), nullptr);

  config         = complete;
  config.for_par = "";
  EXPECT_EQ(ShipPairingRequestCreate(&config), nullptr);

  config        = complete;
  config.secret = nullptr;
  EXPECT_EQ(ShipPairingRequestCreate(&config), nullptr);

  config             = complete;
  config.secret_size = 0;
  EXPECT_EQ(ShipPairingRequestCreate(&config), nullptr);

  config                 = complete;
  config.tls_certificate = nullptr;
  EXPECT_EQ(ShipPairingRequestCreate(&config), nullptr);

  // A fingerprint that is not one invalidates the record it goes into.
  config         = complete;
  config.for_par = "not a fingerprint";
  EXPECT_EQ(ShipPairingRequestCreate(&config), nullptr);

  TlsCertificateMockDelete(cert_mock);
}

TEST_F(ShipPairingTestSuite, SurvivesTheTripThroughATxtRecord) {
  // What a backend does with a request: write every key it exposes into a
  // DNS-SD TXT record, and recover an entry from the record on the other side.
  // The transport is not exercised here, but everything either end does to the
  // record is.
  Announcer announcer;
  Evaluator evaluator;
  ASSERT_NE(announcer.Get(), nullptr);
  ASSERT_NE(evaluator.Get(), nullptr);
  evaluator.SetAnnexASecret();

  const ShipPairingEntry* const sent = ShipPairingRequestGetEntry(announcer.Get());
  ASSERT_NE(sent, nullptr);

  std::vector<TxtPair> pairs;
  for (size_t i = 0; i < ShipPairingEntryGetTxtPairCount(); ++i) {
    pairs.push_back({ShipPairingEntryGetTxtKey(i), ShipPairingEntryGetTxtValue(sent, i)});
  }

  const std::string record = EncodeTxtRecord(pairs);

  EntryPtr received{ShipPairingEntryCreate(TEST_INSTANCE_NAME, SHIP_PAIRING_DOMAIN, 0), ShipPairingEntryDelete};
  ASSERT_NE(received, nullptr);
  ASSERT_EQ(
      ShipPairingEntryParseTxtRecord(received.get(), record.data(), static_cast<uint16_t>(record.size())),
      kEebusErrorOk
  );

  EXPECT_TRUE(ShipPairingEntryIsValid(received.get()));
  EXPECT_EQ(SHIP_PAIRING_EVALUATE(evaluator.Get(), received.get()), kShipPairingResultAccepted);
}
