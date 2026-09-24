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
 * @brief shippairing evaluator interface declarations
 *
 * Evaluates the shippairing requests addressed to this node and decides which
 * of them establish trust (SHIP Pairing Service TS 1.0.0, chapters 9 and 11).
 */

#ifndef SRC_SHIP_API_SHIP_PAIRING_INTERFACE_H_
#define SRC_SHIP_API_SHIP_PAIRING_INTERFACE_H_

#include <stdbool.h>
#include <stdint.h>

#include "src/ship/api/ship_pairing_entry.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @brief Entries the ring buffer holds
 *
 * Section 11.2 requires at least ten. More would only make a replay that is
 * already implausible marginally harder, so ten it is.
 */
#define SHIP_PAIRING_RING_BUFFER_SIZE 10

/** @brief Longest algorithm identifier the ring buffer stores, with terminator */
#define SHIP_PAIRING_ALG_BUF_SIZE 16

/** @brief A digest as 64 hexadecimal digits, with terminator */
#define SHIP_PAIRING_DIGEST_BUF_SIZE ((SHIP_PAIRING_DIGEST_SIZE * 2) + 1)

/**
 * @brief Curves this node can use for a SHIP connection
 *
 * Section 5.4 makes a request naming a curve the receiver does not support
 * invalid, so which curves those are has to be known.
 */
enum ShipPairingCurve {
  kShipPairingCurveSecp256r1       = 1u << 0,
  kShipPairingCurveBrainpoolP256r1 = 1u << 1,
  kShipPairingCurveBrainpoolP384r1 = 1u << 2,
};

typedef enum ShipPairingCurve ShipPairingCurve;

/**
 * @brief Outcome of evaluating one shippairing request
 *
 * Only kShipPairingResultAccepted establishes trust. The rest are reported
 * separately from each other because they are worth telling apart when
 * diagnosing an installation, not because the caller must act on them: section
 * 4.2 requires every one of them to be treated the same way, by ignoring the
 * announcement.
 */
enum ShipPairingResult {
  /** @brief Valid, new, and authenticated with this node's secret */
  kShipPairingResultAccepted,
  /** @brief The TXT record does not conform to section 5.4 */
  kShipPairingResultInvalidRecord,
  /** @brief Addressed to a different node, or naming a different certificate */
  kShipPairingResultNotForThisNode,
  /** @brief Names a curve or a parameter type this node does not support */
  kShipPairingResultUnsupported,
  /** @brief An addCu-request while the processing of them is deactivated */
  kShipPairingResultNotProcessing,
  /** @brief Its digest is already in the ring buffer, so it is not a new request */
  kShipPairingResultAlreadySeen,
  /** @brief Not authenticated with this node's secret */
  kShipPairingResultDigestMismatch,
  /** @brief The request could not be evaluated */
  kShipPairingResultError,
};

typedef enum ShipPairingResult ShipPairingResult;

/**
 * @brief Reports whether the evaluator can currently accept anything
 *
 * An evaluator that has no secret, or that has stopped processing
 * addCu-requests because it is already paired, rejects every request it is
 * given. Telling the node when that changes lets it stop looking for requests
 * altogether rather than discovering and resolving announcements only to throw
 * them away.
 *
 * @param enabled Whether a request could now be accepted
 * @param ctx Context given when the callback was registered
 */
typedef void (*OnShipPairingEnabledCallback)(bool enabled, void* ctx);

/**
 * @brief One entry of the ring buffer
 *
 * An entry is unset when its digest is empty. Only the algorithm and the digest
 * are stored: section 10.4 is explicit that the ring buffer is not a trust
 * store and exists solely to recognise a request that has been seen before.
 */
typedef struct {
  char alg[SHIP_PAIRING_ALG_BUF_SIZE];
  char digest[SHIP_PAIRING_DIGEST_BUF_SIZE];
} ShipPairingRingEntry;

/**
 * @brief The ring buffer and its index, as they are persisted
 *
 * Section 11.2 requires both to be stored persistently. Nothing in this library
 * writes to a file system, so the integrator loads this at start up and saves
 * it whenever it changes, the same way it owns the SKI and SHIP ID of a
 * ServiceDetails. It holds no secret: the secret that verified a digest is not
 * part of it.
 *
 * The layout is plain characters and one octet, so it can be written to storage
 * and read back on any target without conversion.
 */
