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
 * @brief Pending Write Request interface declarations
 */

#ifndef SRC_SPINE_API_PENDING_WRITE_REQUEST_INTERFACE_H_
#define SRC_SPINE_API_PENDING_WRITE_REQUEST_INTERFACE_H_

#include <stdbool.h>
#include <stdint.h>
#include "src/spine/api/message.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @brief Pending Write Request Interface
 * (Pending Write Request "virtual functions table" declaration)
 */
typedef struct PendingWriteRequestInterface PendingWriteRequestInterface;

/**
 * @brief Pending Write Request Object type definition
 * ("abstract class", has no members but only pointer to
 * "virtual functions table")
 */
typedef struct PendingWriteRequestObject PendingWriteRequestObject;

/**
 * @brief PendingWriteRequest Interface Structure
 */
struct PendingWriteRequestInterface {
    void (*destruct)(PendingWriteRequestObject* self);
    const char* (*get_ski)(PendingWriteRequestObject* self);
    uint64_t (*get_message_counter)(PendingWriteRequestObject* self);
    EebusError (*get_message)(PendingWriteRequestObject* self, FeatureLocalObject* fl, Message* msg);
    uint64_t (*get_number_of_approvals)(PendingWriteRequestObject* self);
    void (*add_approval)(PendingWriteRequestObject* self);
    uint32_t (*get_remaining_time)(PendingWriteRequestObject* self);
    void (*update_remaining_time)(PendingWriteRequestObject* self);
    bool (*has_expired)(PendingWriteRequestObject* self);
};

/**
 * @brief Pending Write Request Object Structure
 */
struct PendingWriteRequestObject {
    const PendingWriteRequestInterface* interface_;
};

/**
 * @brief Pending Write Request pointer typecast
 */
#define PENDING_WRITE_REQUEST_OBJECT(obj) ((PendingWriteRequestObject*)(obj))

/**
 * @brief Pending Write Request Interface class pointer typecast
 */
#define PENDING_WRITE_REQUEST_INTERFACE(obj) (PENDING_WRITE_REQUEST_OBJECT(obj)->interface_)

/**
 * @brief Pending Write Request Destruct caller definition
 */
#define PENDING_WRITE_REQUEST_DESTRUCT(obj) (PENDING_WRITE_REQUEST_INTERFACE(obj)->destruct(obj))

/**
 * @brief Pending Write Request Get Ski caller definition
 */
#define PENDING_WRITE_REQUEST_GET_SKI(obj) (PENDING_WRITE_REQUEST_INTERFACE(obj)->get_ski(obj))

/**
 * @brief Pending Write Request Get Message Counter caller definition
 */
#define PENDING_WRITE_REQUEST_GET_MESSAGE_COUNTER(obj) (PENDING_WRITE_REQUEST_INTERFACE(obj)->get_message_counter(obj))

/**
 * @brief Pending Write Request Get Message caller definition
 */
#define PENDING_WRITE_REQUEST_GET_MESSAGE(obj, fl, msg) \
    (PENDING_WRITE_REQUEST_INTERFACE(obj)->get_message(obj, fl, msg))

/**
 * @brief Pending Write Request Get Number Of Approvals caller definition
 */
#define PENDING_WRITE_REQUEST_GET_NUMBER_OF_APPROVALS(obj) \
    (PENDING_WRITE_REQUEST_INTERFACE(obj)->get_number_of_approvals(obj))

/**
 * @brief Pending Write Request Add Approval caller definition
 */
#define PENDING_WRITE_REQUEST_ADD_APPROVAL(obj) (PENDING_WRITE_REQUEST_INTERFACE(obj)->add_approval(obj))

/**
 * @brief Pending Write Request Get Remaining Time caller definition
 */
#define PENDING_WRITE_REQUEST_GET_REMAINING_TIME(obj) (PENDING_WRITE_REQUEST_INTERFACE(obj)->get_remaining_time(obj))

/**
 * @brief Pending Write Request Update Remaining Time caller definition
 */
#define PENDING_WRITE_REQUEST_UPDATE_REMAINING_TIME(obj) \
    (PENDING_WRITE_REQUEST_INTERFACE(obj)->update_remaining_time(obj))

/**
 * @brief Pending Write Request Has Expired caller definition
 */
#define PENDING_WRITE_REQUEST_HAS_EXPIRED(obj) (PENDING_WRITE_REQUEST_INTERFACE(obj)->has_expired(obj))

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SPINE_API_PENDING_WRITE_REQUEST_INTERFACE_H_
