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
 * @brief Tests for the evaluation of shippairing requests, chapters 9 and 11
 */
#include "tests/src/ship/ship_pairing/ship_pairing_test_suite.h"

extern "C" {
#include "src/common/string_util.h"
#include "src/ship/ship_pairing/ship_pairing_digest.h"
}

using testing::_;
using testing::NiceMock;
using testing::Return;

namespace {

constexpr uint32_t kReactivationMs = SHIP_PAIRING_REACTIVATION_MINUTES * 60 * 1000;

/** @brief Returns the Annex A pairs with @p key replaced by @p value */
std::vector<TxtPair> WithValue(const std::string& key, const std::string& value) {
  std::vector<TxtPair> pairs = ShipPairingTestSuite::AnnexAPairs();
  for (auto& pair : pairs) {
    if (pair.first == key) {
      pair.second = value;
    }
  }

  return pairs;
}

/**
 * @brief Builds a request that a given secret authenticates
 *
 * Used to make requests that differ from Annex A but are still genuine, so that
 * a rejection can be attributed to the rule under test rather than to a digest
 * that no longer matches.
 */
std::vector<TxtPair> ResignedPairs(const std::vector<TxtPair>& pairs, const char* secret_hex) {
  ShipPairingTestSuite::EntryPtr entry = ShipPairingTestSuite::ParseEntry(pairs);
  EXPECT_NE(entry, nullptr);

  uint8_t secret[SHIP_PAIRING_SECRET_SIZE];
  EXPECT_TRUE(StringHexToBytes(secret_hex, secret, sizeof(secret)));

  char* const digest = ShipPairingCalcDigest(entry.get(), secret, sizeof(secret));
  EXPECT_NE(digest, nullptr);

  std::vector<TxtPair> resigned = pairs;
  for (auto& pair : resigned) {
    if (pair.first == "digest") {
      pair.second = digest;
    }
  }

  StringDelete(digest);

  return resigned;
}

}  // namespace

TEST_F(ShipPairingTestSuite, AcceptsTheSpecifiedRequest) {
  Evaluator evaluator;
  ASSERT_NE(evaluator.Get(), nullptr);
  evaluator.SetAnnexASecret();

  const EntryPtr entry = AnnexAEntry();
  ASSERT_NE(entry, nullptr);

  EXPECT_TRUE(SHIP_PAIRING_IS_ADD_CU_ACTIVATED(evaluator.Get()));
  EXPECT_FALSE(SHIP_PAIRING_HAS_TRUSTED_PEER(evaluator.Get()));

  EXPECT_EQ(SHIP_PAIRING_EVALUATE(evaluator.Get(), entry.get()), kShipPairingResultAccepted);

  // Section 4.2, step 3: accepting one stops the processing of further ones.
  EXPECT_FALSE(SHIP_PAIRING_IS_ADD_CU_ACTIVATED(evaluator.Get()));
  EXPECT_TRUE(SHIP_PAIRING_HAS_TRUSTED_PEER(evaluator.Get()));
}

TEST_F(ShipPairingTestSuite, RejectsARequestAddressedToAnotherNode) {
  Evaluator evaluator;
  ASSERT_NE(evaluator.Get(), nullptr);
  evaluator.SetAnnexASecret();

  const EntryPtr entry = ParseEntry(ResignedPairs(WithValue("forId", "i:1_u:someone-else"), ANNEX_A_SECRET));
  ASSERT_NE(entry, nullptr);

  EXPECT_EQ(SHIP_PAIRING_EVALUATE(evaluator.Get(), entry.get()), kShipPairingResultNotForThisNode);
  EXPECT_TRUE(SHIP_PAIRING_IS_ADD_CU_ACTIVATED(evaluator.Get()));
}

TEST_F(ShipPairingTestSuite, RejectsARequestNamingAnotherCertificateOfThisNode) {
  Evaluator evaluator;
  ASSERT_NE(evaluator.Get(), nullptr);
  evaluator.SetAnnexASecret();

  // Correctly addressed by SHIP ID, but the fingerprint is not this node's.
  const EntryPtr entry = ParseEntry(ResignedPairs(WithValue("forPar", ANNEX_A_TRUST_PAR), ANNEX_A_SECRET));
  ASSERT_NE(entry, nullptr);

  EXPECT_EQ(SHIP_PAIRING_EVALUATE(evaluator.Get(), entry.get()), kShipPairingResultNotForThisNode);
}