typedef struct {
  ShipPairingRingEntry entries[SHIP_PAIRING_RING_BUFFER_SIZE];
  /**
   * @brief Index the next accepted request is stored at, counting from 1
   *
   * Numbered as section 11.2 numbers it, so that the code can be read against
   * the specification. Zero means the buffer has never been written.
   */
  uint8_t next;
} ShipPairingRingBuffer;

/**
 * @brief shippairing Interface
 * (shippairing "virtual functions table" declaration)
 */
typedef struct ShipPairingInterface ShipPairingInterface;

/**
 * @brief shippairing Object type definition
 * ("abstract class", has no members but only pointer to
 * "virtual functions table")
 */
typedef struct ShipPairingObject ShipPairingObject;

/**
 * @brief ShipPairing Interface Structure
 */
struct ShipPairingInterface {
  void (*destruct)(ShipPairingObject* self);

  /**
   * @brief Evaluates one shippairing request, chapter 9
   *
   * On kShipPairingResultAccepted the caller trusts the node the request names,
   * with the fingerprint and curve it carries, and the processing of
   * addCu-requests has already been deactivated (section 4.2, step 3).
   *
   * Section 9 forbids evaluating two requests at once. Requests arrive from the
   * one thread that browses for them, so this is not made reentrant.
   */
  ShipPairingResult (*evaluate)(ShipPairingObject* self, const ShipPairingEntry* entry);

  /**
   * @brief Reports whether addCu-requests are being processed
   */
  bool (*is_add_cu_activated)(const ShipPairingObject* self);

  /**
   * @brief Begins processing addCu-requests again
   *
   * For the manual procedures of section 4.3.2: the node was reset to factory
   * defaults, the user deleted the pairing, or the user asked for the
   * processing to be activated again.
   */
  void (*activate_add_cu)(ShipPairingObject* self);

  /**
   * @brief Stops processing addCu-requests
   */
  void (*deactivate_add_cu)(ShipPairingObject* self);

  /**
   * @brief Reports that SHIP Message Exchange with the trusted node happened
   *
   * Restarts the fifteen minutes of section 4.3.1. If the processing of
   * addCu-requests had already been activated because those fifteen minutes
   * elapsed, and the trusted node then becomes reachable again, this
   * deactivates it once more and the existing pairing stands (section 4.3.1,
   * item b.ii).
   */
  void (*notify_message_exchange)(ShipPairingObject* self);

  /**
   * @brief Reports whether the ring buffer holds a current entry, section 11.4
   *
   * That entry means a request has been accepted and not been superseded. It is
   * not the trust store: whether the node named by that request is still
   * trusted is answered by the trust store (section 10.4), which a user can
   * empty without the ring buffer changing.
   */
  bool (*has_trusted_peer)(const ShipPairingObject* self);

  /**
   * @brief Copies out the ring buffer, for the integrator to persist
   */
  void (*get_ring_buffer)(const ShipPairingObject* self, ShipPairingRingBuffer* ring_buffer);

  /**
   * @brief Restores a persisted ring buffer
   *
   * Restoring one that holds a current entry means a request has been accepted
   * before, so the processing of addCu-requests is deactivated and the fifteen
   * minutes of section 4.3.1 start counting from now: the node has just started
   * and has not exchanged anything with the trusted node yet.
   */
  void (*set_ring_buffer)(ShipPairingObject* self, const ShipPairingRingBuffer* ring_buffer);

  /**
   * @brief Sets the secret of this node, section 6.2
   *
   * Provided separately from construction because it usually comes from storage
   * that is not readable yet when the service is built. Requests cannot be
   * authenticated until it is set.
   */
  void (*set_secret)(ShipPairingObject* self, const uint8_t* secret, size_t secret_size);

  /**
   * @brief Sets which curves this node can use for a SHIP connection
   *
   * @param curves Bitwise OR of ShipPairingCurve values
   */
  void (*set_supported_curves)(ShipPairingObject* self, uint32_t curves);

