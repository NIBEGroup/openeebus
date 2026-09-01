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
 * @brief Data Reader implementation
 */

#include "src/common/eebus_arguments.h"
#include "src/common/eebus_malloc.h"
#include "src/common/message_buffer.h"
#include "src/ship/api/data_reader_interface.h"
#include "src/spine/api/device_remote_interface.h"

typedef struct DataReader DataReader;

struct DataReader {
    /** Implements the Data Reader Interface */
    DataReaderObject obj;

    DeviceRemoteObject* device_remote;
};

#define DATA_READER(obj) ((DataReader*)(obj))

static void Destruct(DataReaderObject* self);
static void HandleMessage(DataReaderObject* self, MessageBuffer* msg);

static const DataReaderInterface data_reader_methods = {
    .destruct       = Destruct,
    .handle_message = HandleMessage,
};

static void DataReaderConstruct(DataReader* self, DeviceRemoteObject* device_remote);

void DataReaderConstruct(DataReader* self, DeviceRemoteObject* device_remote) {
    // Override "virtual functions table"
    DATA_READER_INTERFACE(self) = &data_reader_methods;

    self->device_remote = device_remote;
}

DataReaderObject* DataReaderCreate(DeviceRemoteObject* device_remote) {
    DataReader* const data_reader = (DataReader*)EEBUS_MALLOC(sizeof(DataReader));

    DataReaderConstruct(data_reader, device_remote);

    return DATA_READER_OBJECT(data_reader);
}

void Destruct(DataReaderObject* self) {
    UNUSED(self);
    // Nothing to be deallocated here
}

void HandleMessage(DataReaderObject* self, MessageBuffer* msg) {
    const DataReader* const data_reader = DATA_READER(self);
    DEVICE_REMOTE_HANDLE_SPINE_MESSSAGE(data_reader->device_remote, msg);
}