TEST_F(ShipPairingTestSuite, RejectsARequestNamingAnUnsupportedCurve) {
  Evaluator evaluator;
  ASSERT_NE(evaluator.Get(), nullptr);
  evaluator.SetAnnexASecret();

  SHIP_PAIRING_SET_SUPPORTED_CURVES(evaluator.Get(), kShipPairingCurveBrainpoolP384r1);

  const EntryPtr entry = AnnexAEntry();
  ASSERT_NE(entry, nullptr);

  EXPECT_EQ(SHIP_PAIRING_EVALUATE(evaluator.Get(), entry.get()), kShipPairingResultUnsupported);
}

TEST_F(ShipPairingTestSuite, RejectsAnInvalidRecordBeforeAnythingElse) {
  Evaluator evaluator;
  ASSERT_NE(evaluator.Get(), nullptr);
  evaluator.SetAnnexASecret();

  std::vector<TxtPair> pairs = AnnexAPairs();
  std::swap(pairs[0], pairs[1]);

  const EntryPtr entry = ParseEntry(pairs);
  ASSERT_NE(entry, nullptr);

  EXPECT_EQ(SHIP_PAIRING_EVALUATE(evaluator.Get(), entry.get()), kShipPairingResultInvalidRecord);
}

TEST_F(ShipPairingTestSuite, RejectsARequestWhileNotProcessingAddCuRequests) {
  Evaluator evaluator;
  ASSERT_NE(evaluator.Get(), nullptr);
  evaluator.SetAnnexASecret();

  SHIP_PAIRING_DEACTIVATE_ADD_CU(evaluator.Get());

  const EntryPtr entry = AnnexAEntry();
  ASSERT_NE(entry, nullptr);

  EXPECT_EQ(SHIP_PAIRING_EVALUATE(evaluator.Get(), entry.get()), kShipPairingResultNotProcessing);
}

TEST_F(ShipPairingTestSuite, RejectsARequestBeforeASecretIsKnown) {
  Evaluator evaluator;
  ASSERT_NE(evaluator.Get(), nullptr);

  const EntryPtr entry = AnnexAEntry();
  ASSERT_NE(entry, nullptr);

  // The secret usually comes from storage that is not readable yet when the
  // service starts. Until it is, nothing can be authenticated.
  EXPECT_EQ(SHIP_PAIRING_EVALUATE(evaluator.Get(), entry.get()), kShipPairingResultError);
  EXPECT_FALSE(SHIP_PAIRING_HAS_TRUSTED_PEER(evaluator.Get()));
}

TEST_F(ShipPairingTestSuite, RejectsARequestAuthenticatedWithTheWrongSecret) {
  Evaluator evaluator;
  ASSERT_NE(evaluator.Get(), nullptr);

  uint8_t secret[SHIP_PAIRING_SECRET_SIZE];
  ASSERT_TRUE(StringHexToBytes(ANNEX_A_SECRET, secret, sizeof(secret)));
  secret[0] ^= 0x01;
  SHIP_PAIRING_SET_SECRET(evaluator.Get(), secret, sizeof(secret));

  const EntryPtr entry = AnnexAEntry();
  ASSERT_NE(entry, nullptr);

  EXPECT_EQ(SHIP_PAIRING_EVALUATE(evaluator.Get(), entry.get()), kShipPairingResultDigestMismatch);

  // A request that failed to authenticate must not be recorded, or it would
  // suppress the genuine one that follows it.
  EXPECT_FALSE(SHIP_PAIRING_HAS_TRUSTED_PEER(evaluator.Get()));
  EXPECT_TRUE(SHIP_PAIRING_IS_ADD_CU_ACTIVATED(evaluator.Get()));
}

