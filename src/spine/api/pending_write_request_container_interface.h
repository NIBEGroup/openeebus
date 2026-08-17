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
 * @brief Pending Write Request Container interface declarations
 */

#ifndef SRC_SPINE_API_PENDING_WRITE_REQUEST_CONTAINER_INTERFACE_H_
#define SRC_SPINE_API_PENDING_WRITE_REQUEST_CONTAINER_INTERFACE_H_

#include <stddef.h>

#include "src/spine/api/pending_write_request_interface.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @brief Pending Write Request Container Interface
 * (Pending Write Request Container "virtual functions table" declaration)
 */
typedef struct PendingWriteRequestContainerInterface PendingWriteRequestContainerInterface;

/**
 * @brief Pending Write Request Container Object type definition
 * ("abstract class", has no members but only pointer to
 * "virtual functions table")
 */
typedef struct PendingWriteRequestContainerObject PendingWriteRequestContainerObject;

/**
 * @brief Pending Write Request Container Interface Structure
 */
struct PendingWriteRequestContainerInterface {
  void (*destruct)(PendingWriteRequestContainerObject* self);
  EebusError (*add)(PendingWriteRequestContainerObject* self, const Message* msg);
  void (*remove)(PendingWriteRequestContainerObject* self, PendingWriteRequestObject* item);
  PendingWriteRequestObject* (*find)(PendingWriteRequestContainerObject* self, const char* ski, MsgCounterType msg_cnt);
  size_t (*get_size)(const PendingWriteRequestContainerObject* self);
  void (*tick)(PendingWriteRequestContainerObject* self, FeatureLocalObject* fl);
};

/**
 * @brief Pending Write Request Container Object Structure
 */
struct PendingWriteRequestContainerObject {
  const PendingWriteRequestContainerInterface* interface_;
};

/**
 * @brief Pending Write Request Container pointer typecast
 */
#define PENDING_WRITE_REQUEST_CONTAINER_OBJECT(obj) ((PendingWriteRequestContainerObject*)(obj))

/**
 * @brief Pending Write Request Container Interface class pointer typecast
 */
#define PENDING_WRITE_REQUEST_CONTAINER_INTERFACE(obj) (PENDING_WRITE_REQUEST_CONTAINER_OBJECT(obj)->interface_)

/**
 * @brief Pending Write Request Container Destruct caller definition
 */
#define PENDING_WRITE_REQUEST_CONTAINER_DESTRUCT(obj) (PENDING_WRITE_REQUEST_CONTAINER_INTERFACE(obj)->destruct(obj))

/**
 * @brief Pending Write Request Container Add caller definition
 */
#define PENDING_WRITE_REQUEST_CONTAINER_ADD(obj, msg) (PENDING_WRITE_REQUEST_CONTAINER_INTERFACE(obj)->add(obj, msg))

/**
 * @brief Pending Write Request Container Remove caller definition
 */
#define PENDING_WRITE_REQUEST_CONTAINER_REMOVE(obj, item) \
  (PENDING_WRITE_REQUEST_CONTAINER_INTERFACE(obj)->remove(obj, item))

/**
 * @brief Pending Write Request Container Find caller definition
 */
#define PENDING_WRITE_REQUEST_CONTAINER_FIND(obj, ski, msg_cnt) \
  (PENDING_WRITE_REQUEST_CONTAINER_INTERFACE(obj)->find(obj, ski, msg_cnt))

/**
 * @brief Pending Write Request Container Get Size caller definition
 */
#define PENDING_WRITE_REQUEST_CONTAINER_GET_SIZE(obj) (PENDING_WRITE_REQUEST_CONTAINER_INTERFACE(obj)->get_size(obj))

/**
 * @brief Pending Write Request Container Tick caller definition
 */
#define PENDING_WRITE_REQUEST_CONTAINER_TICK(obj, fl) (PENDING_WRITE_REQUEST_CONTAINER_INTERFACE(obj)->tick(obj, fl))

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SPINE_API_PENDING_WRITE_REQUEST_CONTAINER_INTERFACE_H_
