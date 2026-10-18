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
 * @brief Cs Lp Write Approval Container implementation declarations
 */

#ifndef SRC_USE_CASE_ACTOR_CS_CS_LP_WRITE_APPROVAL_CONTAINER_H_
#define SRC_USE_CASE_ACTOR_CS_CS_LP_WRITE_APPROVAL_CONTAINER_H_

#include <stddef.h>
#include "src/common/eebus_malloc.h"

#include "src/use_case/api/cs_lp_write_approval_container_interface.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

CsLpWriteApprovalContainerObject* CsLpWriteApprovalContainerCreate(void);

static inline void CsLpWriteApprovalContainerDelete(CsLpWriteApprovalContainerObject* cs_lp_write_approval_container) {
  if (cs_lp_write_approval_container != NULL) {
    CS_LP_WRITE_APPROVAL_CONTAINER_DESTRUCT(cs_lp_write_approval_container);
    EEBUS_FREE(cs_lp_write_approval_container);
  }
}

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_USE_CASE_ACTOR_CS_CS_LP_WRITE_APPROVAL_CONTAINER_H_
