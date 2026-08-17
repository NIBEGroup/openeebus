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
 * @brief Pending Reply Container interface declarations
 */

#ifndef SRC_SPINE_API_PENDING_REPLY_CONTAINER_INTERFACE_H_
#define SRC_SPINE_API_PENDING_REPLY_CONTAINER_INTERFACE_H_

#include "src/common/eebus_errors.h"
#include "src/spine/api/feature_local_interface.h"
#include "src/spine/api/message.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @brief Pending Reply Container Interface
 * (Pending Reply Container "virtual functions table" declaration)
 */
typedef struct PendingReplyContainerInterface PendingReplyContainerInterface;

/**
 * @brief Pending Reply Container Object type definition
 * ("abstract class", has no members but only pointer to
 * "virtual functions table")
 */
typedef struct PendingReplyContainerObject PendingReplyContainerObject;

/**
 * @brief Pending Reply Container Interface Structure
 */
struct PendingReplyContainerInterface {
  void (*destruct)(PendingReplyContainerObject* self);
  EebusError (*add)(
      PendingReplyContainerObject* self,
      MsgCounterType msg_cnt_ref,
      const FeatureAddressType* remote_feature_address,
      FunctionType function_type,
      const char* ski,
      ReplyMessageCallback cb,
      void* ctx
  );
  void (*process)(PendingReplyContainerObject* self, const ReplyMessage* reply_msg, EebusError err);
  void (*tick)(PendingReplyContainerObject* self);
  void (*remove_for_device)(PendingReplyContainerObject* self, const char* device_addr);
  void (*remove_for_ski)(PendingReplyContainerObject* self, const char* ski);
};

/**
 * @brief Pending Reply Container Object Structure
 */
struct PendingReplyContainerObject {
  const PendingReplyContainerInterface* interface_;
};

/**
 * @brief Pending Reply Container pointer typecast
 */
#define PENDING_REPLY_CONTAINER_OBJECT(obj) ((PendingReplyContainerObject*)(obj))

/**
 * @brief Pending Reply Container Interface class pointer typecast
 */
#define PENDING_REPLY_CONTAINER_INTERFACE(obj) (PENDING_REPLY_CONTAINER_OBJECT(obj)->interface_)

/**
 * @brief Pending Reply Container Destruct caller definition
 */
#define PENDING_REPLY_CONTAINER_DESTRUCT(obj) (PENDING_REPLY_CONTAINER_INTERFACE(obj)->destruct(obj))

/**
 * @brief Pending Reply Container Add caller definition
 */
#define PENDING_REPLY_CONTAINER_ADD(obj, msg_cnt_ref, remote_feature_address, function_type, ski, cb, ctx) \
  (PENDING_REPLY_CONTAINER_INTERFACE(obj)->add(obj, msg_cnt_ref, remote_feature_address, function_type, ski, cb, ctx))

/**
 * @brief Pending Reply Container Process caller definition
 */
#define PENDING_REPLY_CONTAINER_PROCESS(obj, reply_msg, err) \
  (PENDING_REPLY_CONTAINER_INTERFACE(obj)->process(obj, reply_msg, err))

/**
 * @brief Pending Reply Container Tick caller definition
 */
#define PENDING_REPLY_CONTAINER_TICK(obj) (PENDING_REPLY_CONTAINER_INTERFACE(obj)->tick(obj))

/**
 * @brief Pending Reply Container Remove For Device caller definition
 */
#define PENDING_REPLY_CONTAINER_REMOVE_FOR_DEVICE(obj, device_addr) \
  (PENDING_REPLY_CONTAINER_INTERFACE(obj)->remove_for_device(obj, device_addr))

/**
 * @brief Pending Reply Container Remove For SKI caller definition
 */
#define PENDING_REPLY_CONTAINER_REMOVE_FOR_SKI(obj, ski) \
  (PENDING_REPLY_CONTAINER_INTERFACE(obj)->remove_for_ski(obj, ski))

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SPINE_API_PENDING_REPLY_CONTAINER_INTERFACE_H_
