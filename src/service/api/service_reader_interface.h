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
 * @brief Service Reader interface declarations
 */

#ifndef SRC_SERVICE_API_SERVICE_READER_INTERFACE_H_
#define SRC_SERVICE_API_SERVICE_READER_INTERFACE_H_

#include "src/common/vector.h"
#include "src/service/api/eebus_service_interface.h"
#include "src/ship/model/types.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @brief Service Reader Interface
 * (Service Reader "virtual functions table" declaration)
 */
typedef struct ServiceReaderInterface ServiceReaderInterface;

/**
 * @brief Service Reader Object type definition
 * ("abstract class", has no members but only pointer to
 * "virtual functions table")
 */
typedef struct ServiceReaderObject ServiceReaderObject;

/**
 * @brief ServiceReader Interface Structure
 */
struct ServiceReaderInterface {
    void (*destruct)(ServiceReaderObject* self);
    void (*on_remote_ski_connected)(ServiceReaderObject* self, EebusServiceObject* service, const char* ski);
    void (*on_remote_ski_disconnected)(ServiceReaderObject* self, EebusServiceObject* service, const char* ski);
    void (*on_remote_services_update)(ServiceReaderObject* self, EebusServiceObject* service, const Vector* entries);
    void (*on_ship_id_update)(ServiceReaderObject* self, const char* ski, const char* ship_id);
    void (*on_ship_state_update)(ServiceReaderObject* self, const char* ski, SmeState state);
    bool (*is_waiting_for_trust_allowed)(const ServiceReaderObject* self, const char* ski);
};

/**
 * @brief Service Reader Object Structure
 */
struct ServiceReaderObject {
    const ServiceReaderInterface* interface_;
};

/**
 * @brief Service Reader pointer typecast
 */
#define SERVICE_READER_OBJECT(obj) ((ServiceReaderObject*)(obj))

/**
 * @brief Service Reader Interface class pointer typecast
 */
#define SERVICE_READER_INTERFACE(obj) (SERVICE_READER_OBJECT(obj)->interface_)

/**
 * @brief Service Reader Destruct caller definition
 */
#define SERVICE_READER_DESTRUCT(obj) (SERVICE_READER_INTERFACE(obj)->destruct(obj))

/**
 * @brief Service Reader On Remote Ski Connected caller definition
 */
#define SERVICE_READER_ON_REMOTE_SKI_CONNECTED(obj, service, ski) \
    (SERVICE_READER_INTERFACE(obj)->on_remote_ski_connected(obj, service, ski))

/**
 * @brief Service Reader On Remote Ski Disconnected caller definition
 */
#define SERVICE_READER_ON_REMOTE_SKI_DISCONNECTED(obj, service, ski) \
    (SERVICE_READER_INTERFACE(obj)->on_remote_ski_disconnected(obj, service, ski))

/**
 * @brief Service Reader On Remote Services Update caller definition
 */
#define SERVICE_READER_ON_REMOTE_SERVICES_UPDATE(obj, service, entries) \
    (SERVICE_READER_INTERFACE(obj)->on_remote_services_update(obj, service, entries))

/**
 * @brief Service Reader On Ship Id Update caller definition
 */
#define SERVICE_READER_ON_SHIP_ID_UPDATE(obj, ski, shipd_id) \
    (SERVICE_READER_INTERFACE(obj)->on_ship_id_update(obj, ski, shipd_id))

/**
 * @brief Service Reader On Ship State Update caller definition
 */
#define SERVICE_READER_ON_SHIP_STATE_UPDATE(obj, ski, state) \
    (SERVICE_READER_INTERFACE(obj)->on_ship_state_update(obj, ski, state))

/**
 * @brief Service Reader Is Waiting For Trust Allowed caller definition
 */
#define SERVICE_READER_IS_WAITING_FOR_TRUST_ALLOWED(obj, ski) \
    (SERVICE_READER_INTERFACE(obj)->is_waiting_for_trust_allowed(obj, ski))

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SERVICE_API_SERVICE_READER_INTERFACE_H_
