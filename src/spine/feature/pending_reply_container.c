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
 * @brief Pending Reply Container implementation
 */

#include "src/spine/feature/pending_reply_container.h"

#include <string.h>

#include "src/common/eebus_arguments.h"
#include "src/common/eebus_errors.h"
#include "src/common/eebus_malloc.h"
#include "src/common/vector.h"
#include "src/spine/feature/pending_reply.h"

typedef struct PendingReplyContainer PendingReplyContainer;

struct PendingReplyContainer {
  /** Implements the Pending Reply Container Interface */
  PendingReplyContainerObject obj;

  Vector items;
};

#define PENDING_REPLY_CONTAINER(obj) ((PendingReplyContainer*)(obj))

static void Destruct(PendingReplyContainerObject* self);
static EebusError Add(
    PendingReplyContainerObject* self,
    MsgCounterType msg_cnt_ref,
    const FeatureAddressType* remote_feature_address,
    FunctionType function_type,
    const char* ski,
    ReplyMessageCallback cb,
    void* ctx
);
static void Process(PendingReplyContainerObject* self, const ReplyMessage* reply_msg, EebusError err);
static void Tick(PendingReplyContainerObject* self);
static void RemoveForDevice(PendingReplyContainerObject* self, const char* device_addr);
static void RemoveForSki(PendingReplyContainerObject* self, const char* ski);

static const PendingReplyContainerInterface pending_reply_container_methods = {
    .destruct          = Destruct,
    .add               = Add,
    .process           = Process,
    .tick              = Tick,
    .remove_for_device = RemoveForDevice,
    .remove_for_ski    = RemoveForSki,
};

static void DeletePendingReply(void* pending) {
  PendingReplyDelete((PendingReplyObject*)pending);
}

static void PendingReplyContainerConstruct(PendingReplyContainer* self);

void PendingReplyContainerConstruct(PendingReplyContainer* self) {
  // Override "virtual functions table"
  PENDING_REPLY_CONTAINER_INTERFACE(self) = &pending_reply_container_methods;

  VectorConstructWithDeallocator(&self->items, DeletePendingReply);
}

PendingReplyContainerObject* PendingReplyContainerCreate(void) {
  PendingReplyContainer* const self = (PendingReplyContainer*)EEBUS_MALLOC(sizeof(PendingReplyContainer));
  if (self == NULL) {
    return NULL;
  }

  PendingReplyContainerConstruct(self);
  return PENDING_REPLY_CONTAINER_OBJECT(self);
}

void Destruct(PendingReplyContainerObject* self) {
  PendingReplyContainer* const prc = PENDING_REPLY_CONTAINER(self);
  VectorFreeElements(&prc->items);
  VectorDestruct(&prc->items);
}

EebusError Add(
    PendingReplyContainerObject* self,
    MsgCounterType msg_cnt_ref,
    const FeatureAddressType* remote_feature_address,
    FunctionType function_type,
    const char* ski,
    ReplyMessageCallback cb,
    void* ctx
) {
  PendingReplyContainer* const prc = PENDING_REPLY_CONTAINER(self);
  PendingReplyObject* const pending
      = PendingReplyCreate(msg_cnt_ref, remote_feature_address, function_type, ski, cb, ctx);
  if (pending == NULL) {
    return kEebusErrorMemoryAllocate;
  }

  VectorPushBack(&prc->items, pending);
  return kEebusErrorOk;
}

void Process(PendingReplyContainerObject* self, const ReplyMessage* reply_msg, EebusError err) {
  PendingReplyContainer* const prc = PENDING_REPLY_CONTAINER(self);

  for (size_t i = 0; i < VectorGetSize(&prc->items); ++i) {
    PendingReplyObject* const pending = (PendingReplyObject*)VectorGetElement(&prc->items, i);
    if (PENDING_REPLY_GET_MSG_CNT_REF(pending) == reply_msg->msg_cnt_ref) {
      PENDING_REPLY_FIRE(pending, reply_msg, err);
      VectorRemove(&prc->items, pending);
      PendingReplyDelete(pending);
      return;
    }
  }
}

void Tick(PendingReplyContainerObject* self) {
  PendingReplyContainer* const prc = PENDING_REPLY_CONTAINER(self);

  size_t i = VectorGetSize(&prc->items);

  while (i > 0) {
    --i;
    PendingReplyObject* const pending = (PendingReplyObject*)VectorGetElement(&prc->items, i);

    if (PENDING_REPLY_HAS_EXPIRED(pending)) {
      PENDING_REPLY_FIRE(pending, NULL, kEebusErrorTime);
      VectorRemove(&prc->items, pending);
      PendingReplyDelete(pending);
    } else {
      PENDING_REPLY_UPDATE_TIME(pending);
    }
  }
}

void RemoveForDevice(PendingReplyContainerObject* self, const char* device_addr) {
  PendingReplyContainer* const prc = PENDING_REPLY_CONTAINER(self);

  if (device_addr == NULL) {
    return;
  }

  size_t i = VectorGetSize(&prc->items);

  while (i > 0) {
    --i;
    PendingReplyObject* const pending       = (PendingReplyObject*)VectorGetElement(&prc->items, i);
    const FeatureAddressType* const fr_addr = PENDING_REPLY_GET_REMOTE_FEATURE_ADDRESS(pending);

    if (FeatureAddressMatchDevice(fr_addr, device_addr)) {
      VectorRemove(&prc->items, pending);
      PendingReplyDelete(pending);
    }
  }
}

void RemoveForSki(PendingReplyContainerObject* self, const char* ski) {
  PendingReplyContainer* const prc = PENDING_REPLY_CONTAINER(self);

  if (ski == NULL) {
    return;
  }

  size_t i = VectorGetSize(&prc->items);

  while (i > 0) {
    --i;
    PendingReplyObject* const pending = (PendingReplyObject*)VectorGetElement(&prc->items, i);
    const char* const pending_ski     = PENDING_REPLY_GET_SKI(pending);

    if ((pending_ski != NULL) && (strcmp(pending_ski, ski) == 0)) {
      VectorRemove(&prc->items, pending);
      PendingReplyDelete(pending);
    }
  }
}