  /**
   * @brief Registers who to tell when this evaluator becomes able to accept
   *
   * Called whenever the answer changes, and not on registration: the node that
   * registers it knows the state it starts in.
   */
  void (*set_enabled_callback)(ShipPairingObject* self, OnShipPairingEnabledCallback cb, void* ctx);

  /**
   * @brief Reports whether a request could currently be accepted
   *
   * True when a secret is set and addCu-requests are being processed. False
   * means every request would be rejected, whatever it contained.
   */
  bool (*is_enabled)(const ShipPairingObject* self);
};

/**
 * @brief shippairing Object Structure
 */
struct ShipPairingObject {
  const ShipPairingInterface* interface_;
};

/**
 * @brief shippairing pointer typecast
 */
#define SHIP_PAIRING_OBJECT(obj) ((ShipPairingObject*)(obj))

/**
 * @brief shippairing Interface class pointer typecast
 */
#define SHIP_PAIRING_INTERFACE(obj) (SHIP_PAIRING_OBJECT(obj)->interface_)

/**
 * @brief shippairing Destruct caller definition
 */
#define SHIP_PAIRING_DESTRUCT(obj) (SHIP_PAIRING_INTERFACE(obj)->destruct(obj))

/**
 * @brief shippairing Evaluate caller definition
 */
#define SHIP_PAIRING_EVALUATE(obj, entry) (SHIP_PAIRING_INTERFACE(obj)->evaluate(obj, entry))

/**
 * @brief shippairing Is Add Cu Activated caller definition
 */
#define SHIP_PAIRING_IS_ADD_CU_ACTIVATED(obj) (SHIP_PAIRING_INTERFACE(obj)->is_add_cu_activated(obj))

/**
 * @brief shippairing Activate Add Cu caller definition
 */
#define SHIP_PAIRING_ACTIVATE_ADD_CU(obj) (SHIP_PAIRING_INTERFACE(obj)->activate_add_cu(obj))

/**
 * @brief shippairing Deactivate Add Cu caller definition
 */
#define SHIP_PAIRING_DEACTIVATE_ADD_CU(obj) (SHIP_PAIRING_INTERFACE(obj)->deactivate_add_cu(obj))

/**
 * @brief shippairing Notify Message Exchange caller definition
 */
#define SHIP_PAIRING_NOTIFY_MESSAGE_EXCHANGE(obj) (SHIP_PAIRING_INTERFACE(obj)->notify_message_exchange(obj))

/**
 * @brief shippairing Has Trusted Peer caller definition
 */
#define SHIP_PAIRING_HAS_TRUSTED_PEER(obj) (SHIP_PAIRING_INTERFACE(obj)->has_trusted_peer(obj))

/**
 * @brief shippairing Get Ring Buffer caller definition
 */
#define SHIP_PAIRING_GET_RING_BUFFER(obj, ring_buffer) (SHIP_PAIRING_INTERFACE(obj)->get_ring_buffer(obj, ring_buffer))

/**
 * @brief shippairing Set Ring Buffer caller definition
 */
#define SHIP_PAIRING_SET_RING_BUFFER(obj, ring_buffer) (SHIP_PAIRING_INTERFACE(obj)->set_ring_buffer(obj, ring_buffer))

/**
 * @brief shippairing Set Secret caller definition
 */
#define SHIP_PAIRING_SET_SECRET(obj, secret, secret_size) \
  (SHIP_PAIRING_INTERFACE(obj)->set_secret(obj, secret, secret_size))

/**
 * @brief shippairing Set Supported Curves caller definition
 */
#define SHIP_PAIRING_SET_SUPPORTED_CURVES(obj, curves) (SHIP_PAIRING_INTERFACE(obj)->set_supported_curves(obj, curves))

/**
 * @brief shippairing Set Enabled Callback caller definition
 */
#define SHIP_PAIRING_SET_ENABLED_CALLBACK(obj, cb, ctx) \
  (SHIP_PAIRING_INTERFACE(obj)->set_enabled_callback(obj, cb, ctx))

/**
 * @brief shippairing Is Enabled caller definition
 */
#define SHIP_PAIRING_IS_ENABLED(obj) (SHIP_PAIRING_INTERFACE(obj)->is_enabled(obj))

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SHIP_API_SHIP_PAIRING_INTERFACE_H_