TEST_F(ShipPairingTestSuite, RejectsAReplayOfAnAcceptedRequest) {
  Evaluator evaluator;
  ASSERT_NE(evaluator.Get(), nullptr);
  evaluator.SetAnnexASecret();

  const EntryPtr first = AnnexAEntry();
  ASSERT_NE(first, nullptr);
  ASSERT_EQ(SHIP_PAIRING_EVALUATE(evaluator.Get(), first.get()), kShipPairingResultAccepted);

  // Processing is reactivated as it would be after fifteen silent minutes, so
  // that what rejects the replay is the ring buffer and not the armed state.
  SHIP_PAIRING_ACTIVATE_ADD_CU(evaluator.Get());

  const EntryPtr replay = AnnexAEntry();
  ASSERT_NE(replay, nullptr);
  EXPECT_EQ(SHIP_PAIRING_EVALUATE(evaluator.Get(), replay.get()), kShipPairingResultAlreadySeen);
}

TEST_F(ShipPairingTestSuite, ForgetsAReplayOnlyAfterElevenFurtherRequests) {
  Evaluator evaluator;
  ASSERT_NE(evaluator.Get(), nullptr);
  evaluator.SetAnnexASecret();

  const EntryPtr first = AnnexAEntry();
  ASSERT_NE(first, nullptr);
  ASSERT_EQ(SHIP_PAIRING_EVALUATE(evaluator.Get(), first.get()), kShipPairingResultAccepted);

  // Section 11.1: the buffer holds ten. Nine more accepted requests keep the
  // first one in it, the tenth overwrites it.
  for (int i = 0; i < 9; ++i) {
    SHIP_PAIRING_ACTIVATE_ADD_CU(evaluator.Get());

    const std::string nonce = std::string(31, '0') + static_cast<char>('1' + i);
    const EntryPtr entry    = ParseEntry(ResignedPairs(WithValue("trustNonce", nonce), ANNEX_A_SECRET));
    ASSERT_NE(entry, nullptr) << "request " << i;
    ASSERT_EQ(SHIP_PAIRING_EVALUATE(evaluator.Get(), entry.get()), kShipPairingResultAccepted) << "request " << i;
  }

  SHIP_PAIRING_ACTIVATE_ADD_CU(evaluator.Get());
  const EntryPtr still_known = AnnexAEntry();
  ASSERT_NE(still_known, nullptr);
  EXPECT_EQ(SHIP_PAIRING_EVALUATE(evaluator.Get(), still_known.get()), kShipPairingResultAlreadySeen);

  // One more accepted request wraps the buffer onto the first entry.
  SHIP_PAIRING_ACTIVATE_ADD_CU(evaluator.Get());
  const EntryPtr overwriting = ParseEntry(ResignedPairs(WithValue("trustNonce", std::string(32, 'F')), ANNEX_A_SECRET));
  ASSERT_NE(overwriting, nullptr);
  ASSERT_EQ(SHIP_PAIRING_EVALUATE(evaluator.Get(), overwriting.get()), kShipPairingResultAccepted);

  SHIP_PAIRING_ACTIVATE_ADD_CU(evaluator.Get());
  const EntryPtr forgotten = AnnexAEntry();
  ASSERT_NE(forgotten, nullptr);
  EXPECT_EQ(SHIP_PAIRING_EVALUATE(evaluator.Get(), forgotten.get()), kShipPairingResultAccepted);
}

