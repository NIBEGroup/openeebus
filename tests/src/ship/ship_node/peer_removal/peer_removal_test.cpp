/*
 * Copyright 2026 NIBE AB
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
 * @brief Tests for the ship node removing peers it no longer trusts
 *
 * Two requests remove peers: unregistering a peer configured by SHIP ID and
 * fingerprint (SHIP_NODE_UNREGISTER_REMOTE_FINGERPRINT_WITH_SHIP_ID()), and
 * replacing the fingerprint accepted from a pairing request, which revokes the
 * peers trusted for the previous one (SHIP_NODE_REGISTER_REMOTE_FINGERPRINT()).
 *
 * Both only queue the work; the removal itself happens on the connection loop
 * thread, so each test starts the node to get that thread running.
 *
 * SHIP_NODE_STOP() cannot be used to wait for the queued work: it sets the
 * cancel flag before queuing its own message, and a loop that sees the flag
 * first exits without handling what is still queued. WaitForQueuedRequests()
 * instead queues the unregistering of a barrier peer and waits until that peer
 * is gone. The queue is handled in order, so by then everything queued before
 * it has been handled too.
 *
 * Stopping the node afterwards also stops, reports and deletes the connections
 * it still holds, but leaves their entries in the table. A peer that is gone
 * from the table was therefore removed by the request.
 */
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <chrono>
#include <thread>

extern "C" {
#include "src/common/service_details.h"
#include "src/ship/mdns/ship_mdns.h"
#include "src/ship/ship_connection/ship_connection.h"
#include "src/ship/ship_connection/ship_connection_trust.h"
#include "src/ship/ship_node/node_connection_internal.h"
#include "src/ship/ship_node/ship_node.h"
#include "src/ship/ship_node/ship_node_internal.h"
#include "src/ship/ship_pairing/ship_pairing.h"
#include "src/ship/websocket/http_server.h"
#include "src/ship/websocket/websocket_client_creator.h"
}

#include "tests/src/memory_leak.inc"
#include "tests/src/mocks/common/eebus_timer/eebus_timer_mock.h"
#include "tests/src/mocks/ship/api/ship_node_reader_mock.h"
#include "tests/src/mocks/ship/mdns/mdns_mock.h"
#include "tests/src/mocks/ship/ship_connection/ship_connection_mock.h"
#include "tests/src/mocks/ship/websocket/http_server_mock.h"

using testing::_;
using testing::InSequence;
using testing::Return;
using testing::StrEq;

// ship_node.c is linked as is. The node creates its mDNS and HTTP server
// through these, so the test hands out mocks and keeps them to set
// expectations on.
static MdnsMock* mdns_mock              = nullptr;
static HttpServerMock* http_server_mock = nullptr;

ShipMdnsObject*
ShipMdnsCreate(const char*, const EebusDeviceInfo*, const char*, int, OnMdnsEntriesFoundCallback, void*) {
  mdns_mock = MdnsMockCreate();
  return SHIP_MDNS_OBJECT(mdns_mock);
}

HttpServerObject* HttpServerCreate(int, const TlsCertificateObject*, WebsocketServerCallbackType, void*) {
  http_server_mock = HttpServerMockCreate();
  return HTTP_SERVER_OBJECT(http_server_mock);
}

// The pairing service and outgoing connections are not involved in
// unregistering a peer.
ShipPairingObject* ShipPairingCreate(const char*, const TlsCertificateObject*) {
  return nullptr;
}

void ShipPairingSetMessageExchangeAvailableCallback(ShipPairingObject*, ShipPairingMessageExchangeAvailableCallback, void*) {
}

EebusError
ShipConnectionApprovePendingHandshakeWithToken(ShipConnectionObject*, uint64_t, ShipConnectionTrustAcceptanceCallback, void*) {
  return kEebusErrorOk;
}

WebsocketCreatorObject*
WebsocketClientCreatorCreateWithFingerprint(const char*, const TlsCertificateObject*, const char*, const char*) {
  return nullptr;
}

ShipConnectionObject* ShipConnectionCreate(InfoProviderObject*, ShipRole, const char*, const char*, const char*) {
  return nullptr;
}

