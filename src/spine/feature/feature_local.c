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
 * @brief Feature Local implementation
 */

#include <string.h>

#include "src/common/eebus_arguments.h"
#include "src/common/eebus_malloc.h"
#include "src/common/uint64_lut.h"
#include "src/spine/api/device_local_interface.h"
#include "src/spine/api/message.h"
#include "src/spine/api/pending_write_request_interface.h"
#include "src/spine/events/events.h"
#include "src/spine/feature/feature.h"
#include "src/spine/feature/feature_local_internal.h"
#include "src/spine/feature/pending_write_request.h"
#include "src/spine/model/cmd.h"
#include "src/spine/model/result_types.h"

typedef struct ResponseMessageCbRecord ResponseMessageCbRecord;

struct ResponseMessageCbRecord {
  ResponseMessageCallback cb;
  void* ctx;
};

typedef struct WriteApprovalCbRecord WriteApprovalCbRecord;

struct WriteApprovalCbRecord {
  WriteApprovalCallback cb;
  void* ctx;
};

static EebusError HandleMessage(FeatureLocalObject* self, const Message* msg);

static const FeatureLocalInterface feature_local_methods = {
     .feature_interface = {
         .destruct                = FeatureLocalDestruct,
         .get_address             = FeatureGetAddress,
         .get_type                = FeatureGetType,
         .get_role                = FeatureGetRole,
         .get_function_operations = FeatureGetFunctionOperations,
         .get_description         = FeatureGetDescription,
         .set_description         = FeatureSetDescription,
         .to_string               = FeatureToString,
     },
 
     .get_device                             = FeatureLocalGetDevice,
     .get_entity                             = FeatureLocalGetEntity,
     .get_data                               = FeatureLocalGetData,
     .set_function_operations                = FeatureLocalSetFunctionOperations,
     .add_response_callback                  = FeatureLocalAddResponseCallback,
     .add_result_callback                    = FeatureLocalAddResultCallback,
     .add_write_approval_callback            = FeatureLocalAddWriteApprovalCallback,
     .try_approve_write                      = FeatureLocalTryApproveWrite,
     .deny_write                             = FeatureLocalDenyWrite,
     .clean_remote_device_caches             = FeatureLocalCleanRemoteDeviceCaches,
     .data_copy                              = FeatureLocalDataCopy,
     .update_data                            = FeatureLocalUpdateData,
     .set_data                               = FeatureLocalSetData,
     .request_remote_data                    = FeatureLocalRequestRemoteData,
     .request_remote_data_by_sender_address  = FeatureLocalRequestRemoteDataBySenderAddress,
     .has_subscription_to_remote             = FeatureLocalHasSubscriptionToRemote,
     .subscribe_to_remote                    = FeatureLocalSubscribeToRemote,
     .remove_remote_subscription             = FeatureLocalRemoveRemoteSubscription,
     .remove_all_remote_subscriptions        = FeatureLocalRemoveAllRemoteSubscriptions,
     .has_binding_to_remote                  = FeatureLocalHasBindingToRemote,
     .bind_to_remote                         = FeatureLocalBindToRemote,
     .remove_remote_binding                  = FeatureLocalRemoveRemoteBinding,
     .remove_all_remote_bindings             = FeatureLocalRemoveAllRemoteBindings,
     .handle_message                         = HandleMessage,
     .create_information                     = FeatureLocalCreateInformation,
     .tick                                   = FeatureLocalTick,
 };

static EebusError FunctionUpdateNotifySubscribers(
    const FeatureLocal* self,
    const FunctionObject* function,
    const void* new_data,
    const FilterType* filter_partial,
    const FilterType* filter_delete
);
static SenderObject* GetRemoteDeviceSender(const FeatureLocal* self, const FeatureAddressType* remote_addr);
static EebusError ProcessRead(FeatureLocal* self, const Message* msg);
static void PublishDataUpdateEvent(
    const FeatureLocal* self,
    FeatureRemoteObject* feature_remote,
    FunctionType function_type,
    const void* const new_data,
    CommandClassifierType cmd_classifier
);
static EebusError ProcessNotify(FeatureLocal* self, const Message* msg);
static EebusError ProcessWriteInternal(FeatureLocal* self, const Message* msg);
static EebusError ProcessWrite(FeatureLocal* self, const Message* msg);
static EebusError ProcessReply(FeatureLocal* self, const Message* msg);
static void AddPendingWriteRequest(FeatureLocal* self, const Message* msg);
static PendingWriteRequestObject*
FindPendingWriteRequest(const FeatureLocal* self, const char* ski, MsgCounterType msg_cnt);
static void FeatureLocalUpdatePendingWriteRequestTime(FeatureLocalObject* self);

void FeatureLocalConstruct(
    FeatureLocal* self,
    uint32_t id,
    EntityLocalObject* entity,
    FeatureTypeType type,
    RoleType role
) {
  FeatureConstruct(FEATURE(self), type, ENTITY_GET_ADDRESS(ENTITY_OBJECT(entity)), id, role);

  //  Override "virtual functions table"
  FEATURE_LOCAL_INTERFACE(self) = &feature_local_methods;

  self->entity = entity;
  Uint64LutConstruct(&self->resp_msg_cbs);
  VectorConstruct(&self->result_cbs);
  VectorConstruct(&self->wr_approval_cbs);
  VectorConstruct(&self->pending_write_requests);

  FeatureAddressContainerConstruct(&self->bindings);
  FeatureAddressContainerConstruct(&self->subscriptions);
}

