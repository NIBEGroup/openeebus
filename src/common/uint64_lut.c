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
#include "src/common/uint64_lut.h"

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "src/common/eebus_malloc.h"

typedef struct Uint64LutRecord Uint64LutRecord;

struct Uint64LutRecord {
    uint64_t key;
    void* value;
    Uint64LutValueDeleter deleter;
};

static EebusError
Uint64LutRecordInit(Uint64LutRecord* record, uint64_t key, void* value, Uint64LutValueDeleter deleter);
static Uint64LutRecord* Uint64LutRecordCreate(uint64_t key, void* value, Uint64LutValueDeleter deleter);
static void Uint64LutRecordRelease(Uint64LutRecord* record);
static void Uint64LutRecordDelete(Uint64LutRecord* record);

EebusError Uint64LutRecordInit(Uint64LutRecord* record, uint64_t key, void* value, Uint64LutValueDeleter deleter) {
    if (record == NULL) {
        return kEebusErrorInputArgumentNull;
    }

    memset(record, 0, sizeof(*record));

    if (value == NULL) {
        return kEebusErrorInputArgumentNull;
    }

    record->key     = key;
    record->value   = value;
    record->deleter = deleter;
    return kEebusErrorOk;
}

Uint64LutRecord* Uint64LutRecordCreate(uint64_t key, void* value, Uint64LutValueDeleter deleter) {
    Uint64LutRecord* const record = (Uint64LutRecord*)EEBUS_MALLOC(sizeof(Uint64LutRecord));
    if (record == NULL) {
        return NULL;
    }

    if (Uint64LutRecordInit(record, key, value, deleter) != kEebusErrorOk) {
        Uint64LutRecordDelete(record);
        return NULL;
    }

    return record;
}

Uint64LutRecord* Uint64LutRecordCreate(uint64_t key, void* value, Uint64LutValueDeleter deleter);

void Uint64LutRecordRelease(Uint64LutRecord* record) {
    if (record->deleter != NULL) {
        record->deleter(record->value);
    }
}

void Uint64LutRecordDelete(Uint64LutRecord* record) {
    Uint64LutRecordRelease(record);
    EEBUS_FREE(record);
}

void Uint64LutConstruct(Uint64Lut* lut) {
    VectorConstruct(&lut->records);
}

void Uint64LutDestruct(Uint64Lut* lut) {
    for (size_t i = 0; i < VectorGetSize(&lut->records); ++i) {
        Uint64LutRecordDelete((Uint64LutRecord*)VectorGetElement(&lut->records, i));
    }

    VectorDestruct(&lut->records);
}

Uint64LutRecord* Uint64LutFindRecord(Uint64Lut* lut, uint64_t key) {
    for (size_t i = 0; i < VectorGetSize(&lut->records); ++i) {
        Uint64LutRecord* const record = (Uint64LutRecord*)VectorGetElement(&lut->records, i);
        if (record->key == key) {
            return record;
        }
    }

    return NULL;
}

void* Uint64LutFind(Uint64Lut* lut, uint64_t key) {
    Uint64LutRecord* const record = Uint64LutFindRecord(lut, key);
    if (record == NULL) {
        return NULL;
    }

    return record->value;
}

EebusError Uint64LutInsert(Uint64Lut* lut, uint64_t key, void* value, Uint64LutValueDeleter deleter) {
    Uint64LutRecord* const record = Uint64LutRecordCreate(key, value, deleter);
    if (record == NULL) {
        return kEebusErrorMemoryAllocate;
    }

    VectorPushBack(&lut->records, record);
    return kEebusErrorOk;
}

EebusError Uint64LutRemove(Uint64Lut* lut, uint64_t key) {
    Uint64LutRecord* const record = Uint64LutFindRecord(lut, key);
    if (record == NULL) {
        return kEebusErrorInputArgument;
    }

    VectorRemove(&lut->records, record);
    Uint64LutRecordDelete(record);
    return kEebusErrorOk;
}
