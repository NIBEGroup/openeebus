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
 * @brief Ship Message Serialize interface declarations
 */

#ifndef SRC_SHIP_API_SHIP_MESSAGE_SERIALIZE_INTERFACE_H_
#define SRC_SHIP_API_SHIP_MESSAGE_SERIALIZE_INTERFACE_H_

#include "src/common/message_buffer.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @brief Ship Message Serialize Interface
 * (Ship Message Serialize "virtual functions table" declaration)
 */
typedef struct ShipMessageSerializeInterface ShipMessageSerializeInterface;

/**
 * @brief Ship Message Serialize Object type definition
 * ("abstract class", has no members but only pointer to
 * "virtual functions table")
 */
typedef struct ShipMessageSerializeObject ShipMessageSerializeObject;

/**
 * @brief ShipMessageSerialize Interface Structure
 */
struct ShipMessageSerializeInterface {
    void (*destruct)(ShipMessageSerializeObject* self);
    MessageBuffer* (*get_buffer)(const ShipMessageSerializeObject* self);
};

/**
 * @brief Ship Message Serialize Object Structure
 */
struct ShipMessageSerializeObject {
    const ShipMessageSerializeInterface* interface_;
};

/**
 * @brief Ship Message Serialize pointer typecast
 */
#define SHIP_MESSAGE_SERIALIZE_OBJECT(obj) ((ShipMessageSerializeObject*)(obj))

/**
 * @brief Ship Message Serialize Interface class pointer typecast
 */
#define SHIP_MESSAGE_SERIALIZE_INTERFACE(obj) (SHIP_MESSAGE_SERIALIZE_OBJECT(obj)->interface_)

/**
 * @brief Ship Message Serialize Destruct caller definition
 */
#define SHIP_MESSAGE_SERIALIZE_DESTRUCT(obj) (SHIP_MESSAGE_SERIALIZE_INTERFACE(obj)->destruct(obj))

/**
 * @brief Ship Message Serialize Get Buffer caller definition
 */
#define SHIP_MESSAGE_SERIALIZE_GET_BUFFER(obj) (SHIP_MESSAGE_SERIALIZE_INTERFACE(obj)->get_buffer(obj))

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SHIP_API_SHIP_MESSAGE_SERIALIZE_INTERFACE_H_
