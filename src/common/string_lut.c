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
#include "src/common/string_lut.h"

#include <stddef.h>

#include "src/common/eebus_malloc.h"
#include "src/common/string_util.h"

typedef struct StringLutRecord StringLutRecord;

struct StringLutRecord {
    char* key;
    void* value;
    StringLutValueDeleter deleter;
};

static EebusError
StringLutRecordInit(StringLutRecord* record, const char* key, void* value, StringLutValueDeleter deleter);
static StringLutRecord* StringLutRecordCreate(const char* key, void* value, StringLutValueDeleter deleter);
static void StringLutRecordRelease(StringLutRecord* record);
static void StringLutRecordDelete(StringLutRecord* record);

EebusError StringLutRecordInit(StringLutRecord* record, const char* key, void* value, StringLutValueDeleter deleter) {
    if (record == NULL) {
        return kEebusErrorInputArgumentNull;
    }

    memset(record, 0, sizeof(*record));

    if ((key == NULL) || (value == NULL)) {
        return kEebusErrorInputArgumentNull;
    }

    record->key = StringCopy(key);
    if (record->key == NULL) {
        return kEebusErrorMemoryAllocate;
    }

    record->value   = value;
    record->deleter = deleter;
    return kEebusErrorOk;
}

StringLutRecord* StringLutRecordCreate(const char* key, void* value, StringLutValueDeleter deleter) {
    StringLutRecord* const record = (StringLutRecord*)EEBUS_MALLOC(sizeof(StringLutRecord));
    if (record == NULL) {
        return NULL;
    }

    if (StringLutRecordInit(record, key, value, deleter) != kEebusErrorOk) {
        StringLutRecordDelete(record);
        return NULL;
    }

    return record;
}

StringLutRecord* StringLutRecordCreate(const char* key, void* value, StringLutValueDeleter deleter);

void StringLutRecordRelease(StringLutRecord* record) {
    StringDelete(record->key);
    if (record->deleter != NULL) {
        record->deleter(record->value);
    }
}

void StringLutRecordDelete(StringLutRecord* record) {
    StringLutRecordRelease(record);
    EEBUS_FREE(record);
}

void StringLutInit(StringLut* lut) {
    VectorConstruct(&lut->records);
}

void StringLutRelease(StringLut* lut) {
    for (size_t i = 0; i < VectorGetSize(&lut->records); ++i) {
        StringLutRecordDelete((StringLutRecord*)VectorGetElement(&lut->records, i));
    }

    VectorDestruct(&lut->records);
}

StringLutRecord* StringLutFindRecord(const StringLut* lut, const char* key) {
    if (key == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < VectorGetSize(&lut->records); ++i) {
        StringLutRecord* const record = (StringLutRecord*)VectorGetElement(&lut->records, i);
        if (!strcmp(record->key, key)) {
            return record;
        }
    }

    return NULL;
}

void* StringLutFind(const StringLut* lut, const char* key) {
    StringLutRecord* const record = StringLutFindRecord(lut, key);
    if (record == NULL) {
        return NULL;
    }

    return record->value;
}

EebusError StringLutInsert(StringLut* lut, const char* key, void* value, StringLutValueDeleter deleter) {
    StringLutRecord* const record = StringLutRecordCreate(key, value, deleter);
    if (record == NULL) {
        return kEebusErrorMemoryAllocate;
    }

    VectorPushBack(&lut->records, record);
    return kEebusErrorOk;
}

EebusError StringLutRemove(StringLut* lut, const char* key) {
    StringLutRecord* const record = StringLutFindRecord(lut, key);
    if (record == NULL) {
        return kEebusErrorInputArgument;
    }

    VectorRemove(&lut->records, record);
    StringLutRecordDelete(record);
    return kEebusErrorOk;
}

size_t StringLutGetSize(const StringLut* lut) {
    return VectorGetSize(&lut->records);
}

void* StringLutGetElementValue(const StringLut* lut, size_t idx) {
    const StringLutRecord* const record = (const StringLutRecord*)VectorGetElement(&lut->records, idx);
    return record->value;
}
