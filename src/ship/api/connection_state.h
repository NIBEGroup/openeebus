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
 * @brief Transformed api/connectionstate.go
 *
 * Connection state for global usage, e.g. UI
 */

#ifndef SRC_SHIP_API_CONNECTION_STATE_H_
#define SRC_SHIP_API_CONNECTION_STATE_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef int8_t ConnectionState;

enum {
    kConnectionStateNone,                   /**< The initial state, when no pairing exists */
    kConnectionStateQueued,                 /** The connection request has been started and is
                                             pending connection initialization */
    kConnectionStateInitiated,              /** This service initiated the connection process
                                             */
    kConnectionStateReceivedPairingRequest, /** A remote service initiated the
                                             connection process */
    kConnectionStateInProgress,             /** The connection handshake is in progress */
    kConnectionStateTrusted,                /** The connection is trusted on both ends */
    kConnectionStatePin,                    /** PIN processing, not supported right now! */
    kConnectionStateCompleted,              /** The connection handshake is completed from
                                             both ends */
    kConnectionStateRemoteDeniedTrust,      /** The remote service denied trust */
    kConnectionStateError,                  /** The connection handshake resulted in an error */
};

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SHIP_API_CONNECTION_STATE_H_
