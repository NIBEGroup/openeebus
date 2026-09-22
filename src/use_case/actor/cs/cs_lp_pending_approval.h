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
 * @brief Cs Lp Pending Approval type declaration
 */

#ifndef SRC_USE_CASE_ACTOR_CS_CS_LP_PENDING_APPROVAL_H_
#define SRC_USE_CASE_ACTOR_CS_CS_LP_PENDING_APPROVAL_H_

#include "src/spine/api/feature_local_interface.h"
#include "src/spine/model/command_frame_types.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct {
  MsgCounterType msg_cnt;
  char* ski;  // heap copy
  FeatureLocalObject* feature;
} CsLpPendingApproval;

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_USE_CASE_ACTOR_CS_CS_LP_PENDING_APPROVAL_H_
