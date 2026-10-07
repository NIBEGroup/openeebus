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
#ifndef SRC_SHIP_SHIP_NODE_SHIP_NODE_INTERNAL_H_
#define SRC_SHIP_SHIP_NODE_SHIP_NODE_INTERNAL_H_

#include <stdbool.h>

#include "node_connection_container.h"
#include "src/common/api/eebus_mutex_interface.h"
#include "src/common/api/eebus_queue_interface.h"
#include "src/common/api/eebus_thread_interface.h"
#include "src/common/service_details.h"
#include "src/common/string_util.h"
#include "src/ship/api/http_server_interface.h"
#include "src/ship/api/ship_mdns_interface.h"
#include "src/ship/api/ship_node_interface.h"
#include "src/ship/api/ship_node_reader_interface.h"
#include "src/ship/api/tls_certificate_interface.h"
#include "src/ship/api/trust_mode.h"
#include "src/ship/ship_connection/types.h"
#include "src/ship/ship_pairing/ship_pairing.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct ShipNode ShipNode;

struct ShipNode {
  /** Implements the Ship Node Interface */
  ShipNodeObject sc_object;

  EebusQueueObject* msg_queue;
  ShipMdnsObject* mdns;
  Vector* mdns_entries;
  EebusMutexObject* mutex;
  bool search_for_remote_ski;
  bool cancel;
  EebusThreadObject* connection_thread;

  NodeConnectionContainerObject* connections;
  ShipNodeReaderObject* ship_node_reader;
  const TlsCertificateObject* tsl_certificate;
  ServiceDetails* local_service_details;
  HttpServerObject* http_server;
  /**
   * @brief Certificate fingerprint named by the current accepted pairing request
   *
   * The pairing service permits one current devZ. Each peer's trust and
   * presented certificate fingerprint are held in its NodeConnection.
   */
  char* remote_fingerprint;

  /** Evaluates the shippairing requests addressed to this node, chapter 9 */
  ShipPairingObject* ship_pairing;
  ShipRole role;
  /** When a foreign SKI is trusted, see EebusTrustMode */
  EebusTrustMode trust_mode;
};

#define SHIP_NODE(obj) ((ShipNode*)(obj))

/**
 * @brief Reports whether a connecting peer is the one this node trusts
 *
 * The whole of the decision, kept apart from the connection handling so that it
 * can be read and tested on its own. A peer is recognised by presenting the
 * trusted SKI, or by presenting the certificate whose fingerprint is trusted
 * (SHIP Pairing Service TS 1.0.0, section 10.2). The second is an additional
 * way to be recognised and never a way to bypass the first.
 *
 * @param peer_ski SKI derived from the certificate the peer presented
 * @param trusted_ski SKI this node trusts, or NULL if none
 * @param peer_fingerprint Fingerprint of the certificate the peer presented
 * @param trusted_fingerprint Fingerprint this node trusts, or NULL if none
 * @return true if the peer is recognised
 */
/**
 * @brief Reports whether a peer presented the certificate that is trusted
 *
 * Section 10.2: a fingerprint that matches is to be treated exactly as a
 * trusted SKI is. It is an additional way to be recognised, never a way to skip
 * being recognised: with nothing registered to compare against, or nothing
 * presented to compare, nobody is recognised here.
 *
 * Separate from ShipNodeIsPeerRecognised() because a peer recognised this way
 * is also trusted outright rather than held for a decision, so the connection
 * handling has to be able to tell which of the two recognised it.
 *
 * @param peer_fingerprint Fingerprint of the certificate the peer presented
 * @param trusted_fingerprint Fingerprint this node trusts, or NULL if none
 * @return true if the peer presented the trusted certificate
 */
static inline bool ShipNodeFingerprintMatches(const char* peer_fingerprint, const char* trusted_fingerprint) {
  if (StringIsEmpty(trusted_fingerprint) || StringIsEmpty(peer_fingerprint)) {
    return false;
  }

  return StringEqualsIgnoreCase(peer_fingerprint, trusted_fingerprint);
}

/**
 * @brief Reports whether a connection in hand should be promoted to trusted
 *
 * Section 4.2 has devZ establishing a SHIP connection before it announces its
 * request, and repeating the attempt for as long as devA does not trust it, so
 * a node being paired is usually already holding a connection that was admitted
 * provisionally and parked awaiting a decision. When the request is accepted,
 * that decision has been made, and the connection in hand should not have to be
 * abandoned and remade for it to take effect.
 *
 * The comparison is against the certificate the peer presented and nothing
 * else. The SKI it arrived with says only who dialled in first, and promoting on
 * that basis would admit them on the strength of a request naming somebody
 * else's certificate.
 *
 * @param has_connection Whether a connection is currently held
 * @param connected_peer_fingerprint Fingerprint that connection's peer presented
 * @param trusted_fingerprint Fingerprint just registered as trusted
 * @return true if the held connection belongs to the node that was authorised
 */
static inline bool ShipNodeShouldPromotePendingPeer(
    bool has_connection,
    const char* connected_peer_fingerprint,
    const char* trusted_fingerprint
) {
  if (!has_connection) {
    return false;
  }

  return ShipNodeFingerprintMatches(connected_peer_fingerprint, trusted_fingerprint);
}

static inline bool ShipNodeIsPeerRecognised(
    const char* peer_ski,
    const char* trusted_ski,
    const char* peer_fingerprint,
    const char* trusted_fingerprint
) {
  if (!StringIsEmpty(peer_ski) && !StringIsEmpty(trusted_ski) && StringEqualsIgnoreCase(peer_ski, trusted_ski)) {
    return true;
  }

  return ShipNodeFingerprintMatches(peer_fingerprint, trusted_fingerprint);
}

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SHIP_SHIP_NODE_SHIP_NODE_INTERNAL_H_
