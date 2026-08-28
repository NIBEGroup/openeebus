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
 * @brief Service interface declarations
 */

#ifndef SRC_SERVICE_API_EEBUS_SERVICE_INTERFACE_H_
#define SRC_SERVICE_API_EEBUS_SERVICE_INTERFACE_H_

#include "src/common/service_details.h"
#include "src/service/api/eebus_service_config.h"
#include "src/ship/api/ship_node_reader_interface.h"
#include "src/spine/api/device_local_interface.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @brief Service Interface
 * (Service "virtual functions table" declaration)
 */
typedef struct EebusServiceInterface EebusServiceInterface;

/**
 * @brief Service Object type definition
 * ("abstract class", has no members but only pointer to
 * "virtual functions table")
 */
typedef struct EebusServiceObject EebusServiceObject;

/**
 * @brief Service Interface Structure
 */
struct EebusServiceInterface {
    /** Extends DeviceInterface */
    ShipNodeReaderInterface ship_node_reader_interface;

    void (*start)(EebusServiceObject* self);
    void (*stop)(EebusServiceObject* self);
    const ServiceDetails* (*get_local_service)(const EebusServiceObject* self);
    DeviceLocalObject* (*get_local_device)(const EebusServiceObject* self);
    const ConnectionState* (*get_connection_state_with_ski)(const EebusServiceObject* self, const char* ski);
    const ServiceDetails* (*get_remote_service_details_with_ski)(const EebusServiceObject* self, const char* ski);
    void (*register_remote_ski)(EebusServiceObject* self, const char* ski, bool enable);
    void (*unregister_remote_ski)(EebusServiceObject* self, const char* ski);
    void (*cancel_pairing_with_ski)(EebusServiceObject* self, const char* ski);
    void (*set_pairing_possible)(EebusServiceObject* self, bool is_pairing_possible);
    const char* (*get_local_ski)(EebusServiceObject* self);
    const char* (*get_qr_code_string)(EebusServiceObject* self);
};

/**
 * @brief EEBUS Service Object Structure
 */
struct EebusServiceObject {
    const EebusServiceInterface* interface_;
};

/**
 * @brief EEBUS Service pointer typecast
 */
#define EEBUS_SERVICE_OBJECT(obj) ((EebusServiceObject*)(obj))

/**
 * @brief EEBUS Service Interface class pointer typecast
 */
#define EEBUS_SERVICE_INTERFACE(obj) (EEBUS_SERVICE_OBJECT(obj)->interface_)

/**
 * @brief EEBUS Service Start caller definition
 */
#define EEBUS_SERVICE_START(obj) (EEBUS_SERVICE_INTERFACE(obj)->start(obj))

/**
 * @brief EEBUS Service Stop caller definition
 */
#define EEBUS_SERVICE_STOP(obj) (EEBUS_SERVICE_INTERFACE(obj)->stop(obj))

/**
 * @brief EEBUS Service Get Local Service caller definition
 */
#define EEBUS_SERVICE_GET_LOCAL_SERVICE(obj) (EEBUS_SERVICE_INTERFACE(obj)->get_local_service(obj))

/**
 * @brief EEBUS Service Get Local Device caller definition
 */
#define EEBUS_SERVICE_GET_LOCAL_DEVICE(obj) (EEBUS_SERVICE_INTERFACE(obj)->get_local_device(obj))

/**
 * @brief EEBUS Service Get Connection State With Ski caller definition
 */
#define EEBUS_SERVICE_GET_CONNECTION_STATE_WITH_SKI(obj, ski) \
    (EEBUS_SERVICE_INTERFACE(obj)->get_connection_state_with_ski(obj, ski))

/**
 * @brief EEBUS Service Get Remote Service Details With Ski caller definition
 */
#define EEBUS_SERVICE_GET_REMOTE_SERVICE_DETAILS_WITH_SKI(obj, ski) \
    (EEBUS_SERVICE_INTERFACE(obj)->get_remote_service_details_with_ski(obj, ski))

/**
 * @brief EEBUS Service Register Remote Ski caller definition
 */
#define EEBUS_SERVICE_REGISTER_REMOTE_SKI(obj, ski, enable) \
    (EEBUS_SERVICE_INTERFACE(obj)->register_remote_ski(obj, ski, enable))

/**
 * @brief EEBUS Service Unregister Remote Ski caller definition
 */
#define EEBUS_SERVICE_UNREGISTER_REMOTE_SKI(obj, ski) (EEBUS_SERVICE_INTERFACE(obj)->unregister_remote_ski(obj, ski))

/**
 * @brief EEBUS Service Cancel Pairing With Ski caller definition
 */
#define EEBUS_SERVICE_CANCEL_PAIRING_WITH_SKI(obj, ski) \
    (EEBUS_SERVICE_INTERFACE(obj)->cancel_pairing_with_ski(obj, ski))

/**
 * @brief EEBUS Service Set Pairing Possible caller definition
 */
#define EEBUS_SERVICE_SET_PAIRING_POSSIBLE(obj, is_pairing_possible) \
    (EEBUS_SERVICE_INTERFACE(obj)->set_pairing_possible(obj, is_pairing_possible))

/**
 * @brief EEBUS Service Get Local Ski caller definition
 */
#define EEBUS_SERVICE_GET_LOCAL_SKI(obj) (EEBUS_SERVICE_INTERFACE(obj)->get_local_ski(obj))

/**
 * @brief Eebus Service Get Qr Code String caller definition
 */
#define EEBUS_SERVICE_GET_QR_CODE_STRING(obj) (EEBUS_SERVICE_INTERFACE(obj)->get_qr_code_string(obj))

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SERVICE_API_EEBUS_SERVICE_INTERFACE_H_