TEST_F(ShipPairingTestSuite, PersistsAndRestoresTheRingBuffer) {
  ShipPairingRingBuffer saved;

  {
    Evaluator evaluator;
    ASSERT_NE(evaluator.Get(), nullptr);
    evaluator.SetAnnexASecret();

    const EntryPtr entry = AnnexAEntry();
    ASSERT_NE(entry, nullptr);
    ASSERT_EQ(SHIP_PAIRING_EVALUATE(evaluator.Get(), entry.get()), kShipPairingResultAccepted);

    SHIP_PAIRING_GET_RING_BUFFER(evaluator.Get(), &saved);
  }

  EXPECT_EQ(saved.next, 2u);
  EXPECT_STREQ(saved.entries[0].digest, ANNEX_A_DIGEST);
  EXPECT_STREQ(saved.entries[0].alg, SHIP_PAIRING_ALG_HMAC_SHA256);

  // A restart: the same buffer is handed back to a new evaluator.
  Evaluator restarted;
  ASSERT_NE(restarted.Get(), nullptr);
  restarted.SetAnnexASecret();

  SHIP_PAIRING_SET_RING_BUFFER(restarted.Get(), &saved);

  EXPECT_TRUE(SHIP_PAIRING_HAS_TRUSTED_PEER(restarted.Get()));
  EXPECT_FALSE(SHIP_PAIRING_IS_ADD_CU_ACTIVATED(restarted.Get()));

  SHIP_PAIRING_ACTIVATE_ADD_CU(restarted.Get());
  const EntryPtr replay = AnnexAEntry();
  ASSERT_NE(replay, nullptr);
  EXPECT_EQ(SHIP_PAIRING_EVALUATE(restarted.Get(), replay.get()), kShipPairingResultAlreadySeen);
}

TEST_F(ShipPairingTestSuite, DiscardsARingBufferWithAnIndexOutsideIt) {
  Evaluator evaluator;
  ASSERT_NE(evaluator.Get(), nullptr);

  ShipPairingRingBuffer damaged;
  memset(&damaged, 0, sizeof(damaged));
  damaged.next = SHIP_PAIRING_RING_BUFFER_SIZE + 1;

  SHIP_PAIRING_SET_RING_BUFFER(evaluator.Get(), &damaged);

  EXPECT_FALSE(SHIP_PAIRING_HAS_TRUSTED_PEER(evaluator.Get()));
  EXPECT_TRUE(SHIP_PAIRING_IS_ADD_CU_ACTIVATED(evaluator.Get()));
}

TEST_F(ShipPairingTestSuite, CountsFifteenMinutesFromTheAcceptedRequest) {
  Evaluator evaluator;
  ASSERT_NE(evaluator.Get(), nullptr);
  evaluator.SetAnnexASecret();

  // The timer is armed for exactly the interval section 4.3.1 names, once.
  EXPECT_CALL(*evaluator.Timer()->gmock, Start(_, kReactivationMs, false)).Times(1);

  const EntryPtr entry = AnnexAEntry();
  ASSERT_NE(entry, nullptr);
  ASSERT_EQ(SHIP_PAIRING_EVALUATE(evaluator.Get(), entry.get()), kShipPairingResultAccepted);
}

TEST_F(ShipPairingTestSuite, ProcessesAddCuRequestsAgainAfterFifteenSilentMinutes) {
  Evaluator evaluator;
  ASSERT_NE(evaluator.Get(), nullptr);
  evaluator.SetAnnexASecret();

  const EntryPtr entry = AnnexAEntry();
  ASSERT_NE(entry, nullptr);
  ASSERT_EQ(SHIP_PAIRING_EVALUATE(evaluator.Get(), entry.get()), kShipPairingResultAccepted);
  ASSERT_FALSE(SHIP_PAIRING_IS_ADD_CU_ACTIVATED(evaluator.Get()));

  EebusTimerMockExpire(evaluator.Timer());

  // Section 4.3.1, item a: the pairing may be over, so a replacement may ask.
  EXPECT_TRUE(SHIP_PAIRING_IS_ADD_CU_ACTIVATED(evaluator.Get()));
}

TEST_F(ShipPairingTestSuite, AcceptsAReplacementOnlyAfterTheSilenceElapses) {
  Evaluator evaluator;
  ASSERT_NE(evaluator.Get(), nullptr);
  evaluator.SetAnnexASecret();

  const EntryPtr first = AnnexAEntry();
  ASSERT_NE(first, nullptr);
  ASSERT_EQ(SHIP_PAIRING_EVALUATE(evaluator.Get(), first.get()), kShipPairingResultAccepted);

  // A different control unit asks while the pairing is healthy.
  const std::vector<TxtPair> successor = ResignedPairs(WithValue("trustId", "i:99_u:replacement"), ANNEX_A_SECRET);

  const EntryPtr too_early = ParseEntry(successor);
  ASSERT_NE(too_early, nullptr);
  EXPECT_EQ(SHIP_PAIRING_EVALUATE(evaluator.Get(), too_early.get()), kShipPairingResultNotProcessing);

  // After fifteen minutes without message exchange it is allowed to
  // (section 4.3.1, item b.i).
  EebusTimerMockExpire(evaluator.Timer());

  const EntryPtr in_time = ParseEntry(successor);
  ASSERT_NE(in_time, nullptr);
  EXPECT_EQ(SHIP_PAIRING_EVALUATE(evaluator.Get(), in_time.get()), kShipPairingResultAccepted);
  EXPECT_FALSE(SHIP_PAIRING_IS_ADD_CU_ACTIVATED(evaluator.Get()));
}

