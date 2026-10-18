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
 * @brief CS LPC Approver mock implementation
 */

#include "cs_lpc_approver_mock.h"

#include <gmock/gmock.h>

#include "src/use_case/api/cs_lpc_approver_interface.h"

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

static const CsLpcApproverInterface cs_lpc_approver_mock_methods = {
    .destruct                                = Destruct,
    .on_power_limit_approval_requested       = OnPowerLimitApprovalRequested,
    .on_failsafe_value_approval_requested    = OnFailsafeValueApprovalRequested,
    .on_failsafe_duration_approval_requested = OnFailsafeDurationApprovalRequested,
    .on_approval_request_expired             = OnApprovalRequestExpired,
};

static EebusError CsLpcApproverMockConstruct(CsLpcApproverMock* self);

EebusError CsLpcApproverMockConstruct(CsLpcApproverMock* self) {
  // Override "virtual functions table"
  CS_LPC_APPROVER_INTERFACE(self) = &cs_lpc_approver_mock_methods;

  self->gmock = new CsLpcApproverGMock();
  if (self->gmock == nullptr) {
    return kEebusErrorMemoryAllocate;
  }

  return kEebusErrorOk;
}

CsLpcApproverMock* CsLpcApproverMockCreate(void) {
  CsLpcApproverMock* const mock = (CsLpcApproverMock*)EEBUS_MALLOC(sizeof(CsLpcApproverMock));
  if (mock == nullptr) {
    return nullptr;
  }

  if (CsLpcApproverMockConstruct(mock) != kEebusErrorOk) {
    CsLpcApproverMockDelete(mock);
    return nullptr;
  }

  return mock;
}

void Destruct(CsLpcApproverObject* self) {
  CsLpcApproverMock* const mock = CS_LPC_APPROVER_MOCK(self);
  mock->gmock->Destruct(self);
  delete mock->gmock;
}

void OnPowerLimitApprovalRequested(
    CsLpcApproverObject* self,
    const char* ski,
    MsgCounterType msg_cnt,
    const ScaledValue* limit,
    const DurationType* duration,
    bool is_active
) {
  CsLpcApproverMock* const mock = CS_LPC_APPROVER_MOCK(self);
  mock->gmock->OnPowerLimitApprovalRequested(self, ski, msg_cnt, limit, duration, is_active);
}

void OnFailsafeValueApprovalRequested(
    CsLpcApproverObject* self,
    const char* ski,
    MsgCounterType msg_cnt,
    const ScaledValue* value
) {
  CsLpcApproverMock* const mock = CS_LPC_APPROVER_MOCK(self);
  mock->gmock->OnFailsafeValueApprovalRequested(self, ski, msg_cnt, value);
}

void OnFailsafeDurationApprovalRequested(
    CsLpcApproverObject* self,
    const char* ski,
    MsgCounterType msg_cnt,
    const DurationType* duration
) {
  CsLpcApproverMock* const mock = CS_LPC_APPROVER_MOCK(self);
  mock->gmock->OnFailsafeDurationApprovalRequested(self, ski, msg_cnt, duration);
}

void OnApprovalRequestExpired(CsLpcApproverObject* self, const char* ski, MsgCounterType msg_cnt) {
  CsLpcApproverMock* const mock = CS_LPC_APPROVER_MOCK(self);
  mock->gmock->OnApprovalRequestExpired(self, ski, msg_cnt);
}
