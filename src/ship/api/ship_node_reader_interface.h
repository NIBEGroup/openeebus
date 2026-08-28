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
 * SHIP Node Reader Interface declarations
 *
 * Used to pass information from the Ship Node to the EEBUS service
 *
 * Implemented by EEBUS Service, used by Ship Node
 */

#ifndef SRC_SHIP_API_SHIP_NODE_READER_INTERFACE_H_
#define SRC_SHIP_API_SHIP_NODE_READER_INTERFACE_H_

#include <stddef.h>

#include "src/common/vector.h"
#include "src/ship/api/connection_state.h"
#include "src/ship/api/data_reader_interface.h"
#include "src/ship/api/data_writer_interface.h"
#include "src/ship/api/remote_service.h"
#include "src/ship/model/types.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @brief Ship Node Reader Interface
 * (Ship Node Reader "virtual functions table" declaration)
 *
 * Transformed from HubReaderInterface
 */
typedef struct ShipNodeReaderInterface ShipNodeReaderInterface;

/**
 * @brief Ship Node Reader Object type definition
 * ("abstract class", has no members but only pointer to
 * "virtual functions table")
 */
typedef struct ShipNodeReaderObject ShipNodeReaderObject;

/**
 * @brief ShipNodeReader Interface Structure
 */
struct ShipNodeReaderInterface {
    /**
     * @brief Ship Node Reader Destructor
     */
    void (*destruct)(ShipNodeReaderObject* self);
    /**
     * @brief Report a connection to a SKI
     *
     * Transformed from RemoteSKIConnected()
     */
    void (*on_remote_ski_connected)(ShipNodeReaderObject* self, const char* ski);
    /**
     * @brief Report a disconnection to a SKI
     *
     * Transformed from RemoteSKIDisconnected()
     */
    void (*on_remote_ski_disconnected)(ShipNodeReaderObject* self, const char* ski);
    // TODO: Find better approach to pair remote device instance with ship
    // connection
    /**
     * @brief Report an approved handshake by a remote device
     */
    DataReaderObject* (*setup_remote_device)(
        ShipNodeReaderObject* self,
        const char* ski,
        DataWriterObject* data_writer
    );
    /**
     * @brief Report all currently visible EEBUS services
     *
     * Transformed from VisibleRemoteServicesUpdated()
     */
    void (*on_remote_services_update)(ShipNodeReaderObject* self, const Vector* entries);
    /**
     * @brief Provide the SHIP ID received during SHIP handshake process
     * the ID needs to be stored and then provided for remote services so it can
     * be compared and verified
     *
     * Trnasformed from ServiceShipIDUpdate()
     */
    void (*on_ship_id_update)(ShipNodeReaderObject* self, const char* ski, const char* ship_id);
    /**
     * @brief Provides the current handshake state for a given SKI
     *
     * Transformed from ServicePairingDetailUpdate()
     */
    void (*on_ship_state_update)(ShipNodeReaderObject* self, const char* ski, SmeState state);

    // TODO: Find better approach to check the allow waiting flag
    /**
     * @brief Return true if the user is still able to trust the connection
     *
     * Transformed from AllowWaitingForTrust()
     */
    bool (*is_waiting_for_trust_allowed)(ShipNodeReaderObject* self, const char* ski);
};

/**
 * @brief Ship Node Reader Object Structure
 */
struct ShipNodeReaderObject {
    const ShipNodeReaderInterface* interface_;
};

/**
 * @brief Ship Node Reader pointer typecast
 */
#define SHIP_NODE_READER_OBJECT(obj) ((ShipNodeReaderObject*)(obj))

/**
 * @brief Ship Node Reader Interface class pointer typecast
 */
#define SHIP_NODE_READER_INTERFACE(obj) (SHIP_NODE_READER_OBJECT(obj)->interface_)

/**
 * @brief Ship Node Reader Destruct caller definition
 */
#define SHIP_NODE_READER_DESTRUCT(obj) (SHIP_NODE_READER_INTERFACE(obj)->destruct(obj))

/**
 * @brief Ship Node Reader On Remote Ski Connected caller definition
 */
#define SHIP_NODE_READER_ON_REMOTE_SKI_CONNECTED(obj, ski) \
    (SHIP_NODE_READER_INTERFACE(obj)->on_remote_ski_connected(obj, ski))

/**
 * @brief Ship Node Reader On Remote Ski Disconnected caller definition
 */
#define SHIP_NODE_READER_ON_REMOTE_SKI_DISCONNECTED(obj, ski) \
    (SHIP_NODE_READER_INTERFACE(obj)->on_remote_ski_disconnected(obj, ski))

/**
 * @brief Ship Node Reader Setup Remote Device caller definition
 */
#define SHIP_NODE_READER_SETUP_REMOTE_DEVICE(obj, ski, data_writer) \
    (SHIP_NODE_READER_INTERFACE(obj)->setup_remote_device(obj, ski, data_writer))

/**
 * @brief Ship Node Reader On Remote Services Update caller definition
 */
#define SHIP_NODE_READER_ON_REMOTE_SERVICES_UPDATE(obj, entries) \
    (SHIP_NODE_READER_INTERFACE(obj)->on_remote_services_update(obj, entries))

/**
 * @brief Ship Node Reader On Ship Id Update caller definition
 */
#define SHIP_NODE_READER_ON_SHIP_ID_UPDATE(obj, ski, ship_id) \
    (SHIP_NODE_READER_INTERFACE(obj)->on_ship_id_update(obj, ski, ship_id))

/**
 * @brief Ship Node Reader On Ship State Update caller definition
 */
#define SHIP_NODE_READER_ON_SHIP_STATE_UPDATE(obj, ski, state) \
    (SHIP_NODE_READER_INTERFACE(obj)->on_ship_state_update(obj, ski, state))

/**
 * @brief Ship Node Reader Is Waiting For Trust Allowed caller definition
 */
#define SHIP_NODE_READER_IS_WAITING_FOR_TRUST_ALLOWED(obj, ski) \
    (SHIP_NODE_READER_INTERFACE(obj)->is_waiting_for_trust_allowed(obj, ski))

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SHIP_API_SHIP_NODE_READER_INTERFACE_H_
