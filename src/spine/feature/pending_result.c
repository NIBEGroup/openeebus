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
 * @brief Pending Result implementation
 */

#include "src/spine/feature/pending_result.h"

#include "src/common/api/eebus_timer_interface.h"
#include "src/common/eebus_arguments.h"
#include "src/common/eebus_countdown/eebus_countdown.h"
#include "src/common/eebus_malloc.h"
#include "src/spine/feature/feature.h"

typedef struct PendingResult PendingResult;

struct PendingResult {
  /** Implements the Pending Result Interface */
  PendingResultObject obj;

  MsgCounterType msg_cnt_ref;
  FunctionType function_type;
  ResultMessageCallback cb;
  void* ctx;
  FeatureAddressType* remote_feature_addr;
  EebusCountdown countdown;
};

#define PENDING_RESULT(obj) ((PendingResult*)(obj))

static void Destruct(PendingResultObject* self);
static MsgCounterType GetMsgCntRef(const PendingResultObject* self);
static bool HasExpired(const PendingResultObject* self);
static void UpdateTime(PendingResultObject* self);
static void Fire(const PendingResultObject* self, const ResultMessage* result_msg, EebusError err);

static const PendingResultInterface pending_result_methods = {
    .destruct        = Destruct,
    .get_msg_cnt_ref = GetMsgCntRef,
    .has_expired     = HasExpired,
    .update_time     = UpdateTime,
    .fire            = Fire,
};

static void PendingResultConstruct(
    PendingResult* self,
    MsgCounterType msg_cnt_ref,
    FunctionType function_type,
    const FeatureAddressType* remote_feature_addr,
    ResultMessageCallback cb,
    void* ctx
);

void PendingResultConstruct(
    PendingResult* self,
    MsgCounterType msg_cnt_ref,
    FunctionType function_type,
    const FeatureAddressType* remote_feature_addr,
    ResultMessageCallback cb,
    void* ctx
) {
  PENDING_RESULT_INTERFACE(self) = &pending_result_methods;

  self->msg_cnt_ref         = msg_cnt_ref;
  self->function_type       = function_type;
  self->remote_feature_addr = FeatureAddressCopy(remote_feature_addr);
  self->cb                  = cb;
  self->ctx                 = ctx;
  self->countdown           = EEBUS_COUNTDOWN(TIME_MS_TO_S(kDefaultMaxResponseDelayMs));
}

PendingResultObject* PendingResultCreate(
    MsgCounterType msg_cnt_ref,
    FunctionType function_type,
    const FeatureAddressType* remote_feature_addr,
    ResultMessageCallback cb,
    void* ctx
) {
  PendingResult* const self = (PendingResult*)EEBUS_MALLOC(sizeof(PendingResult));
  if (self == NULL) {
    return NULL;
  }

  PendingResultConstruct(self, msg_cnt_ref, function_type, remote_feature_addr, cb, ctx);
  return PENDING_RESULT_OBJECT(self);
}

void Destruct(PendingResultObject* self) {
  PendingResult* const pr = PENDING_RESULT(self);
  FeatureAddressDelete(pr->remote_feature_addr);
}

MsgCounterType GetMsgCntRef(const PendingResultObject* self) {
  return PENDING_RESULT(self)->msg_cnt_ref;
}

bool HasExpired(const PendingResultObject* self) {
  return EebusCountdownHasExpired(&PENDING_RESULT(self)->countdown);
}

void UpdateTime(PendingResultObject* self) {
  EebusCountdownTick(&PENDING_RESULT(self)->countdown);
}

void Fire(const PendingResultObject* self, const ResultMessage* result_msg, EebusError err) {
  const PendingResult* const pr = PENDING_RESULT(self);
  if (err == kEebusErrorOk) {
    ResultMessage msg_with_type = *result_msg;
    msg_with_type.function_type = pr->function_type;
    pr->cb(&msg_with_type, pr->remote_feature_addr, err, pr->ctx);
  } else {
    const ResultMessage timeout_msg = {.msg_cnt_ref = pr->msg_cnt_ref, .function_type = pr->function_type};
    pr->cb(&timeout_msg, pr->remote_feature_addr, err, pr->ctx);
  }
}
