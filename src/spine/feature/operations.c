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
 * @brief Operations implementation
 */

#include "src/common/eebus_malloc.h"
#include "src/spine/api/operations_interface.h"

typedef struct Operations Operations;

struct Operations {
    /** Implements the Operations Interface */
    OperationsObject obj;

    bool read;
    bool read_partial;
    bool write;
    bool write_partial;

    const PossibleOperationsType* info;
};

#define OPERATIONS(obj) ((Operations*)(obj))

static void Destruct(OperationsObject* self);
static bool GetRead(const OperationsObject* self);
static bool GetReadPartial(const OperationsObject* self);
static bool GetWrite(const OperationsObject* self);
static bool GetWritePartial(const OperationsObject* self);
static const char* ToString(const OperationsObject* self);
static const PossibleOperationsType* GetInformation(const OperationsObject* self);

static const OperationsInterface operations_methods = {
    .destruct          = Destruct,
    .get_read          = GetRead,
    .get_read_partial  = GetReadPartial,
    .get_write         = GetWrite,
    .get_write_partial = GetWritePartial,
    .to_string         = ToString,
    .get_information   = GetInformation,
};

static void OperationsConstruct(Operations* self, bool read, bool read_partial, bool write, bool write_partial);

void OperationsConstruct(Operations* self, bool read, bool read_partial, bool write, bool write_partial) {
    // Override "virtual functions table"
    OPERATIONS_INTERFACE(self) = &operations_methods;

    self->read          = read;
    self->read_partial  = read_partial;
    self->write         = write;
    self->write_partial = write_partial;
    self->info          = PossibleOperationsCreate(read, read_partial, write, write_partial);
}

OperationsObject* OperationsCreate(bool read, bool read_partial, bool write, bool write_partial) {
    Operations* const operations = (Operations*)EEBUS_MALLOC(sizeof(Operations));
    if (operations == NULL) {
        return NULL;
    }

    OperationsConstruct(operations, read, read_partial, write, write_partial);

    return OPERATIONS_OBJECT(operations);
}

void Destruct(OperationsObject* self) {
    Operations* const op = OPERATIONS(self);

    PossibleOperationsDelete((PossibleOperationsType*)op->info);
    op->info = NULL;
}

bool GetRead(const OperationsObject* self) {
    return OPERATIONS(self)->read;
}
bool GetReadPartial(const OperationsObject* self) {
    return OPERATIONS(self)->read_partial;
}
bool GetWrite(const OperationsObject* self) {
    return OPERATIONS(self)->write;
}
bool GetWritePartial(const OperationsObject* self) {
    return OPERATIONS(self)->write_partial;
}

const char* ToString(const OperationsObject* self) {
    const Operations* const op = OPERATIONS(self);
    return (op->read && !op->write) ? "RO" : (op->read && op->write) ? "RW" : "--";
}

const PossibleOperationsType* GetInformation(const OperationsObject* self) {
    const Operations* const op = OPERATIONS(self);
    return op->info;
}
