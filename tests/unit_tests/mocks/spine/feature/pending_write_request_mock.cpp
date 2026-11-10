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
 * @brief Pending Write Request mock implementation
 */

#include "pending_write_request_mock.h"

#include <gmock/gmock.h>

#include "src/spine/api/pending_write_request_interface.h"

static void Destruct(PendingWriteRequestObject* self);
static const char* GetSki(PendingWriteRequestObject* self);
static uint64_t GetMessageCounter(PendingWriteRequestObject* self);
static EebusError GetMessage(PendingWriteRequestObject* self, FeatureLocalObject* fl, Message* msg);
static uint64_t GetNumberOfApprovals(PendingWriteRequestObject* self);
static void AddApproval(PendingWriteRequestObject* self);
static uint32_t GetRemainingTime(PendingWriteRequestObject* self);
static void UpdateRemainingTime(PendingWriteRequestObject* self);
static bool HasExpired(PendingWriteRequestObject* self);

static const PendingWriteRequestInterface pending_write_request_methods = {
    .destruct                = Destruct,
    .get_ski                 = GetSki,
    .get_message_counter     = GetMessageCounter,
    .get_message             = GetMessage,
    .get_number_of_approvals = GetNumberOfApprovals,
    .add_approval            = AddApproval,
    .get_remaining_time      = GetRemainingTime,
    .update_remaining_time   = UpdateRemainingTime,
    .has_expired             = HasExpired,
};

static EebusError PendingWriteRequestMockConstruct(PendingWriteRequestMock* self);

EebusError PendingWriteRequestMockConstruct(PendingWriteRequestMock* self) {
  // Override "virtual functions table"
  PENDING_WRITE_REQUEST_INTERFACE(self) = &pending_write_request_methods;

  self->gmock = new PendingWriteRequestGMock();
  if (self->gmock == nullptr) {
    return kEebusErrorMemoryAllocate;
  }

  return kEebusErrorOk;
}

PendingWriteRequestMock* PendingWriteRequestMockCreate(void) {
  PendingWriteRequestMock* const mock = (PendingWriteRequestMock*)EEBUS_MALLOC(sizeof(PendingWriteRequestMock));
  if (mock == nullptr) {
    return nullptr;
  }

  if (PendingWriteRequestMockConstruct(mock) != kEebusErrorOk) {
    PendingWriteRequestMockDelete(mock);
    return nullptr;
  }

  return mock;
}

void Destruct(PendingWriteRequestObject* self) {
  PendingWriteRequestMock* const mock = PENDING_WRITE_REQUEST_MOCK(self);
  mock->gmock->Destruct(self);
  delete mock->gmock;
}

const char* GetSki(PendingWriteRequestObject* self) {
  PendingWriteRequestMock* const mock = PENDING_WRITE_REQUEST_MOCK(self);
  return mock->gmock->GetSki(self);
}

uint64_t GetMessageCounter(PendingWriteRequestObject* self) {
  PendingWriteRequestMock* const mock = PENDING_WRITE_REQUEST_MOCK(self);
  return mock->gmock->GetMessageCounter(self);
}

EebusError GetMessage(PendingWriteRequestObject* self, FeatureLocalObject* fl, Message* msg) {
  PendingWriteRequestMock* const mock = PENDING_WRITE_REQUEST_MOCK(self);
  return mock->gmock->GetMessage(self, fl, msg);
}

uint64_t GetNumberOfApprovals(PendingWriteRequestObject* self) {
  PendingWriteRequestMock* const mock = PENDING_WRITE_REQUEST_MOCK(self);
  return mock->gmock->GetNumberOfApprovals(self);
}

void AddApproval(PendingWriteRequestObject* self) {
  PendingWriteRequestMock* const mock = PENDING_WRITE_REQUEST_MOCK(self);
  mock->gmock->AddApproval(self);
}

uint32_t GetRemainingTime(PendingWriteRequestObject* self) {
  PendingWriteRequestMock* const mock = PENDING_WRITE_REQUEST_MOCK(self);
  return mock->gmock->GetRemainingTime(self);
}

void UpdateRemainingTime(PendingWriteRequestObject* self) {
  PendingWriteRequestMock* const mock = PENDING_WRITE_REQUEST_MOCK(self);
  mock->gmock->UpdateRemainingTime(self);
}

bool HasExpired(PendingWriteRequestObject* self) {
  PendingWriteRequestMock* const mock = PENDING_WRITE_REQUEST_MOCK(self);
  return mock->gmock->HasExpired(self);
}
