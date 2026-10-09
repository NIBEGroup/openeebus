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
 * @brief Tests for the shippairing TXT record, one per rule of section 5.4
 */
#include "tests/src/ship/ship_pairing/ship_pairing_test_suite.h"

#include <algorithm>

namespace {

/** @brief Returns the Annex A pairs with the value of @p key replaced */
std::vector<TxtPair> WithValue(const std::string& key, const std::string& value) {
  std::vector<TxtPair> pairs = ShipPairingTestSuite::AnnexAPairs();
  for (auto& pair : pairs) {
    if (pair.first == key) {
      pair.second = value;
    }
  }

  return pairs;
}

}  // namespace

TEST_F(ShipPairingTestSuite, ParsesEveryKeyOfTheSpecifiedRecord) {
  const EntryPtr entry = AnnexAEntry();
  ASSERT_NE(entry, nullptr);

  EXPECT_STREQ(ShipPairingEntryGetTxtVers(entry.get()), SHIP_PAIRING_TXTVERS);
  EXPECT_STREQ(ShipPairingEntryGetParType(entry.get()), SHIP_PAIRING_PAR_TYPE_FP_SHA256);
  EXPECT_STREQ(ShipPairingEntryGetForId(entry.get()), ANNEX_A_FOR_ID);
  EXPECT_STREQ(ShipPairingEntryGetForPar(entry.get()), ANNEX_A_FOR_PAR);
  EXPECT_STREQ(ShipPairingEntryGetTrustId(entry.get()), ANNEX_A_TRUST_ID);
  EXPECT_STREQ(ShipPairingEntryGetTrustPar(entry.get()), ANNEX_A_TRUST_PAR);
  EXPECT_STREQ(ShipPairingEntryGetTrustCurve(entry.get()), "secp256r1");
  EXPECT_STREQ(ShipPairingEntryGetType(entry.get()), SHIP_PAIRING_TYPE_ADD_CU);
  EXPECT_STREQ(ShipPairingEntryGetTrustNonce(entry.get()), ANNEX_A_TRUST_NONCE);
  EXPECT_STREQ(ShipPairingEntryGetAlg(entry.get()), SHIP_PAIRING_ALG_HMAC_SHA256);
  EXPECT_STREQ(ShipPairingEntryGetDigest(entry.get()), ANNEX_A_DIGEST);

  EXPECT_STREQ(ShipPairingEntryGetName(entry.get()), TEST_INSTANCE_NAME);
  EXPECT_STREQ(ShipPairingEntryGetDomain(entry.get()), TEST_DOMAIN);

  EXPECT_TRUE(ShipPairingEntryIsValid(entry.get()));
}

TEST_F(ShipPairingTestSuite, CopyIsIndependentOfItsSource) {
  EntryPtr source = AnnexAEntry();
  ASSERT_NE(source, nullptr);

  EntryPtr copy{ShipPairingEntryCopy(source.get()), ShipPairingEntryDelete};
  ASSERT_NE(copy, nullptr);

  source.reset();

  EXPECT_STREQ(ShipPairingEntryGetDigest(copy.get()), ANNEX_A_DIGEST);
  EXPECT_STREQ(ShipPairingEntryGetForId(copy.get()), ANNEX_A_FOR_ID);
  EXPECT_TRUE(ShipPairingEntryIsValid(copy.get()));
}

TEST_F(ShipPairingTestSuite, IgnoresKeysOutsideTheSchema) {
  std::vector<TxtPair> pairs = AnnexAPairs();
  pairs.push_back({"somethingNew", "whatever"});

  const EntryPtr entry = ParseEntry(pairs);
  ASSERT_NE(entry, nullptr);

  // Section 5.4: a key not listed in table 1 is ignored, so the record stays
  // valid. A future version of the specification may add keys.
  EXPECT_TRUE(ShipPairingEntryIsValid(entry.get()));
}

TEST_F(ShipPairingTestSuite, RejectsARecordWhoseFirstKeyIsNotTxtvers) {
  std::vector<TxtPair> pairs = AnnexAPairs();
  std::swap(pairs[0], pairs[1]);

  const EntryPtr entry = ParseEntry(pairs);
  ASSERT_NE(entry, nullptr);

  // Every key is present and permitted, so only the ordering rule rejects this.
  EXPECT_STREQ(ShipPairingEntryGetTxtVers(entry.get()), SHIP_PAIRING_TXTVERS);
  EXPECT_FALSE(ShipPairingEntryIsValid(entry.get()));
}

TEST_F(ShipPairingTestSuite, RejectsARecordMissingAMandatoryKey) {
  const std::vector<std::string> keys = {
      "txtvers",
      "parType",
      "forId",
      "forPar",
      "trustId",
      "trustPar",
      "trustCurve",
      "type",
      "trustNonce",
      "alg",
      "digest",
  };

  for (const auto& missing : keys) {
    std::vector<TxtPair> pairs = AnnexAPairs();
    pairs.erase(
        std::remove_if(pairs.begin(), pairs.end(), [&](const TxtPair& p) { return p.first == missing; }),
        pairs.end()
    );

    const EntryPtr entry = ParseEntry(pairs);
    ASSERT_NE(entry, nullptr) << "missing key: " << missing;
    EXPECT_FALSE(ShipPairingEntryIsValid(entry.get())) << "missing key: " << missing;
  }
}

