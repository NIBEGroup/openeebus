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
 *
 * Ship Connection Info Provider Info Provider Interface declarations
 *
 * Interface for getting service wide information
 *
 * Implemented by ShipNode, used by ShipConnection
 */

#ifndef SRC_SHIP_API_INFO_PROVIDER_INTERFACE_H_
#define SRC_SHIP_API_INFO_PROVIDER_INTERFACE_H_

#include "src/ship/api/data_reader_interface.h"
#include "src/ship/api/data_writer_interface.h"
#include "src/ship/api/ship_connection_interface.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @brief Info Provider Interface
 * (Info Provider "virtual functions table" declaration)
 *
 * Transformed from ShipConnectionInfoProviderInterface
 */
typedef struct InfoProviderInterface InfoProviderInterface;

/**
 * @brief Info Provider Object type definition
 * ("abstract class", has no members but only pointer to
 * "virtual functions table")
 */
typedef struct InfoProviderObject InfoProviderObject;

/**
 * @brief Info Provider Interface structure
 */
struct InfoProviderInterface {
    /**
     * @brief Info Provider Destructor
     */
    void (*destruct)(InfoProviderObject* self);
    /**
     * @brief Check if the SKI is paired
     */
    bool (*is_remote_service_for_ski_paired)(InfoProviderObject* self, const char* ski);
    /**
     * @brief Report closing of a connection and if handshake did complete
     */
    void (*handle_connection_closed)(InfoProviderObject* self, ShipConnectionObject* sc, bool had_error);
    /**
     * @brief Report the ship ID provided during the handshake
     */
    void (*report_service_ship_id)(InfoProviderObject* self, const char* service_id, const char* ship_id);
    /**
     * @brief Check if the user is still able to trust the connection
     */
    bool (*is_waiting_for_trust_allowed)(InfoProviderObject*, const char* ski);
    /**
     * @brief Report the updated SHIP handshake state and optional error message
     * for a SKI
     */
    void (*handle_ship_state_update)(InfoProviderObject* self, const char* ski, SmeState state, const char* err);
    /**
     * @brief Report an approved handshake by a remote device
     */
    DataReaderObject* (*setup_remote_device)(InfoProviderObject* self, const char* ski, DataWriterObject* data_writer);
};

/**
 * @brief Info Provider Object structure
 */
struct InfoProviderObject {
    const InfoProviderInterface* interface_;
};

/**
 * @brief Info Provider Object pointer typecast
 */
#define INFO_PROVIDER_OBJECT(obj) ((InfoProviderObject*)(obj))

/**
 * @brief Info Provider Interface class pointer typecast
 */
#define INFO_PROVIDER_INTERFACE(obj) (INFO_PROVIDER_OBJECT(obj)->interface_)

/**
 * @brief Info Provider Destructor caller definition
 */
#define INFO_PROVIDER_DESTRUCT(obj) (INFO_PROVIDER_INTERFACE(obj)->destruct(INFO_PROVIDER_OBJECT(obj)))

/**
 * @brief Info Provider Is Remote Service For SKI Paired caller definition
 */
#define INFO_PROVIDER_IS_REMOTE_SERVICE_FOR_SKI_PAIRED(obj, ski) \
    (INFO_PROVIDER_INTERFACE(obj)->is_remote_service_for_ski_paired(INFO_PROVIDER_OBJECT(obj), (ski)))

/**
 * @brief Info Provider Handle Connection Closed caller definition
 */
#define INFO_PROVIDER_HANDLE_CONNECTION_CLOSED(obj, sc, err) \
    (INFO_PROVIDER_INTERFACE(obj)->handle_connection_closed(INFO_PROVIDER_OBJECT(obj), (sc), (err)))

/**
 * @brief Info Provider Report Service Ship ID caller definition
 */
#define INFO_PROVIDER_REPORT_SERVICE_SHIP_ID(obj, s_id, ship_id) \
    (INFO_PROVIDER_INTERFACE(obj)->report_service_ship_id(INFO_PROVIDER_OBJECT(obj), (s_id), (ship_id)))

/**
 * @brief Info Provider Is Waiting For Trust Allowed caller definition
 */
#define INFO_PROVIDER_IS_WAITING_FOR_TRUST_ALLOWED(obj, ski) \
    (INFO_PROVIDER_INTERFACE(obj)->is_waiting_for_trust_allowed(INFO_PROVIDER_OBJECT(obj), (ski)))

/**
 * @brief Info Provider Handle Ship State Update caller definition
 */
#define INFO_PROVIDER_HANDLE_SHIP_STATE_UPDATE(obj, ski, state, err) \
    (INFO_PROVIDER_INTERFACE(obj)->handle_ship_state_update(INFO_PROVIDER_OBJECT(obj), (ski), (state), (err)))

/**
 * @brief Info Provider Setup Remote Device caller definition
 */
#define INFO_PROVIDER_SETUP_REMOTE_DEVICE(obj, ski, dw) \
    (INFO_PROVIDER_INTERFACE(obj)->setup_remote_device(INFO_PROVIDER_OBJECT(obj), (ski), (dw)))

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SHIP_API_INFO_PROVIDER_INTERFACE_H_