FeatureLocalObject* FeatureLocalCreate(uint32_t id, EntityLocalObject* entity, FeatureTypeType type, RoleType role) {
  FeatureLocal* const feature_local = (FeatureLocal*)EEBUS_MALLOC(sizeof(FeatureLocal));

  FeatureLocalConstruct(feature_local, id, entity, type, role);

  return FEATURE_LOCAL_OBJECT(feature_local);
}

void FeatureLocalDestruct(FeatureObject* self) {
  FeatureLocal* const fl = FEATURE_LOCAL(self);

  FeatureAddressContainerDestruct(&fl->subscriptions);
  FeatureAddressContainerDestruct(&fl->bindings);

  VectorFreeElements(&fl->pending_write_requests);
  VectorDestruct(&fl->pending_write_requests);
  VectorFreeElements(&fl->wr_approval_cbs);
  VectorDestruct(&fl->wr_approval_cbs);
  VectorFreeElements(&fl->result_cbs);
  VectorDestruct(&fl->result_cbs);
  Uint64LutDestruct(&fl->resp_msg_cbs);

  // TODO: Implement destructor
  FeatureDestruct(self);
}

DeviceLocalObject* FeatureLocalGetDevice(const FeatureLocalObject* self) {
  const FeatureLocal* const fl = FEATURE_LOCAL(self);
  return ENTITY_LOCAL_GET_DEVICE(fl->entity);
}

EntityLocalObject* FeatureLocalGetEntity(const FeatureLocalObject* self) {
  return FEATURE_LOCAL(self)->entity;
}

const void* FeatureLocalGetData(const FeatureLocalObject* self, FunctionType function_type) {
  const FunctionObject* const function = FeatureGetFunction(FEATURE(self), function_type);
  if (function == NULL) {
    return NULL;
  }

  return FUNCTION_GET_DATA(function);
}

void FeatureLocalSetFunctionOperations(FeatureLocalObject* self, FunctionType type, bool read, bool write) {
  const Feature* const feature = FEATURE(self);

  if ((feature->role != kRoleTypeServer) && (feature->role != kRoleTypeSpecial)) {
    return;
  }

  FunctionObject* const function = FeatureGetFunction(feature, type);
  if (function == NULL) {
    return;
  }

  // Partial reads are currently not supported
  FUNCTION_SET_OPERATIONS(function, read, false, write, true);

  if ((feature->role == kRoleTypeServer) && (feature->type == kFeatureTypeTypeDeviceDiagnosis)
      && (type == kFunctionTypeDeviceDiagnosisHeartbeatData)) {
    // Update HeartbeatManager
    const FeatureLocal* const fl = FEATURE_LOCAL(self);
    HeartbeatManagerObject* hm   = ENTITY_LOCAL_GET_HEARTBEAT_MANAGER(fl->entity);
    HEARTBEAT_MANAGER_SET_LOCAL_FEATURE(hm, fl->entity, self);
  }
}

ResponseMessageCbRecord* ResponseMessageCbRecordCreate(ResponseMessageCallback cb, void* ctx) {
  ResponseMessageCbRecord* const resp_msg_record
      = (ResponseMessageCbRecord*)EEBUS_MALLOC(sizeof(ResponseMessageCbRecord));
  if (resp_msg_record != NULL) {
    resp_msg_record->cb  = cb;
    resp_msg_record->ctx = ctx;
  }

  return resp_msg_record;
}

void ResponseMessageContainerDelete(void* p) {
  if (p == NULL) {
    return;
  }

  Vector* resp_msg_cbs_vec = (Vector*)p;
  VectorFreeElements(resp_msg_cbs_vec);
  VectorDestruct(resp_msg_cbs_vec);
  EEBUS_FREE(resp_msg_cbs_vec);
}

EebusError FeatureLocalAddResponseCallback(
    FeatureLocalObject* self,
    MsgCounterType msg_counter_ref,
    ResponseMessageCallback cb,
    void* ctx
) {
  FeatureLocal* const fl = FEATURE_LOCAL(self);

  Vector* resp_msg_cbs_vec = (Vector*)Uint64LutFind(&fl->resp_msg_cbs, msg_counter_ref);

  if (resp_msg_cbs_vec != NULL) {
    for (size_t i = 0; i < VectorGetSize(resp_msg_cbs_vec); ++i) {
      const ResponseMessageCbRecord* const resp_msg_cb_record = VectorGetElement(resp_msg_cbs_vec, i);
      if ((resp_msg_cb_record->cb == cb) && (resp_msg_cb_record->ctx == ctx)) {
        return kEebusErrorNoChange;
      }
    }
  } else {
    resp_msg_cbs_vec = VectorCreate();
    Uint64LutInsert(&fl->resp_msg_cbs, msg_counter_ref, resp_msg_cbs_vec, ResponseMessageContainerDelete);
  }

  VectorPushBack(resp_msg_cbs_vec, ResponseMessageCbRecordCreate(cb, ctx));
  return kEebusErrorOk;
}

