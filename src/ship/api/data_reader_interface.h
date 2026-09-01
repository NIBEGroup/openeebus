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
 * SHIP Connection Data Reader Interface declarations
 *
 * Used to pass an incoming SPINE message from a SHIP connection to the proper
 * DeviceRemote
 *
 * Implemented by spine DeviceRemote, used by ShipConnection
 */

#ifndef SRC_SHIP_API_DATA_READER_INTERFACE_H_
#define SRC_SHIP_API_DATA_READER_INTERFACE_H_

#include <stddef.h>
#include <stdint.h>

#include "src/common/message_buffer.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @brief Data Reader Interface
 * (Data Reader "virtual functions table" declaration)
 *
 * Transformed from ShipConnectionDataReaderInterface
 */
typedef struct DataReaderInterface DataReaderInterface;

/**
 * Data Reader Object type definition
 * ("abstract class", has no members but only pointer to
 * "virtual functions table")
 */
typedef struct DataReaderObject DataReaderObject;

/**
 * @brief Data Reader Interface structure
 */
struct DataReaderInterface {
    void (*destruct)(DataReaderObject* self);
    /**
     * @brief Pass an incoming SPINE message from a SHIP connection to the proper
     * DeviceRemote
     *
     * Transformed from HandleShipPayloadMessage()
     */
    void (*handle_message)(DataReaderObject* self, MessageBuffer* msg);
};

/**
 * @brief Data Reader Object structure
 */
struct DataReaderObject {
    const DataReaderInterface* interface_;
};

/**
 * @brief Data Reader Object pointer typecast
 */
#define DATA_READER_OBJECT(obj) ((DataReaderObject*)(obj))

/**
 * @brief Data Reader Interface class pointer typecast
 */
#define DATA_READER_INTERFACE(obj) (DATA_READER_OBJECT(obj)->interface_)

/**
 * @brief Data Reader Destructor caller definition
 */
#define DATA_READER_DESTRUCT(obj) (DATA_READER_INTERFACE(obj)->destruct(obj))

/**
 * @brief Data Reader Handle Payload Message caller definition
 */
#define DATA_READER_HANDLE_MESSAGE(obj, msg) (DATA_READER_INTERFACE(obj)->handle_message(obj, msg))

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SHIP_API_DATA_READER_INTERFACE_H_
