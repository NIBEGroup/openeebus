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
 * @brief Cem Ohpcf Listener interface declarations
 */

#ifndef SRC_USE_CASE_API_CEM_OHPCF_LISTENER_INTERFACE_H_
#define SRC_USE_CASE_API_CEM_OHPCF_LISTENER_INTERFACE_H_

#include "src/common/eebus_date_time/eebus_date_time.h"
#include "src/spine/model/entity_types.h"
#include "src/use_case/model/ohpcf_types.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @brief Cem Ohpcf Listener Interface
 * (Cem Ohpcf Listener "virtual functions table" declaration)
 */
typedef struct CemOhpcfListenerInterface CemOhpcfListenerInterface;

/**
 * @brief Cem Ohpcf Listener Object type definition
 * ("abstract class", has no members but only pointer to
 * "virtual functions table")
 */
typedef struct CemOhpcfListenerObject CemOhpcfListenerObject;

/**
 * @brief CemOhpcfListener Interface Structure
 */
struct CemOhpcfListenerInterface {
  void (*destruct)(CemOhpcfListenerObject* self);
  void (*on_remote_compressor_added)(CemOhpcfListenerObject* self, const EntityAddressType* entity_addr);
  void (*on_remote_compressor_removed)(CemOhpcfListenerObject* self, const EntityAddressType* entity_addr);
  void (*on_announce)(
      CemOhpcfListenerObject* self,
      const OptionalPowerConsumption* optional_power_consumption,
      const EntityAddressType* entity_addr
  );
  void (*on_state_report)(
      CemOhpcfListenerObject* self,
      CompressorOhpcfState state,
      const EebusDuration* start_time,
      const EntityAddressType* entity_addr
  );
  void (*on_clear_process)(CemOhpcfListenerObject* self, const EntityAddressType* entity_addr);
};

/**
 * @brief Cem Ohpcf Listener Object Structure
 */
struct CemOhpcfListenerObject {
  const CemOhpcfListenerInterface* interface_;
};

/**
 * @brief Cem Ohpcf Listener pointer typecast
 */
#define CEM_OHPCF_LISTENER_OBJECT(obj) ((CemOhpcfListenerObject*)(obj))

/**
 * @brief Cem Ohpcf Listener Interface class pointer typecast
 */
#define CEM_OHPCF_LISTENER_INTERFACE(obj) (CEM_OHPCF_LISTENER_OBJECT(obj)->interface_)

/**
 * @brief Cem Ohpcf Listener Destruct caller definition
 */
#define CEM_OHPCF_LISTENER_DESTRUCT(obj) (CEM_OHPCF_LISTENER_INTERFACE(obj)->destruct(obj))

/**
 * @brief Cem Ohpcf Listener On Remote Compressor Added caller definition
 */
#define CEM_OHPCF_LISTENER_ON_REMOTE_COMPRESSOR_ADDED(obj, entity_addr) \
  (CEM_OHPCF_LISTENER_INTERFACE(obj)->on_remote_compressor_added(obj, entity_addr))

/**
 * @brief Cem Ohpcf Listener On Remote Compressor Removed caller definition
 */
#define CEM_OHPCF_LISTENER_ON_REMOTE_COMPRESSOR_REMOVED(obj, entity_addr) \
  (CEM_OHPCF_LISTENER_INTERFACE(obj)->on_remote_compressor_removed(obj, entity_addr))

/**
 * @brief Cem Ohpcf Listener On Announce caller definition
 */
#define CEM_OHPCF_LISTENER_ON_ANNOUNCE(obj, optional_power_consumption, entity_addr) \
  (CEM_OHPCF_LISTENER_INTERFACE(obj)->on_announce(obj, optional_power_consumption, entity_addr))

/**
 * @brief Cem Ohpcf Listener On State Report caller definition
 */
#define CEM_OHPCF_LISTENER_ON_STATE_REPORT(obj, state, start_time, entity_addr) \
  (CEM_OHPCF_LISTENER_INTERFACE(obj)->on_state_report(obj, state, start_time, entity_addr))

/**
 * @brief Cem Ohpcf Listener On Clear Process caller definition
 */
#define CEM_OHPCF_LISTENER_ON_CLEAR_PROCESS(obj, entity_addr) \
  (CEM_OHPCF_LISTENER_INTERFACE(obj)->on_clear_process(obj, entity_addr))

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_USE_CASE_API_CEM_OHPCF_LISTENER_INTERFACE_H_
