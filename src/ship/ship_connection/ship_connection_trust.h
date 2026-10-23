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
#ifndef SRC_SHIP_SHIP_CONNECTION_SHIP_CONNECTION_TRUST_H_
#define SRC_SHIP_SHIP_CONNECTION_SHIP_CONNECTION_TRUST_H_

#include "src/ship/api/ship_connection_interface.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Called on the connection thread in HELLO PENDING; false aborts the handshake. */
typedef bool (*ShipConnectionTrustAcceptanceCallback)(ShipConnectionObject* sc, uint64_t approval_id, void* ctx);

/**
 * @brief Internal approval transport between ShipNode and concrete ShipConnection
 *
 * Success means queued, not accepted. The callback commits trust before READY;
 * a zero token verifies trust already established by an accepted addCu request.
 * The callback context must remain valid until the connection has stopped.
 */
EebusError ShipConnectionApprovePendingHandshakeWithToken(
    ShipConnectionObject* self,
    uint64_t approval_id,
    ShipConnectionTrustAcceptanceCallback accept_cb,
    void* ctx
);

#ifdef __cplusplus
}
#endif

#endif  // SRC_SHIP_SHIP_CONNECTION_SHIP_CONNECTION_TRUST_H_
