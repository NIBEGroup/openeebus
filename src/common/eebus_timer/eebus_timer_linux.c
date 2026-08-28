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

#if __linux__

#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <time.h>

#include "src/common/api/eebus_timer_interface.h"
#include "src/common/eebus_errors.h"
#include "src/common/eebus_malloc.h"
#include "src/common/eebus_thread/eebus_thread.h"
#include "src/common/eebus_timer/eebus_timer.h"

typedef struct EebusTimer EebusTimer;

struct EebusTimer {
    /** Implements the EEBUS Timer Interface */
    EebusTimerObject obj;

    EebusTimerTimeoutCallback cb;
    void* ctx;
    uint32_t timeout_ms;
    struct timespec timeout;
    bool autoreload;
    bool cancel;
    struct timespec start_time;
    EebusThreadObject* thread;
    int cond_status;
    pthread_cond_t cond;
    int mutex_status;
    pthread_mutex_t mutex;
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

static EebusError EebusTimerConstruct(EebusTimer* self, EebusTimerTimeoutCallback cb, void* ctx);

EebusError EebusTimerConstruct(EebusTimer* self, EebusTimerTimeoutCallback cb, void* ctx) {
    // Override "virtual functions table"
    EEBUS_TIMER_INTERFACE(self) = &eebus_timer_methods;

    self->cb           = cb;
    self->ctx          = ctx;
    self->timeout_ms   = 0;
    self->timeout      = (struct timespec){0};
    self->cancel       = false;
    self->autoreload   = false;
    self->start_time   = (struct timespec){0};
    self->thread       = NULL;
    self->cond_status  = -1;
    self->mutex_status = -1;
    self->timer_state  = kEebusTimerStateIdle;

    self->cond_status = pthread_cond_init(&self->cond, NULL);
    if (self->cond_status != 0) {
        return kEebusErrorInit;
    }

    self->mutex_status = pthread_mutex_init(&self->mutex, NULL);
    if (self->mutex_status != 0) {
        return kEebusErrorInit;
    }

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

    if (eebus_timer->thread != NULL) {
        pthread_mutex_lock(&eebus_timer->mutex);
        eebus_timer->cancel = true;
        pthread_cond_signal(&eebus_timer->cond);
        pthread_mutex_unlock(&eebus_timer->mutex);
        EEBUS_THREAD_JOIN(eebus_timer->thread);
        EebusThreadDelete(eebus_timer->thread);
        eebus_timer->thread = NULL;
    }

    if (eebus_timer->cond_status == 0) {
        pthread_cond_destroy(&eebus_timer->cond);
        eebus_timer->cond_status = -1;
    }

    if (eebus_timer->mutex_status == 0) {
        pthread_mutex_destroy(&eebus_timer->mutex);
        eebus_timer->mutex_status = -1;
    }
}

void UpdateWaitCondTimeout(EebusTimer* eebus_timer) {
    clock_gettime(CLOCK_REALTIME, &eebus_timer->timeout);
    eebus_timer->timeout.tv_sec += TIME_MS_TO_S(eebus_timer->timeout_ms);
    eebus_timer->timeout.tv_nsec += NANOSECONDS(eebus_timer->timeout_ms % 1000);
    if (eebus_timer->timeout.tv_nsec >= 1000000000) {
        eebus_timer->timeout.tv_sec += eebus_timer->timeout.tv_nsec / 1000000000;
        eebus_timer->timeout.tv_nsec %= 1000000000;
    }
}

void* TimerLoop(void* parameters) {
    EebusTimer* const eebus_timer = (EebusTimer*)parameters;

    pthread_mutex_lock(&eebus_timer->mutex);
    while (!eebus_timer->cancel) {
        pthread_cond_timedwait(&eebus_timer->cond, &eebus_timer->mutex, &eebus_timer->timeout);

        if (!eebus_timer->cancel) {
            eebus_timer->cb(eebus_timer->ctx);

            if (eebus_timer->autoreload) {
                UpdateWaitCondTimeout(eebus_timer);
            } else {
                break;
            }
        }

        if (!eebus_timer->autoreload) {
            break;
        }
    }

    if (!eebus_timer->cancel && !eebus_timer->autoreload) {
        eebus_timer->timer_state = kEebusTimerStateExpired;
    }

    pthread_mutex_unlock(&eebus_timer->mutex);
    return NULL;
}

void Start(EebusTimerObject* self, uint32_t timeout_ms, bool autoreload) {
    EebusTimer* const eebus_timer = EEBUS_TIMER(self);

    if (eebus_timer->thread != NULL) {
        return;
    }

    eebus_timer->cancel      = false;
    eebus_timer->timeout_ms  = timeout_ms;
    eebus_timer->timer_state = kEebusTimerStateIdle;
    eebus_timer->autoreload  = autoreload;

    if (timeout_ms == 0) {
        return;
    }

    // Save start time and timeout
    clock_gettime(CLOCK_MONOTONIC, &eebus_timer->start_time);

    UpdateWaitCondTimeout(eebus_timer);

    if ((eebus_timer->timeout.tv_sec == 0) && (eebus_timer->timeout.tv_nsec == 0)) {
        return;
    }

    eebus_timer->thread = EebusThreadCreate(TimerLoop, eebus_timer, 4096);
    if (eebus_timer->thread == NULL) {
        return;
    }

    eebus_timer->timer_state = kEebusTimerStateRunning;
}

void Stop(EebusTimerObject* self) {
    EebusTimer* const eebus_timer = EEBUS_TIMER(self);

    if (eebus_timer->thread == NULL) {
        return;
    }

    pthread_mutex_lock(&eebus_timer->mutex);
    eebus_timer->cancel = true;
    pthread_cond_signal(&eebus_timer->cond);
    pthread_mutex_unlock(&eebus_timer->mutex);

    EEBUS_THREAD_JOIN(eebus_timer->thread);
    EebusThreadDelete(eebus_timer->thread);
    eebus_timer->thread = NULL;

    if (eebus_timer->timer_state != kEebusTimerStateExpired) {
        eebus_timer->timer_state = kEebusTimerStateIdle;
    }
}

uint32_t GetRemainingTime(const EebusTimerObject* self) {
    EebusTimer* const eebus_timer = EEBUS_TIMER(self);

    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);

    int64_t elapsed_sec  = now.tv_sec - eebus_timer->start_time.tv_sec;
    int64_t elapsed_nsec = now.tv_nsec - eebus_timer->start_time.tv_nsec;
    if (elapsed_nsec < 0) {
        elapsed_sec -= 1;
        elapsed_nsec += 1000000000L;
    }

    if (elapsed_sec < 0) {
        return 0;
    }

    const uint64_t elapsed_ms = (uint64_t)elapsed_sec * 1000 + (uint64_t)elapsed_nsec / 1000000;
    if (elapsed_ms >= eebus_timer->timeout_ms) {
        return 0;
    }

    return eebus_timer->timeout_ms - elapsed_ms;
}

EebusTimerState GetTimerState(const EebusTimerObject* self) {
    EebusTimer* const eebus_timer = EEBUS_TIMER(self);
    return eebus_timer->timer_state;
}

#endif  // __linux__
