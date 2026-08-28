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

#include "src/common/vector.h"
#include "src/service/api/service_reader_interface.h"
#include "src/ship/api/info_provider_interface.h"

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

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SHIP_API_SHIP_NODE_INTERFACE_H_
