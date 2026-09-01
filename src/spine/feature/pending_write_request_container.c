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
 * @brief Pending Write Request Container implementation
 */

#include "src/spine/feature/pending_write_request_container.h"

#include <string.h>

#include "src/common/eebus_malloc.h"
#include "src/common/vector.h"
#include "src/spine/api/feature_interface.h"
#include "src/spine/feature/pending_write_request.h"
#include "src/spine/model/result_types.h"

typedef struct PendingWriteRequestContainer PendingWriteRequestContainer;

struct PendingWriteRequestContainer {
    /** Implements the Pending Write Request Container Interface */
    PendingWriteRequestContainerObject obj;

    Vector items;
};

#define PENDING_WRITE_REQUEST_CONTAINER(obj) ((PendingWriteRequestContainer*)(obj))

static void Destruct(PendingWriteRequestContainerObject* self);
static EebusError Add(PendingWriteRequestContainerObject* self, const Message* msg);
static void Remove(PendingWriteRequestContainerObject* self, PendingWriteRequestObject* item);
static PendingWriteRequestObject*
Find(PendingWriteRequestContainerObject* self, const char* ski, MsgCounterType msg_cnt);
static size_t GetSize(const PendingWriteRequestContainerObject* self);
static void Tick(PendingWriteRequestContainerObject* self, FeatureLocalObject* fl);

static const PendingWriteRequestContainerInterface pending_write_request_container_methods = {
    .destruct = Destruct,
    .add      = Add,
    .remove   = Remove,
    .find     = Find,
    .get_size = GetSize,
    .tick     = Tick,
};

static void DeletePendingWriteRequest(void* item) {
    PendingWriteRequestDelete((PendingWriteRequestObject*)item);
}

static void PendingWriteRequestContainerConstruct(PendingWriteRequestContainer* self);

void PendingWriteRequestContainerConstruct(PendingWriteRequestContainer* self) {
    PENDING_WRITE_REQUEST_CONTAINER_INTERFACE(self) = &pending_write_request_container_methods;

    VectorConstructWithDeallocator(&self->items, DeletePendingWriteRequest);
}

PendingWriteRequestContainerObject* PendingWriteRequestContainerCreate(void) {
    PendingWriteRequestContainer* const pwrc
        = (PendingWriteRequestContainer*)EEBUS_MALLOC(sizeof(PendingWriteRequestContainer));
    if (pwrc == NULL) {
        return NULL;
    }

    PendingWriteRequestContainerConstruct(pwrc);
    return PENDING_WRITE_REQUEST_CONTAINER_OBJECT(pwrc);
}

void Destruct(PendingWriteRequestContainerObject* self) {
    PendingWriteRequestContainer* const pwrc = PENDING_WRITE_REQUEST_CONTAINER(self);
    VectorFreeElements(&pwrc->items);
    VectorDestruct(&pwrc->items);
}

EebusError Add(PendingWriteRequestContainerObject* self, const Message* msg) {
    PendingWriteRequestContainer* const pwrc = PENDING_WRITE_REQUEST_CONTAINER(self);

    if ((msg == NULL) || (msg->device_remote == NULL) || (msg->request_header == NULL)
        || (msg->request_header->msg_cnt == NULL)) {
        return kEebusErrorInputArgumentNull;
    }

    const char* const ski        = DEVICE_REMOTE_GET_SKI(msg->device_remote);
    const MsgCounterType msg_cnt = *msg->request_header->msg_cnt;

    if (Find(self, ski, msg_cnt) != NULL) {
        return kEebusErrorNoChange;
    }

    PendingWriteRequestObject* const pending = PendingWriteRequestCreate(msg);
    if (pending == NULL) {
        return kEebusErrorMemoryAllocate;
    }

    VectorPushBack(&pwrc->items, pending);
    return kEebusErrorOk;
}

void Remove(PendingWriteRequestContainerObject* self, PendingWriteRequestObject* item) {
    PendingWriteRequestContainer* const pwrc = PENDING_WRITE_REQUEST_CONTAINER(self);

    VectorRemove(&pwrc->items, item);
    PendingWriteRequestDelete(item);
}

PendingWriteRequestObject* Find(PendingWriteRequestContainerObject* self, const char* ski, MsgCounterType msg_cnt) {
    if (ski == NULL) {
        return NULL;
    }

    PendingWriteRequestContainer* const pwrc = PENDING_WRITE_REQUEST_CONTAINER(self);
    for (size_t i = 0; i < VectorGetSize(&pwrc->items); ++i) {
        PendingWriteRequestObject* const item = (PendingWriteRequestObject*)VectorGetElement(&pwrc->items, i);
        if ((strcmp(PENDING_WRITE_REQUEST_GET_SKI(item), ski) == 0)
            && (PENDING_WRITE_REQUEST_GET_MESSAGE_COUNTER(item) == msg_cnt)) {
            return item;
        }
    }

    return NULL;
}

size_t GetSize(const PendingWriteRequestContainerObject* self) {
    return VectorGetSize(&PENDING_WRITE_REQUEST_CONTAINER(self)->items);
}

void Tick(PendingWriteRequestContainerObject* self, FeatureLocalObject* fl) {
    PendingWriteRequestContainer* const pwrc = PENDING_WRITE_REQUEST_CONTAINER(self);

    size_t i = VectorGetSize(&pwrc->items);

    while (i > 0) {
        --i;
        PendingWriteRequestObject* const item = (PendingWriteRequestObject*)VectorGetElement(&pwrc->items, i);

        if (PENDING_WRITE_REQUEST_HAS_EXPIRED(item)) {
            Message msg;
            const EebusError status = PENDING_WRITE_REQUEST_GET_MESSAGE(item, fl, &msg);
            if (status == kEebusErrorOk) {
                const ErrorType err = {
                    .error_number = kErrorNumberTypeTimeout,
                    .description  = "Write request timed out",
                };

                SEND_RESULT_ERROR(
                    MessageGetSender(&msg),
                    msg.request_header,
                    FEATURE_GET_ADDRESS(FEATURE_OBJECT(fl)),
                    &err
                );
            }

            VectorRemove(&pwrc->items, item);
            PendingWriteRequestDelete(item);
        } else {
            PENDING_WRITE_REQUEST_UPDATE_REMAINING_TIME(item);
        }
    }
}
