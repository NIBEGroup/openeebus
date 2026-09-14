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
 * @brief Cs Lp Write Approval Container implementation
 */

#include <string.h>

#include "src/common/eebus_errors.h"
#include "src/common/eebus_malloc.h"
#include "src/common/string_util.h"
#include "src/common/vector.h"

#include "cs_lp_write_approval_container.h"
#include "src/use_case/api/cs_lp_write_approval_container_interface.h"

typedef struct CsLpWriteApprovalContainer CsLpWriteApprovalContainer;

struct CsLpWriteApprovalContainer {
  /** Implements the Cs Lp Write Approval Container Interface */
  CsLpWriteApprovalContainerObject obj;
  Vector approvals;
};

#define CS_LP_WRITE_APPROVAL_CONTAINER(obj) ((CsLpWriteApprovalContainer*)(obj))

static void Destruct(CsLpWriteApprovalContainerObject* self);
static void
Add(CsLpWriteApprovalContainerObject* self, const char* ski, MsgCounterType msg_cnt, FeatureLocalObject* feature);
static CsLpPendingApproval* Find(CsLpWriteApprovalContainerObject* self, const char* ski, MsgCounterType msg_cnt);
static void Remove(CsLpWriteApprovalContainerObject* self, const char* ski, MsgCounterType msg_cnt);

static const CsLpWriteApprovalContainerInterface cs_lp_write_approval_container_methods = {
    .destruct = Destruct,
    .add      = Add,
    .find     = Find,
    .remove   = Remove,
};

static EebusError CsLpWriteApprovalContainerConstruct(CsLpWriteApprovalContainer* self);

static EebusError CsLpWriteApprovalContainerConstruct(CsLpWriteApprovalContainer* self) {
  // Override "virtual functions table"
  CS_LP_WRITE_APPROVAL_CONTAINER_INTERFACE(self) = &cs_lp_write_approval_container_methods;
  VectorConstruct(&self->approvals);
  return kEebusErrorOk;
}

CsLpWriteApprovalContainerObject* CsLpWriteApprovalContainerCreate(void) {
  CsLpWriteApprovalContainer* const cs_lp_write_approval_container
      = (CsLpWriteApprovalContainer*)EEBUS_MALLOC(sizeof(CsLpWriteApprovalContainer));
  if (cs_lp_write_approval_container == NULL) {
    return NULL;
  }

  const EebusError err = CsLpWriteApprovalContainerConstruct(cs_lp_write_approval_container);
  if (err != kEebusErrorOk) {
    CsLpWriteApprovalContainerDelete(CS_LP_WRITE_APPROVAL_CONTAINER_OBJECT(cs_lp_write_approval_container));
    return NULL;
  }

  return CS_LP_WRITE_APPROVAL_CONTAINER_OBJECT(cs_lp_write_approval_container);
}

void Destruct(CsLpWriteApprovalContainerObject* self) {
  CsLpWriteApprovalContainer* const wac = CS_LP_WRITE_APPROVAL_CONTAINER(self);
  for (size_t i = 0; i < VectorGetSize(&wac->approvals); ++i) {
    CsLpPendingApproval* const entry = (CsLpPendingApproval*)VectorGetElement(&wac->approvals, i);
    StringDelete(entry->ski);
    EEBUS_FREE(entry);
  }

  VectorDestruct(&wac->approvals);
}

void Add(CsLpWriteApprovalContainerObject* self, const char* ski, MsgCounterType msg_cnt, FeatureLocalObject* feature) {
  CsLpPendingApproval* const entry = (CsLpPendingApproval*)EEBUS_MALLOC(sizeof(CsLpPendingApproval));
  if (entry == NULL) {
    return;
  }

  entry->msg_cnt = msg_cnt;
  entry->ski     = StringCopy(ski);
  entry->feature = feature;

  CsLpWriteApprovalContainer* const wac = CS_LP_WRITE_APPROVAL_CONTAINER(self);
  VectorPushBack(&wac->approvals, entry);
}

CsLpPendingApproval* Find(CsLpWriteApprovalContainerObject* self, const char* ski, MsgCounterType msg_cnt) {
  CsLpWriteApprovalContainer* const wac = CS_LP_WRITE_APPROVAL_CONTAINER(self);

  for (size_t i = 0; i < VectorGetSize(&wac->approvals); ++i) {
    CsLpPendingApproval* const entry = (CsLpPendingApproval*)VectorGetElement(&wac->approvals, i);
    if (entry->msg_cnt == msg_cnt && StringNCompare(entry->ski, ski, strlen(ski))) {
      return entry;
    }
  }

  return NULL;
}

void Remove(CsLpWriteApprovalContainerObject* self, const char* ski, MsgCounterType msg_cnt) {
  CsLpWriteApprovalContainer* const wac = CS_LP_WRITE_APPROVAL_CONTAINER(self);
  for (size_t i = 0; i < VectorGetSize(&wac->approvals); ++i) {
    CsLpPendingApproval* const entry = (CsLpPendingApproval*)VectorGetElement(&wac->approvals, i);
    if (entry->msg_cnt == msg_cnt && StringNCompare(entry->ski, ski, strlen(ski))) {
      StringDelete(entry->ski);
      VectorRemove(&wac->approvals, entry);
      EEBUS_FREE(entry);
      return;
    }
  }
}
