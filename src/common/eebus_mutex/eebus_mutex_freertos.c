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
 * @brief FreeRTOS Eebus Mutex implementation
 */

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

#include "src/common/api/eebus_mutex_interface.h"
#include "src/common/eebus_errors.h"
#include "src/common/eebus_malloc.h"
#include "src/common/eebus_mutex/eebus_mutex.h"

typedef struct EebusMutex EebusMutex;

struct EebusMutex {
    /** Implements the Eebus Mutex Interface */
    EebusMutexObject obj;

    bool is_recursive;
    StaticSemaphore_t mutex_buffer;
    SemaphoreHandle_t mutex_handle;
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

static EebusError EebusMutexConstruct(EebusMutex* self, bool is_recursive);

EebusError EebusMutexConstruct(EebusMutex* self, bool is_recursive) {
    // Override "virtual functions table"
    EEBUS_MUTEX_INTERFACE(self) = &eebus_mutex_methods;

    self->is_recursive = is_recursive;
    self->mutex_handle = NULL;

    if (is_recursive) {
        self->mutex_handle = xSemaphoreCreateRecursiveMutexStatic(&self->mutex_buffer);
    } else {
        self->mutex_handle = xSemaphoreCreateMutexStatic(&self->mutex_buffer);
    }

    if (self->mutex_handle == NULL) {
        return kEebusErrorInit;
    }

    return kEebusErrorOk;
}

EebusMutexObject* EebusMutexCreateInternal(bool is_recursive) {
    EebusMutex* const eebus_mutex = (EebusMutex*)EEBUS_MALLOC(sizeof(EebusMutex));
    if (eebus_mutex == NULL) {
        return NULL;
    }

    if (EebusMutexConstruct(eebus_mutex, is_recursive) != kEebusErrorOk) {
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

    if (mutex->mutex_handle != NULL) {
        vSemaphoreDelete(mutex->mutex_handle);
        mutex->mutex_handle = NULL;
    }
}

void Lock(EebusMutexObject* self) {
    EebusMutex* const mutex = EEBUS_MUTEX(self);

    if (mutex->is_recursive) {
        xSemaphoreTakeRecursive(mutex->mutex_handle, portMAX_DELAY);
    } else {
        xSemaphoreTake(mutex->mutex_handle, portMAX_DELAY);
    }
}

void Unlock(EebusMutexObject* self) {
    EebusMutex* const mutex = EEBUS_MUTEX(self);
    if (mutex->is_recursive) {
        xSemaphoreGiveRecursive(mutex->mutex_handle);
    } else {
        xSemaphoreGive(mutex->mutex_handle);
    }
}
