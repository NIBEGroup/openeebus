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
 * @brief EEBUS Timer Linux implementation
 */

#include <stdbool.h>

#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"

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
    TimerHandle_t timer_handle;
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

static void EebusTimerExpiredCallback(TimerHandle_t xTimer);
static EebusError EebusTimerConstruct(EebusTimer* self, EebusTimerTimeoutCallback cb, void* ctx);

void EebusTimerExpiredCallback(TimerHandle_t xTimer) {
    EebusTimer* const eebus_timer = (EebusTimer*)pvTimerGetTimerID(xTimer);
    eebus_timer->timer_state      = kEebusTimerStateExpired;
    eebus_timer->cb(eebus_timer->ctx);
}

EebusError EebusTimerConstruct(EebusTimer* self, EebusTimerTimeoutCallback cb, void* ctx) {
    // Override "virtual functions table"
    EEBUS_TIMER_INTERFACE(self) = &eebus_timer_methods;

    self->cb           = cb;
    self->ctx          = ctx;
    self->timer_handle = NULL;
    self->timer_state  = kEebusTimerStateIdle;

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
    if (eebus_timer->timer_handle != NULL) {
        xTimerDelete(eebus_timer->timer_handle, 0);
        eebus_timer->timer_handle = NULL;
    }
}

void Start(EebusTimerObject* self, uint32_t timeout_ms, bool autoreload) {
    EebusTimer* const eebus_timer = EEBUS_TIMER(self);

    if (timeout_ms == 0) {
        return;
    }

    BaseType_t autoreload_tmp = autoreload ? pdTRUE : pdFALSE;
    TickType_t timeout_ticks  = pdMS_TO_TICKS(timeout_ms);

    eebus_timer->timer_handle
        = xTimerCreate("EebusTimer", timeout_ticks, autoreload_tmp, (void*)eebus_timer, EebusTimerExpiredCallback);
    if (eebus_timer->timer_handle == NULL) {
        return;
    }

    xTimerStart(eebus_timer->timer_handle, 0);
    eebus_timer->timer_state = kEebusTimerStateRunning;
}

void Stop(EebusTimerObject* self) {
    EebusTimer* const eebus_timer = EEBUS_TIMER(self);

    if (eebus_timer->timer_handle != NULL) {
        xTimerStop(eebus_timer->timer_handle, 0);
        xTimerDelete(eebus_timer->timer_handle, 0);
        eebus_timer->timer_handle = NULL;
    }

    if (eebus_timer->timer_state != kEebusTimerStateExpired) {
        eebus_timer->timer_state = kEebusTimerStateIdle;
    }
}

uint32_t GetRemainingTime(const EebusTimerObject* self) {
    EebusTimer* const eebus_timer = EEBUS_TIMER(self);

    if (eebus_timer->timer_handle != NULL) {
        TickType_t remaining_ticks = xTimerGetExpiryTime(eebus_timer->timer_handle) - xTaskGetTickCount();
        return pdTICKS_TO_MS(remaining_ticks);
    }

    return 0;
}

EebusTimerState GetTimerState(const EebusTimerObject* self) {
    EebusTimer* const eebus_timer = EEBUS_TIMER(self);
    return eebus_timer->timer_state;
}
