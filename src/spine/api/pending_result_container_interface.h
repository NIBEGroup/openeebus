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
 * @brief Pending Result Container interface declarations
 */

#ifndef SRC_SPINE_API_PENDING_RESULT_CONTAINER_INTERFACE_H_
#define SRC_SPINE_API_PENDING_RESULT_CONTAINER_INTERFACE_H_

#include "src/common/eebus_errors.h"
#include "src/spine/api/feature_local_interface.h"
#include "src/spine/api/message.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @brief Pending Result Container Interface
 * (Pending Result Container "virtual functions table" declaration)
 */
typedef struct PendingResultContainerInterface PendingResultContainerInterface;

/**
 * @brief Pending Result Container Object type definition
 * ("abstract class", has no members but only pointer to
 * "virtual functions table")
 */
typedef struct PendingResultContainerObject PendingResultContainerObject;

/**
 * @brief Pending Result Container Interface Structure
 */
struct PendingResultContainerInterface {
    void (*destruct)(PendingResultContainerObject* self);
    EebusError (*add)(
        PendingResultContainerObject* self,
        MsgCounterType msg_cnt_ref,
        FunctionType function_type,
        const FeatureAddressType* remote_feature_addr,
        ResultMessageCallback cb,
        void* ctx
    );
    void (*process)(PendingResultContainerObject* self, const ResultMessage* result_msg);
    void (*tick)(PendingResultContainerObject* self);
};

/**
 * @brief Pending Result Container Object Structure
 */
struct PendingResultContainerObject {
    const PendingResultContainerInterface* interface_;
};

/**
 * @brief Pending Result Container pointer typecast
 */
#define PENDING_RESULT_CONTAINER_OBJECT(obj) ((PendingResultContainerObject*)(obj))

/**
 * @brief Pending Result Container Interface class pointer typecast
 */
#define PENDING_RESULT_CONTAINER_INTERFACE(obj) (PENDING_RESULT_CONTAINER_OBJECT(obj)->interface_)

/**
 * @brief Pending Result Container Destruct caller definition
 */
#define PENDING_RESULT_CONTAINER_DESTRUCT(obj) (PENDING_RESULT_CONTAINER_INTERFACE(obj)->destruct(obj))

/**
 * @brief Pending Result Container Add caller definition
 */
#define PENDING_RESULT_CONTAINER_ADD(obj, msg_cnt_ref, function_type, remote_feature_addr, cb, ctx) \
    (PENDING_RESULT_CONTAINER_INTERFACE(obj)->add(obj, msg_cnt_ref, function_type, remote_feature_addr, cb, ctx))

/**
 * @brief Pending Result Container Process caller definition
 */
#define PENDING_RESULT_CONTAINER_PROCESS(obj, result_msg) \
    (PENDING_RESULT_CONTAINER_INTERFACE(obj)->process(obj, result_msg))

/**
 * @brief Pending Result Container Tick caller definition
 */
#define PENDING_RESULT_CONTAINER_TICK(obj) (PENDING_RESULT_CONTAINER_INTERFACE(obj)->tick(obj))

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SPINE_API_PENDING_RESULT_CONTAINER_INTERFACE_H_