static constexpr const char* kShipIdA            = "ship-id-a";
static constexpr const char* kShipIdB            = "ship-id-b";
static constexpr const char* kFingerprintA       = "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";
static constexpr const char* kFingerprintB       = "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB";
static constexpr const char* kFingerprintD       = "DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD";
static constexpr const char* kBarrierShipId      = "ship-id-barrier";
static constexpr const char* kBarrierFingerprint = "CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC";

static void NoOpRetryFn(void*) {}

class ShipNodePeerRemovalTest : public ::testing::Test {
 protected:
  void SetUp() override {
    reader_                = ShipNodeReaderMockCreate();
    local_service_details_ = ServiceDetailsCreate("local-ski", "local-ship-id", "EnergyManagementSystem", false);
    ASSERT_NE(local_service_details_, nullptr);
    // A server node never connects out, so registering a peer queues nothing.
    ship_node_ = ShipNodeCreate(
        "local-ski",
        "server",
        nullptr,
        "local-service",
        4711,
        nullptr,
        SHIP_NODE_READER_OBJECT(reader_),
        local_service_details_,
        kEebusTrustModePreTrust
    );
    ASSERT_NE(ship_node_, nullptr);
    ASSERT_NE(mdns_mock, nullptr);
    ASSERT_NE(http_server_mock, nullptr);

    EXPECT_CALL(*mdns_mock->gmock, Start(_)).WillOnce(Return(kEebusErrorOk));
    EXPECT_CALL(*mdns_mock->gmock, Stop(_));
    EXPECT_CALL(*mdns_mock->gmock, Destruct(_));
    EXPECT_CALL(*http_server_mock->gmock, Start(_)).WillOnce(Return(kEebusErrorOk));
    EXPECT_CALL(*http_server_mock->gmock, Stop(_));
    EXPECT_CALL(*http_server_mock->gmock, Destruct(_));
  }

  void TearDown() override {
    ShipNodeDelete(ship_node_);
    mdns_mock        = nullptr;
    http_server_mock = nullptr;
    ServiceDetailsDelete(local_service_details_);
    EXPECT_CALL(*reader_->gmock, Destruct(_));
    ShipNodeReaderMockDelete(reader_);
    EXPECT_EQ(heap_used, 0);
    CheckForMemoryLeaks();
  }

  ShipNode* Node() {
    return SHIP_NODE(ship_node_);
  }

  size_t PeerCount() {
    return NODE_CONNECTION_CONTAINER_GET_SIZE(Node()->connections);
  }

  NodeConnection* Peer(size_t i) {
    return NODE_CONNECTION(NODE_CONNECTION_CONTAINER_GET_WITH_INDEX(Node()->connections, i));
  }

  bool HasPeerWithShipId(const char* ship_id) {
    EEBUS_MUTEX_LOCK(Node()->mutex);
    bool found = false;
    for (size_t i = 0; i < PeerCount(); ++i) {
      found = found || ((Peer(i)->ship_id != nullptr) && (strcmp(Peer(i)->ship_id, ship_id) == 0));
    }
    EEBUS_MUTEX_UNLOCK(Node()->mutex);
    return found;
  }

