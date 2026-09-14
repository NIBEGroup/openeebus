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
 * @brief Pending Write Request Container mock implementation
 */

#include "pending_write_request_container_mock.h"

#include <gmock/gmock.h>

#include "src/common/eebus_errors.h"
#include "src/spine/api/pending_write_request_container_interface.h"

static void Destruct(PendingWriteRequestContainerObject* self);
static EebusError Add(PendingWriteRequestContainerObject* self, const Message* msg);
static void Remove(PendingWriteRequestContainerObject* self, PendingWriteRequestObject* item);
static PendingWriteRequestObject*
Find(PendingWriteRequestContainerObject* self, const char* ski, MsgCounterType msg_cnt);
static size_t GetSize(PendingWriteRequestContainerObject* self);
static void Tick(PendingWriteRequestContainerObject* self, FeatureLocalObject* fl);
static void SetExpiredCallback(PendingWriteRequestContainerObject* self, PendingWriteRequestExpiredCb cb, void* ctx);

static const PendingWriteRequestContainerInterface pending_write_request_container_methods = {
    .destruct             = Destruct,
    .add                  = Add,
    .remove               = Remove,
    .find                 = Find,
    .get_size             = GetSize,
    .tick                 = Tick,
    .set_expired_callback = SetExpiredCallback,
};

static EebusError PendingWriteRequestContainerMockConstruct(PendingWriteRequestContainerMock* self);

EebusError PendingWriteRequestContainerMockConstruct(PendingWriteRequestContainerMock* self) {
  // Override "virtual functions table"
  PENDING_WRITE_REQUEST_CONTAINER_INTERFACE(self) = &pending_write_request_container_methods;

  self->gmock = new PendingWriteRequestContainerGMock();
  if (self->gmock == nullptr) {
    return kEebusErrorMemoryAllocate;
  }

  return kEebusErrorOk;
}

PendingWriteRequestContainerMock* PendingWriteRequestContainerMockCreate(void) {
  PendingWriteRequestContainerMock* const mock
      = (PendingWriteRequestContainerMock*)EEBUS_MALLOC(sizeof(PendingWriteRequestContainerMock));
  if (mock == nullptr) {
    return nullptr;
  }

  if (PendingWriteRequestContainerMockConstruct(mock) != kEebusErrorOk) {
    PendingWriteRequestContainerMockDelete(mock);
    return nullptr;
  }

  return mock;
}

void Destruct(PendingWriteRequestContainerObject* self) {
  PendingWriteRequestContainerMock* const mock = PENDING_WRITE_REQUEST_CONTAINER_MOCK(self);
  mock->gmock->Destruct(self);
  delete mock->gmock;
}

EebusError Add(PendingWriteRequestContainerObject* self, const Message* msg) {
  PendingWriteRequestContainerMock* const mock = PENDING_WRITE_REQUEST_CONTAINER_MOCK(self);
  return mock->gmock->Add(self, msg);
}

void Remove(PendingWriteRequestContainerObject* self, PendingWriteRequestObject* item) {
  PendingWriteRequestContainerMock* const mock = PENDING_WRITE_REQUEST_CONTAINER_MOCK(self);
  mock->gmock->Remove(self, item);
}

PendingWriteRequestObject* Find(PendingWriteRequestContainerObject* self, const char* ski, MsgCounterType msg_cnt) {
  PendingWriteRequestContainerMock* const mock = PENDING_WRITE_REQUEST_CONTAINER_MOCK(self);
  return mock->gmock->Find(self, ski, msg_cnt);
}

size_t GetSize(PendingWriteRequestContainerObject* self) {
  PendingWriteRequestContainerMock* const mock = PENDING_WRITE_REQUEST_CONTAINER_MOCK(self);
  return mock->gmock->GetSize(self);
}

void Tick(PendingWriteRequestContainerObject* self, FeatureLocalObject* fl) {
  PendingWriteRequestContainerMock* const mock = PENDING_WRITE_REQUEST_CONTAINER_MOCK(self);
  mock->gmock->Tick(self, fl);
}

void SetExpiredCallback(PendingWriteRequestContainerObject* self, PendingWriteRequestExpiredCb cb, void* ctx) {
  PendingWriteRequestContainerMock* const mock = PENDING_WRITE_REQUEST_CONTAINER_MOCK(self);
  mock->gmock->SetExpiredCallback(self, cb, ctx);
}