TEST_F(ShipPairingTestSuite, KeepsThePairingWhenTheTrustedNodeReturns) {
  Evaluator evaluator;
  ASSERT_NE(evaluator.Get(), nullptr);
  evaluator.SetAnnexASecret();

  const EntryPtr entry = AnnexAEntry();
  ASSERT_NE(entry, nullptr);
  ASSERT_EQ(SHIP_PAIRING_EVALUATE(evaluator.Get(), entry.get()), kShipPairingResultAccepted);

  EebusTimerMockExpire(evaluator.Timer());
  ASSERT_TRUE(SHIP_PAIRING_IS_ADD_CU_ACTIVATED(evaluator.Get()));

  // Section 4.3.1, item b.ii: it was a network problem, not a dead control
  // unit. The existing pairing stands and no replacement is accepted.
  SHIP_PAIRING_NOTIFY_MESSAGE_EXCHANGE(evaluator.Get());

  EXPECT_FALSE(SHIP_PAIRING_IS_ADD_CU_ACTIVATED(evaluator.Get()));

  const EntryPtr successor = ParseEntry(ResignedPairs(WithValue("trustId", "i:99_u:replacement"), ANNEX_A_SECRET));
  ASSERT_NE(successor, nullptr);
  EXPECT_EQ(SHIP_PAIRING_EVALUATE(evaluator.Get(), successor.get()), kShipPairingResultNotProcessing);
}

TEST_F(ShipPairingTestSuite, DoesNotCountTheSilenceWhileNothingIsPaired) {
  Evaluator evaluator;
  ASSERT_NE(evaluator.Get(), nullptr);

  // Nothing is paired, so there is no pairing to protect and nothing to time.
  EXPECT_CALL(*evaluator.Timer()->gmock, Start(_, _, _)).Times(0);

  SHIP_PAIRING_NOTIFY_MESSAGE_EXCHANGE(evaluator.Get());

  EXPECT_TRUE(SHIP_PAIRING_IS_ADD_CU_ACTIVATED(evaluator.Get()));
}

TEST_F(ShipPairingTestSuite, KeepsTheReplayHistoryWhenTheUserRemovesThePairing) {
  Evaluator evaluator;
  ASSERT_NE(evaluator.Get(), nullptr);
  evaluator.SetAnnexASecret();

  const EntryPtr entry = AnnexAEntry();
  ASSERT_NE(entry, nullptr);
  ASSERT_EQ(SHIP_PAIRING_EVALUATE(evaluator.Get(), entry.get()), kShipPairingResultAccepted);

  // Section 10.4: removing the trust makes the node process requests again.
  // The ring buffer is not a trust store and must survive it, or the request
  // just removed could be replayed straight back in.
  SHIP_PAIRING_ACTIVATE_ADD_CU(evaluator.Get());

  const EntryPtr replay = AnnexAEntry();
  ASSERT_NE(replay, nullptr);
  EXPECT_EQ(SHIP_PAIRING_EVALUATE(evaluator.Get(), replay.get()), kShipPairingResultAlreadySeen);
}

TEST_F(ShipPairingTestSuite, RejectsInvalidConstructionArguments) {
  TlsCertificateMock* const cert_mock = TlsCertificateMockCreate();
  ASSERT_NE(cert_mock, nullptr);
  EXPECT_CALL(*cert_mock->gmock, Destruct(_)).Times(testing::AnyNumber());

  EXPECT_EQ(ShipPairingCreate(nullptr, TLS_CERTIFICATE_OBJECT(cert_mock)), nullptr);
  EXPECT_EQ(ShipPairingCreate("", TLS_CERTIFICATE_OBJECT(cert_mock)), nullptr);
  EXPECT_EQ(ShipPairingCreate(ANNEX_A_FOR_ID, nullptr), nullptr);

  TlsCertificateMockDelete(cert_mock);
}

