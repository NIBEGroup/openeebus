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
 * @brief A part of transformed api/shipconnection.go
 */

#ifndef SRC_SHIP_API_SHIP_CONNECTION_INTERFACE_H_
#define SRC_SHIP_API_SHIP_CONNECTION_INTERFACE_H_

#include <stdbool.h>
#include <stdint.h>

#include "src/common/eebus_errors.h"
#include "src/ship/api/data_writer_interface.h"
#include "src/ship/api/websocket_creator_interface.h"
#include "src/ship/api/websocket_interface.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @brief Ship Connection Interface
 * (Ship Connection "virtual functions table" declaration)
 *
 * Transformed from ShipConnectionInterface
 */
typedef struct ShipConnectionInterface ShipConnectionInterface;

/**
 * @brief Ship Connection Object type definition
 * ("abstract class", has no members but only pointer to
 * "virtual functions table")
 */
typedef struct ShipConnectionObject ShipConnectionObject;

/**
 * @brief Ship Connection Interface structure
 */
struct ShipConnectionInterface {
    /**
     * @brief "Extends" the DataWriterInterface
     */
    DataWriterInterface data_writer_interface;
    /**
     * @brief Transformed from Run()
     * @return kEebusErrorOk if started ok, error code otherwise
     */
    EebusError (*start)(ShipConnectionObject* self, WebsocketCreatorObject* websocket_creator);
    /**
     * @brief Executes close_connection(false, 0, "")
     */
    void (*stop)(ShipConnectionObject* self);
    /**
     * @brief Transformed from DataHandler()
     */
    WebsocketObject* (*get_websocket_connection)(ShipConnectionObject* self);
    /**
     * @brief Transformed from CloseConnection()
     */
    void (*close_connection)(ShipConnectionObject* self, bool safe, int32_t code, const char* reason);
    /**
     * @brief Transformed from RemoteSKI()
     */
    const char* (*get_remote_ski)(ShipConnectionObject* self);
    /**
     * @brief Transformed from ApprovePendingHandshake()
     */
    void (*approve_pending_handshake)(ShipConnectionObject* self);
    /**
     * @brief Transformed from AbortPendingHandshake()
     */
    void (*abort_pending_handshake)(ShipConnectionObject* self);
    /**
     * @brief Transformed from ShipHandshakeState()
     */
    SmeState (*get_state)(ShipConnectionObject* self, EebusError* err);
};

/**
 * @brief Ship Connection Object structure
 */
struct ShipConnectionObject {
    const ShipConnectionInterface* interface_;
};

/**
 * @brief Ship Connection Object pointer typecast
 */
#define SHIP_CONNECTION_OBJECT(obj) ((ShipConnectionObject*)(obj))

/**
 * @brief Ship Connection Interface class pointer typecast
 */
#define SHIP_CONNECTION_INTERFACE(obj) (SHIP_CONNECTION_OBJECT(obj)->interface_)

/**
 * @brief Ship Connection destructor caller definition
 */
#define SHIP_CONNECTION_DESTRUCT(obj) (DATA_WRITER_INTERFACE(obj)->destruct(DATA_WRITER_OBJECT(obj)))

/**
 * @brief Ship Connection Start caller definition
 * @return kEebusErrorOk if started ok, error code otherwise
 */
#define SHIP_CONNECTION_START(obj, wsc) (SHIP_CONNECTION_INTERFACE(obj)->start(SHIP_CONNECTION_OBJECT(obj), wsc))

/**
 * @brief Ship Connection Stop caller definition
 */
#define SHIP_CONNECTION_STOP(obj) (SHIP_CONNECTION_INTERFACE(obj)->stop(SHIP_CONNECTION_OBJECT(obj)))

/**
 * @brief Ship Connection Get Websocket Connection caller definition
 */
#define SHIP_CONNECTION_GET_WEBSOCKET_CONNECTION(obj) \
    (SHIP_CONNECTION_INTERFACE(obj)->get_websocket_connection(SHIP_CONNECTION_OBJECT(obj)))

/**
 * @brief Ship Connection Close connection caller definition
 */
#define SHIP_CONNECTION_CLOSE_CONNECTION(obj, safe, code, reason) \
    (SHIP_CONNECTION_INTERFACE(obj)->close_connection(SHIP_CONNECTION_OBJECT(obj), (safe), (code), (reason)))

/**
 * @brief Ship Connection Remote SKI caller definition
 */
#define SHIP_CONNECTION_GET_REMOTE_SKI(obj) \
    (SHIP_CONNECTION_INTERFACE(obj)->get_remote_ski(SHIP_CONNECTION_OBJECT(obj)))

/**
 * @brief Ship Connection Approve Pending Handshake caller definition
 */
#define SHIP_CONNECTION_APPROVE_PENDING_HANDSHAKE(obj) \
    (SHIP_CONNECTION_INTERFACE(obj)->approve_pending_handshake(SHIP_CONNECTION_OBJECT(obj)))

/**
 * @brief Ship Connection Abort Pending Handshake caller definition
 */
#define SHIP_CONNECTION_ABORT_PENDING_HANDSHAKE(obj) \
    (SHIP_CONNECTION_INTERFACE(obj)->abort_pending_handshake(SHIP_CONNECTION_OBJECT(obj)))

/**
 * @brief Ship Connection Get State caller definition
 */
#define SHIP_CONNECTION_GET_SHIP_STATE(obj, err) \
    (SHIP_CONNECTION_INTERFACE(obj)->get_state(SHIP_CONNECTION_OBJECT(obj), err))

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SHIP_API_SHIP_CONNECTION_INTERFACE_H_