TEST_F(ShipPairingTestSuite, RejectsUnsupportedSingleValuedKeys) {
  const std::vector<TxtPair> unsupported = {
      {   "txtvers",          "2"},
      {   "parType",   "fpSha512"},
      {      "type",   "removeCu"},
      {       "alg", "hmacSha512"},
      {"trustCurve",  "secp384r1"},
  };

  for (const auto& [key, value] : unsupported) {
    const EntryPtr entry = ParseEntry(WithValue(key, value));
    ASSERT_NE(entry, nullptr) << key << "=" << value;
    EXPECT_FALSE(ShipPairingEntryIsValid(entry.get())) << key << "=" << value;
  }
}

TEST_F(ShipPairingTestSuite, AcceptsEveryCurvePermittedForAShipCertificate) {
  for (const auto* curve : {"secp256r1", "brainpoolP256r1", "brainpoolP384r1"}) {
    const EntryPtr entry = ParseEntry(WithValue("trustCurve", curve));
    ASSERT_NE(entry, nullptr) << curve;
    EXPECT_TRUE(ShipPairingEntryIsValid(entry.get())) << curve;
  }
}

TEST_F(ShipPairingTestSuite, RejectsHexadecimalValuesOfTheWrongLength) {
  const std::vector<TxtPair> wrong_length = {
      {    "forPar", std::string(63, 'A')},
      {    "forPar", std::string(65, 'A')},
      {  "trustPar", std::string(63, 'A')},
      {    "digest", std::string(65, 'A')},
      {"trustNonce", std::string(31, 'A')},
      {"trustNonce", std::string(33, 'A')},
  };

  for (const auto& [key, value] : wrong_length) {
    const EntryPtr entry = ParseEntry(WithValue(key, value));
    ASSERT_NE(entry, nullptr) << key << " length " << value.size();
    EXPECT_FALSE(ShipPairingEntryIsValid(entry.get())) << key << " length " << value.size();
  }
}

TEST_F(ShipPairingTestSuite, RejectsHexadecimalValuesThatAreNotUppercase) {
  // Section 5.4 specifies [0-9A-F]. SKIs are rendered lowercase elsewhere in
  // SHIP, so a value in the wrong case is a plausible mistake rather than a
  // far fetched one.
  std::string lowercase = ANNEX_A_FOR_PAR;
  std::transform(lowercase.begin(), lowercase.end(), lowercase.begin(), [](unsigned char c) {
    return static_cast<char>(std::tolower(c));
  });

  const EntryPtr entry = ParseEntry(WithValue("forPar", lowercase));
  ASSERT_NE(entry, nullptr);
  EXPECT_FALSE(ShipPairingEntryIsValid(entry.get()));
}

TEST_F(ShipPairingTestSuite, RejectsHexadecimalValuesHoldingNonHexadecimalDigits) {
  const EntryPtr entry = ParseEntry(WithValue("digest", std::string(63, 'A') + "G"));
  ASSERT_NE(entry, nullptr);
  EXPECT_FALSE(ShipPairingEntryIsValid(entry.get()));
}

TEST_F(ShipPairingTestSuite, RejectsEmptyShipIds) {
  // An empty value cannot be encoded as "key=" in a record whose entries carry
  // their own length, so absence is how it presents.
  for (const auto* key : {"forId", "trustId"}) {
    std::vector<TxtPair> pairs = AnnexAPairs();
    pairs.erase(
        std::remove_if(pairs.begin(), pairs.end(), [&](const TxtPair& p) { return p.first == key; }),
        pairs.end()
    );

    const EntryPtr entry = ParseEntry(pairs);
    ASSERT_NE(entry, nullptr) << key;
    EXPECT_FALSE(ShipPairingEntryIsValid(entry.get())) << key;
  }
}

TEST_F(ShipPairingTestSuite, KeepsTheFirstValueOfARepeatedKey) {
  // A repeated key must not let a later value replace what the digest was
  // calculated over.
  std::vector<TxtPair> pairs = AnnexAPairs();
  pairs.push_back({"trustId", "i:1_u:attacker"});

  const EntryPtr entry = ParseEntry(pairs);
  ASSERT_NE(entry, nullptr);

  EXPECT_STREQ(ShipPairingEntryGetTrustId(entry.get()), ANNEX_A_TRUST_ID);
}

TEST_F(ShipPairingTestSuite, DoesNotMistakeATruncatedKeyForAKnownOne) {
  std::vector<TxtPair> pairs = AnnexAPairs();
  pairs.push_back({"trust", "not a real key"});

  const EntryPtr entry = ParseEntry(pairs);
  ASSERT_NE(entry, nullptr);

  EXPECT_STREQ(ShipPairingEntryGetTrustId(entry.get()), ANNEX_A_TRUST_ID);
  EXPECT_TRUE(ShipPairingEntryIsValid(entry.get()));
}

TEST_F(ShipPairingTestSuite, RejectsAMalformedRecord) {
  EntryPtr entry{ShipPairingEntryCreate(TEST_INSTANCE_NAME, TEST_DOMAIN, 0), ShipPairingEntryDelete};
  ASSERT_NE(entry, nullptr);

  // A length octet claiming more than the record holds.
  const char truncated[]
      = "\x40"
        "txtvers=1";
  EXPECT_EQ(ShipPairingEntryParseTxtRecord(entry.get(), truncated, sizeof(truncated) - 1), kEebusErrorParse);
}

TEST_F(ShipPairingTestSuite, RejectsANullEntry) {
  EXPECT_FALSE(ShipPairingEntryIsValid(nullptr));
  EXPECT_EQ(ShipPairingEntryCopy(nullptr), nullptr);
  EXPECT_EQ(ShipPairingEntryParseTxtRecord(nullptr, "", 0), kEebusErrorParse);
}
