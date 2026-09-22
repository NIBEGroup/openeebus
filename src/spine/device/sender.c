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
 * @brief Sender implementation
 */

#include <string.h>

#include "src/common/array_util.h"
#include "src/common/debug.h"
#include "src/common/eebus_arguments.h"
#include "src/common/eebus_malloc.h"
#include "src/common/json.h"
#include "src/ship/api/data_writer_interface.h"
#include "src/spine/api/sender_interface.h"
#include "src/spine/model/node_management_types.h"
#include "src/spine/model/result_types.h"
#include "src/spine/model/specification_version.h"
#include "src/spine/model/subscription_management_types.h"

/** Set SENDER_DEBUG 1 to enable debug prints */
#ifndef SENDER_DEBUG
#define SENDER_DEBUG 0
#endif

/** Sender debug printf(), enabled whith SENDER_DEBUG = 1 */
#if SENDER_DEBUG
#define SENDER_DEBUG_PRINTF(fmt, ...) DebugPrintf(fmt, ##__VA_ARGS__)
#else
#define SENDER_DEBUG_PRINTF(fmt, ...)
#endif  // SENDER_DEBUG

typedef struct Sender Sender;

struct Sender {
  /** Implements the Sender Interface */
  SenderObject obj;

  uint64_t msg_num;

  // TODO: Add message cache

  DataWriterObject* writer;
};

#define SENDER(obj) ((Sender*)(obj))

static void Destruct(SenderObject* self);
static EebusError Read(
    SenderObject* self,
    const FeatureAddressType* sender_addr,
    const FeatureAddressType* dest_addr,
    const CmdType* cmd,
    uint64_t* msg_cnt
);
static EebusError
Reply(SenderObject* self, const HeaderType* request_header, const FeatureAddressType* sender_addr, const CmdType* cmd);
static EebusError Notify(
    SenderObject* self,
    const FeatureAddressType* sender_addr,
    const FeatureAddressType* dest_addr,
    const CmdType* cmd
);
static EebusError Write(
    SenderObject* self,
    const FeatureAddressType* sender_addr,
    const FeatureAddressType* dest_addr,
    const CmdType* cmd,
    uint64_t* msg_cnt
);
static EebusError CallSubscribe(
    SenderObject* self,
    const FeatureAddressType* sender_addr,
    const FeatureAddressType* dest_addr,
    FeatureTypeType server_feature_type
);
static EebusError
CallUnsubscribe(SenderObject* self, const FeatureAddressType* sender_addr, const FeatureAddressType* dest_addr);
static EebusError CallBind(
    SenderObject* self,
    const FeatureAddressType* sender_addr,
    const FeatureAddressType* dest_addr,
    FeatureTypeType server_feature_type
);
static EebusError
CallUnbind(SenderObject* self, const FeatureAddressType* sender_addr, const FeatureAddressType* dest_addr);
static EebusError
ResultSuccess(SenderObject* self, const HeaderType* request_header, const FeatureAddressType* sender_addr);
static EebusError ResultError(
    SenderObject* self,
    const HeaderType* request_header,
    const FeatureAddressType* sender_addr,
    const ErrorType* err
);

static const SenderInterface sender_methods = {
    .destruct         = Destruct,
    .read             = Read,
    .reply            = Reply,
    .notify           = Notify,
    .write            = Write,
    .call_subscribe   = CallSubscribe,
    .call_unsubscribe = CallUnsubscribe,
    .call_bind        = CallBind,
    .call_unbind      = CallUnbind,
    .result_success   = ResultSuccess,
    .result_error     = ResultError,
};

