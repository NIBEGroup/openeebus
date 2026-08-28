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
 * @brief Ship Message Deserialize interface declarations
 */

#ifndef SRC_SHIP_API_SHIP_MESSAGE_DESERIALIZE_INTERFACE_H_
#define SRC_SHIP_API_SHIP_MESSAGE_DESERIALIZE_INTERFACE_H_

#include "src/common/message_buffer.h"
#include "src/ship/model/model.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @brief Ship Message Deserialize Interface
 * (Ship Message Deserialize "virtual functions table" declaration)
 */
typedef struct ShipMessageDeserializeInterface ShipMessageDeserializeInterface;

/**
 * @brief Ship Message Deserialize Object type definition
 * ("abstract class", has no members but only pointer to
 * "virtual functions table")
 */
typedef struct ShipMessageDeserializeObject ShipMessageDeserializeObject;

/**
 * @brief ShipMessageDeserialize Interface Structure
 */
struct ShipMessageDeserializeInterface {
    void (*destruct)(ShipMessageDeserializeObject* self);
    MsgValueType (*get_value_type)(const ShipMessageDeserializeObject* self);
    void* (*get_value)(const ShipMessageDeserializeObject* self);
};

/**
 * @brief Ship Message Deserialize Object Structure
 */
struct ShipMessageDeserializeObject {
    const ShipMessageDeserializeInterface* interface_;
};

/**
 * @brief Ship Message Deserialize pointer typecast
 */
#define SHIP_MESSAGE_DESERIALIZE_OBJECT(obj) ((ShipMessageDeserializeObject*)(obj))

/**
 * @brief Ship Message Deserialize Interface class pointer typecast
 */
#define SHIP_MESSAGE_DESERIALIZE_INTERFACE(obj) (SHIP_MESSAGE_DESERIALIZE_OBJECT(obj)->interface_)

/**
 * @brief Ship Message Deserialize Destruct caller definition
 */
#define SHIP_MESSAGE_DESERIALIZE_DESTRUCT(obj) (SHIP_MESSAGE_DESERIALIZE_INTERFACE(obj)->destruct(obj))

/**
 * @brief Ship Message Deserialize Get Value Type caller definition
 */
#define SHIP_MESSAGE_DESERIALIZE_GET_VALUE_TYPE(obj) (SHIP_MESSAGE_DESERIALIZE_INTERFACE(obj)->get_value_type(obj))

/**
 * @brief Ship Message Deserialize Get Value caller definition
 */
#define SHIP_MESSAGE_DESERIALIZE_GET_VALUE(obj) (SHIP_MESSAGE_DESERIALIZE_INTERFACE(obj)->get_value(obj))

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SHIP_API_SHIP_MESSAGE_DESERIALIZE_INTERFACE_H_