void ProcessResponseMsgCallbacks(FeatureLocal* self, MsgCounterType msg_counter_ref, const ResponseMessage* resp_msg) {
  Vector* resp_msg_cbs_vec = Uint64LutFind(&self->resp_msg_cbs, msg_counter_ref);
  if (resp_msg_cbs_vec == NULL) {
    return;
  }

  for (size_t i = 0; i < VectorGetSize(resp_msg_cbs_vec); ++i) {
    ResponseMessageCbRecord* const resp_msg_record = VectorGetElement(resp_msg_cbs_vec, i);
    resp_msg_record->cb(resp_msg, resp_msg_record->ctx);
  }

  Uint64LutRemove(&self->resp_msg_cbs, msg_counter_ref);
}

void FeatureLocalAddResultCallback(FeatureLocalObject* self, ResponseMessageCallback cb, void* ctx) {
  FeatureLocal* const fl = FEATURE_LOCAL(self);
  VectorPushBack(&fl->result_cbs, ResponseMessageCbRecordCreate(cb, ctx));
}

void ProcessResultCallbacks(FeatureLocal* self, const ResponseMessage* resp_msg) {
  for (size_t i = 0; i < VectorGetSize(&self->result_cbs); ++i) {
    ResponseMessageCbRecord* const resp_msg_record = VectorGetElement(&self->result_cbs, i);
    resp_msg_record->cb(resp_msg, resp_msg_record->ctx);
  }
}

WriteApprovalCbRecord* WriteApprovalCbRecordCreate(WriteApprovalCallback cb, void* ctx) {
  WriteApprovalCbRecord* const wr_approval_record = (WriteApprovalCbRecord*)EEBUS_MALLOC(sizeof(WriteApprovalCbRecord));
  if (wr_approval_record != NULL) {
    wr_approval_record->cb  = cb;
    wr_approval_record->ctx = ctx;
  }

  return wr_approval_record;
}

EebusError FeatureLocalAddWriteApprovalCallback(FeatureLocalObject* self, WriteApprovalCallback cb, void* ctx) {
  if (FEATURE_GET_ROLE(FEATURE_OBJECT(self)) != kRoleTypeServer) {
    return kEebusErrorNoChange;
  }

  FeatureLocal* const fl = FEATURE_LOCAL(self);

  VectorPushBack(&fl->wr_approval_cbs, WriteApprovalCbRecordCreate(cb, ctx));

  return kEebusErrorOk;
}

EebusError FeatureLocalTryApproveWrite(FeatureLocalObject* self, const char* ski, MsgCounterType msg_cnt) {
  FeatureLocal* const fl = FEATURE_LOCAL(self);

  if ((FEATURE_GET_ROLE(FEATURE_OBJECT(self)) != kRoleTypeServer) || (ski == NULL)) {
    return kEebusErrorInputArgument;
  }

  // Get pending write request for ski and message count
  PendingWriteRequestObject* pending_write_request = FindPendingWriteRequest(fl, ski, msg_cnt);
  if (pending_write_request == NULL) {
    return kEebusErrorNoChange;
  }

  PENDING_WRITE_REQUEST_ADD_APPROVAL(pending_write_request);

  // Check if there are enough approvals for the write
  size_t num_req_approvals = VectorGetSize(&fl->wr_approval_cbs);
  if (num_req_approvals > PENDING_WRITE_REQUEST_GET_NUMBER_OF_APPROVALS(pending_write_request)) {
    return kEebusErrorOk;
  }

  // If there are enough write approvals, remove pending request and process the write
  Message msg;
  EebusError status = PENDING_WRITE_REQUEST_GET_MESSAGE(pending_write_request, self, &msg);
  if (status != kEebusErrorOk) {
    return status;
  }

  status = ProcessWriteInternal(FEATURE_LOCAL(self), &msg);
  VectorRemove(&fl->pending_write_requests, pending_write_request);
  PendingWriteRequestDelete(pending_write_request);

  return status;
}

EebusError
FeatureLocalDenyWrite(FeatureLocalObject* self, const char* ski, MsgCounterType msg_cnt, const ErrorType* err) {
  FeatureLocal* const fl = FEATURE_LOCAL(self);

  if ((FEATURE_GET_ROLE(FEATURE_OBJECT(self)) != kRoleTypeServer) || (ski == NULL)) {
    return kEebusErrorInputArgument;
  }

  if ((err == NULL) || (err->error_number == kErrorNumberTypeNoError)) {
    return kEebusErrorInputArgument;
  }

  // Get pending write request for ski and message count
  PendingWriteRequestObject* pending_write_request = FindPendingWriteRequest(fl, ski, msg_cnt);
  if (pending_write_request == NULL) {
    return kEebusErrorNoChange;
  }

  Message msg;
  EebusError status = PENDING_WRITE_REQUEST_GET_MESSAGE(pending_write_request, self, &msg);
  if (status != kEebusErrorOk) {
    return status;
  }

  SEND_RESULT_ERROR(MessageGetSender(&msg), msg.request_header, FEATURE_GET_ADDRESS(FEATURE_OBJECT(self)), err);
  VectorRemove(&fl->pending_write_requests, pending_write_request);
  PendingWriteRequestDelete(pending_write_request);

  return kEebusErrorOk;
}

