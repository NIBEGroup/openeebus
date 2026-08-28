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
 * @brief Ma Mpc Listener interface declarations
 */

#ifndef SRC_CLI_MA_MPC_LISTENER_INTERFACE_H_
#define SRC_CLI_MA_MPC_LISTENER_INTERFACE_H_

#include "src/spine/model/entity_types.h"
#include "src/use_case/model/mpc_types.h"
#include "src/use_case/model/scaled_value.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @brief Ma Mpc Listener Interface
 * (Ma Mpc Listener "virtual functions table" declaration)
 */
typedef struct MaMpcListenerInterface MaMpcListenerInterface;

/**
 * @brief Ma Mpc Listener Object type definition
 * ("abstract class", has no members but only pointer to
 * "virtual functions table")
 */
typedef struct MaMpcListenerObject MaMpcListenerObject;

/**
 * @brief MaMpcListener Interface Structure
 */
struct MaMpcListenerInterface {
    void (*destruct)(MaMpcListenerObject* self);
    void (*on_remote_mu_added)(MaMpcListenerObject* self, const EntityAddressType* entity_addr);
    void (*on_remote_mu_removed)(MaMpcListenerObject* self, const EntityAddressType* entity_addr);
    void (*on_measurement_receive)(
        MaMpcListenerObject* self,
        MuMpcMeasurementNameId name_id,
        const ScaledValue* measurement_value,
        const EntityAddressType* remote_entity_addr
    );
};

/**
 * @brief Ma Mpc Listener Object Structure
 */
struct MaMpcListenerObject {
    const MaMpcListenerInterface* interface;
};

/**
 * @brief Ma Mpc Listener pointer typecast
 */
#define MA_MPC_LISTENER_OBJECT(obj) ((MaMpcListenerObject*)(obj))

/**
 * @brief Ma Mpc Listener Interface class pointer typecast
 */
#define MA_MPC_LISTENER_INTERFACE(obj) (MA_MPC_LISTENER_OBJECT(obj)->interface)

/**
 * @brief Ma Mpc Listener Destruct caller definition
 */
#define MA_MPC_LISTENER_DESTRUCT(obj) (MA_MPC_LISTENER_INTERFACE(obj)->destruct(obj))

/**
 * @brief Ma Mpc Listener On Remote MU Added caller definition
 */
#define MA_MPC_LISTENER_ON_REMOTE_MU_ADDED(obj, entity_addr) \
    (MA_MPC_LISTENER_INTERFACE(obj)->on_remote_mu_added(obj, entity_addr))

/**
 * @brief Ma Mpc Listener On Remote MU Removed caller definition
 */
#define MA_MPC_LISTENER_ON_REMOTE_MU_REMOVED(obj, entity_addr) \
    (MA_MPC_LISTENER_INTERFACE(obj)->on_remote_mu_removed(obj, entity_addr))

/**
 * @brief Ma Mpc Listener On Measurement Receive caller definition
 */
#define MA_MPC_LISTENER_ON_MEASUREMENT_RECEIVE(obj, name_id, measurement_value, remote_entity_addr) \
    (MA_MPC_LISTENER_INTERFACE(obj)->on_measurement_receive(obj, name_id, measurement_value, remote_entity_addr))

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_CLI_MA_MPC_LISTENER_INTERFACE_H_
