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
 * @brief Pending Result interface declarations
 */

#ifndef SRC_SPINE_API_PENDING_RESULT_INTERFACE_H_
#define SRC_SPINE_API_PENDING_RESULT_INTERFACE_H_

#include <stdbool.h>

#include "src/spine/api/feature_local_interface.h"
#include "src/spine/api/message.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @brief Pending Result Interface
 * (Pending Result "virtual functions table" declaration)
 */
typedef struct PendingResultInterface PendingResultInterface;

/**
 * @brief Pending Result Object type definition
 * ("abstract class", has no members but only pointer to
 * "virtual functions table")
 */
typedef struct PendingResultObject PendingResultObject;

/**
 * @brief Pending Result Interface Structure
 */
struct PendingResultInterface {
    void (*destruct)(PendingResultObject* self);
    MsgCounterType (*get_msg_cnt_ref)(const PendingResultObject* self);
    bool (*has_expired)(const PendingResultObject* self);
    void (*update_time)(PendingResultObject* self);
    void (*fire)(const PendingResultObject* self, const ResultMessage* result_msg, EebusError err);
};

/**
 * @brief Pending Result Object Structure
 */
struct PendingResultObject {
    const PendingResultInterface* interface_;
};

/**
 * @brief Pending Result pointer typecast
 */
#define PENDING_RESULT_OBJECT(obj) ((PendingResultObject*)(obj))

/**
 * @brief Pending Result Interface class pointer typecast
 */
#define PENDING_RESULT_INTERFACE(obj) (PENDING_RESULT_OBJECT(obj)->interface_)

/**
 * @brief Pending Result Destruct caller definition
 */
#define PENDING_RESULT_DESTRUCT(obj) (PENDING_RESULT_INTERFACE(obj)->destruct(obj))

/**
 * @brief Pending Result Get Msg Cnt Ref caller definition
 */
#define PENDING_RESULT_GET_MSG_CNT_REF(obj) (PENDING_RESULT_INTERFACE(obj)->get_msg_cnt_ref(obj))

/**
 * @brief Pending Result Has Expired caller definition
 */
#define PENDING_RESULT_HAS_EXPIRED(obj) (PENDING_RESULT_INTERFACE(obj)->has_expired(obj))

/**
 * @brief Pending Result Update Time caller definition
 */
#define PENDING_RESULT_UPDATE_TIME(obj) (PENDING_RESULT_INTERFACE(obj)->update_time(obj))

/**
 * @brief Pending Result Fire caller definition
 */
#define PENDING_RESULT_FIRE(obj, result_msg, err) (PENDING_RESULT_INTERFACE(obj)->fire(obj, result_msg, err))

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SPINE_API_PENDING_RESULT_INTERFACE_H_
