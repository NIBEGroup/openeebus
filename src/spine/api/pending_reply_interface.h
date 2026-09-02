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
 * @brief Pending Reply interface declarations
 */

#ifndef SRC_SPINE_API_PENDING_REPLY_INTERFACE_H_
#define SRC_SPINE_API_PENDING_REPLY_INTERFACE_H_

#include <stdbool.h>

#include "src/spine/api/feature_local_interface.h"
#include "src/spine/api/message.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @brief Pending Reply Interface
 * (Pending Reply "virtual functions table" declaration)
 */
typedef struct PendingReplyInterface PendingReplyInterface;

/**
 * @brief Pending Reply Object type definition
 * ("abstract class", has no members but only pointer to
 * "virtual functions table")
 */
typedef struct PendingReplyObject PendingReplyObject;

/**
 * @brief Pending Reply Interface Structure
 */
struct PendingReplyInterface {
  void (*destruct)(PendingReplyObject* self);
  MsgCounterType (*get_msg_cnt_ref)(const PendingReplyObject* self);
  const FeatureAddressType* (*get_remote_feature_address)(const PendingReplyObject* self);
  const char* (*get_ski)(const PendingReplyObject* self);
  bool (*has_expired)(const PendingReplyObject* self);
  void (*update_time)(PendingReplyObject* self);
  void (*fire)(const PendingReplyObject* self, const ReplyMessage* reply_msg, EebusError err);
};

/**
 * @brief Pending Reply Object Structure
 */
struct PendingReplyObject {
  const PendingReplyInterface* interface_;
};

/**
 * @brief Pending Reply pointer typecast
 */
#define PENDING_REPLY_OBJECT(obj) ((PendingReplyObject*)(obj))

/**
 * @brief Pending Reply Interface class pointer typecast
 */
#define PENDING_REPLY_INTERFACE(obj) (PENDING_REPLY_OBJECT(obj)->interface_)

/**
 * @brief Pending Reply Destruct caller definition
 */
#define PENDING_REPLY_DESTRUCT(obj) (PENDING_REPLY_INTERFACE(obj)->destruct(obj))

/**
 * @brief Pending Reply Get Msg Cnt Ref caller definition
 */
#define PENDING_REPLY_GET_MSG_CNT_REF(obj) (PENDING_REPLY_INTERFACE(obj)->get_msg_cnt_ref(obj))

/**
 * @brief Pending Reply Get Remote Feature Address caller definition
 */
#define PENDING_REPLY_GET_REMOTE_FEATURE_ADDRESS(obj) (PENDING_REPLY_INTERFACE(obj)->get_remote_feature_address(obj))

/**
 * @brief Pending Reply Get SKI caller definition
 */
#define PENDING_REPLY_GET_SKI(obj) (PENDING_REPLY_INTERFACE(obj)->get_ski(obj))

/**
 * @brief Pending Reply Has Expired caller definition
 */
#define PENDING_REPLY_HAS_EXPIRED(obj) (PENDING_REPLY_INTERFACE(obj)->has_expired(obj))

/**
 * @brief Pending Reply Update Time caller definition
 */
#define PENDING_REPLY_UPDATE_TIME(obj) (PENDING_REPLY_INTERFACE(obj)->update_time(obj))

/**
 * @brief Pending Reply Fire caller definition
 */
#define PENDING_REPLY_FIRE(obj, reply_msg, err) (PENDING_REPLY_INTERFACE(obj)->fire(obj, reply_msg, err))

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SPINE_API_PENDING_REPLY_INTERFACE_H_
