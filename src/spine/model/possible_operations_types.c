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
 * @brief SPINE Datagram Possible Operations payload functions implementation
 */

#include "src/spine/model/possible_operations_types.h"

#include <stddef.h>
#include <string.h>

#include "src/common/eebus_data/eebus_data.h"
#include "src/common/eebus_malloc.h"
#include "src/spine/model/model.h"

static PossibleOperationsReadType* PossibleOperationsReadCreate(bool partial);
static PossibleOperationsWriteType* PossibleOperationsWriteCreate(bool partial);

PossibleOperationsReadType* PossibleOperationsReadCreate(bool partial) {
    PossibleOperationsReadType* const op_rd
        = (PossibleOperationsReadType*)EEBUS_MALLOC(sizeof(PossibleOperationsReadType));
    if (op_rd == NULL) {
        return NULL;
    }

    op_rd->partial = EEBUS_TAG_FROM_BOOL(partial);
    return op_rd;
}

PossibleOperationsWriteType* PossibleOperationsWriteCreate(bool partial) {
    PossibleOperationsWriteType* const op_wr
        = (PossibleOperationsWriteType*)EEBUS_MALLOC(sizeof(PossibleOperationsWriteType));
    if (op_wr == NULL) {
        return NULL;
    }

    op_wr->partial = EEBUS_TAG_FROM_BOOL(partial);
    return op_wr;
}

PossibleOperationsType* PossibleOperationsCreate(bool read, bool read_partial, bool write, bool write_partial) {
    PossibleOperationsType* const op = (PossibleOperationsType*)EEBUS_MALLOC(sizeof(PossibleOperationsType));
    if (op == NULL) {
        return NULL;
    }

    memset(op, 0, sizeof(*op));

    if (read && !(op->read = PossibleOperationsReadCreate(read_partial))) {
        PossibleOperationsDelete(op);
        return NULL;
    }

    if (write && !(op->write = PossibleOperationsWriteCreate(write_partial))) {
        PossibleOperationsDelete(op);
        return NULL;
    }

    return op;
}

void PossibleOperationsDelete(PossibleOperationsType* self) {
    EebusDataCfg cfg = EEBUS_DATA_SEQUENCE_TMP(PossibleOperationsType, ModelGetPossibleOperationsCfg());
    EEBUS_DATA_DELETE(&cfg, &self);
}
