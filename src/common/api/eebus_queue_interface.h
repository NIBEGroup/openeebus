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
 * @brief Eebus Queue interface declarations
 */

#ifndef SRC_COMMON_API_EEBUS_QUEUE_INTERFACE_H_
#define SRC_COMMON_API_EEBUS_QUEUE_INTERFACE_H_

#include <stdbool.h>
#include <stdint.h>

#include "src/common/eebus_errors.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @brief Eebus Queue Interface
 * (Eebus Queue "virtual functions table" declaration)
 */
typedef struct EebusQueueInterface EebusQueueInterface;

/**
 * @brief Eebus Queue Object type definition
 * ("abstract class", has no members but only pointer to
 * "virtual functions table")
 */
typedef struct EebusQueueObject EebusQueueObject;

/**
 * @brief EebusQueue Interface Structure
 */
struct EebusQueueInterface {
    void (*destruct)(EebusQueueObject* self);
    EebusError (*send)(EebusQueueObject* self, const void* msg, uint32_t timeout_ms);
    EebusError (*receive)(EebusQueueObject* self, void* msg, uint32_t timeout_ms);
    bool (*is_empty)(const EebusQueueObject* self);
    bool (*is_full)(const EebusQueueObject* self);
    void (*clear)(EebusQueueObject* self);
};

/**
 * @brief Eebus Queue Object Structure
 */
struct EebusQueueObject {
    const EebusQueueInterface* interface_;
};

/**
 * @brief Eebus Queue pointer typecast
 */
#define EEBUS_QUEUE_OBJECT(obj) ((EebusQueueObject*)(obj))

/**
 * @brief Eebus Queue Interface class pointer typecast
 */
#define EEBUS_QUEUE_INTERFACE(obj) (EEBUS_QUEUE_OBJECT(obj)->interface_)

/**
 * @brief Eebus Queue Destruct caller definition
 */
#define EEBUS_QUEUE_DESTRUCT(obj) (EEBUS_QUEUE_INTERFACE(obj)->destruct(obj))

/**
 * @brief Eebus Queue Send caller definition
 */
#define EEBUS_QUEUE_SEND(obj, msg, timeout_ms) (EEBUS_QUEUE_INTERFACE(obj)->send(obj, msg, timeout_ms))

/**
 * @brief Eebus Queue Receive caller definition
 */
#define EEBUS_QUEUE_RECEIVE(obj, msg, timeout_ms) (EEBUS_QUEUE_INTERFACE(obj)->receive(obj, msg, timeout_ms))

/**
 * @brief Eebus Queue Is Empty caller definition
 */
#define EEBUS_QUEUE_IS_EMPTY(obj) (EEBUS_QUEUE_INTERFACE(obj)->is_empty(obj))

/**
 * @brief Eebus Queue Is Full caller definition
 */
#define EEBUS_QUEUE_IS_FULL(obj) (EEBUS_QUEUE_INTERFACE(obj)->is_full(obj))

/**
 * @brief Eebus Queue Clear caller definition
 */
#define EEBUS_QUEUE_CLEAR(obj) (EEBUS_QUEUE_INTERFACE(obj)->clear(obj))

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_COMMON_API_EEBUS_QUEUE_INTERFACE_H_
