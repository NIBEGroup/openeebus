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
 * @brief FreeRTOS Eebus Thread implementation. Internal FreeRTOS task is created as "static"
 * just to be able to use the custom buffer allocation. Note that EEBUS_MALLOC() can
 * use the external RAM on some specific platforms what provides more flexible memory usage.
 */

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdint.h>
#include <string.h>

#include "src/common/api/eebus_thread_interface.h"
#include "src/common/eebus_errors.h"
#include "src/common/eebus_malloc.h"
#include "src/common/eebus_thread/eebus_thread.h"

typedef struct EebusThread EebusThread;

struct EebusThread {
    /** Implements the Eebus Thread Interface */
    EebusThreadObject obj;

    EebusThreadRoutine routine;
    void* parameters;
    SemaphoreHandle_t mutex;
    StaticTask_t* static_task;
    StackType_t* task_stack;
    TaskHandle_t task;
    TaskHandle_t join_task;
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

static void EebusThreadTask(void* arg) {
    EebusThread* eebus_thread = (EebusThread*)arg;

    eebus_thread->routine(eebus_thread->parameters);

    xSemaphoreTake(eebus_thread->mutex, portMAX_DELAY);

    TaskHandle_t task_handle = eebus_thread->task;

    eebus_thread->task = NULL;

    if (eebus_thread->join_task != NULL) {
        xTaskNotify(eebus_thread->join_task, 0, eNoAction);
        eebus_thread->join_task = NULL;
    }

    xSemaphoreGive(eebus_thread->mutex);

    vTaskDelete(task_handle);
}

EebusError EebusThreadConstruct(EebusThread* self, EebusThreadRoutine routine, void* parameters, size_t stack_size) {
    // Override "virtual functions table"
    EEBUS_THREAD_INTERFACE(self) = &eebus_thread_methods;

    self->routine     = routine;
    self->parameters  = parameters;
    self->mutex       = NULL;
    self->static_task = NULL;
    self->task_stack  = NULL;
    self->task        = NULL;
    self->join_task   = NULL;

    if (routine == NULL) {
        return kEebusErrorInputArgumentNull;
    }

    // Create Mutex
    self->mutex = xSemaphoreCreateMutex();
    if (self->mutex == NULL) {
        return kEebusErrorThread;
    }

    // Create task buffer
    // Note: stack can be allocated in the external RAM
    const size_t stack_size_words = (stack_size + sizeof(StackType_t) - 1) / sizeof(StackType_t);

    // Note: shall be always in internal RAM
    self->static_task = (StaticTask_t*)pvPortMalloc(sizeof(StaticTask_t));
    if (self->static_task == NULL) {
        EEBUS_FREE(self->task_stack);
        return kEebusErrorMemoryAllocate;
    }

    // Create stack buffer
    self->task_stack = (StackType_t*)EEBUS_MALLOC(stack_size_words * sizeof(StackType_t));
    if (self->task_stack == NULL) {
        return kEebusErrorMemoryAllocate;
    }

    self->task = xTaskCreateStatic(
        EebusThreadTask,
        "EebusThread",
        stack_size_words,
        self,
        tskIDLE_PRIORITY,
        self->task_stack,
        self->static_task
    );

    if (self->task == NULL) {
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
    if (eebus_thread->task != NULL) {
        vTaskDelete(eebus_thread->task);
        eebus_thread->task = NULL;
    }

    if (eebus_thread->mutex != NULL) {
        vSemaphoreDelete(eebus_thread->mutex);
        eebus_thread->mutex = NULL;
    }

    if (eebus_thread->task_stack != NULL) {
        EEBUS_FREE(eebus_thread->task_stack);
        eebus_thread->task_stack = NULL;
    }

    if (eebus_thread->static_task != NULL) {
        vPortFree(eebus_thread->static_task);
        eebus_thread->static_task = NULL;
    }
}

void Join(EebusThreadObject* self) {
    EebusThread* const eebus_thread = EEBUS_THREAD(self);

    bool should_wait = false;

    xSemaphoreTake(eebus_thread->mutex, portMAX_DELAY);

    if (eebus_thread->task != NULL) {
        eebus_thread->join_task = xTaskGetCurrentTaskHandle();

        should_wait = true;
    }

    xSemaphoreGive(eebus_thread->mutex);

    if (should_wait) {
        xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);
    }
}

void EebusThreadSleep(int32_t seconds) {
    vTaskDelay(seconds * 1000 / portTICK_PERIOD_MS);
}

void EebusThreadUsleep(int32_t useconds) {
    vTaskDelay(useconds / 1000 / portTICK_PERIOD_MS);
}
