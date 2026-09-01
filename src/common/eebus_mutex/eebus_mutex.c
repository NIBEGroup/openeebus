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
 * @brief POSIX Eebus Mutex implementation
 */

#include <pthread.h>
#include <stdbool.h>

#include "src/common/api/eebus_mutex_interface.h"
#include "src/common/eebus_errors.h"
#include "src/common/eebus_malloc.h"
#include "src/common/eebus_mutex/eebus_mutex.h"

typedef struct EebusMutex EebusMutex;

struct EebusMutex {
    /** Implements the Eebus Mutex Interface */
    EebusMutexObject obj;

    pthread_mutex_t mutex;
    int mutex_init_ret;
};

#define EEBUS_MUTEX(obj) ((EebusMutex*)(obj))

static void Destruct(EebusMutexObject* self);
static void Lock(EebusMutexObject* self);
static void Unlock(EebusMutexObject* self);

static const EebusMutexInterface eebus_mutex_methods = {
    .destruct = Destruct,
    .lock     = Lock,
    .unlock   = Unlock,
};

static EebusError EebusMutexConstruct(EebusMutex* self);
static EebusError EebusMutexConstructRecursive(EebusMutex* self);

EebusError EebusMutexConstruct(EebusMutex* self) {
    // Override "virtual functions table"
    EEBUS_MUTEX_INTERFACE(self) = &eebus_mutex_methods;

    self->mutex_init_ret = pthread_mutex_init(&self->mutex, NULL);
    if (self->mutex_init_ret != 0) {
        return kEebusErrorInit;
    }

    return kEebusErrorOk;
}

EebusError EebusMutexConstructRecursive(EebusMutex* self) {
    // Override "virtual functions table"
    EEBUS_MUTEX_INTERFACE(self) = &eebus_mutex_methods;

    self->mutex_init_ret = -1;

    pthread_mutexattr_t attr;
    if (pthread_mutexattr_init(&attr) != 0) {
        return kEebusErrorInit;
    }

    if (pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE) != 0) {
        pthread_mutexattr_destroy(&attr);
        return kEebusErrorInit;
    }

    self->mutex_init_ret = pthread_mutex_init(&self->mutex, &attr);
    pthread_mutexattr_destroy(&attr);

    if (self->mutex_init_ret != 0) {
        return kEebusErrorInit;
    }

    return kEebusErrorOk;
}

EebusMutexObject* EebusMutexCreateInternal(bool is_recursive) {
    EebusMutex* const eebus_mutex = (EebusMutex*)EEBUS_MALLOC(sizeof(EebusMutex));
    if (eebus_mutex == NULL) {
        return NULL;
    }

    EebusError err = kEebusErrorInit;
    if (is_recursive) {
        err = EebusMutexConstructRecursive(eebus_mutex);
    } else {
        err = EebusMutexConstruct(eebus_mutex);
    }

    if (err != kEebusErrorOk) {
        EebusMutexDelete(EEBUS_MUTEX_OBJECT(eebus_mutex));
        return NULL;
    }

    return EEBUS_MUTEX_OBJECT(eebus_mutex);
}

EebusMutexObject* EebusMutexCreate(void) {
    return EebusMutexCreateInternal(false);
}

EebusMutexObject* EebusMutexCreateRecursive(void) {
    return EebusMutexCreateInternal(true);
}

void Destruct(EebusMutexObject* self) {
    EebusMutex* const mutex = EEBUS_MUTEX(self);

    if (mutex->mutex_init_ret == 0) {
        pthread_mutex_destroy(&mutex->mutex);
    }
}

void Lock(EebusMutexObject* self) {
    EebusMutex* const mutex = EEBUS_MUTEX(self);

    pthread_mutex_lock(&mutex->mutex);
}

void Unlock(EebusMutexObject* self) {
    EebusMutex* const mutex = EEBUS_MUTEX(self);

    pthread_mutex_unlock(&mutex->mutex);
}
