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
 * @brief EEBUS Timer Windows implementation
 */

#if _WIN32

#include <stdbool.h>
#include <windows.h>

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
    HANDLE timer_handle;
    DWORD timeout_ms;
    bool autoreload;
    EebusTimerState timer_state;
    ULONGLONG start_tick;  // Windows tick count when timer started
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

static void EebusTimerExpiredCallback(PVOID lpParameter, BOOLEAN TimerOrWaitFired);
static EebusError EebusTimerConstruct(EebusTimer* self, EebusTimerTimeoutCallback cb, void* ctx);

void EebusTimerExpiredCallback(PVOID lpParameter, BOOLEAN TimerOrWaitFired) {
    (void)TimerOrWaitFired;  // Unused parameter
    EebusTimer* const eebus_timer = (EebusTimer*)lpParameter;
    eebus_timer->timer_state      = kEebusTimerStateExpired;
    eebus_timer->cb(eebus_timer->ctx);
}

EebusError EebusTimerConstruct(EebusTimer* self, EebusTimerTimeoutCallback cb, void* ctx) {
    // Override "virtual functions table"
    EEBUS_TIMER_INTERFACE(self) = &eebus_timer_methods;

    self->cb           = cb;
    self->ctx          = ctx;
    self->timer_handle = NULL;
    self->timeout_ms   = 0;
    self->autoreload   = false;
    self->timer_state  = kEebusTimerStateIdle;
    self->start_tick   = 0;

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
        DeleteTimerQueueTimer(NULL, eebus_timer->timer_handle, NULL);
        eebus_timer->timer_handle = NULL;
    }
}

void Start(EebusTimerObject* self, uint32_t timeout_ms, bool autoreload) {
    EebusTimer* const eebus_timer = EEBUS_TIMER(self);

    if (timeout_ms == 0) {
        return;
    }

    // Stop any existing timer
    if (eebus_timer->timer_handle != NULL) {
        DeleteTimerQueueTimer(NULL, eebus_timer->timer_handle, NULL);
        eebus_timer->timer_handle = NULL;
    }

    eebus_timer->start_tick = GetTickCount64();
    eebus_timer->timeout_ms = timeout_ms;
    eebus_timer->autoreload = autoreload;

    DWORD period = autoreload ? timeout_ms : 0;  // 0 means single-shot

    BOOL success = CreateTimerQueueTimer(
        &eebus_timer->timer_handle,
        NULL,  // Use default timer queue
        EebusTimerExpiredCallback,
        eebus_timer,
        timeout_ms,  // Due time
        period,      // Period (0 for single-shot)
        0            // Flags
    );

    if (success) {
        eebus_timer->timer_state = kEebusTimerStateRunning;
    }
}

void Stop(EebusTimerObject* self) {
    EebusTimer* const eebus_timer = EEBUS_TIMER(self);

    if (eebus_timer->timer_handle != NULL) {
        DeleteTimerQueueTimer(NULL, eebus_timer->timer_handle, NULL);
        eebus_timer->timer_handle = NULL;
    }

    if (eebus_timer->timer_state != kEebusTimerStateExpired) {
        eebus_timer->timer_state = kEebusTimerStateIdle;
    }
}

uint32_t GetRemainingTime(const EebusTimerObject* self) {
    EebusTimer* const eebus_timer = EEBUS_TIMER(self);

    if ((eebus_timer->timer_state != kEebusTimerStateRunning) || (eebus_timer->timer_handle == NULL)) {
        return 0;
    }

    ULONGLONG current_tick = GetTickCount64();
    ULONGLONG elapsed_ms   = current_tick - eebus_timer->start_tick;

    if (elapsed_ms >= eebus_timer->timeout_ms) {
        return 0;  // Timer should have expired
    }

    return (uint32_t)(eebus_timer->timeout_ms - elapsed_ms);
}

EebusTimerState GetTimerState(const EebusTimerObject* self) {
    EebusTimer* const eebus_timer = EEBUS_TIMER(self);
    return eebus_timer->timer_state;
}

#endif  // _WIN32
