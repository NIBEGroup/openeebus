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
 * @brief A part of transformed api/shipconnection.go
 *
 * SHIP Connection Data Writer Interface declarations
 *
 * Used to pass an outgoing SPINE message from a DeviceLocal to the SHIP
 * connection
 *
 * Implemented by ShipConnection, used by spine DeviceLocal
 */

#ifndef SRC_SHIP_API_DATA_WRITER_INTERFACE_H_
#define SRC_SHIP_API_DATA_WRITER_INTERFACE_H_

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @brief Data Writer Interface
 * (Data Writer "virtual functions table" declaration)
 *
 * Transformed from ShipConnectionDataWriterInterface
 */
typedef struct DataWriterInterface DataWriterInterface;

/**
 * @brief Data Writer Object type definition
 * ("abstract class", has no members but only pointer to
 * "virtual functions table")
 */
typedef struct DataWriterObject DataWriterObject;

/**
 * @brief Data Writer Interface Structure
 */
struct DataWriterInterface {
    void (*destruct)(DataWriterObject* self);
    /**
     * @brief Pass an outgoing SPINE message from a DeviceLocal to the SHIP
     * connection (the SHIP message includes payload)
     *
     * Transformed from WriteShipMessageWithPayload()
     */
    void (*write_message)(DataWriterObject* self, const uint8_t* msg, size_t msg_size);
};

/**
 * @brief Data Writer Object structure
 */
struct DataWriterObject {
    const DataWriterInterface* interface_;
};

/**
 * @brief Data Writer Object pointer typecast
 */
#define DATA_WRITER_OBJECT(obj) ((DataWriterObject*)(obj))

/**
 * @brief Data Writer Interface class pointer typecast
 */
#define DATA_WRITER_INTERFACE(obj) (DATA_WRITER_OBJECT(obj)->interface_)

/**
 * @brief Data Writer Destructor caller definition
 */
#define DATA_WRITER_DESTRUCT(obj) (DATA_WRITER_INTERFACE(obj)->destruct(obj))

/**
 * @brief Data Writer Write Message caller definition
 */
#define DATA_WRITER_WRITE_MESSAGE(obj, msg, sz) (DATA_WRITER_INTERFACE(obj)->write_message(obj, msg, sz))

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SHIP_API_DATA_WRITER_INTERFACE_H_
