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
 * @brief Mu Mpc Listener interface declarations
 */

#ifndef SRC_USE_CASE_API_MU_MPC_LISTENER_INTERFACE_H_
#define SRC_USE_CASE_API_MU_MPC_LISTENER_INTERFACE_H_

#include "src/spine/model/entity_types.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @brief Mu Mpc Listener Interface
 * (Mu Mpc Listener "virtual functions table" declaration)
 */
typedef struct MuMpcListenerInterface MuMpcListenerInterface;

/**
 * @brief Mu Mpc Listener Object type definition
 * ("abstract class", has no members but only pointer to
 * "virtual functions table")
 */
typedef struct MuMpcListenerObject MuMpcListenerObject;

/**
 * @brief MuMpcListener Interface Structure
 */
struct MuMpcListenerInterface {
    void (*destruct)(MuMpcListenerObject* self);
    void (*on_remote_ma_added)(MuMpcListenerObject* self, const EntityAddressType* entity_addr);
    void (*on_remote_ma_removed)(MuMpcListenerObject* self, const EntityAddressType* entity_addr);
};

/**
 * @brief Mu Mpc Listener Object Structure
 */
struct MuMpcListenerObject {
    const MuMpcListenerInterface* interface;
};

/**
 * @brief Mu Mpc Listener pointer typecast
 */
#define MU_MPC_LISTENER_OBJECT(obj) ((MuMpcListenerObject*)(obj))

/**
 * @brief Mu Mpc Listener Interface class pointer typecast
 */
#define MU_MPC_LISTENER_INTERFACE(obj) (MU_MPC_LISTENER_OBJECT(obj)->interface)

/**
 * @brief Mu Mpc Listener Destruct caller definition
 */
#define MU_MPC_LISTENER_DESTRUCT(obj) (MU_MPC_LISTENER_INTERFACE(obj)->destruct(obj))

/**
 * @brief Mu Mpc Listener On Remote MA Added caller definition
 */
#define MU_MPC_LISTENER_ON_REMOTE_MA_ADDED(obj, entity_addr) \
    (MU_MPC_LISTENER_INTERFACE(obj)->on_remote_ma_added(obj, entity_addr))

/**
 * @brief Mu Mpc Listener On Remote MA Removed caller definition
 */
#define MU_MPC_LISTENER_ON_REMOTE_MA_REMOVED(obj, entity_addr) \
    (MU_MPC_LISTENER_INTERFACE(obj)->on_remote_ma_removed(obj, entity_addr))

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_USE_CASE_API_MU_MPC_LISTENER_INTERFACE_H_