static void SenderConstruct(Sender* self, DataWriterObject* writer);
static EebusError SendSpineMessage(
    Sender* self,
    CommandClassifierType cmd_classifier,
    const FeatureAddressType* src_addr,
    const FeatureAddressType* dst_addr,
    const uint64_t* msg_counter_ref,
    bool request_ack,
    const CmdType* cmd,
    size_t cmd_size,
    uint64_t* msg_cnt
);
static uint64_t SenderGetNextMsgCounter(Sender* self);
static FeatureAddressType NodeManagementAddress(const char* device_addr);
static EebusError SendNodeManagmentCall(
    Sender* self,
    const void* data,
    FunctionType data_type_id,
    const char* local_device,
    const char* remote_device
);
static EebusError
SendResult(Sender* self, const HeaderType* request_header, const FeatureAddressType* sender_addr, const ErrorType* err);

void SenderConstruct(Sender* self, DataWriterObject* writer) {
  // Override "virtual functions table"
  SENDER_INTERFACE(self) = &sender_methods;

  self->msg_num = 0;
  self->writer  = writer;
}

SenderObject* SenderCreate(DataWriterObject* writer) {
  Sender* const sender = (Sender*)EEBUS_MALLOC(sizeof(Sender));

  SenderConstruct(sender, writer);

  return SENDER_OBJECT(sender);
}

void Destruct(SenderObject* self) {
  UNUSED(self);
  // Nothing to be deallocated yet
}

EebusError SendSpineMessage(
    Sender* self,
    CommandClassifierType cmd_classifier,
    const FeatureAddressType* src_addr,
    const FeatureAddressType* dst_addr,
    const uint64_t* msg_counter_ref,
    bool request_ack,
    const CmdType* cmd,
    size_t cmd_size,
    uint64_t* msg_cnt
) {
  if ((src_addr == NULL) || (dst_addr == NULL) || (cmd == NULL) || (cmd_size == 0)) {
    return kEebusErrorInputArgumentNull;
  }

  if (self->writer == NULL) {
    return kEebusErrorInit;
  }

  const uint64_t msg_counter = SenderGetNextMsgCounter(self);

  if (msg_cnt != NULL) {
    *msg_cnt = msg_counter;
  }

  const HeaderType header = {
      .spec_version   = specification_version,
      .src_addr       = src_addr,
      .dest_addr      = dst_addr,
      .msg_cnt        = &msg_counter,
      .msg_cnt_ref    = msg_counter_ref,
      .cmd_classifier = &cmd_classifier,
      .ack_request    = request_ack ? &request_ack : NULL,
  };

  // Allocate array for command pointers
  const CmdType** const p_cmd = (const CmdType**)EEBUS_MALLOC(cmd_size * sizeof(CmdType*));
  if (p_cmd == NULL) {
    return kEebusErrorMemoryAllocate;
  }

  for (size_t i = 0; i < cmd_size; ++i) {
    p_cmd[i] = &cmd[i];
  }

  // clang-format off
  const DatagramType datagram = {
      .header  = &header,
      .payload = &(PayloadType){
          .cmd      = p_cmd,
          .cmd_size = cmd_size,
      },
  };
  // clang-format on

  const char* const msg = DatagramPrintUnformatted(&datagram);
  if (msg == NULL) {
    EEBUS_FREE(p_cmd);
    return kEebusErrorMemoryAllocate;
  }

  SENDER_DEBUG_PRINTF("%s: sending %s\n", __func__, msg);

  DATA_WRITER_WRITE_MESSAGE(self->writer, (uint8_t*)msg, strlen(msg) + 1);
  JsonFree((void*)msg);
  EEBUS_FREE(p_cmd);

  return kEebusErrorOk;
}

uint64_t SenderGetNextMsgCounter(Sender* self) {
  return ++self->msg_num;
}

#ifdef GTEST
void SenderSetMsgCounter(SenderObject* self, uint64_t msg_num) {
  SENDER(self)->msg_num = msg_num;
}
#endif

EebusError Read(
    SenderObject* self,
    const FeatureAddressType* sender_addr,
    const FeatureAddressType* dest_addr,
    const CmdType* cmd,
    uint64_t* msg_cnt
) {
  return SendSpineMessage(
      SENDER(self),
      kCommandClassifierTypeRead,
      sender_addr,
      dest_addr,
      NULL,
      false,
      cmd,
      1,
      msg_cnt
  );
}

