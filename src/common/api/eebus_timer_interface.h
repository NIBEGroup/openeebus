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
 * @brief EEBUS Timer interface declarations
 */

#ifndef SRC_COMMON_API_EEBUS_TIMER_INTERFACE_H_
#define SRC_COMMON_API_EEBUS_TIMER_INTERFACE_H_

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

#define SECONDS(t) ((t) * 1000)
#define MILLISECONDS(t) (t)
#define NANOSECONDS(t) ((t) * 1000000)
#define TIME_MS_TO_S(t) ((t) / 1000)
#define TIME_NS_TO_MS(t) ((t) / 1000000)

enum EebusTimerState {
    kEebusTimerStateIdle,
    kEebusTimerStateRunning,
    kEebusTimerStateExpired,
};

typedef enum EebusTimerState EebusTimerState;

/**
 * @brief On timeout callback type definition
 */
typedef void (*EebusTimerTimeoutCallback)(void* ctx);

/**
 * @brief EEBUS Timer Interface
 * (EEBUS Timer "virtual functions table" declaration)
 */
typedef struct EebusTimerInterface EebusTimerInterface;

/**
 * @brief EEBUS Timer Object type definition
 * ("abstract class", has no members but only pointer to
 * "virtual functions table")
 */
typedef struct EebusTimerObject EebusTimerObject;

/**
 * @brief EebusTimer Interface Structure
 */
struct EebusTimerInterface {
    void (*destruct)(EebusTimerObject* self);
    void (*start)(EebusTimerObject* self, uint32_t timeout_ms, bool autoreload);
    void (*stop)(EebusTimerObject* self);
    uint32_t (*get_remaining_time)(const EebusTimerObject* self);
    EebusTimerState (*get_timer_state)(const EebusTimerObject* self);
};

/**
 * @brief EEBUS Timer Object Structure
 */
struct EebusTimerObject {
    const EebusTimerInterface* interface_;
};

/**
 * @brief EEBUS Timer pointer typecast
 */
#define EEBUS_TIMER_OBJECT(obj) ((EebusTimerObject*)(obj))

/**
 * @brief EEBUS Timer Interface class pointer typecast
 */
#define EEBUS_TIMER_INTERFACE(obj) (EEBUS_TIMER_OBJECT(obj)->interface_)

/**
 * @brief EEBUS Timer Destruct caller definition
 */
#define EEBUS_TIMER_DESTRUCT(obj) (EEBUS_TIMER_INTERFACE(obj)->destruct(obj))

/**
 * @brief EEBUS Timer Start caller definition
 */
#define EEBUS_TIMER_START(obj, timeout_ms, autoreload) (EEBUS_TIMER_INTERFACE(obj)->start(obj, timeout_ms, autoreload))

/**
 * @brief EEBUS Timer Stop caller definition
 */
#define EEBUS_TIMER_STOP(obj) (EEBUS_TIMER_INTERFACE(obj)->stop(obj))

/**
 * @brief EEBUS Timer Get Remaining Time caller definition
 */
#define EEBUS_TIMER_GET_REMAINING_TIME(obj) (EEBUS_TIMER_INTERFACE(obj)->get_remaining_time(obj))

/**
 * @brief EEBUS Timer Get Timer State caller definition
 */
#define EEBUS_TIMER_GET_TIMER_STATE(obj) (EEBUS_TIMER_INTERFACE(obj)->get_timer_state(obj))

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_COMMON_API_EEBUS_TIMER_INTERFACE_H_
