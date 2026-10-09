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
 * @brief shippairing evaluator internal structure
 */

#ifndef SRC_SHIP_SHIP_PAIRING_SHIP_PAIRING_INTERNAL_H_
#define SRC_SHIP_SHIP_PAIRING_SHIP_PAIRING_INTERNAL_H_

#include "src/common/api/eebus_timer_interface.h"
#include "src/ship/ship_pairing/ship_pairing.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct ShipPairing ShipPairing;

struct ShipPairing {
  /** Implements the shippairing Interface */
  ShipPairingObject obj;

  /** SHIP ID of this node, the value a request's "forId" must hold */
  const char* own_ship_id;

  /** SHA-256 fingerprint of this node's certificate, the value of "forPar" */
  const char* own_fingerprint;

  /** Secret of this node, section 6.2 */
  uint8_t secret[SHIP_PAIRING_SECRET_SIZE];

  /** Octets of secret that are set, zero until a secret is provided */
  size_t secret_size;

  /** Curves this node can use, a bitwise OR of ShipPairingCurve values */
  uint32_t supported_curves;

  /** Whether addCu-requests are being processed, sections 4.2 and 4.3 */
  bool add_cu_activated;

  /** Digests of accepted requests, chapter 11 */
  ShipPairingRingBuffer ring_buffer;

  /** Counts the fifteen minutes of section 4.3.1 */
  EebusTimerObject* reactivation_timer;

  /** Told when this evaluator becomes able, or unable, to accept a request */
  OnShipPairingEnabledCallback on_enabled_cb;
  void* enabled_ctx;

  /** What was last reported through @ref on_enabled_cb */
  bool reported_enabled;
};

#define SHIP_PAIRING(obj) ((ShipPairing*)(obj))

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SHIP_SHIP_PAIRING_SHIP_PAIRING_INTERNAL_H_
