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
 * @brief Pending Result Container implementation
 */

#include "src/spine/feature/pending_result_container.h"

#include "src/common/eebus_arguments.h"
#include "src/common/eebus_malloc.h"
#include "src/common/vector.h"
#include "src/spine/feature/pending_result.h"

typedef struct PendingResultContainer PendingResultContainer;

struct PendingResultContainer {
  /** Implements the Pending Result Container Interface */
  PendingResultContainerObject obj;

  Vector items;
};

#define PENDING_RESULT_CONTAINER(obj) ((PendingResultContainer*)(obj))

static void Destruct(PendingResultContainerObject* self);
static EebusError Add(
    PendingResultContainerObject* self,
    MsgCounterType msg_cnt_ref,
    FunctionType function_type,
    const FeatureAddressType* remote_feature_addr,
    ResultMessageCallback cb,
    void* ctx
);
static void Process(PendingResultContainerObject* self, const ResultMessage* result_msg);
static void Tick(PendingResultContainerObject* self);

static const PendingResultContainerInterface pending_result_container_methods = {
    .destruct = Destruct,
    .add      = Add,
    .process  = Process,
    .tick     = Tick,
};

static void DeletePendingResult(void* pending) {
  PendingResultDelete((PendingResultObject*)pending);
}

static void PendingResultContainerConstruct(PendingResultContainer* self);

void PendingResultContainerConstruct(PendingResultContainer* self) {
  // Override "virtual functions table"
  PENDING_RESULT_CONTAINER_INTERFACE(self) = &pending_result_container_methods;

  VectorConstructWithDeallocator(&self->items, DeletePendingResult);
}

PendingResultContainerObject* PendingResultContainerCreate(void) {
  PendingResultContainer* const self = (PendingResultContainer*)EEBUS_MALLOC(sizeof(PendingResultContainer));
  if (self == NULL) {
    return NULL;
  }

  PendingResultContainerConstruct(self);
  return PENDING_RESULT_CONTAINER_OBJECT(self);
}

void Destruct(PendingResultContainerObject* self) {
  PendingResultContainer* const prc = PENDING_RESULT_CONTAINER(self);
  VectorFreeElements(&prc->items);
  VectorDestruct(&prc->items);
}

EebusError Add(
    PendingResultContainerObject* self,
    MsgCounterType msg_cnt_ref,
    FunctionType function_type,
    const FeatureAddressType* remote_feature_addr,
    ResultMessageCallback cb,
    void* ctx
) {
  PendingResultContainer* const prc  = PENDING_RESULT_CONTAINER(self);
  PendingResultObject* const pending = PendingResultCreate(msg_cnt_ref, function_type, remote_feature_addr, cb, ctx);
  if (pending == NULL) {
    return kEebusErrorMemoryAllocate;
  }

  VectorPushBack(&prc->items, pending);
  return kEebusErrorOk;
}

void Process(PendingResultContainerObject* self, const ResultMessage* result_msg) {
  PendingResultContainer* const prc = PENDING_RESULT_CONTAINER(self);

  for (size_t i = 0; i < VectorGetSize(&prc->items); ++i) {
    PendingResultObject* const pending = (PendingResultObject*)VectorGetElement(&prc->items, i);
    if (PENDING_RESULT_GET_MSG_CNT_REF(pending) == result_msg->msg_cnt_ref) {
      PENDING_RESULT_FIRE(pending, result_msg, kEebusErrorOk);
      VectorRemove(&prc->items, pending);
      PendingResultDelete(pending);
      return;
    }
  }
}

void Tick(PendingResultContainerObject* self) {
  PendingResultContainer* const prc = PENDING_RESULT_CONTAINER(self);

  size_t i = VectorGetSize(&prc->items);

  while (i > 0) {
    --i;
    PendingResultObject* const pending = (PendingResultObject*)VectorGetElement(&prc->items, i);

    if (PENDING_RESULT_HAS_EXPIRED(pending)) {
      PENDING_RESULT_FIRE(pending, NULL, kEebusErrorTime);
      VectorRemove(&prc->items, pending);
      PendingResultDelete(pending);
    } else {
      PENDING_RESULT_UPDATE_TIME(pending);
    }
  }
}
