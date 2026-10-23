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
 * @brief Tests for the certificate and cryptographic primitives
 *
 * The fixtures are the worked example of the SHIP Pairing Service TS 1.0.0,
 * Annex A. Using the specification's own values means these tests fail if an
 * implementation is self-consistent but does not interoperate.
 */

#include <gtest/gtest.h>

#include <cstdint>
#include <cstring>
#include <set>
#include <string>

extern "C" {
#include "src/common/string_util.h"
#include "src/ship/tls_certificate/tls_certificate.h"
}

#include "tests/src/memory_leak.inc"
#include "tests/src/ship/annex_a.h"

namespace {

/**
 * @brief Owns a string returned by the functions under test
 *
 * Distinct from the project's StringPtr, which compares but does not own.
 */
class OwnedString {
 public:
  explicit OwnedString(const char* s) : s_(s) {}
  ~OwnedString() {
    StringDelete(const_cast<char*>(s_));
  }

  OwnedString(const OwnedString&)            = delete;
  OwnedString& operator=(const OwnedString&) = delete;

  const char* Get() const {
    return s_;
  }

 private:
  const char* s_;
};

/**
 * @brief Asserts that everything the functions under test allocated was released
 */
class TlsCertificateTest : public ::testing::Test {
 protected:
  void TearDown() override {
    EXPECT_EQ(heap_used, 0u);
    CheckForMemoryLeaks();
  }
};

TEST_F(TlsCertificateTest, DevACertificateMatchesSpecification) {
  const OwnedString fingerprint(TlsCertificateCalcFingerprintSha256(kDevACertDer, sizeof(kDevACertDer)));

  ASSERT_NE(fingerprint.Get(), nullptr);
  EXPECT_STREQ(fingerprint.Get(), ANNEX_A_FOR_PAR);
}

TEST_F(TlsCertificateTest, DevZCertificateMatchesSpecification) {
  const OwnedString fingerprint(TlsCertificateCalcFingerprintSha256(kDevZCertDer, sizeof(kDevZCertDer)));

  ASSERT_NE(fingerprint.Get(), nullptr);
  EXPECT_STREQ(fingerprint.Get(), ANNEX_A_TRUST_PAR);
}

TEST_F(TlsCertificateTest, IsUppercaseHexOf64Digits) {
  const OwnedString fingerprint(TlsCertificateCalcFingerprintSha256(kDevACertDer, sizeof(kDevACertDer)));

  ASSERT_NE(fingerprint.Get(), nullptr);
  const std::string s(fingerprint.Get());

  EXPECT_EQ(s.size(), 64u);
  EXPECT_EQ(s.find_first_not_of("0123456789ABCDEF"), std::string::npos);
}

TEST_F(TlsCertificateTest, IgnoresOctetsFollowingTheCertificate) {
  // A buffer holding more than the certificate must still yield the
  // certificate's own fingerprint, not a hash of the whole buffer.
  uint8_t padded[sizeof(kDevACertDer) + 16];
  std::memcpy(padded, kDevACertDer, sizeof(kDevACertDer));
  std::memset(padded + sizeof(kDevACertDer), 0xAB, 16);

  const OwnedString fingerprint(TlsCertificateCalcFingerprintSha256(padded, sizeof(padded)));

  ASSERT_NE(fingerprint.Get(), nullptr);
  EXPECT_STREQ(fingerprint.Get(), ANNEX_A_FOR_PAR);
}

TEST_F(TlsCertificateTest, FingerprintRejectsInvalidInput) {
  EXPECT_EQ(TlsCertificateCalcFingerprintSha256(nullptr, 42), nullptr);
  EXPECT_EQ(TlsCertificateCalcFingerprintSha256(kDevACertDer, 0), nullptr);

  const uint8_t not_a_certificate[] = {0x01, 0x02, 0x03, 0x04};
  EXPECT_EQ(TlsCertificateCalcFingerprintSha256(not_a_certificate, sizeof(not_a_certificate)), nullptr);
}

TEST_F(TlsCertificateTest, DevACertificateMatchesItsQrCode) {
  const OwnedString ski(TlsCertificateCalcPublicKeySki(kDevACertDer, sizeof(kDevACertDer)));

  ASSERT_NE(ski.Get(), nullptr);
  EXPECT_STREQ(ski.Get(), ANNEX_A_FOR_SKI);
}

TEST_F(TlsCertificateTest, ReportsTheSpecifiedCurveName) {
  EXPECT_STREQ(TlsCertificateGetCurveName(kDevACertDer, sizeof(kDevACertDer)), TLS_CERTIFICATE_CURVE_SECP256R1);
  EXPECT_STREQ(TlsCertificateGetCurveName(kDevZCertDer, sizeof(kDevZCertDer)), TLS_CERTIFICATE_CURVE_SECP256R1);
}

TEST_F(TlsCertificateTest, ReturnsAStaticStringThatNeedsNoDeallocation) {
  const char* const first  = TlsCertificateGetCurveName(kDevACertDer, sizeof(kDevACertDer));
  const char* const second = TlsCertificateGetCurveName(kDevZCertDer, sizeof(kDevZCertDer));

  EXPECT_EQ(first, second);
}

TEST_F(TlsCertificateTest, CurveNameRejectsInvalidInput) {
  EXPECT_EQ(TlsCertificateGetCurveName(nullptr, 42), nullptr);
  EXPECT_EQ(TlsCertificateGetCurveName(kDevACertDer, 0), nullptr);

  const uint8_t not_a_certificate[] = {0x01, 0x02, 0x03, 0x04};
  EXPECT_EQ(TlsCertificateGetCurveName(not_a_certificate, sizeof(not_a_certificate)), nullptr);
}

TEST_F(TlsCertificateTest, MatchesRfc4231TestCase2) {
  // RFC 4231, section 4.3: key "Jefe", data "what do ya want for nothing?".
  const uint8_t key[]   = {'J', 'e', 'f', 'e'};
  const char* const msg = "what do ya want for nothing?";

  uint8_t digest[TLS_CERTIFICATE_SHA256_SIZE];
  ASSERT_EQ(
      TlsCertificateHmacSha256(key, sizeof(key), reinterpret_cast<const uint8_t*>(msg), std::strlen(msg), digest),
      kEebusErrorOk
  );

  const OwnedString hex(StringWithHexUpper(digest, sizeof(digest)));
  ASSERT_NE(hex.Get(), nullptr);
  EXPECT_STREQ(hex.Get(), "5BDCC146BF60754E6A042426089575C75A003F089D2739839DEC58B964EC3843");
}

TEST_F(TlsCertificateTest, ReproducesTheSpecificationsWorkedDigest) {
  // SHIP Pairing Service TS 1.0.0, A.3. K is the binary concatenation of the
  // devA-secret and the devZ-nonce (section 7.3); M is the ordered
  // concatenation of TXT record keys and values (section 7.4).
  const uint8_t key[]
      = {0x7A, 0x37, 0xDC, 0xF8, 0x1B, 0xDB, 0x50, 0xF8, 0xE9, 0x2C, 0xFA, 0x41, 0x60, 0xCC, 0xB3, 0xDE,
         0xBD, 0xCE, 0xE4, 0x27, 0xFA, 0x72, 0x08, 0xDF, 0x3C, 0x1F, 0x2A, 0x74, 0x9B, 0xA6, 0xF4, 0xD4};

  const char* const msg
      = "txtvers=1;"
        "parType=fpSha256;"
        "forId=i:983327_u:C8277H008F-3;"
        "forPar=C74B7855D3479415F62CC01E5F6D9A93EBC676057D85417ADA16FD1384338943;"
        "trustId=i:46925_u:43652bk-2-gt1;"
        "trustPar=2CC72E781F7A7D2A08D50196C50FEDF0F7BA583F43F76C8C0DDEC9EEF0D005B4;"
        "trustCurve=secp256r1;"
        "type=addCu;"
        "trustNonce=BDCEE427FA7208DF3C1F2A749BA6F4D4;"
        "alg=hmacSha256;";

  uint8_t digest[TLS_CERTIFICATE_SHA256_SIZE];
  ASSERT_EQ(
      TlsCertificateHmacSha256(key, sizeof(key), reinterpret_cast<const uint8_t*>(msg), std::strlen(msg), digest),
      kEebusErrorOk
  );

  const OwnedString hex(StringWithHexUpper(digest, sizeof(digest)));
  ASSERT_NE(hex.Get(), nullptr);
  EXPECT_STREQ(hex.Get(), "BCBB62B2176DA2CEE545784CEB1F2A55E049451B12A549C98E8CA213F001DA25");
}

TEST_F(TlsCertificateTest, AcceptsAnEmptyMessage) {
  const uint8_t key[] = {0x00, 0x01, 0x02, 0x03};
  const uint8_t msg[] = {0x00};

  uint8_t digest[TLS_CERTIFICATE_SHA256_SIZE];
  EXPECT_EQ(TlsCertificateHmacSha256(key, sizeof(key), msg, 0, digest), kEebusErrorOk);
}

TEST_F(TlsCertificateTest, HmacRejectsInvalidInput) {
  const uint8_t key[] = {0x00, 0x01, 0x02, 0x03};
  const uint8_t msg[] = {0x04, 0x05};
  uint8_t digest[TLS_CERTIFICATE_SHA256_SIZE];

  EXPECT_NE(TlsCertificateHmacSha256(nullptr, sizeof(key), msg, sizeof(msg), digest), kEebusErrorOk);
  EXPECT_NE(TlsCertificateHmacSha256(key, sizeof(key), nullptr, sizeof(msg), digest), kEebusErrorOk);
  EXPECT_NE(TlsCertificateHmacSha256(key, sizeof(key), msg, sizeof(msg), nullptr), kEebusErrorOk);
  EXPECT_NE(TlsCertificateHmacSha256(key, 0, msg, sizeof(msg), digest), kEebusErrorOk);
}

TEST_F(TlsCertificateTest, FillsTheWholeBuffer) {
  uint8_t buf[16];
  std::memset(buf, 0xA5, sizeof(buf));

  ASSERT_EQ(TlsCertificateRandomBytes(buf, sizeof(buf)), kEebusErrorOk);

  // A fill of a single repeated octet is astronomically unlikely and would
  // indicate the buffer was left untouched.
  bool all_same = true;
  for (size_t i = 1; i < sizeof(buf); ++i) {
    if (buf[i] != buf[0]) {
      all_same = false;
      break;
    }
  }
  EXPECT_FALSE(all_same);
}

TEST_F(TlsCertificateTest, DoesNotRepeatItself) {
  // A 128 bit value is what a devA-secret and a trustNonce are
  // (SHIP Pairing Service TS 1.0.0, section 6.2), and both must be practically
  // unique per device and per request.
  std::set<std::string> seen;
  for (int i = 0; i < 16; ++i) {
    uint8_t buf[16];
    ASSERT_EQ(TlsCertificateRandomBytes(buf, sizeof(buf)), kEebusErrorOk);
    seen.insert(std::string(reinterpret_cast<const char*>(buf), sizeof(buf)));
  }
  EXPECT_EQ(seen.size(), 16u);
}

TEST_F(TlsCertificateTest, RandomBytesRejectsInvalidInput) {
  uint8_t buf[4];

  EXPECT_NE(TlsCertificateRandomBytes(nullptr, sizeof(buf)), kEebusErrorOk);
  EXPECT_NE(TlsCertificateRandomBytes(buf, 0), kEebusErrorOk);
}

}  // namespace