void ProcessWriteApprovalCallbacks(FeatureLocal* self, const Message* msg) {
  for (size_t i = 0; i < VectorGetSize(&self->wr_approval_cbs); ++i) {
    const WriteApprovalCbRecord* const wr_approval_record
        = (WriteApprovalCbRecord*)VectorGetElement(&self->wr_approval_cbs, i);
    wr_approval_record->cb(msg, wr_approval_record->ctx);
  }
}

void* FeatureLocalDataCopy(const FeatureLocalObject* self, FunctionType function_type) {
  const FunctionObject* function = FeatureGetFunction(FEATURE(self), function_type);
  if (function == NULL) {
    return NULL;
  }

  return FUNCTION_DATA_COPY(function);
}

void FeatureLocalCleanRemoteDeviceCaches(FeatureLocalObject* self, const DeviceAddressType* remote_addr) {
  FeatureLocal* const fl = FEATURE_LOCAL(self);

  if ((remote_addr == NULL) || (remote_addr->device == NULL)) {
    return;
  }

  size_t i = FeatureAddressContainerGetSize(&fl->subscriptions);
  while (i > 0) {
    --i;
    const FeatureAddressType* const addr = FeatureAddressContainerGetElement(&fl->subscriptions, i);
    if ((addr != NULL) && (addr->device != NULL) && (strcmp(addr->device, remote_addr->device) == 0)) {
      FeatureAddressContainerRemove(&fl->subscriptions, addr);
    }
  }

  i = FeatureAddressContainerGetSize(&fl->bindings);
  while (i > 0) {
    --i;
    const FeatureAddressType* const addr = FeatureAddressContainerGetElement(&fl->bindings, i);
    if ((addr != NULL) && (addr->device != NULL) && (strcmp(addr->device, remote_addr->device) == 0)) {
      FeatureAddressContainerRemove(&fl->bindings, addr);
    }
  }
}

EebusError FunctionUpdateNotifySubscribers(
    const FeatureLocal* self,
    const FunctionObject* function,
    const void* new_data,
    const FilterType* filter_partial,
    const FilterType* filter_delete
) {
  DeviceLocalObject* const device      = FEATURE_LOCAL_GET_DEVICE(FEATURE_LOCAL_OBJECT(self));
  const FeatureAddressType* const addr = FEATURE_GET_ADDRESS(FEATURE_OBJECT(self));

  const CmdType* const cmd = FUNCTION_CREATE_NOTIFY_CMD(function, new_data, filter_partial, filter_delete);
  if (cmd == NULL) {
    return kEebusErrorMemoryAllocate;
  }

  DEVICE_LOCAL_NOTIFY_SUBSCRIBERS(device, addr, cmd);
  CmdDelete((CmdType*)cmd);
  return kEebusErrorOk;
}

EebusError FeatureLocalUpdateData(
    FeatureLocalObject* self,
    FunctionType fcn_type,
    const void* data,
    const FilterType* filter_partial,
    const FilterType* filter_delete
) {
  FunctionObject* function = FeatureGetFunction(FEATURE(self), fcn_type);
  if (function == NULL) {
    return kEebusErrorNoChange;
  }

  EebusError err = FUNCTION_UPDATE_DATA(function, data, filter_partial, filter_delete, false, true);
  if (err != kEebusErrorOk) {
    return err;
  }

  return FunctionUpdateNotifySubscribers(FEATURE_LOCAL(self), function, data, filter_partial, filter_delete);
}

void FeatureLocalUpdatePendingWriteRequestTime(FeatureLocalObject* self) {
  FeatureLocal* const fl = FEATURE_LOCAL(self);

  size_t i = VectorGetSize(&fl->pending_write_requests);

  while (i > 0) {
    --i;
    PendingWriteRequestObject* const request = VectorGetElement(&fl->pending_write_requests, i);
    if (request == NULL) {
      continue;
    }

    if (PENDING_WRITE_REQUEST_HAS_EXPIRED(request)) {
      Message msg;
      EebusError status = PENDING_WRITE_REQUEST_GET_MESSAGE(request, self, &msg);
      if (status != kEebusErrorOk) {
        continue;
      }

      const ErrorType err = {
          .error_number = kErrorNumberTypeTimeout,
          .description  = "Write request timed out",
      };

      SEND_RESULT_ERROR(MessageGetSender(&msg), msg.request_header, FEATURE_GET_ADDRESS(FEATURE_OBJECT(self)), &err);
      VectorRemove(&fl->pending_write_requests, request);
      PendingWriteRequestDelete(request);
    } else {
      PENDING_WRITE_REQUEST_UPDATE_REMAINING_TIME(request);
    }
  }
}

void FeatureLocalSetData(FeatureLocalObject* self, FunctionType function_type, void* data) {
  FEATURE_LOCAL_UPDATE_DATA(self, function_type, data, NULL, NULL);
}

