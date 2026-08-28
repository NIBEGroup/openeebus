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
 * @brief Eebus Queue implementation
 */

#include <errno.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <sys/timeb.h>
#include <time.h>
#else
#include <time.h>
#endif

#include "src/common/api/eebus_queue_interface.h"
#include "src/common/eebus_errors.h"
#include "src/common/eebus_malloc.h"
#include "src/common/eebus_queue/eebus_queue.h"

typedef bool (*Predicate)(void* ctx);

typedef struct EebusQueue EebusQueue;

struct EebusQueue {
    /** Implements the Eebus Queue Interface */
    EebusQueueObject obj;

    /** Maximal number of messages in msg_buf */
    size_t max_msg;
    /** Queue message size */
    size_t msg_size;
    /** Queue message deallocator (used to clear pending messages when deallocating queue) */
    QueueMsgDeallocator msg_deallocator;
    /** Cyclic buffer containing up to msg_buf_size messages msg_size each */
    uint8_t* msg_buf;
    /** Message buffer "head" pointer */
    uint8_t* head;
    /** Message buffer "tail" pointer */
    uint8_t* tail;
    /** Queue is closed for any send/receive operations */
    bool is_closed;
    /** Queue is full flag */
    bool is_full;
    /** Queue is empty flag */
    bool is_empty;
    /** Mutex initialisation return value */
    int mutex_init_ret;
    /** Condition variable for not empty state initialisation return value */
    int is_not_empty_cond_init_ret;
    /** Condition variable for not full state initialisation return value */
    int is_not_full_cond_init_ret;
    /** Mutex for thread safety */
    pthread_mutex_t mutex;
    /** Condition variable for not empty state */
    pthread_cond_t is_not_empty_cond;
    /** Condition variable for not full state */
    pthread_cond_t is_not_full_cond;
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
static void ClearInternal(EebusQueue* self);
static struct timespec MsToTimespec(uint32_t timeout_ms);
static int
WaitCond(EebusQueue* self, pthread_cond_t* cond, pthread_mutex_t* mutex, uint32_t timeout_ms, Predicate predicate);
static bool SendPredicate(void* ctx);
static EebusError SendInternal(EebusQueue* self, const void* msg, uint32_t timeout_ms);
static bool ReceivePredicate(void* ctx);
static EebusError ReceiveInternal(EebusQueue* self, void* msg, uint32_t timeout_ms);

EebusError EebusQueueConstruct(EebusQueue* self, size_t max_msg, size_t msg_size, QueueMsgDeallocator msg_deallocator) {
    // Override "virtual functions table"
    EEBUS_QUEUE_INTERFACE(self) = &eebus_queue_methods;

    self->msg_size        = msg_size;
    self->max_msg         = max_msg;
    self->msg_deallocator = msg_deallocator;
    self->msg_buf         = NULL;
    self->head            = NULL;
    self->tail            = NULL;
    self->is_closed       = false;
    self->is_full         = false;
    self->is_empty        = true;

    self->mutex_init_ret             = -1;
    self->is_not_empty_cond_init_ret = -1;
    self->is_not_full_cond_init_ret  = -1;

    if ((max_msg == 0) || (msg_size == 0) || (max_msg > SIZE_MAX / msg_size)) {
        return kEebusErrorMemoryAllocate;
    }

    self->msg_buf = (uint8_t*)EEBUS_MALLOC(max_msg * msg_size);
    if (self->msg_buf == NULL) {
        return kEebusErrorMemoryAllocate;
    }

    self->head = self->msg_buf;
    self->tail = self->msg_buf;

    self->mutex_init_ret = pthread_mutex_init(&self->mutex, NULL);
    if (self->mutex_init_ret != 0) {
        return kEebusErrorInit;
    }

    self->is_not_empty_cond_init_ret = pthread_cond_init(&self->is_not_empty_cond, NULL);
    if (self->is_not_empty_cond_init_ret != 0) {
        return kEebusErrorInit;
    }

    self->is_not_full_cond_init_ret = pthread_cond_init(&self->is_not_full_cond, NULL);
    if (self->is_not_full_cond_init_ret != 0) {
        return kEebusErrorInit;
    }

    return kEebusErrorOk;
}

EebusQueueObject* EebusQueueCreate(size_t max_msg, size_t msg_size, QueueMsgDeallocator msg_deallocator) {
    EebusQueue* const eebus_queue = (EebusQueue*)EEBUS_MALLOC(sizeof(EebusQueue));
    if (eebus_queue == NULL) {
        return NULL;
    }

    const EebusError err = EebusQueueConstruct(eebus_queue, max_msg, msg_size, msg_deallocator);
    if (err != kEebusErrorOk) {
        EebusQueueDelete(EEBUS_QUEUE_OBJECT(eebus_queue));
        return NULL;
    }

    return EEBUS_QUEUE_OBJECT(eebus_queue);
}

void Destruct(EebusQueueObject* self) {
    EebusQueue* const queue = EEBUS_QUEUE(self);

    if ((queue->is_not_empty_cond_init_ret == 0) && (queue->is_not_full_cond_init_ret == 0)
        && (queue->mutex_init_ret == 0)) {
        pthread_mutex_lock(&queue->mutex);
        queue->is_closed = true;

        pthread_cond_signal(&queue->is_not_full_cond);
        pthread_cond_signal(&queue->is_not_empty_cond);

        ClearInternal(queue);

        pthread_mutex_unlock(&queue->mutex);
    }

    if (queue->is_not_empty_cond_init_ret == 0) {
        pthread_cond_destroy(&queue->is_not_empty_cond);
    }

    if (queue->is_not_full_cond_init_ret == 0) {
        pthread_cond_destroy(&queue->is_not_full_cond);
    }

    if (queue->mutex_init_ret == 0) {
        pthread_mutex_destroy(&queue->mutex);
    }

    EEBUS_FREE(queue->msg_buf);
}

struct timespec MsToTimespec(uint32_t timeout_ms) {
    struct timespec ts;
#ifdef _WIN32
    // Windows doesn't have clock_gettime, use time() + conversion
    time_t now = time(NULL);
    ts.tv_sec  = now + timeout_ms / 1000;
    ts.tv_nsec = (timeout_ms % 1000) * 1000000;
#else
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += timeout_ms / 1000;
    ts.tv_nsec += (timeout_ms % 1000) * 1000000;
    if (ts.tv_nsec >= 1000000000) {
        ts.tv_sec += ts.tv_nsec / 1000000000;
        ts.tv_nsec %= 1000000000;
    }
#endif
    return ts;
}

int WaitCond(EebusQueue* self, pthread_cond_t* cond, pthread_mutex_t* mutex, uint32_t timeout_ms, Predicate predicate) {
    struct timespec ts = {0};

    if (timeout_ms != kTimeoutInfinite) {
        ts = MsToTimespec(timeout_ms);
    }

    while (!predicate(self)) {
        if (timeout_ms == kTimeoutInfinite) {
            pthread_cond_wait(cond, mutex);
        } else {
            if (pthread_cond_timedwait(cond, mutex, &ts) == ETIMEDOUT) {
                return -1;
            }
        }
    }

    return 0;
}

bool SendPredicate(void* ctx) {
    const EebusQueue* const queue = (EebusQueue*)ctx;
    return (!queue->is_full) || (queue->is_closed);
}

EebusError SendInternal(EebusQueue* self, const void* msg, uint32_t timeout_ms) {
    if (WaitCond(self, &self->is_not_full_cond, &self->mutex, timeout_ms, SendPredicate) != 0) {
        return kEebusErrorTime;
    }

    if (self->is_closed) {
        return kEebusErrorNoChange;
    }

    memcpy(self->head, msg, self->msg_size);
    self->head += self->msg_size;
    if (self->head >= self->msg_buf + self->msg_size * self->max_msg) {
        self->head = self->msg_buf;
    }

    self->is_empty = false;
    self->is_full  = (self->head == self->tail);

    pthread_cond_signal(&self->is_not_empty_cond);

    return kEebusErrorOk;
}

EebusError Send(EebusQueueObject* self, const void* msg, uint32_t timeout_ms) {
    EebusQueue* const queue = EEBUS_QUEUE(self);

    EebusError err = kEebusErrorNoChange;

    pthread_mutex_lock(&queue->mutex);
    if (!queue->is_closed) {
        err = SendInternal(queue, msg, timeout_ms);
    }

    pthread_mutex_unlock(&queue->mutex);

    return err;
}

bool ReceivePredicate(void* ctx) {
    const EebusQueue* const queue = (EebusQueue*)ctx;
    return (!queue->is_empty) || (queue->is_closed);
}

EebusError ReceiveInternal(EebusQueue* self, void* msg, uint32_t timeout_ms) {
    if (WaitCond(self, &self->is_not_empty_cond, &self->mutex, timeout_ms, ReceivePredicate) != 0) {
        return kEebusErrorTime;
    }

    if (self->is_closed) {
        return kEebusErrorNoChange;
    }

    memcpy(msg, self->tail, self->msg_size);
    self->tail += self->msg_size;
    if (self->tail >= self->msg_buf + self->msg_size * self->max_msg) {
        self->tail = self->msg_buf;
    }

    self->is_full  = false;
    self->is_empty = (self->head == self->tail);

    pthread_cond_signal(&self->is_not_full_cond);

    return kEebusErrorOk;
}

EebusError Receive(EebusQueueObject* self, void* msg, uint32_t timeout_ms) {
    EebusQueue* const queue = EEBUS_QUEUE(self);

    EebusError err = kEebusErrorNoChange;

    pthread_mutex_lock(&queue->mutex);
    if (!queue->is_closed) {
        err = ReceiveInternal(queue, msg, timeout_ms);
    }

    pthread_mutex_unlock(&queue->mutex);
    return err;
}

bool IsEmpty(const EebusQueueObject* self) {
    return EEBUS_QUEUE(self)->is_empty;
}

bool IsFull(const EebusQueueObject* self) {
    return EEBUS_QUEUE(self)->is_full;
}

void ClearInternal(EebusQueue* self) {
    while (self->tail != self->head) {
        if (self->msg_deallocator != NULL) {
            self->msg_deallocator(self->tail);
        }

        self->tail += self->msg_size;
        if (self->tail >= self->msg_buf + self->msg_size * self->max_msg) {
            self->tail = self->msg_buf;
        }
    }

    self->is_empty = true;
    self->is_full  = false;
    pthread_cond_signal(&self->is_not_full_cond);
}

void Clear(EebusQueueObject* self) {
    EebusQueue* const queue = EEBUS_QUEUE(self);

    pthread_mutex_lock(&queue->mutex);
    ClearInternal(queue);
    pthread_mutex_unlock(&queue->mutex);
}