EebusError
Reply(SenderObject* self, const HeaderType* request_header, const FeatureAddressType* sender_addr, const CmdType* cmd) {
  if ((request_header == NULL) || (sender_addr == NULL)) {
    return kEebusErrorInputArgumentNull;
  }

  if (request_header->src_addr == NULL) {
    return kEebusErrorInputArgument;
  }

  FeatureAddressType src_addr = *request_header->dest_addr;

  src_addr.device = sender_addr->device;

  return SendSpineMessage(
      SENDER(self),
      kCommandClassifierTypeReply,
      &src_addr,
      request_header->src_addr,
      request_header->msg_cnt,
      false,
      cmd,
      1,
      NULL
  );
}

EebusError Notify(
    SenderObject* self,
    const FeatureAddressType* sender_addr,
    const FeatureAddressType* dest_addr,
    const CmdType* cmd
) {
  static const CommandClassifierType cmd_classifier = kCommandClassifierTypeNotify;
  return SendSpineMessage(SENDER(self), cmd_classifier, sender_addr, dest_addr, NULL, false, cmd, 1, NULL);
}

EebusError Write(
    SenderObject* self,
    const FeatureAddressType* sender_addr,
    const FeatureAddressType* dest_addr,
    const CmdType* cmd,
    uint64_t* msg_cnt
) {
  static const CommandClassifierType cmd_classifier = kCommandClassifierTypeWrite;
  return SendSpineMessage(SENDER(self), cmd_classifier, sender_addr, dest_addr, NULL, true, cmd, 1, msg_cnt);
}

FeatureAddressType NodeManagementAddress(const char* device_addr) {
  static const uint32_t entity_id        = NODE_MANAGEMENT_ENTITY_ID;
  static const uint32_t* const entity[1] = {&entity_id};
  static const uint32_t feature          = NODE_MANAGEMENT_FEATURE_ID;

  return (FeatureAddressType){
      .entity      = entity,
      .entity_size = ARRAY_SIZE(entity),
      .feature     = &feature,
      .device      = device_addr,
  };
}

EebusError SendNodeManagmentCall(
    Sender* self,
    const void* data,
    FunctionType data_type_id,
    const char* local_device,
    const char* remote_device
) {
  const CmdType cmd = {
      .data_choice         = data,
      .data_choice_type_id = data_type_id,
  };

  // We always send it to the remote NodeManagement feature, which always is at entity: [0], feature: 0
  const FeatureAddressType local_addr  = NodeManagementAddress(local_device);
  const FeatureAddressType remote_addr = NodeManagementAddress(remote_device);

  return SendSpineMessage(self, kCommandClassifierTypeCall, &local_addr, &remote_addr, NULL, true, &cmd, 1, NULL);
}

EebusError CallSubscribe(
    SenderObject* self,
    const FeatureAddressType* sender_addr,
    const FeatureAddressType* dest_addr,
    FeatureTypeType server_feature_type
) {
  if ((sender_addr == NULL) || (dest_addr == NULL)) {
    return kEebusErrorInputArgumentNull;
  }

  // clang-format off
  const NodeManagementSubscriptionRequestCallType node_management_subscription_request = {
      .subscription_request = &(SubscriptionManagementRequestCallType){
          .client_address      = sender_addr,
          .server_address      = dest_addr,
          .server_feature_type = &server_feature_type,
      },
  };
  // clang-format on

  return SendNodeManagmentCall(
      SENDER(self),
      &node_management_subscription_request,
      kFunctionTypeNodeManagementSubscriptionRequestCall,
      sender_addr->device,
      dest_addr->device
  );
}