EebusError FeatureLocalRequestRemoteData(
    FeatureLocalObject* self,
    FunctionType function_type,
    const FilterType* filter_partial,
    FeatureRemoteObject* dest_feature
) {
  FunctionObject* function = FeatureGetFunction(FEATURE(self), function_type);
  if (function == NULL) {
    return kEebusErrorNoChange;
  }

  const CmdType* cmd = FUNCTION_CREATE_READ_CMD(function, filter_partial);
  if (cmd == NULL) {
    return kEebusErrorMemoryAllocate;
  }

  const DeviceRemoteObject* const dest_device = FEATURE_REMOTE_GET_DEVICE(dest_feature);

  SenderObject* const sender     = DEVICE_REMOTE_GET_SENDER(dest_device);
  const char* const ski          = DEVICE_REMOTE_GET_SKI(dest_device);
  const FeatureAddressType* addr = FEATURE_GET_ADDRESS(FEATURE_OBJECT(dest_feature));
  const uint32_t max_resp_delay  = FEATURE_REMOTE_GET_MAX_RESPONSE_DELAY(dest_feature);

  const EebusError ret
      = FEATURE_LOCAL_REQUEST_REMOTE_DATA_BY_SENDER_ADDRESS(self, cmd, sender, ski, addr, max_resp_delay);
  CmdDelete((CmdType*)cmd);
  return ret;
}

EebusError FeatureLocalRequestRemoteDataBySenderAddress(
    FeatureLocalObject* self,
    const CmdType* cmd,
    SenderObject* sender,
    const char* dest_ski,
    const FeatureAddressType* dest_addr,
    uint32_t max_delay
) {
  UNUSED(dest_ski);
  UNUSED(max_delay);

  return SEND_READ(sender, FEATURE_GET_ADDRESS(FEATURE_OBJECT(self)), dest_addr, cmd);
}

bool FeatureLocalHasSubscriptionToRemote(const FeatureLocalObject* self, const FeatureAddressType* remote_addr) {
  const FeatureLocal* const fl = FEATURE_LOCAL(self);
  return FeatureAddressContainerFind(&fl->subscriptions, remote_addr) != NULL;
}

SenderObject* GetRemoteDeviceSender(const FeatureLocal* self, const FeatureAddressType* remote_addr) {
  if ((remote_addr == NULL) || (remote_addr->device == NULL)) {
    return NULL;
  }

  const DeviceLocalObject* const dl = FEATURE_LOCAL_GET_DEVICE(FEATURE_LOCAL_OBJECT(self));
  if (dl == NULL) {
    return NULL;
  }

  const DeviceRemoteObject* dr = DEVICE_LOCAL_GET_REMOTE_DEVICE_WITH_ADDRESS(dl, remote_addr->device);
  if (dr == NULL) {
    return NULL;
  }

  return DEVICE_REMOTE_GET_SENDER(dr);
}

EebusError FeatureLocalSubscribeToRemote(FeatureLocalObject* self, const FeatureAddressType* remote_addr) {
  FeatureObject* const feature = FEATURE_OBJECT(self);
  if (FEATURE_GET_ROLE(feature) == kRoleTypeServer) {
    return kEebusErrorNoChange;
  }

  FeatureLocal* const fl     = FEATURE_LOCAL(self);
  SenderObject* const sender = GetRemoteDeviceSender(fl, remote_addr);
  if (sender == NULL) {
    return kEebusErrorNoChange;
  }

  const FeatureAddressType* const addr = FEATURE_GET_ADDRESS(feature);
  const FeatureTypeType feature_type   = FEATURE_GET_TYPE(feature);

  const EebusError err = SEND_CALL_SUBSCRIBE(sender, addr, remote_addr, feature_type);
  if (err != kEebusErrorOk) {
    return err;
  }

  FeatureAddressContainerAdd(&fl->subscriptions, remote_addr);
  return kEebusErrorOk;
}

EebusError FeatureLocalRemoveRemoteSubscription(FeatureLocalObject* self, const FeatureAddressType* remote_addr) {
  FeatureLocal* const fl     = FEATURE_LOCAL(self);
  SenderObject* const sender = GetRemoteDeviceSender(fl, remote_addr);
  if (sender == NULL) {
    return kEebusErrorNoChange;
  }

  FeatureObject* const feature = FEATURE_OBJECT(self);

  const FeatureAddressType* const addr = FEATURE_GET_ADDRESS(feature);

  const EebusError err = SEND_CALL_UNSUBSCRIBE(sender, addr, remote_addr);
  if (err != kEebusErrorOk) {
    return err;
  }

  FeatureAddressContainerRemove(&fl->subscriptions, remote_addr);
  return kEebusErrorOk;
}

void FeatureLocalRemoveAllRemoteSubscriptions(FeatureLocalObject* self) {
  FeatureLocal* const fl = FEATURE_LOCAL(self);

  size_t n = 0;
  while ((n = FeatureAddressContainerGetSize(&fl->subscriptions)) != 0) {
    const FeatureAddressType* const addr = FeatureAddressContainerGetElement(&fl->subscriptions, n - 1);
    FEATURE_LOCAL_REMOVE_REMOTE_SUBSCRIPTION(self, addr);
  }
}

