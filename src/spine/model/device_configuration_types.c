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
 * @brief SPINE Datagram Device Configuration data types declarations
 */

#include "src/spine/model/device_configuration_types.h"

#include <string.h>

bool DeviceConfigurationKeyValueIsValid(const DeviceConfigurationKeyValueDataType* key_value) {
    if (key_value == NULL) {
        return false;
    }

    return (key_value->key_id != NULL) && (key_value->value != NULL);
}

EebusError
DeviceConfigurationKeyValueGetDuration(const DeviceConfigurationKeyValueDataType* key_value, DurationType* duration) {
    if ((key_value == NULL) || (duration == NULL)) {
        return kEebusErrorInputArgumentNull;
    }

    if ((key_value->value == NULL) || (key_value->value->duration == NULL)) {
        return kEebusErrorNoChange;
    }

    memcpy(duration, key_value->value->duration, sizeof(*duration));
    return kEebusErrorOk;
}

const ScaledNumberType* DeviceConfigurationKeyValueGetScaledNumber(
    const DeviceConfigurationKeyValueDataType* key_value
) {
    if ((key_value == NULL) || (key_value->value == NULL)) {
        return NULL;
    }

    return key_value->value->scaled_number;
}

bool DeviceConfigurationKeyValueIsChangeable(const DeviceConfigurationKeyValueDataType* key_value) {
    if (key_value == NULL) {
        return false;
    }

    return (key_value->is_value_changeable != NULL) && (*key_value->is_value_changeable);
}