  // Registering the barrier peer itself queues nothing on a server node.
  void WaitForQueuedRequests() {
    EXPECT_EQ(
        SHIP_NODE_REGISTER_REMOTE_FINGERPRINT_WITH_SHIP_ID(ship_node_, kBarrierShipId, kBarrierFingerprint),
        kEebusErrorOk
    );
    EXPECT_EQ(
        SHIP_NODE_UNREGISTER_REMOTE_FINGERPRINT_WITH_SHIP_ID(ship_node_, kBarrierShipId, kBarrierFingerprint),
        kEebusErrorOk
    );

    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(5);
    while (HasPeerWithShipId(kBarrierShipId) && (std::chrono::steady_clock::now() < deadline)) {
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    EXPECT_FALSE(HasPeerWithShipId(kBarrierShipId)) << "the connection loop did not handle the request";
  }

  EebusError UnregisterAndWait(const char* ship_id, const char* fingerprint) {
    const EebusError ret = SHIP_NODE_UNREGISTER_REMOTE_FINGERPRINT_WITH_SHIP_ID(ship_node_, ship_id, fingerprint);
    WaitForQueuedRequests();
    return ret;
  }

  void ReplacePairingFingerprintAndWait(const char* fingerprint) {
    SHIP_NODE_REGISTER_REMOTE_FINGERPRINT(ship_node_, fingerprint);
    WaitForQueuedRequests();
  }

  // The state a peer is left in once it was trusted from an accepted pairing
  // request; see ShipNodePromoteFingerprint().
  NodeConnection* AddPeerTrustedByPairing(const char* ski, const char* fingerprint) {
    NodeConnectionObject* const nc
        = NODE_CONNECTION_CONTAINER_GET_OR_CREATE(Node()->connections, ski, Node(), NoOpRetryFn);
    EXPECT_NE(nc, nullptr);
    NodeConnection* const peer = NODE_CONNECTION(nc);
    peer->is_trusted           = true;
    peer->trusted_by_pairing   = true;
    peer->expected_fingerprint = StringCopy(fingerprint);
    return peer;
  }

  ShipNodeReaderMock* reader_            = nullptr;
  ServiceDetails* local_service_details_ = nullptr;
  ShipNodeObject* ship_node_             = nullptr;
};

using ShipNodeUnregisterFingerprintTest = ShipNodePeerRemovalTest;

TEST_F(ShipNodeUnregisterFingerprintTest, StopsReportsAndDeletesTheConnectionOfTheMatchingPeer) {
  ASSERT_EQ(SHIP_NODE_REGISTER_REMOTE_FINGERPRINT_WITH_SHIP_ID(ship_node_, kShipIdA, kFingerprintA), kEebusErrorOk);
  ASSERT_EQ(PeerCount(), 1u);
  ShipConnectionMock* const sc = ShipConnectionMockCreate();
  Peer(0)->connection          = SHIP_CONNECTION_OBJECT(sc);

  {
    // The SKI is read from the connection, so it has to be reported before the
    // connection is deleted.
    InSequence seq;
    EXPECT_CALL(*sc->gmock, Stop(SHIP_CONNECTION_OBJECT(sc)));
    EXPECT_CALL(*sc->gmock, GetRemoteSki(SHIP_CONNECTION_OBJECT(sc))).WillOnce(Return("remote-ski-a"));
    EXPECT_CALL(*reader_->gmock, OnRemoteSkiDisconnected(SHIP_NODE_READER_OBJECT(reader_), StrEq("remote-ski-a")));
    EXPECT_CALL(*sc->gmock, Destruct(_));
  }

  SHIP_NODE_START(ship_node_);
  EXPECT_EQ(UnregisterAndWait(kShipIdA, kFingerprintA), kEebusErrorOk);
  SHIP_NODE_STOP(ship_node_);

  EXPECT_EQ(PeerCount(), 0u);
}

TEST_F(ShipNodeUnregisterFingerprintTest, RemovesAMatchingPeerThatIsNotConnected) {
  ASSERT_EQ(SHIP_NODE_REGISTER_REMOTE_FINGERPRINT_WITH_SHIP_ID(ship_node_, kShipIdA, kFingerprintA), kEebusErrorOk);
  ASSERT_EQ(PeerCount(), 1u);
  ASSERT_EQ(Peer(0)->connection, nullptr);

  EXPECT_CALL(*reader_->gmock, OnRemoteSkiDisconnected(_, _)).Times(0);

  SHIP_NODE_START(ship_node_);
  EXPECT_EQ(UnregisterAndWait(kShipIdA, kFingerprintA), kEebusErrorOk);
  SHIP_NODE_STOP(ship_node_);

  EXPECT_EQ(PeerCount(), 0u);
}

TEST_F(ShipNodeUnregisterFingerprintTest, MatchesTheFingerprintRegardlessOfCase) {
  ASSERT_EQ(SHIP_NODE_REGISTER_REMOTE_FINGERPRINT_WITH_SHIP_ID(ship_node_, kShipIdA, kFingerprintA), kEebusErrorOk);
  ASSERT_EQ(PeerCount(), 1u);

  SHIP_NODE_START(ship_node_);
  EXPECT_EQ(
      UnregisterAndWait(kShipIdA, "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"),
      kEebusErrorOk
  );
  SHIP_NODE_STOP(ship_node_);

  EXPECT_EQ(PeerCount(), 0u);
}

TEST_F(ShipNodeUnregisterFingerprintTest, RejectsAnInvalidRequestAndKeepsThePeer) {
  ASSERT_EQ(SHIP_NODE_REGISTER_REMOTE_FINGERPRINT_WITH_SHIP_ID(ship_node_, kShipIdA, kFingerprintA), kEebusErrorOk);
  ASSERT_EQ(PeerCount(), 1u);

  SHIP_NODE_START(ship_node_);
  EXPECT_EQ(
      SHIP_NODE_UNREGISTER_REMOTE_FINGERPRINT_WITH_SHIP_ID(ship_node_, "", kFingerprintA),
      kEebusErrorInputArgument
  );
  EXPECT_EQ(
      SHIP_NODE_UNREGISTER_REMOTE_FINGERPRINT_WITH_SHIP_ID(ship_node_, kShipIdA, "not-a-fingerprint"),
      kEebusErrorInputArgument
  );
  SHIP_NODE_STOP(ship_node_);

  ASSERT_EQ(PeerCount(), 1u);
  EXPECT_STREQ(Peer(0)->ship_id, kShipIdA);
}

TEST_F(ShipNodeUnregisterFingerprintTest, KeepsPeersWithAnotherShipIdOrFingerprint) {
  ASSERT_EQ(SHIP_NODE_REGISTER_REMOTE_FINGERPRINT_WITH_SHIP_ID(ship_node_, kShipIdB, kFingerprintA), kEebusErrorOk);
  ASSERT_EQ(SHIP_NODE_REGISTER_REMOTE_FINGERPRINT_WITH_SHIP_ID(ship_node_, kShipIdA, kFingerprintB), kEebusErrorOk);
  ASSERT_EQ(PeerCount(), 2u);
  ShipConnectionMock* const other_ship_id     = ShipConnectionMockCreate();
  ShipConnectionMock* const other_fingerprint = ShipConnectionMockCreate();
  Peer(0)->connection                         = SHIP_CONNECTION_OBJECT(other_ship_id);
  Peer(1)->connection                         = SHIP_CONNECTION_OBJECT(other_fingerprint);

  // Stopping the node is what closes these two connections, once each.
  EXPECT_CALL(*other_ship_id->gmock, Stop(_));
  EXPECT_CALL(*other_ship_id->gmock, GetRemoteSki(_)).WillOnce(Return("remote-ski-b"));
  EXPECT_CALL(*other_ship_id->gmock, Destruct(_));
  EXPECT_CALL(*other_fingerprint->gmock, Stop(_));
  EXPECT_CALL(*other_fingerprint->gmock, GetRemoteSki(_)).WillOnce(Return("remote-ski-c"));
  EXPECT_CALL(*other_fingerprint->gmock, Destruct(_));
  EXPECT_CALL(*reader_->gmock, OnRemoteSkiDisconnected(_, StrEq("remote-ski-b")));
  EXPECT_CALL(*reader_->gmock, OnRemoteSkiDisconnected(_, StrEq("remote-ski-c")));

  SHIP_NODE_START(ship_node_);
  EXPECT_EQ(UnregisterAndWait(kShipIdA, kFingerprintA), kEebusErrorOk);
  SHIP_NODE_STOP(ship_node_);

  ASSERT_EQ(PeerCount(), 2u);
  EXPECT_STREQ(Peer(0)->ship_id, kShipIdB);
  EXPECT_STREQ(Peer(0)->expected_fingerprint, kFingerprintA);
  EXPECT_STREQ(Peer(1)->ship_id, kShipIdA);
  EXPECT_STREQ(Peer(1)->expected_fingerprint, kFingerprintB);
}

TEST_F(ShipNodeUnregisterFingerprintTest, KeepsAPeerTrustedFromAPairingRequest) {
  ASSERT_EQ(SHIP_NODE_REGISTER_REMOTE_FINGERPRINT_WITH_SHIP_ID(ship_node_, kShipIdA, kFingerprintA), kEebusErrorOk);
  ASSERT_EQ(PeerCount(), 1u);
  // That trust is withdrawn through the pairing service, not by unregistering.
  Peer(0)->trusted_by_pairing  = true;
  ShipConnectionMock* const sc = ShipConnectionMockCreate();
  Peer(0)->connection          = SHIP_CONNECTION_OBJECT(sc);

  EXPECT_CALL(*sc->gmock, Stop(_));
  EXPECT_CALL(*sc->gmock, GetRemoteSki(_)).WillOnce(Return("remote-ski-a"));
  EXPECT_CALL(*sc->gmock, Destruct(_));
  EXPECT_CALL(*reader_->gmock, OnRemoteSkiDisconnected(_, StrEq("remote-ski-a")));

  SHIP_NODE_START(ship_node_);
  EXPECT_EQ(UnregisterAndWait(kShipIdA, kFingerprintA), kEebusErrorOk);
  SHIP_NODE_STOP(ship_node_);

  ASSERT_EQ(PeerCount(), 1u);
  EXPECT_STREQ(Peer(0)->ship_id, kShipIdA);
}

TEST_F(ShipNodeUnregisterFingerprintTest, RemovesEveryMatchingPeerAndKeepsTheOthers) {
  // Registering the same SHIP ID and fingerprint twice reuses the first entry,
  // so the second matching entry is one that was first known by its SKI and
  // was given the same SHIP ID and fingerprint afterwards.
  ASSERT_EQ(SHIP_NODE_REGISTER_REMOTE_FINGERPRINT_WITH_SHIP_ID(ship_node_, kShipIdA, kFingerprintA), kEebusErrorOk);
  NodeConnectionObject* const by_ski
      = NODE_CONNECTION_CONTAINER_GET_OR_CREATE(Node()->connections, "remote-ski-2", Node(), NoOpRetryFn);
  ASSERT_NE(by_ski, nullptr);
  NODE_CONNECTION(by_ski)->ship_id              = StringCopy(kShipIdA);
  NODE_CONNECTION(by_ski)->expected_fingerprint = StringCopy(kFingerprintA);
  ASSERT_EQ(SHIP_NODE_REGISTER_REMOTE_FINGERPRINT_WITH_SHIP_ID(ship_node_, kShipIdB, kFingerprintB), kEebusErrorOk);
  ASSERT_EQ(PeerCount(), 3u);

  // The two matching peers sit next to each other, so removing the first one
  // moves the second one to the index that was just checked.
  ShipConnectionMock* const first  = ShipConnectionMockCreate();
  ShipConnectionMock* const second = ShipConnectionMockCreate();
  ShipConnectionMock* const other  = ShipConnectionMockCreate();
  Peer(0)->connection              = SHIP_CONNECTION_OBJECT(first);
  Peer(1)->connection              = SHIP_CONNECTION_OBJECT(second);
  Peer(2)->connection              = SHIP_CONNECTION_OBJECT(other);

  EXPECT_CALL(*first->gmock, Stop(_));
  EXPECT_CALL(*first->gmock, GetRemoteSki(_)).WillOnce(Return("remote-ski-1"));
  EXPECT_CALL(*first->gmock, Destruct(_));
  EXPECT_CALL(*second->gmock, Stop(_));
  EXPECT_CALL(*second->gmock, GetRemoteSki(_)).WillOnce(Return("remote-ski-2"));
  EXPECT_CALL(*second->gmock, Destruct(_));
  EXPECT_CALL(*other->gmock, Stop(_));
  EXPECT_CALL(*other->gmock, GetRemoteSki(_)).WillOnce(Return("remote-ski-3"));
  EXPECT_CALL(*other->gmock, Destruct(_));
  EXPECT_CALL(*reader_->gmock, OnRemoteSkiDisconnected(_, StrEq("remote-ski-1")));
  EXPECT_CALL(*reader_->gmock, OnRemoteSkiDisconnected(_, StrEq("remote-ski-2")));
  EXPECT_CALL(*reader_->gmock, OnRemoteSkiDisconnected(_, StrEq("remote-ski-3")));

  SHIP_NODE_START(ship_node_);
  EXPECT_EQ(UnregisterAndWait(kShipIdA, kFingerprintA), kEebusErrorOk);
  SHIP_NODE_STOP(ship_node_);

  ASSERT_EQ(PeerCount(), 1u);
  EXPECT_STREQ(Peer(0)->ship_id, kShipIdB);
}

using ShipNodeRevokePairingFingerprintTest = ShipNodePeerRemovalTest;

TEST_F(ShipNodeRevokePairingFingerprintTest, StopsReportsAndDeletesTheConnectionOfThePreviouslyPairedPeer) {
  SHIP_NODE_REGISTER_REMOTE_FINGERPRINT(ship_node_, kFingerprintA);
  NodeConnection* const paired = AddPeerTrustedByPairing("remote-ski-a", kFingerprintA);
  ShipConnectionMock* const sc = ShipConnectionMockCreate();
  paired->connection           = SHIP_CONNECTION_OBJECT(sc);
  ASSERT_EQ(PeerCount(), 1u);

  {
    InSequence seq;
    EXPECT_CALL(*sc->gmock, Stop(SHIP_CONNECTION_OBJECT(sc)));
    EXPECT_CALL(*sc->gmock, GetRemoteSki(SHIP_CONNECTION_OBJECT(sc))).WillOnce(Return("remote-ski-a"));
    EXPECT_CALL(*reader_->gmock, OnRemoteSkiDisconnected(SHIP_NODE_READER_OBJECT(reader_), StrEq("remote-ski-a")));
    EXPECT_CALL(*sc->gmock, Destruct(_));
  }

  SHIP_NODE_START(ship_node_);
  ReplacePairingFingerprintAndWait(kFingerprintB);
  SHIP_NODE_STOP(ship_node_);

  EXPECT_EQ(PeerCount(), 0u);
}

TEST_F(ShipNodeRevokePairingFingerprintTest, RemovesAPreviouslyPairedPeerThatIsNotConnected) {
  SHIP_NODE_REGISTER_REMOTE_FINGERPRINT(ship_node_, kFingerprintA);
  AddPeerTrustedByPairing("remote-ski-a", kFingerprintA);
  ASSERT_EQ(PeerCount(), 1u);

  EXPECT_CALL(*reader_->gmock, OnRemoteSkiDisconnected(_, _)).Times(0);

  SHIP_NODE_START(ship_node_);
  ReplacePairingFingerprintAndWait(kFingerprintB);
  SHIP_NODE_STOP(ship_node_);

  EXPECT_EQ(PeerCount(), 0u);
}

TEST_F(ShipNodeRevokePairingFingerprintTest, RevokesNothingWhenTheSameFingerprintIsRegisteredAgain) {
  SHIP_NODE_REGISTER_REMOTE_FINGERPRINT(ship_node_, kFingerprintA);
  AddPeerTrustedByPairing("remote-ski-a", kFingerprintA);
  ASSERT_EQ(PeerCount(), 1u);

  SHIP_NODE_START(ship_node_);
  // Fingerprints are compared regardless of case, so this is not a new one.
  ReplacePairingFingerprintAndWait("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
  SHIP_NODE_STOP(ship_node_);

  ASSERT_EQ(PeerCount(), 1u);
  EXPECT_TRUE(Peer(0)->trusted_by_pairing);
}

TEST_F(ShipNodeRevokePairingFingerprintTest, KeepsConfiguredPeersAndPeersPairedWithAnotherFingerprint) {
  SHIP_NODE_REGISTER_REMOTE_FINGERPRINT(ship_node_, kFingerprintA);
  // Configured with the same fingerprint, but not trusted through pairing.
  ASSERT_EQ(SHIP_NODE_REGISTER_REMOTE_FINGERPRINT_WITH_SHIP_ID(ship_node_, kShipIdA, kFingerprintA), kEebusErrorOk);
  AddPeerTrustedByPairing("remote-ski-d", kFingerprintD);
  ASSERT_EQ(PeerCount(), 2u);
  ShipConnectionMock* const configured   = ShipConnectionMockCreate();
  ShipConnectionMock* const other_paired = ShipConnectionMockCreate();
  Peer(0)->connection                    = SHIP_CONNECTION_OBJECT(configured);
  Peer(1)->connection                    = SHIP_CONNECTION_OBJECT(other_paired);

  // Stopping the node is what closes these two connections, once each.
  EXPECT_CALL(*configured->gmock, Stop(_));
  EXPECT_CALL(*configured->gmock, GetRemoteSki(_)).WillOnce(Return("remote-ski-a"));
  EXPECT_CALL(*configured->gmock, Destruct(_));
  EXPECT_CALL(*other_paired->gmock, Stop(_));
  EXPECT_CALL(*other_paired->gmock, GetRemoteSki(_)).WillOnce(Return("remote-ski-d"));
  EXPECT_CALL(*other_paired->gmock, Destruct(_));
  EXPECT_CALL(*reader_->gmock, OnRemoteSkiDisconnected(_, StrEq("remote-ski-a")));
  EXPECT_CALL(*reader_->gmock, OnRemoteSkiDisconnected(_, StrEq("remote-ski-d")));

  SHIP_NODE_START(ship_node_);
  ReplacePairingFingerprintAndWait(kFingerprintB);
  SHIP_NODE_STOP(ship_node_);

  ASSERT_EQ(PeerCount(), 2u);
  EXPECT_STREQ(Peer(0)->ship_id, kShipIdA);
  EXPECT_STREQ(Peer(1)->expected_fingerprint, kFingerprintD);
}

TEST_F(ShipNodeRevokePairingFingerprintTest, RemovesEveryPreviouslyPairedPeerAndKeepsTheOthers) {
  SHIP_NODE_REGISTER_REMOTE_FINGERPRINT(ship_node_, kFingerprintA);
  // The two revoked peers sit next to each other, so removing the first one
  // moves the second one to the index that was just checked.
  AddPeerTrustedByPairing("remote-ski-1", kFingerprintA);
  AddPeerTrustedByPairing("remote-ski-2", kFingerprintA);
  AddPeerTrustedByPairing("remote-ski-3", kFingerprintD);
  ASSERT_EQ(PeerCount(), 3u);
  ShipConnectionMock* const first  = ShipConnectionMockCreate();
  ShipConnectionMock* const second = ShipConnectionMockCreate();
  ShipConnectionMock* const other  = ShipConnectionMockCreate();
  Peer(0)->connection              = SHIP_CONNECTION_OBJECT(first);
  Peer(1)->connection              = SHIP_CONNECTION_OBJECT(second);
  Peer(2)->connection              = SHIP_CONNECTION_OBJECT(other);

  EXPECT_CALL(*first->gmock, Stop(_));
  EXPECT_CALL(*first->gmock, GetRemoteSki(_)).WillOnce(Return("remote-ski-1"));
  EXPECT_CALL(*first->gmock, Destruct(_));
  EXPECT_CALL(*second->gmock, Stop(_));
  EXPECT_CALL(*second->gmock, GetRemoteSki(_)).WillOnce(Return("remote-ski-2"));
  EXPECT_CALL(*second->gmock, Destruct(_));
  EXPECT_CALL(*other->gmock, Stop(_));
  EXPECT_CALL(*other->gmock, GetRemoteSki(_)).WillOnce(Return("remote-ski-3"));
  EXPECT_CALL(*other->gmock, Destruct(_));
  EXPECT_CALL(*reader_->gmock, OnRemoteSkiDisconnected(_, StrEq("remote-ski-1")));
  EXPECT_CALL(*reader_->gmock, OnRemoteSkiDisconnected(_, StrEq("remote-ski-2")));
  EXPECT_CALL(*reader_->gmock, OnRemoteSkiDisconnected(_, StrEq("remote-ski-3")));

  SHIP_NODE_START(ship_node_);
  ReplacePairingFingerprintAndWait(kFingerprintB);
  SHIP_NODE_STOP(ship_node_);

  ASSERT_EQ(PeerCount(), 1u);
  EXPECT_STREQ(Peer(0)->expected_fingerprint, kFingerprintD);
}
