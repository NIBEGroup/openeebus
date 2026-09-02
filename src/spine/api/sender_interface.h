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
 * @brief Sender interface declarations
 */

#ifndef SRC_SPINE_API_SENDER_INTERFACE_H_
#define SRC_SPINE_API_SENDER_INTERFACE_H_

#include "src/common/eebus_errors.h"
#include "src/common/vector.h"
#include "src/spine/model/command_frame_types.h"
#include "src/spine/model/datagram.h"
#include "src/spine/model/error_types.h"
#include "src/spine/model/feature_types.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @brief Sender Interface
 * (Sender "virtual functions table" declaration)
 */
typedef struct SenderInterface SenderInterface;

/**
 * @brief Sender Object type definition
 * ("abstract class", has no members but only pointer to
 * "virtual functions table")
 */
typedef struct SenderObject SenderObject;

/**
 * @brief Sender Interface Structure
 */
struct SenderInterface {
  void (*destruct)(SenderObject* self);
  EebusError (*read)(
      SenderObject* self,
      const FeatureAddressType* sender_addr,
      const FeatureAddressType* dest_addr,
      const CmdType* cmd,
      uint64_t* msg_cnt
  );
  EebusError (*reply)(
      SenderObject* self,
      const HeaderType* request_header,
      const FeatureAddressType* sender_addr,
      const CmdType* cmd
  );
  EebusError (*notify)(
      SenderObject* self,
      const FeatureAddressType* sender_addr,
      const FeatureAddressType* dest_addr,
      const CmdType* cmd
  );
  EebusError (*write)(
      SenderObject* self,
      const FeatureAddressType* sender_addr,
      const FeatureAddressType* dest_addr,
      const CmdType* cmd,
      uint64_t* msg_cnt
  );
  EebusError (*call_subscribe)(
      SenderObject* self,
      const FeatureAddressType* sender_addr,
      const FeatureAddressType* dest_addr,
      FeatureTypeType server_feature_type
  );
  EebusError (*call_unsubscribe)(
      SenderObject* self,
      const FeatureAddressType* sender_addr,
      const FeatureAddressType* dest_addr
  );
  EebusError (*call_bind)(
      SenderObject* self,
      const FeatureAddressType* sender_addr,
      const FeatureAddressType* dest_addr,
      FeatureTypeType server_feature_type
  );
  EebusError (*call_unbind)(
      SenderObject* self,
      const FeatureAddressType* sender_addr,
      const FeatureAddressType* dest_addr
  );
  EebusError (*result_success)(
      SenderObject* self,
      const HeaderType* request_header,
      const FeatureAddressType* sender_addr
  );
  EebusError (*result_error)(
      SenderObject* self,
      const HeaderType* request_header,
      const FeatureAddressType* sender_addr,
      const ErrorType* err
  );
};

/**
 * @brief Sender Object Structure
 */
struct SenderObject {
  const SenderInterface* interface_;
};

/**
 * @brief Sender pointer typecast
 */
#define SENDER_OBJECT(obj) ((SenderObject*)(obj))

/**
 * @brief Sender Interface class pointer typecast
 */
#define SENDER_INTERFACE(obj) (SENDER_OBJECT(obj)->interface_)

/**
 * @brief Sender Destruct caller definition
 */
#define SENDER_DESTRUCT(obj) (SENDER_INTERFACE(obj)->destruct(obj))

/**
 * @brief Sender Read caller definition
 */
#define SEND_READ(obj, sender_addr, dest_addr, cmd, msg_cnt) \
  (SENDER_INTERFACE(obj)->read(obj, sender_addr, dest_addr, cmd, msg_cnt))

/**
 * @brief Sender Reply caller definition
 */
#define SEND_REPLY(obj, request_header, sender_addr, cmd) \
  (SENDER_INTERFACE(obj)->reply(obj, request_header, sender_addr, cmd))

/**
 * @brief Sender Notify caller definition
 */
#define SEND_NOTIFY(obj, sender_addr, dest_addr, cmd) (SENDER_INTERFACE(obj)->notify(obj, sender_addr, dest_addr, cmd))

/**
 * @brief Sender Write caller definition
 */
#define SEND_WRITE(obj, sender_addr, dest_addr, cmd, msg_cnt) \
  (SENDER_INTERFACE(obj)->write(obj, sender_addr, dest_addr, cmd, msg_cnt))

/**
 * @brief Sender Call Subscribe caller definition
 */
#define SEND_CALL_SUBSCRIBE(obj, sender_addr, dest_addr, server_feature_type) \
  (SENDER_INTERFACE(obj)->call_subscribe(obj, sender_addr, dest_addr, server_feature_type))

/**
 * @brief Sender Call Unsubscribe caller definition
 */
#define SEND_CALL_UNSUBSCRIBE(obj, sender_addr, dest_addr) \
  (SENDER_INTERFACE(obj)->call_unsubscribe(obj, sender_addr, dest_addr))

/**
 * @brief Sender Call Bind caller definition
 */
#define SEND_CALL_BIND(obj, sender_addr, dest_addr, server_feature_type) \
  (SENDER_INTERFACE(obj)->call_bind(obj, sender_addr, dest_addr, server_feature_type))

/**
 * @brief Sender Call Unbind caller definition
 */
#define SEND_CALL_UNBIND(obj, sender_addr, dest_addr) (SENDER_INTERFACE(obj)->call_unbind(obj, sender_addr, dest_addr))

/**
 * @brief Sender Result Success caller definition
 */
#define SEND_RESULT_SUCCESS(obj, request_header, sender_addr) \
  (SENDER_INTERFACE(obj)->result_success(obj, request_header, sender_addr))

/**
 * @brief Sender Result Error caller definition
 */
#define SEND_RESULT_ERROR(obj, request_header, sender_addr, err) \
  (SENDER_INTERFACE(obj)->result_error(obj, request_header, sender_addr, err))

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SPINE_API_SENDER_INTERFACE_H_