bool FeatureLocalHasBindingToRemote(const FeatureLocalObject* self, const FeatureAddressType* remote_addr) {
  const FeatureLocal* const fl = FEATURE_LOCAL(self);
  return FeatureAddressContainerFind(&fl->bindings, remote_addr) != NULL;
}

EebusError FeatureLocalBindToRemote(FeatureLocalObject* self, const FeatureAddressType* remote_addr) {
  FeatureObject* const feature = FEATURE_OBJECT(self);
  if (FEATURE_GET_ROLE(feature) == kRoleTypeServer) {
    return kEebusErrorNoChange;
  }

  FeatureLocal* const fl     = FEATURE_LOCAL(self);
  SenderObject* const sender = GetRemoteDeviceSender(fl, remote_addr);
  if (sender == NULL) {
    return kEebusErrorNoChange;
  }

  const FeatureAddressType* const addr = FEATURE_GET_ADDRESS(feature);
  const FeatureTypeType feature_type   = FEATURE_GET_TYPE(feature);

  const EebusError err = SEND_CALL_BIND(sender, addr, remote_addr, feature_type);
  if (err != kEebusErrorOk) {
    return err;
  }

  FeatureAddressContainerAdd(&fl->bindings, remote_addr);
  return kEebusErrorOk;
}

EebusError FeatureLocalRemoveRemoteBinding(FeatureLocalObject* self, const FeatureAddressType* remote_addr) {
  FeatureLocal* const fl     = FEATURE_LOCAL(self);
  SenderObject* const sender = GetRemoteDeviceSender(fl, remote_addr);
  if (sender == NULL) {
    return kEebusErrorNoChange;
  }

  FeatureObject* const feature = FEATURE_OBJECT(self);

  const FeatureAddressType* const addr = FEATURE_GET_ADDRESS(feature);

  const EebusError err = SEND_CALL_UNBIND(sender, addr, remote_addr);
  if (err != kEebusErrorOk) {
    return err;
  }

  FeatureAddressContainerRemove(&fl->bindings, remote_addr);
  return kEebusErrorOk;
}

void FeatureLocalRemoveAllRemoteBindings(FeatureLocalObject* self) {
  FeatureLocal* const fl = FEATURE_LOCAL(self);

  size_t n = 0;
  while ((n = FeatureAddressContainerGetSize(&fl->bindings)) != 0) {
    const FeatureAddressType* const addr = FeatureAddressContainerGetElement(&fl->bindings, n - 1);
    FEATURE_LOCAL_REMOVE_REMOTE_BINDING(self, addr);
  }
}

EebusError FeatureLocalProcessResult(FeatureLocal* self, const Message* msg) {
  if (msg->cmd->data_choice_type_id != kFunctionTypeResultData) {
    return kEebusErrorInputArgument;
  }

  const ResultDataType* const result_data = msg->cmd->data_choice;
  if ((result_data == NULL) || (result_data->error_number == NULL)) {
    return kEebusErrorNotImplemented;
  }

  if (*result_data->error_number != kErrorNumberTypeNoError) {
    // Error num bers explained in Resource Spec 3.11
    // TODO: Add error logging
  }

  // We don't need to populate this msg if there is no MsgCounterReference
  if ((msg->request_header == NULL) || (msg->request_header->msg_cnt_ref == NULL)) {
    return kEebusErrorOk;
  }

  const MsgCounterType msg_cnt_ref = *msg->request_header->msg_cnt_ref;

  const ResponseMessage resp_msg = {
      .msg_cnt_ref    = msg_cnt_ref,
      .function_data  = result_data,
      .function_type  = kFunctionTypeResultData,
      .feature_local  = FEATURE_LOCAL_OBJECT(self),
      .feature_remote = msg->feature_remote,
      .entity_remote  = msg->entity_remote,
      .device_remote  = msg->device_remote,
  };

  ProcessResponseMsgCallbacks(self, msg_cnt_ref, &resp_msg);
  ProcessResultCallbacks(self, &resp_msg);
  return kEebusErrorOk;
}

EebusError ProcessRead(FeatureLocal* self, const Message* msg) {
  // Is this a read request to a local server/special feature?
  FeatureObject* const feature = FEATURE_OBJECT(self);
  if (FEATURE_GET_ROLE(feature) == kRoleTypeClient) {
    // Read requests to a client feature are not allowed
    return kEebusErrorNoChange;
  }

  const FunctionType function_type = msg->cmd->data_choice_type_id;

  FunctionObject* const function = FeatureGetFunction(FEATURE(feature), function_type);
  if (function == NULL) {
    return kEebusErrorNoChange;
  }

  const CmdType* const cmd = FUNCTION_CREATE_REPLY_CMD(function);
  if (cmd == NULL) {
    return kEebusErrorMemoryAllocate;
  }

  const FeatureAddressType* const addr = FEATURE_GET_ADDRESS(feature);

  const EebusError err = SEND_REPLY(MessageGetSender(msg), msg->request_header, addr, cmd);
  CmdDelete((CmdType*)cmd);
  return err;
}

