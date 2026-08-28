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
 * @brief SHIP Message utility
 */
#ifndef SRC_SHIP_SHIP_CONNECTION_SHIP_MESSAGE_H_
#define SRC_SHIP_SHIP_CONNECTION_SHIP_MESSAGE_H_

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct ShipMessage ShipMessage;

struct ShipMessage {
    uint8_t type;
    void* value;
};

/**
 * @brief Create the SHIP Message with type and value buufer size specified
 * @param type SHIP message type
 * @param value_size SHIP Message value size
 */
ShipMessage* ShipMessageCreate(uint8_t type, size_t value_size);

/**
 * @brief Deallocate the SHIP Message instance
 * @param ship_msg SHIP Message to be deallocated
 */
void ShipMessageRelease(ShipMessage* ship_msg);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SHIP_SHIP_CONNECTION_SHIP_MESSAGE_H_