namespace {

/** @brief Records what the evaluator reported about being able to accept */
struct EnabledLog {
  std::vector<bool> transitions;
};

void RecordEnabled(bool enabled, void* ctx) {
  static_cast<EnabledLog*>(ctx)->transitions.push_back(enabled);
}

}  // namespace

TEST_F(ShipPairingTestSuite, CannotAcceptAnythingUntilASecretIsSet) {
  Evaluator evaluator;
  ASSERT_NE(evaluator.Get(), nullptr);

  // Nothing could be accepted, so a node has no reason to look for requests.
  EXPECT_FALSE(SHIP_PAIRING_IS_ENABLED(evaluator.Get()));

  evaluator.SetAnnexASecret();
  EXPECT_TRUE(SHIP_PAIRING_IS_ENABLED(evaluator.Get()));
}

TEST_F(ShipPairingTestSuite, CannotAcceptAnythingWhileAlreadyPaired) {
  Evaluator evaluator;
  ASSERT_NE(evaluator.Get(), nullptr);
  evaluator.SetAnnexASecret();

  const EntryPtr entry = AnnexAEntry();
  ASSERT_NE(entry, nullptr);
  ASSERT_EQ(SHIP_PAIRING_EVALUATE(evaluator.Get(), entry.get()), kShipPairingResultAccepted);

  // Section 4.2, step 3 stopped the processing of addCu-requests, so until that
  // changes there is nothing to listen for.
  EXPECT_FALSE(SHIP_PAIRING_IS_ENABLED(evaluator.Get()));

  EebusTimerMockExpire(evaluator.Timer());
  EXPECT_TRUE(SHIP_PAIRING_IS_ENABLED(evaluator.Get()));
}

TEST_F(ShipPairingTestSuite, ReportsOnlyChangesInWhatItCanDo) {
  Evaluator evaluator;
  ASSERT_NE(evaluator.Get(), nullptr);

  EnabledLog log;
  SHIP_PAIRING_SET_ENABLED_CALLBACK(evaluator.Get(), RecordEnabled, &log);

  // Registering says nothing: the node knows the state it started in.
  EXPECT_TRUE(log.transitions.empty());

  evaluator.SetAnnexASecret();
  ASSERT_EQ(log.transitions.size(), 1u);
  EXPECT_TRUE(log.transitions[0]);

  // Setting the same secret again changes nothing, so nothing is reported.
  evaluator.SetAnnexASecret();
  EXPECT_EQ(log.transitions.size(), 1u);

  const EntryPtr entry = AnnexAEntry();
  ASSERT_NE(entry, nullptr);
  ASSERT_EQ(SHIP_PAIRING_EVALUATE(evaluator.Get(), entry.get()), kShipPairingResultAccepted);

  ASSERT_EQ(log.transitions.size(), 2u);
  EXPECT_FALSE(log.transitions[1]);

  EebusTimerMockExpire(evaluator.Timer());
  ASSERT_EQ(log.transitions.size(), 3u);
  EXPECT_TRUE(log.transitions[2]);
}

TEST_F(ShipPairingTestSuite, ReportsThatItCanNoLongerAcceptWhenTheSecretIsCleared) {
  Evaluator evaluator;
  ASSERT_NE(evaluator.Get(), nullptr);
  evaluator.SetAnnexASecret();

  EnabledLog log;
  SHIP_PAIRING_SET_ENABLED_CALLBACK(evaluator.Get(), RecordEnabled, &log);

  SHIP_PAIRING_SET_SECRET(evaluator.Get(), nullptr, 0);

  ASSERT_EQ(log.transitions.size(), 1u);
  EXPECT_FALSE(log.transitions[0]);
  EXPECT_FALSE(SHIP_PAIRING_IS_ENABLED(evaluator.Get()));
}