void PublishDataUpdateEvent(
    const FeatureLocal* self,
    FeatureRemoteObject* feature_remote,
    FunctionType function_type,
    const void* const new_data,
    CommandClassifierType cmd_classifier
) {
  DeviceRemoteObject* const device_remote = FEATURE_REMOTE_GET_DEVICE(feature_remote);

  const EventPayload payload = {
      .ski            = DEVICE_REMOTE_GET_SKI(device_remote),
      .event_type     = kEventTypeDataChange,
      .change_type    = kElementChangeUpdate,
      .feature        = feature_remote,
      .device         = device_remote,
      .entity         = FEATURE_REMOTE_GET_ENTITY(feature_remote),
      .local_feature  = FEATURE_LOCAL_OBJECT(self),
      .function_type  = function_type,
      .function_data  = new_data,
      .cmd_classifier = &cmd_classifier,
  };

  EVENTS_PUBLISH(DEVICE_LOCAL_GET_EVENTS_MANAGER(FEATURE_LOCAL_GET_DEVICE(FEATURE_LOCAL_OBJECT(self))), &payload);
}

EebusError ProcessNotify(FeatureLocal* self, const Message* msg) {
  FeatureRemoteObject* const fr          = msg->feature_remote;
  const FunctionType function_type       = msg->cmd->data_choice_type_id;
  const void* const new_data             = msg->cmd->data_choice;
  const FilterType* const filter_partial = msg->filter_partial;
  const FilterType* const filter_delete  = msg->filter_delete;

  const EebusError err = FEATURE_REMOTE_UPDATE_DATA(fr, function_type, new_data, filter_partial, filter_delete, true);
  if (err != kEebusErrorOk) {
    return err;
  }

  PublishDataUpdateEvent(self, fr, function_type, new_data, kCommandClassifierTypeNotify);

  return kEebusErrorOk;
}

EebusError ProcessWriteFunctionData(FeatureLocal* self, const Message* msg) {
  const FunctionType function_type       = msg->cmd->data_choice_type_id;
  const void* const new_data             = msg->cmd->data_choice;
  const FilterType* const filter_partial = msg->filter_partial;
  const FilterType* const filter_delete  = msg->filter_delete;

  FunctionObject* function = FeatureGetFunction(FEATURE(self), function_type);
  if (function == NULL) {
    return kEebusErrorNoChange;
  }

  EebusError err = FUNCTION_UPDATE_DATA(function, new_data, filter_partial, filter_delete, true, true);
  if (err != kEebusErrorOk) {
    return err;
  }

  FunctionUpdateNotifySubscribers(self, function, new_data, filter_partial, filter_delete);

  PublishDataUpdateEvent(self, msg->feature_remote, function_type, new_data, kCommandClassifierTypeWrite);
  return kEebusErrorOk;
}

EebusError ProcessWriteInternal(FeatureLocal* self, const Message* msg) {
  if (msg->request_header == NULL) {
    return kEebusErrorInvalid;
  }

  SenderObject* const sender = MessageGetSender(msg);

  const FeatureAddressType* const addr = FEATURE_GET_ADDRESS(FEATURE_OBJECT(self));

  if (ProcessWriteFunctionData(self, msg) != kEebusErrorOk) {
    const ErrorType error = {
        .error_number = kErrorNumberTypeGeneralError,
        .description  = "Error processing write request",
    };

    SEND_RESULT_ERROR(sender, msg->request_header, addr, &error);
  } else {
    const bool* const ack_request = msg->request_header->ack_request;
    if ((ack_request != NULL) && (*ack_request)) {
      SEND_RESULT_SUCCESS(sender, msg->request_header, addr);
    }
  }

  return kEebusErrorOk;
}

PendingWriteRequestObject* FindPendingWriteRequest(const FeatureLocal* self, const char* ski, MsgCounterType msg_cnt) {
  if (self == NULL || ski == NULL) {
    return NULL;
  }

  for (size_t i = 0; i < VectorGetSize(&self->pending_write_requests); ++i) {
    PendingWriteRequestObject* const request = VectorGetElement(&self->pending_write_requests, i);
    if ((strcmp(PENDING_WRITE_REQUEST_GET_SKI(request), ski) == 0)
        && (PENDING_WRITE_REQUEST_GET_MESSAGE_COUNTER(request) == msg_cnt)) {
      return request;
    }
  }

  return NULL;
}

void AddPendingWriteRequest(FeatureLocal* self, const Message* msg) {
  Feature* const feature = FEATURE(self);
  if ((FEATURE_GET_ROLE(FEATURE_OBJECT(feature)) != kRoleTypeServer) || msg == NULL || msg->device_remote == NULL
      || msg->request_header == NULL || msg->request_header->msg_cnt == NULL) {
    return;
  }

  const char* ski = DEVICE_REMOTE_GET_SKI(msg->device_remote);

  PendingWriteRequestObject* pending_request = FindPendingWriteRequest(self, ski, *msg->request_header->msg_cnt);
  if (pending_request != NULL) {
    // If we already have a pending write request for this ski and msg_cnt, we don't want to add it again
    return;
  }

  pending_request = PendingWriteRequestCreate(msg);
  if (pending_request == NULL) {
    return;
  }

  VectorPushBack(&self->pending_write_requests, pending_request);
}

