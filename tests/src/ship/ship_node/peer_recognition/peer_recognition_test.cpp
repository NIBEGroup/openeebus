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
 * @brief Tests for who a connecting peer is allowed to be
 *
 * SHIP Pairing Service TS 1.0.0, section 10.2: a peer that presents the trusted
 * certificate is to be recognised exactly as one presenting the trusted SKI is.
 * This is the decision that admits a grid control connection, so it is tested
 * on its own rather than only through the connection handling around it.
 */
#include <gtest/gtest.h>

extern "C" {
#include "src/common/service_details.h"
#include "src/ship/ship_node/ship_node_internal.h"
}

#include "tests/src/memory_leak.inc"
#include "tests/src/ship/annex_a.h"

TEST(ShipNodePeerRecognitionTest, RecognisesTheTrustedSki) {
  EXPECT_TRUE(ShipNodeIsPeerRecognised("ski-a", "ski-a", nullptr, nullptr));
  EXPECT_FALSE(ShipNodeIsPeerRecognised("ski-b", "ski-a", nullptr, nullptr));
}

TEST(ShipNodePeerRecognitionTest, RecognisesTheTrustedCertificate) {
  // A node trusted from a shippairing request has no known SKI until it
  // connects, so its certificate is all there is to recognise it by.
  EXPECT_TRUE(ShipNodeIsPeerRecognised("ski-b", nullptr, "AABB", "AABB"));
  EXPECT_TRUE(ShipNodeIsPeerRecognised("ski-b", "ski-a", "AABB", "AABB"));
}

TEST(ShipNodePeerRecognitionTest, DoesNotRecogniseAWrongFingerprint) {
  EXPECT_FALSE(ShipNodeIsPeerRecognised("ski-b", "ski-a", "AABB", "CCDD"));
}

TEST(ShipNodePeerRecognitionTest, RecognisesNobodyWithNothingToCompareAgainst) {
  // The direction that matters: an unset fingerprint must not become a way past
  // the SKI check.
  EXPECT_FALSE(ShipNodeIsPeerRecognised("ski-b", "ski-a", "AABB", nullptr));
  EXPECT_FALSE(ShipNodeIsPeerRecognised("ski-b", "ski-a", "AABB", ""));
  EXPECT_FALSE(ShipNodeIsPeerRecognised("ski-b", nullptr, "AABB", nullptr));
  EXPECT_FALSE(ShipNodeIsPeerRecognised(nullptr, nullptr, nullptr, nullptr));
}

TEST(ShipNodePeerRecognitionTest, RecognisesNobodyWhoPresentedNothing) {
  // A peer whose certificate could not be read must not match a node that does
  // have a fingerprint registered.
  EXPECT_FALSE(ShipNodeIsPeerRecognised("ski-b", "ski-a", nullptr, "AABB"));
  EXPECT_FALSE(ShipNodeIsPeerRecognised("ski-b", "ski-a", "", "AABB"));
}

TEST(ShipNodePeerRecognitionTest, ComparesTheWholeFingerprint) {
  EXPECT_FALSE(ShipNodeIsPeerRecognised("ski-b", "ski-a", "AABB", "AABBCC"));
  EXPECT_FALSE(ShipNodeIsPeerRecognised("ski-b", "ski-a", "AABBCC", "AABB"));
}

TEST(ShipNodePeerRecognitionTest, DoesNotConfuseTheTwoKindsOfIdentity) {
  // An SKI that happens to equal the trusted fingerprint, or the reverse, is
  // not a match: each is compared only against its own kind.
  EXPECT_FALSE(ShipNodeIsPeerRecognised("AABB", nullptr, nullptr, "AABB"));
  EXPECT_FALSE(ShipNodeIsPeerRecognised(nullptr, "AABB", "AABB", nullptr));
}

/**
 * @brief Tests for the certificate fingerprint in the trust store
 *
 * SHIP Pairing Service TS 1.0.0, section 10.4: a service trusted either way is
 * one entry, which may carry both an SKI and a fingerprint.
 */
/*
 * Whether a connection already in hand is promoted when a shippairing request is
 * accepted. Section 4.2 has devZ connecting before it announces and retrying
 * until it is trusted, so at the moment a request is accepted the peer it names
 * is usually already parked in the hello PENDING phase. Without this the
 * decision reaches only the next connection, and the one in hand waits out its
 * own patience first - which on a bench is a minute or two of an installation
 * looking like it failed.
 */

TEST(ShipNodePromotePendingTest, PromotesThePeerThatPresentedTheCertificate) {
  EXPECT_TRUE(ShipNodeShouldPromotePendingPeer(true, "AABB", "AABB"));
}

