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
#include "tests/src/ship/ship_pairing/ship_pairing_test_suite.h"

#include "tests/src/memory_leak.inc"

void ShipPairingTestSuite::TearDown() {
  EXPECT_EQ(heap_used, 0u);
  CheckForMemoryLeaks();
}

std::vector<TxtPair> ShipPairingTestSuite::AnnexAPairs() {
  return {
      {   "txtvers",            SHIP_PAIRING_TXTVERS},
      {   "parType", SHIP_PAIRING_PAR_TYPE_FP_SHA256},
      {     "forId",                  ANNEX_A_FOR_ID},
      {    "forPar",                 ANNEX_A_FOR_PAR},
      {   "trustId",                ANNEX_A_TRUST_ID},
      {  "trustPar",               ANNEX_A_TRUST_PAR},
      {"trustCurve",                     "secp256r1"},
      {      "type",        SHIP_PAIRING_TYPE_ADD_CU},
      {"trustNonce",             ANNEX_A_TRUST_NONCE},
      {       "alg",    SHIP_PAIRING_ALG_HMAC_SHA256},
      {    "digest",                  ANNEX_A_DIGEST},
  };
}

std::string ShipPairingTestSuite::EncodeTxtRecord(const std::vector<TxtPair>& pairs) {
  std::string record;
  for (const auto& [key, value] : pairs) {
    const std::string entry = key + "=" + value;
    record.push_back(static_cast<char>(entry.size()));
    record.append(entry);
  }

  return record;
}

ShipPairingTestSuite::EntryPtr ShipPairingTestSuite::ParseEntry(const std::vector<TxtPair>& pairs) {
  EntryPtr entry{ShipPairingEntryCreate(TEST_INSTANCE_NAME, TEST_DOMAIN, 0), ShipPairingEntryDelete};
  if (entry == nullptr) {
    return entry;
  }

  const std::string record = EncodeTxtRecord(pairs);
  if (ShipPairingEntryParseTxtRecord(entry.get(), record.data(), static_cast<uint16_t>(record.size()))
      != kEebusErrorOk) {
    entry.reset();
  }

  return entry;
}

ShipPairingTestSuite::EntryPtr ShipPairingTestSuite::AnnexAEntry() {
  return ParseEntry(AnnexAPairs());
}
