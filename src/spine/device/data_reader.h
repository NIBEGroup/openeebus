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
 * @brief Data Reader implementation declarations
 */

#ifndef SRC_SPINE_DEVICE_DATA_READER_H_
#define SRC_SPINE_DEVICE_DATA_READER_H_

#include <stddef.h>

#include "src/common/eebus_malloc.h"
#include "src/ship/api/data_reader_interface.h"
#include "src/spine/api/device_remote_interface.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

DataReaderObject* DataReaderCreate(DeviceRemoteObject* device_remote);

static inline void DataReaderDelete(DataReaderObject* data_reader) {
    if (data_reader != NULL) {
        DATA_READER_DESTRUCT(data_reader);
        EEBUS_FREE(data_reader);
    }
}

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SPINE_DEVICE_DATA_READER_H_
