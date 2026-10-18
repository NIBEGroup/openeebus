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
 * @brief Cs Lp Write Approval Container interface declarations
 */

#ifndef SRC_USE_CASE_API_CS_LP_WRITE_APPROVAL_CONTAINER_INTERFACE_H_
#define SRC_USE_CASE_API_CS_LP_WRITE_APPROVAL_CONTAINER_INTERFACE_H_

#include "src/spine/api/feature_local_interface.h"
#include "src/use_case/actor/cs/cs_lp_pending_approval.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @brief Cs Lp Write Approval Container Interface
 * (Cs Lp Write Approval Container "virtual functions table" declaration)
 */
typedef struct CsLpWriteApprovalContainerInterface CsLpWriteApprovalContainerInterface;

/**
 * @brief Cs Lp Write Approval Container Object type definition
 * ("abstract class", has no members but only pointer to
 * "virtual functions table")
 */
typedef struct CsLpWriteApprovalContainerObject CsLpWriteApprovalContainerObject;

/**
 * @brief CsLpWriteApprovalContainer Interface Structure
 */
struct CsLpWriteApprovalContainerInterface {
  void (*destruct)(CsLpWriteApprovalContainerObject* self);
  void (*add)(
      CsLpWriteApprovalContainerObject* self,
      const char* ski,
      MsgCounterType msg_cnt,
      FeatureLocalObject* feature
  );
  CsLpPendingApproval* (*find)(CsLpWriteApprovalContainerObject* self, const char* ski, MsgCounterType msg_cnt);
  void (*remove)(CsLpWriteApprovalContainerObject* self, const char* ski, MsgCounterType msg_cnt);
};

/**
 * @brief Cs Lp Write Approval Container Object Structure
 */
struct CsLpWriteApprovalContainerObject {
  const CsLpWriteApprovalContainerInterface* interface_;
};

/**
 * @brief Cs Lp Write Approval Container pointer typecast
 */
#define CS_LP_WRITE_APPROVAL_CONTAINER_OBJECT(obj) ((CsLpWriteApprovalContainerObject*)(obj))

/**
 * @brief Cs Lp Write Approval Container Interface class pointer typecast
 */
#define CS_LP_WRITE_APPROVAL_CONTAINER_INTERFACE(obj) (CS_LP_WRITE_APPROVAL_CONTAINER_OBJECT(obj)->interface_)

/**
 * @brief Cs Lp Write Approval Container Destruct caller definition
 */
#define CS_LP_WRITE_APPROVAL_CONTAINER_DESTRUCT(obj) (CS_LP_WRITE_APPROVAL_CONTAINER_INTERFACE(obj)->destruct(obj))

/**
 * @brief Cs Lp Write Approval Container Add caller definition
 */
#define CS_LP_WRITE_APPROVAL_CONTAINER_ADD(obj, ski, msg_cnt, feature) \
  (CS_LP_WRITE_APPROVAL_CONTAINER_INTERFACE(obj)->add(obj, ski, msg_cnt, feature))

/**
 * @brief Cs Lp Write Approval Container Find caller definition
 */
#define CS_LP_WRITE_APPROVAL_CONTAINER_FIND(obj, ski, msg_cnt) \
  (CS_LP_WRITE_APPROVAL_CONTAINER_INTERFACE(obj)->find(obj, ski, msg_cnt))

/**
 * @brief Cs Lp Write Approval Container Remove caller definition
 */
#define CS_LP_WRITE_APPROVAL_CONTAINER_REMOVE(obj, ski, msg_cnt) \
  (CS_LP_WRITE_APPROVAL_CONTAINER_INTERFACE(obj)->remove(obj, ski, msg_cnt))

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_USE_CASE_API_CS_LP_WRITE_APPROVAL_CONTAINER_INTERFACE_H_
