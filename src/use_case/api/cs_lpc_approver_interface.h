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
 * @brief Cs Lpc Approver interface declarations
 */

#ifndef SRC_USE_CASE_API_CS_LPC_APPROVER_INTERFACE_H_
#define SRC_USE_CASE_API_CS_LPC_APPROVER_INTERFACE_H_

#include "src/spine/model/command_frame_types.h"
#include "src/spine/model/common_data_types.h"
#include "src/use_case/model/scaled_value.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @brief Cs Lpc Approver Interface
 * (Cs Lpc Approver "virtual functions table" declaration)
 */
typedef struct CsLpcApproverInterface CsLpcApproverInterface;

/**
 * @brief Cs Lpc Approver Object type definition
 * ("abstract class", has no members but only pointer to
 * "virtual functions table")
 */
typedef struct CsLpcApproverObject CsLpcApproverObject;

/**
 * @brief CsLpcApprover Interface Structure
 */
struct CsLpcApproverInterface {
  void (*destruct)(CsLpcApproverObject* self);
  void (*on_power_limit_approval_requested)(
      CsLpcApproverObject* self,
      const char* ski,
      MsgCounterType msg_cnt,
      const ScaledValue* limit,
      const DurationType* duration,
      bool is_active
  );
  void (*on_failsafe_value_approval_requested)(
      CsLpcApproverObject* self,
      const char* ski,
      MsgCounterType msg_cnt,
      const ScaledValue* value
  );
  void (*on_failsafe_duration_approval_requested)(
      CsLpcApproverObject* self,
      const char* ski,
      MsgCounterType msg_cnt,
      const DurationType* duration
  );
  void (*on_approval_request_expired)(CsLpcApproverObject* self, const char* ski, MsgCounterType msg_cnt);
};

/**
 * @brief Cs Lpc Approver Object Structure
 */
struct CsLpcApproverObject {
  const CsLpcApproverInterface* interface_;
};

/**
 * @brief Cs Lpc Approver pointer typecast
 */
#define CS_LPC_APPROVER_OBJECT(obj) ((CsLpcApproverObject*)(obj))

/**
 * @brief Cs Lpc Approver Interface class pointer typecast
 */
#define CS_LPC_APPROVER_INTERFACE(obj) (CS_LPC_APPROVER_OBJECT(obj)->interface_)

/**
 * @brief Cs Lpc Approver Destruct caller definition
 */
#define CS_LPC_APPROVER_DESTRUCT(obj) (CS_LPC_APPROVER_INTERFACE(obj)->destruct(obj))

/**
 * @brief Cs Lpc Approver On Power Limit Approval Requested caller definition
 */
#define CS_LPC_APPROVER_ON_POWER_LIMIT_APPROVAL_REQUESTED(obj, ski, msg_cnt, limit, duration, is_active) \
  (CS_LPC_APPROVER_INTERFACE(obj)->on_power_limit_approval_requested(obj, ski, msg_cnt, limit, duration, is_active))

/**
 * @brief Cs Lpc Approver On Failsafe Value Approval Requested caller definition
 */
#define CS_LPC_APPROVER_ON_FAILSAFE_VALUE_APPROVAL_REQUESTED(obj, ski, msg_cnt, value) \
  (CS_LPC_APPROVER_INTERFACE(obj)->on_failsafe_value_approval_requested(obj, ski, msg_cnt, value))

/**
 * @brief Cs Lpc Approver On Failsafe Duration Approval Requested caller definition
 */
#define CS_LPC_APPROVER_ON_FAILSAFE_DURATION_APPROVAL_REQUESTED(obj, ski, msg_cnt, duration) \
  (CS_LPC_APPROVER_INTERFACE(obj)->on_failsafe_duration_approval_requested(obj, ski, msg_cnt, duration))

/**
 * @brief Cs Lpc Approver On Approval Request Expired caller definition
 */
#define CS_LPC_APPROVER_ON_APPROVAL_REQUEST_EXPIRED(obj, ski, msg_cnt) \
  (CS_LPC_APPROVER_INTERFACE(obj)->on_approval_request_expired(obj, ski, msg_cnt))

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_USE_CASE_API_CS_LPC_APPROVER_INTERFACE_H_