EebusError
CallUnsubscribe(SenderObject* self, const FeatureAddressType* sender_addr, const FeatureAddressType* dest_addr) {
  if ((sender_addr == NULL) || (dest_addr == NULL)) {
    return kEebusErrorInputArgumentNull;
  }

  // clang-format off
  const NodeManagementSubscriptionDeleteCallType node_managment_subscription_delete_call = {
      .subscription_delete = &(SubscriptionManagementDeleteCallType){
          .subscription_id = NULL,
          .client_address  = sender_addr,
          .server_address  = dest_addr,
      },
  };
  // clang-format on

  return SendNodeManagmentCall(
      SENDER(self),
      &node_managment_subscription_delete_call,
      kFunctionTypeNodeManagementSubscriptionDeleteCall,
      sender_addr->device,
      dest_addr->device
  );
}

EebusError CallBind(
    SenderObject* self,
    const FeatureAddressType* sender_addr,
    const FeatureAddressType* dest_addr,
    FeatureTypeType server_feature_type
) {
  if ((sender_addr == NULL) || (dest_addr == NULL)) {
    return kEebusErrorInputArgumentNull;
  }

  // clang-format off
  const NodeManagementBindingRequestCallType node_management_binding_request_call = {
      .binding_request = &(BindingManagementRequestCallType){
          .client_address      = sender_addr,
          .server_address      = dest_addr,
          .server_feature_type = &server_feature_type,
      },
  };
  // clang-format on

  return SendNodeManagmentCall(
      SENDER(self),
      &node_management_binding_request_call,
      kFunctionTypeNodeManagementBindingRequestCall,
      sender_addr->device,
      dest_addr->device
  );
}

EebusError CallUnbind(SenderObject* self, const FeatureAddressType* sender_addr, const FeatureAddressType* dest_addr) {
  if ((sender_addr == NULL) || (dest_addr == NULL)) {
    return kEebusErrorInputArgumentNull;
  }

  // clang-format off
  const NodeManagementBindingDeleteCallType node_management_binding_delete_call = {
      .binding_delete = &(BindingManagementDeleteCallType){
          .binding_id     = NULL,
          .client_address = sender_addr,
          .server_address = dest_addr,
      },
  };
  // clang-format on

  return SendNodeManagmentCall(
      SENDER(self),
      &node_management_binding_delete_call,
      kFunctionTypeNodeManagementBindingDeleteCall,
      sender_addr->device,
      dest_addr->device
  );
}

EebusError SendResult(
    Sender* self,
    const HeaderType* request_header,
    const FeatureAddressType* sender_addr,
    const ErrorType* err
) {
  if ((request_header == NULL) || (sender_addr == NULL)) {
    return kEebusErrorInputArgumentNull;
  }

  if (request_header->dest_addr == NULL) {
    return kEebusErrorInputArgument;
  }

  const FeatureAddressType src_addr = {
      .device      = sender_addr->device,
      .entity      = request_header->dest_addr->entity,
      .entity_size = request_header->dest_addr->entity_size,
      .feature     = request_header->dest_addr->feature,
  };

  const ErrorNumberType err_number = (err != NULL) ? err->error_number : kErrorNumberTypeNoError;
  const ResultDataType result_data = {
      .error_number = &err_number,
      .description  = (err != NULL) ? (char*)err->description : NULL,
  };

  const CmdType cmd = {
      .data_choice         = &result_data,
      .data_choice_type_id = kFunctionTypeResultData,
  };

  return SendSpineMessage(
      self,
      kCommandClassifierTypeResult,
      &src_addr,
      request_header->src_addr,
      request_header->msg_cnt,
      false,
      &cmd,
      1,
      NULL
  );
}

EebusError ResultSuccess(SenderObject* self, const HeaderType* request_header, const FeatureAddressType* sender_addr) {
  return SendResult(SENDER(self), request_header, sender_addr, NULL);
}

EebusError ResultError(
    SenderObject* self,
    const HeaderType* request_header,
    const FeatureAddressType* sender_addr,
    const ErrorType* err
) {
  return SendResult(SENDER(self), request_header, sender_addr, err);
}