/* The whole reason this is decided on the certificate. The SKI a pending
 * connection arrived with says only who dialled in first; promoting on that
 * basis would admit them on the strength of a request naming a different
 * node's certificate. Nothing here can be satisfied by an SKI. */
TEST(ShipNodePromotePendingTest, PromotesNobodyWhoPresentedADifferentCertificate) {
  EXPECT_FALSE(ShipNodeShouldPromotePendingPeer(true, "AABB", "CCDD"));
}

TEST(ShipNodePromotePendingTest, PromotesNothingWithNoConnectionInHand) {
  // The fingerprint of a peer that has since gone is not a peer to promote, and
  // approving a handshake that does not exist is at best a no-op.
  EXPECT_FALSE(ShipNodeShouldPromotePendingPeer(false, "AABB", "AABB"));
}

/* An unset fingerprint on either side must not become a way to be promoted.
 * That is the direction that matters: a node with nothing registered, or a
 * backend that reports no peer certificate, must promote nobody rather than
 * everybody. */
TEST(ShipNodePromotePendingTest, PromotesNobodyWithNothingToCompare) {
  EXPECT_FALSE(ShipNodeShouldPromotePendingPeer(true, nullptr, "AABB"));
  EXPECT_FALSE(ShipNodeShouldPromotePendingPeer(true, "", "AABB"));
  EXPECT_FALSE(ShipNodeShouldPromotePendingPeer(true, "AABB", nullptr));
  EXPECT_FALSE(ShipNodeShouldPromotePendingPeer(true, "AABB", ""));
  EXPECT_FALSE(ShipNodeShouldPromotePendingPeer(true, nullptr, nullptr));
}

TEST(ShipNodePromotePendingTest, ComparesTheWholeFingerprint) {
  EXPECT_FALSE(ShipNodeShouldPromotePendingPeer(true, ANNEX_A_FOR_PAR, "C74B7855"));
  EXPECT_TRUE(ShipNodeShouldPromotePendingPeer(true, ANNEX_A_FOR_PAR, ANNEX_A_FOR_PAR));
}

TEST(ServiceDetailsFingerprintTest, HasNoFingerprintUntilOneIsSet) {
  ServiceDetails* const details = ServiceDetailsCreate("ski", "ship-id", "device-type", false);
  ASSERT_NE(details, nullptr);

  EXPECT_EQ(ServiceDetailsGetCertFingerprint(details), nullptr);

  ServiceDetailsDelete(details);
}

TEST(ServiceDetailsFingerprintTest, KeepsTheFingerprintItIsGiven) {
  ServiceDetails* const details = ServiceDetailsCreate("ski", "ship-id", "device-type", false);
  ASSERT_NE(details, nullptr);

  ServiceDetailsSetCertFingerprint(details, ANNEX_A_TRUST_PAR);
  EXPECT_STREQ(ServiceDetailsGetCertFingerprint(details), ANNEX_A_TRUST_PAR);

  // Set again: the previous value is released rather than leaked, which the
  // fixture's leak check confirms.
  ServiceDetailsSetCertFingerprint(details, ANNEX_A_FOR_PAR);
  EXPECT_STREQ(ServiceDetailsGetCertFingerprint(details), ANNEX_A_FOR_PAR);

  ServiceDetailsDelete(details);
  EXPECT_EQ(heap_used, 0u);
  CheckForMemoryLeaks();
}

TEST(ServiceDetailsFingerprintTest, CopiesCarryTheFingerprint) {
  ServiceDetails* const details = ServiceDetailsCreate("ski", "ship-id", "device-type", false);
  ASSERT_NE(details, nullptr);

  ServiceDetailsSetIpv4(details, "192.0.2.1");
  ServiceDetailsSetCertFingerprint(details, ANNEX_A_TRUST_PAR);

  ServiceDetails* const copy = ServiceDetailsCopy(details);
  ASSERT_NE(copy, nullptr);

  ServiceDetailsDelete(details);

  EXPECT_STREQ(ServiceDetailsGetCertFingerprint(copy), ANNEX_A_TRUST_PAR);

  ServiceDetailsDelete(copy);
}

TEST(ServiceDetailsFingerprintTest, CopiesAServiceThatHasNoFingerprint) {
  // A service trusted by a classic SHIP mechanism never has one, and copying it
  // must not fail on that account.
  ServiceDetails* const details = ServiceDetailsCreate("ski", "ship-id", "device-type", false);
  ASSERT_NE(details, nullptr);
  ServiceDetailsSetIpv4(details, "192.0.2.1");

  ServiceDetails* const copy = ServiceDetailsCopy(details);
  ASSERT_NE(copy, nullptr);
  EXPECT_EQ(ServiceDetailsGetCertFingerprint(copy), nullptr);

  ServiceDetailsDelete(details);
  ServiceDetailsDelete(copy);
}
