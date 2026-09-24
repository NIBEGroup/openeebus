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
 * @brief A part of transformed api/hub.go
 *
 * Ship Node Interface declarations
 *
 * Interface for handling the server and remote connections
 */

#ifndef SRC_SHIP_API_SHIP_NODE_INTERFACE_H_
#define SRC_SHIP_API_SHIP_NODE_INTERFACE_H_

#include <stdbool.h>
#include <stdint.h>

#include "src/common/vector.h"
#include "src/service/api/service_reader_interface.h"
#include "src/ship/api/info_provider_interface.h"
#include "src/ship/api/ship_pairing_interface.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @brief Ship Node Interface
 * (Ship Node "virtual functions table" declaration)
 *
 * Transformed from HubInterface
 */
typedef struct ShipNodeInterface ShipNodeInterface;

/**
 * @brief Ship Node type definition
 * ("abstract class", has no members but only pointer to
 * "virtual functions table")
 */
typedef struct ShipNodeObject ShipNodeObject;

/**
 * @brief Ship Node Interface structure
 */
struct ShipNodeInterface {
  /**
   * @brief "Extends" the InfoProviderInterface
   */
  InfoProviderInterface info_provider_interface;
  /**
   * @brief Transformed from Start()
   */
  void (*start)(ShipNodeObject* self);
  /**
   * @brief Transformed from Shutdown()
   */
  void (*stop)(ShipNodeObject* self);
  /**
   * @brief Transformed from RegisterRemoteSKI()
   */
  void (*register_remote_ski)(ShipNodeObject* self, const char* ski, bool is_trusted);
  /**
   * @brief Transformed from UnregisterRemoteSKI()
   */
  void (*unregister_remote_ski)(ShipNodeObject* self, const char* ski);
  /**
   * @brief Transformed from CancelPairingWithSKI()
   */
  void (*cancel_pairing_with_ski)(ShipNodeObject* self, const char* ski);
  /**
   * @brief Trust a peer held in the "hello" PENDING phase, letting it proceed
   */
  void (*approve_pending_handshake_with_ski)(ShipNodeObject* self, const char* ski);
  /**
   * @brief The "waiting" value the peer last granted, in ms, or 0 if not pending
   */
  uint32_t (*get_pending_waiting_ms_with_ski)(ShipNodeObject* self, const char* ski);

  /**
   * @brief Sets the certificate fingerprint the trusted node is expected to present
   *
   * A node trusted from a shippairing request is identified by the fingerprint
   * of its certificate rather than by its SKI, which the request does not carry
   * (SHIP Pairing Service TS 1.0.0, section 10.2). Once set, a peer presenting
   * that certificate is admitted exactly as a peer presenting the registered
   * SKI is.
   *
   * Appended to the end of the table and may be NULL, so that a node
   * implemented outside this repository neither has to change nor stops
   * compiling.
   *
   * @param fingerprint Uppercase hexadecimal digits, or NULL to expect none
   */
  void (*register_remote_fingerprint)(ShipNodeObject* self, const char* fingerprint);

  /**
   * @brief The evaluator for the shippairing requests addressed to this node
   *
   * Exposed rather than wrapped, because everything an integrator has to do
   * with it, providing the secret and loading and saving the ring buffer, is
   * already on its own interface (SHIP Pairing Service TS 1.0.0, chapters 9
   * and 11). Until a secret is set on it, no request can be authenticated.
   *
   * Appended to the end of the table and may be NULL.
   *
   * @return The evaluator, owned by the node, or NULL if it has none
   */
  ShipPairingObject* (*get_ship_pairing)(ShipNodeObject* self);

  /**
   * @brief Announces a shippairing request from this node
   *
   * For a node asking to be trusted. Announcing again replaces the previous
   * announcement, which is withdrawn first (section 5.5).
   *
   * Appended to the end of the table and may be NULL.
   *
   * @param entry Request to announce, or NULL to withdraw one
   */
  EebusError (*announce_ship_pairing_request)(ShipNodeObject* self, const ShipPairingEntry* entry);
};

