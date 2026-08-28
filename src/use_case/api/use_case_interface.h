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
 * @brief Use Case interface declarations
 */

#ifndef SRC_USE_CASE_API_USE_CASE_INTERFACE_H_
#define SRC_USE_CASE_API_USE_CASE_INTERFACE_H_

#include "src/spine/api/entity_remote_interface.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @brief Use Case Interface
 * (Use Case "virtual functions table" declaration)
 */
typedef struct UseCaseInterface UseCaseInterface;

/**
 * @brief Use Case Object type definition
 * ("abstract class", has no members but only pointer to
 * "virtual functions table")
 */
typedef struct UseCaseObject UseCaseObject;

/**
 * @brief UseCase Interface Structure
 */
struct UseCaseInterface {
    void (*destruct)(UseCaseObject* self);
    bool (*is_entity_compatible)(const UseCaseObject* self, const EntityRemoteObject* remote_entity);
    bool (*is_use_case_compatible)(const UseCaseObject* self, const UseCaseFilterType* use_case_filter);
    EntityRemoteObject* (*get_remote_entity_with_address)(
        const UseCaseObject* self,
        const EntityAddressType* remote_entity_addr
    );
};

/**
 * @brief Use Case Object Structure
 */
struct UseCaseObject {
    const UseCaseInterface* interface_;
};

/**
 * @brief Use Case pointer typecast
 */
#define USE_CASE_OBJECT(obj) ((UseCaseObject*)(obj))

/**
 * @brief Use Case Interface class pointer typecast
 */
#define USE_CASE_INTERFACE(obj) (USE_CASE_OBJECT(obj)->interface_)

/**
 * @brief Use Case Destruct caller definition
 */
#define USE_CASE_DESTRUCT(obj) (USE_CASE_INTERFACE(obj)->destruct(obj))

/**
 * @brief Use Case Is Entity Compatible caller definition
 */
#define USE_CASE_IS_ENTITY_COMPATIBLE(obj, remote_entity) \
    (USE_CASE_INTERFACE(obj)->is_entity_compatible(obj, remote_entity))

/**
 * @brief Use Case Is Use Case Compatible caller definition
 */
#define USE_CASE_IS_USE_CASE_COMPATIBLE(obj, use_case_filter) \
    (USE_CASE_INTERFACE(obj)->is_use_case_compatible(obj, use_case_filter))

/**
 * @brief Use Case Get Remote Entity With Address caller definition
 */
#define USE_CASE_GET_REMOTE_ENTITY_WITH_ADDRESS(obj, remote_entity_addr) \
    (USE_CASE_INTERFACE(obj)->get_remote_entity_with_address(obj, remote_entity_addr))

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_USE_CASE_API_USE_CASE_INTERFACE_H_
