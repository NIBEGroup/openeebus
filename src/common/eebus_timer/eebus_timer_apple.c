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
 * @brief EEBUS Timer Apple implementation
 */

#if __APPLE__

#include <dispatch/dispatch.h>
#include <stdbool.h>
#include <time.h>

#include "src/common/api/eebus_timer_interface.h"
#include "src/common/eebus_errors.h"
#include "src/common/eebus_malloc.h"
#include "src/common/eebus_timer/eebus_timer.h"

typedef struct EebusTimer EebusTimer;

struct EebusTimer {
    /** Implements the EEBUS Timer Interface */
    EebusTimerObject obj;

    EebusTimerTimeoutCallback cb;
    void* ctx;
    dispatch_queue_t queue;
    dispatch_source_t timer_source;
    struct timespec start_time;
    uint32_t timeout_ms;
    EebusTimerState timer_state;
};

#define EEBUS_TIMER(obj) ((EebusTimer*)(obj))

static void Destruct(EebusTimerObject* self);
static void Start(EebusTimerObject* self, uint32_t timeout_ms, bool autoreload);
static void Stop(EebusTimerObject* self);
static uint32_t GetRemainingTime(const EebusTimerObject* self);
static EebusTimerState GetTimerState(const EebusTimerObject* self);

static const EebusTimerInterface eebus_timer_methods = {
    .destruct           = Destruct,
    .start              = Start,
    .stop               = Stop,
    .get_remaining_time = GetRemainingTime,
    .get_timer_state    = GetTimerState,
};

static void EebusTimerExpiredCallback(void* context);
static EebusError EebusTimerConstruct(EebusTimer* self, EebusTimerTimeoutCallback cb, void* ctx);

void EebusTimerExpiredCallback(void* context) {
    EebusTimer* const eebus_timer = (EebusTimer*)context;
    eebus_timer->timer_state      = kEebusTimerStateExpired;
    eebus_timer->cb(eebus_timer->ctx);
}

EebusError EebusTimerConstruct(EebusTimer* self, EebusTimerTimeoutCallback cb, void* ctx) {
    // Override "virtual functions table"
    EEBUS_TIMER_INTERFACE(self) = &eebus_timer_methods;

    self->cb           = cb;
    self->ctx          = ctx;
    self->queue        = NULL;
    self->timer_source = NULL;
    self->timer_state  = kEebusTimerStateIdle;

    self->queue = dispatch_queue_create("eebus.timer.queue", DISPATCH_QUEUE_SERIAL);
    if (self->queue == NULL) {
        return kEebusErrorInit;
    }

    self->timer_source = dispatch_source_create(DISPATCH_SOURCE_TYPE_TIMER, 0, 0, self->queue);
    if (self->timer_source == NULL) {
        return kEebusErrorInit;
    }

    dispatch_set_context(self->timer_source, self);
    dispatch_source_set_event_handler_f(self->timer_source, EebusTimerExpiredCallback);
    dispatch_resume(self->timer_source);

    return kEebusErrorOk;
}

EebusTimerObject* EebusTimerCreate(EebusTimerTimeoutCallback cb, void* ctx) {
    EebusTimer* const eebus_timer = (EebusTimer*)EEBUS_MALLOC(sizeof(EebusTimer));
    if (eebus_timer == NULL) {
        return NULL;
    }

    const EebusError err = EebusTimerConstruct(eebus_timer, cb, ctx);
    if (err != kEebusErrorOk) {
        EebusTimerDelete(EEBUS_TIMER_OBJECT(eebus_timer));
        return NULL;
    }

    return EEBUS_TIMER_OBJECT(eebus_timer);
}

void Destruct(EebusTimerObject* self) {
    EebusTimer* const eebus_timer = EEBUS_TIMER(self);

    if (eebus_timer->timer_source != NULL) {
        dispatch_release(eebus_timer->timer_source);
        eebus_timer->timer_source = NULL;
    }

    if (eebus_timer->queue != NULL) {
        dispatch_release(eebus_timer->queue);
        eebus_timer->queue = NULL;
    }
}

void Start(EebusTimerObject* self, uint32_t timeout_ms, bool autoreload) {
    EebusTimer* const eebus_timer = EEBUS_TIMER(self);

    if (eebus_timer->timer_source == NULL) {
        return;
    }

    if (timeout_ms == 0) {
        return;
    }

    // Save start time and timeout
    clock_gettime(CLOCK_MONOTONIC, &eebus_timer->start_time);
    eebus_timer->timeout_ms = timeout_ms;

    const uint64_t timeout_ns = NANOSECONDS((uint64_t)timeout_ms);
    dispatch_time_t timeout   = dispatch_time(DISPATCH_TIME_NOW, timeout_ns);
    const uint64_t interval   = autoreload ? timeout_ns : DISPATCH_TIME_FOREVER;

    dispatch_source_set_timer(eebus_timer->timer_source, timeout, interval, 0);

    eebus_timer->timer_state = kEebusTimerStateRunning;
}

void Stop(EebusTimerObject* self) {
    EebusTimer* const eebus_timer = EEBUS_TIMER(self);

    if (eebus_timer->timer_source == NULL) {
        return;
    }

    dispatch_source_cancel(eebus_timer->timer_source);

    if (eebus_timer->timer_state != kEebusTimerStateExpired) {
        eebus_timer->timer_state = kEebusTimerStateIdle;
    }
}

uint32_t GetRemainingTime(const EebusTimerObject* self) {
    EebusTimer* const eebus_timer = EEBUS_TIMER(self);

    if (eebus_timer->timer_source == NULL || eebus_timer->timer_state != kEebusTimerStateRunning) {
        return 0;
    }

    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);

    uint64_t elapsed_ms = (now.tv_sec - eebus_timer->start_time.tv_sec) * 1000
                          + (now.tv_nsec - eebus_timer->start_time.tv_nsec) / 1000000;

    if (elapsed_ms >= eebus_timer->timeout_ms) {
        return 0;
    }

    return eebus_timer->timeout_ms - elapsed_ms;
}

EebusTimerState GetTimerState(const EebusTimerObject* self) {
    EebusTimer* const eebus_timer = EEBUS_TIMER(self);
    return eebus_timer->timer_state;
}

#endif  // __APPLE__