/**
 * @brief Ship Node structure
 */
struct ShipNodeObject {
  const ShipNodeInterface* interface_;
};

/**
 * @brief Ship Node pointer typecast
 */
#define SHIP_NODE_OBJECT(obj) ((ShipNodeObject*)(obj))

/**
 * @brief Ship Node Interface class pointer typecast
 */
#define SHIP_NODE_INTERFACE(obj) (SHIP_NODE_OBJECT(obj)->interface_)

/**
 * @brief Ship Node Start caller definition
 */
#define SHIP_NODE_START(obj) (SHIP_NODE_INTERFACE(obj)->start(obj))

/**
 * @brief Ship Node Stop caller definition
 */
#define SHIP_NODE_STOP(obj) (SHIP_NODE_INTERFACE(obj)->stop(obj))

/**
 * @brief Ship Node Register Remote SKI caller definition
 */
#define SHIP_NODE_REGISTER_REMOTE_SKI(obj, ski, is_trusted) \
  (SHIP_NODE_INTERFACE(obj)->register_remote_ski(obj, ski, is_trusted))

/**
 * @brief Ship Node Unregister Remote Ski caller definition
 */
#define SHIP_NODE_UNREGISTER_REMOTE_SKI(obj, ski) (SHIP_NODE_INTERFACE(obj)->unregister_remote_ski(obj, ski))

/**
 * @brief Ship Node Cancel Ppairing With SKI caller definition
 */
#define SHIP_NODE_CANCEL_PAIRING_WITH_SKI(obj, ski) (SHIP_NODE_INTERFACE(obj)->cancel_pairing_with_ski(obj, ski))

/**
 * @brief Ship Node Approve Pending Handshake With SKI caller definition
 */
#define SHIP_NODE_APPROVE_PENDING_HANDSHAKE_WITH_SKI(obj, ski) \
  (SHIP_NODE_INTERFACE(obj)->approve_pending_handshake_with_ski(obj, ski))

/**
 * @brief Ship Node Get Pending Waiting Ms With SKI caller definition
 */
#define SHIP_NODE_GET_PENDING_WAITING_MS_WITH_SKI(obj, ski) \
  (SHIP_NODE_INTERFACE(obj)->get_pending_waiting_ms_with_ski(obj, ski))

/**
 * @brief Ship Node Register Remote Fingerprint caller definition
 *
 * register_remote_fingerprint may be NULL (see the interface declaration above), so this checks for it before
 * calling.
 */
#define SHIP_NODE_REGISTER_REMOTE_FINGERPRINT(obj, fingerprint)                  \
  ((SHIP_NODE_INTERFACE(obj)->register_remote_fingerprint != NULL)               \
       ? SHIP_NODE_INTERFACE(obj)->register_remote_fingerprint(obj, fingerprint) \
       : (void)0)

/**
 * @brief Ship Node Get Ship Pairing caller definition
 *
 * get_ship_pairing may be NULL (see the interface declaration above), so this checks for it before calling.
 */
#define SHIP_NODE_GET_SHIP_PAIRING(obj) \
  ((SHIP_NODE_INTERFACE(obj)->get_ship_pairing != NULL) ? SHIP_NODE_INTERFACE(obj)->get_ship_pairing(obj) : NULL)

/**
 * @brief Ship Node Announce Ship Pairing Request caller definition
 *
 * announce_ship_pairing_request may be NULL (see the interface declaration above), so this checks for it before
 * calling.
 */
#define SHIP_NODE_ANNOUNCE_SHIP_PAIRING_REQUEST(obj, entry)                  \
  ((SHIP_NODE_INTERFACE(obj)->announce_ship_pairing_request != NULL)         \
       ? SHIP_NODE_INTERFACE(obj)->announce_ship_pairing_request(obj, entry) \
       : kEebusErrorNotSupported)

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SHIP_API_SHIP_NODE_INTERFACE_H_
