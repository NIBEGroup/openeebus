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
 * @brief CS LPC Approver implementation
 */

#include "examples/heat_pump/cs_lpc_approver.h"

#include <inttypes.h>
#include <stdio.h>

#include "src/common/eebus_arguments.h"
#include "src/common/eebus_date_time/eebus_date_time.h"
#include "src/common/eebus_malloc.h"
#include "src/spine/model/error_types.h"
#include "src/spine/model/result_types.h"
#include "src/use_case/model/scaled_value.h"

typedef struct CsLpcApprover CsLpcApprover;

struct CsLpcApprover {
  /** Implements the CS LPC Approver Interface */
  CsLpcApproverObject obj;

  CsLpUseCaseObject* cs_lpc;
};

#define CS_LPC_APPROVER(obj) ((CsLpcApprover*)(obj))

static void Destruct(CsLpcApproverObject* self);
static void OnPowerLimitApprovalRequested(
    CsLpcApproverObject* self,
    const char* ski,
    MsgCounterType msg_cnt,
    const ScaledValue* limit,
    const DurationType* duration,
    bool is_active
);
static void OnFailsafeValueApprovalRequested(
    CsLpcApproverObject* self,
    const char* ski,
    MsgCounterType msg_cnt,
    const ScaledValue* value
);
static void OnFailsafeDurationApprovalRequested(
    CsLpcApproverObject* self,
    const char* ski,
    MsgCounterType msg_cnt,
    const DurationType* duration
);
static void OnApprovalRequestExpired(CsLpcApproverObject* self, const char* ski, MsgCounterType msg_cnt);

static const CsLpcApproverInterface cs_lpc_approver_methods = {
    .destruct                                = Destruct,
    .on_power_limit_approval_requested       = OnPowerLimitApprovalRequested,
    .on_failsafe_value_approval_requested    = OnFailsafeValueApprovalRequested,
    .on_failsafe_duration_approval_requested = OnFailsafeDurationApprovalRequested,
    .on_approval_request_expired             = OnApprovalRequestExpired,
};

static EebusError CsLpcApproverConstruct(CsLpcApprover* self, CsLpUseCaseObject* cs_lpc);

EebusError CsLpcApproverConstruct(CsLpcApprover* self, CsLpUseCaseObject* cs_lpc) {
  // Override "virtual functions table"
  CS_LPC_APPROVER_INTERFACE(self) = &cs_lpc_approver_methods;

  self->cs_lpc = cs_lpc;

  return kEebusErrorOk;
}

CsLpcApproverObject* CsLpcApproverCreate(CsLpUseCaseObject* cs_lpc) {
  CsLpcApprover* const cs_lpc_approver = (CsLpcApprover*)EEBUS_MALLOC(sizeof(CsLpcApprover));
  if (cs_lpc_approver == NULL) {
    return NULL;
  }

  if (CsLpcApproverConstruct(cs_lpc_approver, cs_lpc) != kEebusErrorOk) {
    CsLpcApproverDelete(CS_LPC_APPROVER_OBJECT(cs_lpc_approver));
    return NULL;
  }

  return CS_LPC_APPROVER_OBJECT(cs_lpc_approver);
}

void Destruct(CsLpcApproverObject* self) {
  UNUSED(self);

  // Nothing to be deallocated yet
}

void OnPowerLimitApprovalRequested(
    CsLpcApproverObject* self,
    const char* ski,
    MsgCounterType msg_cnt,
    const ScaledValue* limit,
    const DurationType* duration,
    bool is_active
) {
  UNUSED(is_active);

  CsLpUseCaseObject* const cs_lpc = CS_LPC_APPROVER(self)->cs_lpc;

  double limit_value = 0.0;
  ScaledValueToDouble(limit, &limit_value);
  const int32_t duration_seconds = (int32_t)EebusDurationToSeconds(duration);

  if (CsLpIsLimitValid(limit_value, duration_seconds)) {
    printf(
        "CS LPC approving power limit write from SKI %s: %fW for %" PRId32 "s\n",
        ski,
        limit_value,
        duration_seconds
    );
    CsLpApproveWrite(cs_lpc, ski, msg_cnt);
    return;
  }

  printf("CS LPC denying power limit write from SKI %s: %fW for %" PRId32 "s\n", ski, limit_value, duration_seconds);
  const ErrorType err = {.error_number = kErrorNumberTypeCommandRejected, .description = "Invalid limit or duration"};
  CsLpDenyWrite(cs_lpc, ski, msg_cnt, &err);
}

void OnFailsafeValueApprovalRequested(
    CsLpcApproverObject* self,
    const char* ski,
    MsgCounterType msg_cnt,
    const ScaledValue* value
) {
  UNUSED(value);

  CsLpApproveWrite(CS_LPC_APPROVER(self)->cs_lpc, ski, msg_cnt);
}

void OnFailsafeDurationApprovalRequested(
    CsLpcApproverObject* self,
    const char* ski,
    MsgCounterType msg_cnt,
    const DurationType* duration
) {
  UNUSED(duration);

  CsLpApproveWrite(CS_LPC_APPROVER(self)->cs_lpc, ski, msg_cnt);
}

void OnApprovalRequestExpired(CsLpcApproverObject* self, const char* ski, MsgCounterType msg_cnt) {
  UNUSED(self);

  printf("CS LPC approval request expired for SKI %s, msg_cnt = %" PRIu64 "\n", ski, msg_cnt);
}
