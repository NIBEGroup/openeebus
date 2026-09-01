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
 * @brief Operations interface declarations
 */

#ifndef SRC_SPINE_API_OPERATIONS_INTERFACE_H_
#define SRC_SPINE_API_OPERATIONS_INTERFACE_H_

#include <stdbool.h>

#include "src/spine/model/possible_operations_types.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @brief Operations Interface
 * (Operations "virtual functions table" declaration)
 */
typedef struct OperationsInterface OperationsInterface;

/**
 * @brief Operations Object type definition
 * ("abstract class", has no members but only pointer to
 * "virtual functions table")
 */
typedef struct OperationsObject OperationsObject;

/**
 * @brief Operations Interface Structure
 */
struct OperationsInterface {
    void (*destruct)(OperationsObject* self);
    bool (*get_read)(const OperationsObject* self);
    bool (*get_read_partial)(const OperationsObject* self);
    bool (*get_write)(const OperationsObject* self);
    bool (*get_write_partial)(const OperationsObject* self);
    const char* (*to_string)(const OperationsObject* self);
    const PossibleOperationsType* (*get_information)(const OperationsObject* self);
};

/**
 * @brief Operations Object Structure
 */
struct OperationsObject {
    const OperationsInterface* interface_;
};

/**
 * @brief Operations pointer typecast
 */
#define OPERATIONS_OBJECT(obj) ((OperationsObject*)(obj))

/**
 * @brief Operations Interface class pointer typecast
 */
#define OPERATIONS_INTERFACE(obj) (OPERATIONS_OBJECT(obj)->interface_)

/**
 * @brief Operations Destruct caller definition
 */
#define OPERATIONS_DESTRUCT(obj) (OPERATIONS_INTERFACE(obj)->destruct(obj))

/**
 * @brief Operations Get Read caller definition
 */
#define OPERATIONS_GET_READ(obj) (OPERATIONS_INTERFACE(obj)->get_read(obj))

/**
 * @brief Operations Get Read Partial caller definition
 */
#define OPERATIONS_GET_READ_PARTIAL(obj) (OPERATIONS_INTERFACE(obj)->get_read_partial(obj))

/**
 * @brief Operations Get Write caller definition
 */
#define OPERATIONS_GET_WRITE(obj) (OPERATIONS_INTERFACE(obj)->get_write(obj))

/**
 * @brief Operations Get Write Partial caller definition
 */
#define OPERATIONS_GET_WRITE_PARTIAL(obj) (OPERATIONS_INTERFACE(obj)->get_write_partial(obj))

/**
 * @brief Operations To String caller definition
 */
#define OPERATIONS_TO_STRING(obj) (OPERATIONS_INTERFACE(obj)->to_string(obj))

/**
 * @brief Operations Get Information caller definition
 */
#define OPERATIONS_GET_INFORMATION(obj) (OPERATIONS_INTERFACE(obj)->get_information(obj))

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SPINE_API_OPERATIONS_INTERFACE_H_
