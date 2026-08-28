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
 * @brief FreeRTOS Eebus Queue implementation. Internal FreeRTOS queue is created as "static"
 * just to be able to use the custom buffer allocation. Note that EEBUS_MALLOC() can
 * use the external RAM on some specific platforms what provides more flexible memory usage.
 */

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <stdbool.h>
#include <stdint.h>

#include "src/common/api/eebus_queue_interface.h"
#include "src/common/eebus_errors.h"
#include "src/common/eebus_malloc.h"
#include "src/common/eebus_queue/eebus_queue.h"

typedef bool (*Predicate)(void* ctx);

typedef struct EebusQueue EebusQueue;

struct EebusQueue {
    /** Implements the Eebus Queue Interface */
    EebusQueueObject obj;

    /** Queue message size */
    size_t msg_size;
    /** Queue buffer */
    uint8_t* queue_buf;
    /** Static queue buffer */
    StaticQueue_t static_queue;
    /** Queue message deallocator (used to clear pending messages when deallocating queue) */
    QueueMsgDeallocator msg_deallocator;
    /** FreeRTOS queue handle */
    QueueHandle_t queue_handle;
};

#define EEBUS_QUEUE(obj) ((EebusQueue*)(obj))

static void Destruct(EebusQueueObject* self);
static EebusError Send(EebusQueueObject* self, const void* msg, uint32_t timeout_ms);
static EebusError Receive(EebusQueueObject* self, void* msg, uint32_t timeout_ms);
static bool IsEmpty(const EebusQueueObject* self);
static bool IsFull(const EebusQueueObject* self);
static void Clear(EebusQueueObject* self);

static const EebusQueueInterface eebus_queue_methods = {
    .destruct = Destruct,
    .send     = Send,
    .receive  = Receive,
    .is_empty = IsEmpty,
    .is_full  = IsFull,
    .clear    = Clear,
};

static EebusError
EebusQueueConstruct(EebusQueue* self, size_t max_msg, size_t msg_size, QueueMsgDeallocator msg_deallocator);

EebusError EebusQueueConstruct(EebusQueue* self, size_t max_msg, size_t msg_size, QueueMsgDeallocator msg_deallocator) {
    // Override "virtual functions table"
    EEBUS_QUEUE_INTERFACE(self) = &eebus_queue_methods;

    self->msg_size        = msg_size;
    self->msg_deallocator = msg_deallocator;
    self->queue_buf       = NULL;
    self->queue_handle    = NULL;

    self->queue_buf = (uint8_t*)EEBUS_MALLOC(max_msg * msg_size);
    if (self->queue_buf == NULL) {
        return kEebusErrorMemoryAllocate;
    }

    self->queue_handle = xQueueCreateStatic(max_msg, msg_size, self->queue_buf, &self->static_queue);
    if (self->queue_handle == NULL) {
        return kEebusErrorMemoryAllocate;
    }

    return kEebusErrorOk;
}

EebusQueueObject* EebusQueueCreate(size_t max_msg, size_t msg_size, QueueMsgDeallocator msg_deallocator) {
    EebusQueue* const eebus_queue = (EebusQueue*)EEBUS_MALLOC(sizeof(EebusQueue));

    const EebusError err = EebusQueueConstruct(eebus_queue, max_msg, msg_size, msg_deallocator);
    if (err != kEebusErrorOk) {
        EebusQueueDelete(EEBUS_QUEUE_OBJECT(eebus_queue));
        return NULL;
    }

    return EEBUS_QUEUE_OBJECT(eebus_queue);
}

void Destruct(EebusQueueObject* self) {
    EebusQueue* const queue = EEBUS_QUEUE(self);

    if (queue->queue_handle != NULL) {
        Clear(self);
        vQueueDelete(queue->queue_handle);
        queue->queue_handle = NULL;
    }

    if (queue->queue_buf != NULL) {
        EEBUS_FREE(queue->queue_buf);
        queue->queue_buf = NULL;
    }
}

TickType_t TimeoutTicks(uint32_t timeout_ms) {
    if (timeout_ms == kTimeoutInfinite) {
        return portMAX_DELAY;
    }

    return pdMS_TO_TICKS(timeout_ms);
}

EebusError Send(EebusQueueObject* self, const void* msg, uint32_t timeout_ms) {
    EebusQueue* const queue = EEBUS_QUEUE(self);

    EebusError err = kEebusErrorNoChange;

    if (queue->queue_handle != NULL) {
        if (xQueueSend(queue->queue_handle, msg, TimeoutTicks(timeout_ms)) == pdTRUE) {
            err = kEebusErrorOk;
        } else {
            err = kEebusErrorTime;
        }
    }

    return err;
}

EebusError Receive(EebusQueueObject* self, void* msg, uint32_t timeout_ms) {
    EebusQueue* const queue = EEBUS_QUEUE(self);

    if (queue->queue_handle == NULL) {
        return kEebusErrorNoChange;
    }

    if (xQueueReceive(queue->queue_handle, msg, TimeoutTicks(timeout_ms)) == pdTRUE) {
        return kEebusErrorOk;
    } else {
        return kEebusErrorTime;
    }
}

bool IsEmpty(const EebusQueueObject* self) {
    const EebusQueue* const queue = EEBUS_QUEUE(self);

    if (queue->queue_handle == NULL) {
        return true;
    }

    return (uxQueueMessagesWaiting(queue->queue_handle) == 0);
}

bool IsFull(const EebusQueueObject* self) {
    const EebusQueue* const queue = EEBUS_QUEUE(self);

    if (queue->queue_handle == NULL) {
        return true;
    }

    return (uxQueueSpacesAvailable(queue->queue_handle) == 0);
}

void Clear(EebusQueueObject* self) {
    EebusQueue* const queue = EEBUS_QUEUE(self);

    if ((queue->queue_handle == NULL) || (queue->msg_deallocator == NULL)) {
        return;
    }

    uint8_t msg[queue->msg_size] = {};

    while (!IsEmpty(self)) {
        Receive(self, &msg, 0);
        queue->msg_deallocator(msg);
    }
}
