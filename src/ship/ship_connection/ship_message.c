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
 * @brief Ship Message utility implementation
 */

#include "src/ship/api/ship_message.h"

#include <string.h>

#include "src/common/eebus_malloc.h"

ShipMessage* ShipMessageCreate(uint8_t type, size_t value_size) {
    ShipMessage* const ship_msg = (ShipMessage*)EEBUS_MALLOC(sizeof(ShipMessage));

    if (ship_msg == NULL) {
        return ship_msg;
    }

    ship_msg->type  = type;
    ship_msg->value = EEBUS_MALLOC(value_size);
    if (ship_msg->value != NULL) {
        memset(ship_msg->value, 0, value_size);
    }

    return ship_msg;
}

void ShipMessageRelease(ShipMessage* ship_msg) {
    if (ship_msg == NULL) {
        return;
    }

    if (ship_msg->value != NULL) {
        EEBUS_FREE(ship_msg->value);
        ship_msg->value = NULL;
    }

    EEBUS_FREE(ship_msg);
}
