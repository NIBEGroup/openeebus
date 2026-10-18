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
 * @brief CS LPC Approver Mock "class"
 */

#ifndef TESTS_SRC_MOCKS_USE_CASE_API_CS_LPC_APPROVER_MOCK_H_
#define TESTS_SRC_MOCKS_USE_CASE_API_CS_LPC_APPROVER_MOCK_H_

#include <gmock/gmock.h>

#include <memory>

#include "src/common/eebus_malloc.h"
#include "src/use_case/api/cs_lpc_approver_interface.h"

class CsLpcApproverGMockInterface {
 public:
  virtual ~CsLpcApproverGMockInterface() {};
  virtual void Destruct(CsLpcApproverObject* self) = 0;
  virtual void OnPowerLimitApprovalRequested(
      CsLpcApproverObject* self,
      const char* ski,
      MsgCounterType msg_cnt,
      const ScaledValue* limit,
      const DurationType* duration,
      bool is_active
  ) = 0;
  virtual void OnFailsafeValueApprovalRequested(
      CsLpcApproverObject* self,
      const char* ski,
      MsgCounterType msg_cnt,
      const ScaledValue* value
  ) = 0;
  virtual void OnFailsafeDurationApprovalRequested(
      CsLpcApproverObject* self,
      const char* ski,
      MsgCounterType msg_cnt,
      const DurationType* duration
  )                                                                                                         = 0;
  virtual void OnApprovalRequestExpired(CsLpcApproverObject* self, const char* ski, MsgCounterType msg_cnt) = 0;
};

class CsLpcApproverGMock : public CsLpcApproverGMockInterface {
 public:
  virtual ~CsLpcApproverGMock() {};
  MOCK_METHOD1(Destruct, void(CsLpcApproverObject*));
  MOCK_METHOD6(
      OnPowerLimitApprovalRequested,
      void(CsLpcApproverObject*, const char*, MsgCounterType, const ScaledValue*, const DurationType*, bool)
  );
  MOCK_METHOD4(
      OnFailsafeValueApprovalRequested,
      void(CsLpcApproverObject*, const char*, MsgCounterType, const ScaledValue*)
  );
  MOCK_METHOD4(
      OnFailsafeDurationApprovalRequested,
      void(CsLpcApproverObject*, const char*, MsgCounterType, const DurationType*)
  );
  MOCK_METHOD3(OnApprovalRequestExpired, void(CsLpcApproverObject*, const char*, MsgCounterType));
};

typedef struct CsLpcApproverMock {
  /** Implements the CS LPC Approver Interface */
  CsLpcApproverObject obj;
  CsLpcApproverGMock* gmock;
} CsLpcApproverMock;

#define CS_LPC_APPROVER_MOCK(obj) ((CsLpcApproverMock*)(obj))

CsLpcApproverMock* CsLpcApproverMockCreate(void);

static inline void CsLpcApproverMockDelete(CsLpcApproverMock* self) {
  if (self != nullptr) {
    CS_LPC_APPROVER_DESTRUCT(CS_LPC_APPROVER_OBJECT(self));
    EEBUS_FREE(self);
  }
}

#endif  // TESTS_SRC_MOCKS_USE_CASE_API_CS_LPC_APPROVER_MOCK_H_
