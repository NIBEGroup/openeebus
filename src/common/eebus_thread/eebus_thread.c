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
 * @brief Eebus Thread implementation for POSIX (macos, linux, windows)
 */

#include <pthread.h>
#include <stdint.h>
#include <string.h>
#ifndef _WIN32
#include <unistd.h>
#else
#include <windows.h>
#endif

#include "src/common/api/eebus_thread_interface.h"
#include "src/common/eebus_errors.h"
#include "src/common/eebus_malloc.h"
#include "src/common/eebus_thread/eebus_thread.h"

typedef struct EebusThread EebusThread;

struct EebusThread {
    /** Implements the Eebus Thread Interface */
    EebusThreadObject obj;

    pthread_t thread;
    int thread_status;
};

#define EEBUS_THREAD(obj) ((EebusThread*)(obj))

static void Destruct(EebusThreadObject* self);
static void Join(EebusThreadObject* self);

static const EebusThreadInterface eebus_thread_methods = {
    .destruct = Destruct,
    .join     = Join,
};

static EebusError
EebusThreadConstruct(EebusThread* self, EebusThreadRoutine routine, void* parameters, size_t stack_size);

EebusError EebusThreadConstruct(EebusThread* self, EebusThreadRoutine routine, void* parameters, size_t stack_size) {
    // Override "virtual functions table"
    EEBUS_THREAD_INTERFACE(self) = &eebus_thread_methods;

    // Stack size is used on FreeRTOS only
    (void)stack_size;

    memset(&self->thread, 0, sizeof(self->thread));
    self->thread_status = -1;

    if (routine == NULL) {
        return kEebusErrorInputArgumentNull;
    }

    // Create thread
    pthread_attr_t attrs;

    pthread_attr_init(&attrs);
    int32_t ret = pthread_attr_setdetachstate(&attrs, PTHREAD_CREATE_JOINABLE);
    if (ret != 0) {
        pthread_attr_destroy(&attrs);
        return kEebusErrorThread;
    }

    // Create thread
    self->thread_status = pthread_create(&self->thread, &attrs, routine, parameters);
    pthread_attr_destroy(&attrs);
    if (self->thread_status != 0) {
        return kEebusErrorThread;
    }

    return kEebusErrorOk;
}

EebusThreadObject* EebusThreadCreate(EebusThreadRoutine routine, void* parameters, size_t stack_size) {
    EebusThread* const eebus_thread = (EebusThread*)EEBUS_MALLOC(sizeof(EebusThread));
    if (eebus_thread == NULL) {
        return NULL;
    }

    const EebusError err = EebusThreadConstruct(eebus_thread, routine, parameters, stack_size);
    if (err != kEebusErrorOk) {
        EebusThreadDelete(EEBUS_THREAD_OBJECT(eebus_thread));
        return NULL;
    }

    return EEBUS_THREAD_OBJECT(eebus_thread);
}

void Destruct(EebusThreadObject* self) {
    EebusThread* const eebus_thread = EEBUS_THREAD(self);
    // Note: make sure to call EebusThread::Join() before destructing
    eebus_thread->thread_status = -1;
}

void Join(EebusThreadObject* self) {
    EebusThread* const eebus_thread = EEBUS_THREAD(self);

    if (eebus_thread->thread_status != 0) {
        return;  // Thread is not running
    }

    pthread_join(eebus_thread->thread, NULL);
}

void EebusThreadSleep(int32_t seconds) {
#ifndef _WIN32
    sleep(seconds);
#else
    Sleep(seconds * 1000);
#endif  // _WIN32
}

void EebusThreadUsleep(int32_t useconds) {
#ifndef _WIN32
    usleep(useconds);
#else
    Sleep(useconds / 1000);
#endif  // _WIN32
}
