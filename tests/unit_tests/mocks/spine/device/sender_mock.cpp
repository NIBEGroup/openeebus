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
 * @brief Sender mock implementation
 */

#include "sender_mock.h"

#include <gmock/gmock.h>

#include "src/spine/api/sender_interface.h"

static void Destruct(SenderObject* self);
static EebusError Read(
    SenderObject* self,
    const FeatureAddressType* sender_addr,
    const FeatureAddressType* dest_addr,
    const CmdType* cmd,
    uint64_t* msg_cnt
);
static EebusError
Reply(SenderObject* self, const HeaderType* request_header, const FeatureAddressType* sender_addr, const CmdType* cmd);
static EebusError Notify(
    SenderObject* self,
    const FeatureAddressType* sender_addr,
    const FeatureAddressType* dest_addr,
    const CmdType* cmd
);
static EebusError Write(
    SenderObject* self,
    const FeatureAddressType* sender_addr,
    const FeatureAddressType* dest_addr,
    const CmdType* cmd,
    uint64_t* msg_cnt
);
static EebusError CallSubscribe(
    SenderObject* self,
    const FeatureAddressType* sender_addr,
    const FeatureAddressType* dest_addr,
    FeatureTypeType server_feature_type
);
static EebusError
CallUnsubscribe(SenderObject* self, const FeatureAddressType* sender_addr, const FeatureAddressType* dest_addr);
static EebusError CallBind(
    SenderObject* self,
    const FeatureAddressType* sender_addr,
    const FeatureAddressType* dest_addr,
    FeatureTypeType server_feature_type
);
static EebusError
CallUnbind(SenderObject* self, const FeatureAddressType* sender_addr, const FeatureAddressType* dest_addr);
static EebusError
ResultSuccess(SenderObject* self, const HeaderType* request_header, const FeatureAddressType* sender_addr);
static EebusError ResultError(
    SenderObject* self,
    const HeaderType* request_header,
    const FeatureAddressType* sender_addr,
    const ErrorType* err
);

static const SenderInterface sender_methods = {
    .destruct         = Destruct,
    .read             = Read,
    .reply            = Reply,
    .notify           = Notify,
    .write            = Write,
    .call_subscribe   = CallSubscribe,
    .call_unsubscribe = CallUnsubscribe,
    .call_bind        = CallBind,
    .call_unbind      = CallUnbind,
    .result_success   = ResultSuccess,
    .result_error     = ResultError,
};

static EebusError SenderMockConstruct(SenderMock* self);

EebusError SenderMockConstruct(SenderMock* self) {
  // Override "virtual functions table"
  SENDER_INTERFACE(self) = &sender_methods;

  self->gmock = new SenderGMock();
  if (self->gmock == nullptr) {
    return kEebusErrorMemoryAllocate;
  }

  return kEebusErrorOk;
}

SenderMock* SenderMockCreate(void) {
  SenderMock* const mock = (SenderMock*)EEBUS_MALLOC(sizeof(SenderMock));
  if (mock == nullptr) {
    return nullptr;
  }

  if (SenderMockConstruct(mock) != kEebusErrorOk) {
    SenderMockDelete(mock);
    return nullptr;
  }

  return mock;
}

void Destruct(SenderObject* self) {
  SenderMock* const mock = SENDER_MOCK(self);
  mock->gmock->Destruct(self);
  delete mock->gmock;
}

EebusError Read(
    SenderObject* self,
    const FeatureAddressType* sender_addr,
    const FeatureAddressType* dest_addr,
    const CmdType* cmd,
    uint64_t* msg_cnt
) {
  SenderMock* const mock = SENDER_MOCK(self);
  return mock->gmock->Read(self, sender_addr, dest_addr, cmd, msg_cnt);
}

EebusError
Reply(SenderObject* self, const HeaderType* request_header, const FeatureAddressType* sender_addr, const CmdType* cmd) {
  SenderMock* const mock = SENDER_MOCK(self);
  return mock->gmock->Reply(self, request_header, sender_addr, cmd);
}

EebusError Notify(
    SenderObject* self,
    const FeatureAddressType* sender_addr,
    const FeatureAddressType* dest_addr,
    const CmdType* cmd
) {
  SenderMock* const mock = SENDER_MOCK(self);
  return mock->gmock->Notify(self, sender_addr, dest_addr, cmd);
}

EebusError Write(
    SenderObject* self,
    const FeatureAddressType* sender_addr,
    const FeatureAddressType* dest_addr,
    const CmdType* cmd,
    uint64_t* msg_cnt
) {
  SenderMock* const mock = SENDER_MOCK(self);
  return mock->gmock->Write(self, sender_addr, dest_addr, cmd, msg_cnt);
}

EebusError CallSubscribe(
    SenderObject* self,
    const FeatureAddressType* sender_addr,
    const FeatureAddressType* dest_addr,
    FeatureTypeType server_feature_type
) {
  SenderMock* const mock = SENDER_MOCK(self);
  return mock->gmock->CallSubscribe(self, sender_addr, dest_addr, server_feature_type);
}

EebusError
CallUnsubscribe(SenderObject* self, const FeatureAddressType* sender_addr, const FeatureAddressType* dest_addr) {
  SenderMock* const mock = SENDER_MOCK(self);
  return mock->gmock->CallUnsubscribe(self, sender_addr, dest_addr);
}

EebusError CallBind(
    SenderObject* self,
    const FeatureAddressType* sender_addr,
    const FeatureAddressType* dest_addr,
    FeatureTypeType server_feature_type
) {
  SenderMock* const mock = SENDER_MOCK(self);
  return mock->gmock->CallBind(self, sender_addr, dest_addr, server_feature_type);
}

EebusError CallUnbind(SenderObject* self, const FeatureAddressType* sender_addr, const FeatureAddressType* dest_addr) {
  SenderMock* const mock = SENDER_MOCK(self);
  return mock->gmock->CallUnbind(self, sender_addr, dest_addr);
}

EebusError ResultSuccess(SenderObject* self, const HeaderType* request_header, const FeatureAddressType* sender_addr) {
  SenderMock* const mock = SENDER_MOCK(self);
  return mock->gmock->ResultSuccess(self, request_header, sender_addr);
}

EebusError ResultError(
    SenderObject* self,
    const HeaderType* request_header,
    const FeatureAddressType* sender_addr,
    const ErrorType* err
) {
  SenderMock* const mock = SENDER_MOCK(self);
  return mock->gmock->ResultError(self, request_header, sender_addr, err);
}
