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
 * @brief Compressor OHPCF Listener interface declarations
 */

#ifndef SRC_USE_CASE_API_COMPRESSOR_OHPCF_LISTENER_INTERFACE_H_
#define SRC_USE_CASE_API_COMPRESSOR_OHPCF_LISTENER_INTERFACE_H_

#include "src/common/eebus_date_time/eebus_date_time.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @brief Compressor OHPCF Listener Interface
 * (Compressor OHPCF Listener "virtual functions table" declaration)
 */
typedef struct CompressorOhpcfListenerInterface CompressorOhpcfListenerInterface;

/**
 * @brief Compressor OHPCF Listener Object type definition
 * ("abstract class", has no members but only pointer to
 * "virtual functions table")
 */
typedef struct CompressorOhpcfListenerObject CompressorOhpcfListenerObject;

/**
 * @brief CompressorOhpcfListener Interface Structure
 */
struct CompressorOhpcfListenerInterface {
  void (*destruct)(CompressorOhpcfListenerObject* self);
  void (*on_remote_cem_added)(CompressorOhpcfListenerObject* self);
  void (*on_remote_cem_removed)(CompressorOhpcfListenerObject* self);
  void (*on_schedule_optional_power_consumption)(CompressorOhpcfListenerObject* self, const EebusDuration* start_time);
  void (*on_stop)(CompressorOhpcfListenerObject* self);
  void (*on_pause)(CompressorOhpcfListenerObject* self);
  void (*on_resume)(CompressorOhpcfListenerObject* self);
};

/**
 * @brief Compressor OHPCF Listener Object Structure
 */
struct CompressorOhpcfListenerObject {
  const CompressorOhpcfListenerInterface* interface_;
};

/**
 * @brief Compressor OHPCF Listener pointer typecast
 */
#define COMPRESSOR_OHPCF_LISTENER_OBJECT(obj) ((CompressorOhpcfListenerObject*)(obj))

/**
 * @brief Compressor OHPCF Listener Interface class pointer typecast
 */
#define COMPRESSOR_OHPCF_LISTENER_INTERFACE(obj) (COMPRESSOR_OHPCF_LISTENER_OBJECT(obj)->interface_)

/**
 * @brief Compressor OHPCF Listener Destruct caller definition
 */
#define COMPRESSOR_OHPCF_LISTENER_DESTRUCT(obj) (COMPRESSOR_OHPCF_LISTENER_INTERFACE(obj)->destruct(obj))

/**
 * @brief Compressor OHPCF Listener On Remote Cem Added caller definition
 */
#define COMPRESSOR_OHPCF_LISTENER_ON_REMOTE_CEM_ADDED(obj) \
  (COMPRESSOR_OHPCF_LISTENER_INTERFACE(obj)->on_remote_cem_added(obj))

/**
 * @brief Compressor OHPCF Listener On Remote Cem Removed caller definition
 */
#define COMPRESSOR_OHPCF_LISTENER_ON_REMOTE_CEM_REMOVED(obj) \
  (COMPRESSOR_OHPCF_LISTENER_INTERFACE(obj)->on_remote_cem_removed(obj))

/**
 * @brief Compressor OHPCF Listener On Schedule Optional Power Consumption caller definition
 */
#define COMPRESSOR_OHPCF_LISTENER_ON_SCHEDULE_OPTIONAL_POWER_CONSUMPTION(obj, start_time) \
  (COMPRESSOR_OHPCF_LISTENER_INTERFACE(obj)->on_schedule_optional_power_consumption(obj, start_time))

/**
 * @brief Compressor OHPCF Listener On Stop caller definition
 */
#define COMPRESSOR_OHPCF_LISTENER_ON_STOP(obj) (COMPRESSOR_OHPCF_LISTENER_INTERFACE(obj)->on_stop(obj))

/**
 * @brief Compressor OHPCF Listener On Pause caller definition
 */
#define COMPRESSOR_OHPCF_LISTENER_ON_PAUSE(obj) (COMPRESSOR_OHPCF_LISTENER_INTERFACE(obj)->on_pause(obj))

/**
 * @brief Compressor OHPCF Listener On Resume caller definition
 */
#define COMPRESSOR_OHPCF_LISTENER_ON_RESUME(obj) (COMPRESSOR_OHPCF_LISTENER_INTERFACE(obj)->on_resume(obj))

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_USE_CASE_API_COMPRESSOR_OHPCF_LISTENER_INTERFACE_H_