EebusError ProcessWrite(FeatureLocal* self, const Message* msg) {
  if (VectorGetSize(&self->wr_approval_cbs) > 0) {
    AddPendingWriteRequest(self, msg);
    ProcessWriteApprovalCallbacks(self, msg);
    return kEebusErrorOk;
  } else {
    return ProcessWriteInternal(self, msg);
  }
}

EebusError ProcessReply(FeatureLocal* self, const Message* msg) {
  const FunctionType function_type       = msg->cmd->data_choice_type_id;
  const void* const new_data             = msg->cmd->data_choice;
  const FilterType* const filter_partial = msg->filter_partial;
  const FilterType* const filter_delete  = msg->filter_delete;
  FeatureRemoteObject* const fr          = msg->feature_remote;

  const EebusError err = FEATURE_REMOTE_UPDATE_DATA(fr, function_type, new_data, filter_partial, filter_delete, true);
  if (err != kEebusErrorOk) {
    return err;
  }

  PublishDataUpdateEvent(self, fr, function_type, new_data, kCommandClassifierTypeReply);

  // We don't need to populate this message if there is no MsgCounterReference
  if ((msg->request_header == NULL) || (msg->request_header->msg_cnt_ref == NULL)) {
    return kEebusErrorOk;
  }

  const MsgCounterType msg_cnt_ref = *msg->request_header->msg_cnt_ref;

  const ResponseMessage resp_msg = {
      .msg_cnt_ref    = msg_cnt_ref,
      .function_data  = new_data,
      .function_type  = function_type,
      .feature_local  = FEATURE_LOCAL_OBJECT(self),
      .feature_remote = msg->feature_remote,
      .entity_remote  = msg->entity_remote,
      .device_remote  = msg->device_remote,
  };

  ProcessResponseMsgCallbacks(self, msg_cnt_ref, &resp_msg);
  return kEebusErrorOk;
}

EebusError HandleMessage(FeatureLocalObject* self, const Message* msg) {
  if ((msg == NULL) || (msg->cmd == NULL) || (msg->cmd->data_choice == NULL)) {
    return kEebusErrorInputArgumentNull;
  }

  FeatureLocal* const fl = FEATURE_LOCAL(self);

  switch (msg->cmd_classifier) {
    case kCommandClassifierTypeResult: return FeatureLocalProcessResult(fl, msg);
    case kCommandClassifierTypeRead: return ProcessRead(fl, msg);
    case kCommandClassifierTypeReply: return ProcessReply(fl, msg);
    case kCommandClassifierTypeNotify: return ProcessNotify(fl, msg);
    case kCommandClassifierTypeWrite: return ProcessWrite(fl, msg);
    default: return kEebusErrorNotImplemented;
  }
}

EebusError AddSupportedFunctions(const FeatureLocal* self, NetworkManagementFeatureDescriptionDataType* description) {
  Feature* feature = FEATURE(self);

  size_t n = 0;
  for (size_t i = 0; i < VectorGetSize(&feature->functions); ++i) {
    FunctionObject* const function = (FunctionObject*)VectorGetElement(&feature->functions, i);
    if (FUNCTION_GET_OPERATIONS(function) != NULL) {
      ++n;
    }
  }

  description->supported_function      = NULL;
  description->supported_function_size = 0;
  if (n == 0) {
    // Ok - nothing to be added
    return kEebusErrorOk;
  }

  const FunctionPropertyType** const supported_function
      = (const FunctionPropertyType**)EEBUS_MALLOC(sizeof(description->supported_function[0]) * n);

  description->supported_function = supported_function;

  if (description->supported_function == NULL) {
    return kEebusErrorMemoryAllocate;
  }

  for (size_t i = 0; i < VectorGetSize(&feature->functions); ++i) {
    FunctionObject* const function = (FunctionObject*)VectorGetElement(&feature->functions, i);

    const OperationsObject* const ops = FUNCTION_GET_OPERATIONS(function);
    if (ops != NULL) {
      const FunctionType function_type = FUNCTION_GET_FUNCTION_TYPE(function);

      const PossibleOperationsType* const ops_info = OPERATIONS_GET_INFORMATION(ops);

      supported_function[description->supported_function_size] = FunctionPropertyCreate(function_type, ops_info);
      if (supported_function[description->supported_function_size] == NULL) {
        return kEebusErrorMemoryAllocate;
      }

      description->supported_function_size++;
    }
  }

  return kEebusErrorOk;
}

NodeManagementDetailedDiscoveryFeatureInformationType* FeatureLocalCreateInformation(const FeatureLocalObject* self) {
  const FeatureLocal* const fl = FEATURE_LOCAL(self);

  const Feature* const feature = FEATURE(self);

  NodeManagementDetailedDiscoveryFeatureInformationType* const info
      = NodeManagementDetailedDiscoveryFeatureInformationCreate(
          feature->address,
          feature->type,
          feature->role,
          feature->description
      );

  if (info == NULL) {
    return NULL;
  }

  if (AddSupportedFunctions(fl, (NetworkManagementFeatureDescriptionDataType*)info->description) != kEebusErrorOk) {
    NodeManagementDetailedDiscoveryFeatureInformationDelete(info);
    return NULL;
  }

  return info;
}

void FeatureLocalTick(FeatureLocalObject* self) {
  FeatureLocalUpdatePendingWriteRequestTime(self);
}
