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
 * @brief Sender Mock "class"
 */

#ifndef TESTS_SRC_MOCKS_SPINE_DEVICE_SENDER_MOCK_H_
#define TESTS_SRC_MOCKS_SPINE_DEVICE_SENDER_MOCK_H_

#include <gmock/gmock.h>

#include <memory>

#include "src/common/eebus_malloc.h"
#include "src/spine/api/sender_interface.h"

class SenderGMockInterface {
 public:
  virtual ~SenderGMockInterface() {};
  virtual void Destruct(SenderObject* self) = 0;
  virtual EebusError Read(
      SenderObject* self,
      const FeatureAddressType* sender_addr,
      const FeatureAddressType* dest_addr,
      const CmdType* cmd,
      uint64_t* msg_cnt
  ) = 0;
  virtual EebusError
  Reply(SenderObject* self, const HeaderType* request_header, const FeatureAddressType* sender_addr, const CmdType* cmd)
      = 0;
  virtual EebusError Notify(
      SenderObject* self,
      const FeatureAddressType* sender_addr,
      const FeatureAddressType* dest_addr,
      const CmdType* cmd
  ) = 0;
  virtual EebusError Write(
      SenderObject* self,
      const FeatureAddressType* sender_addr,
      const FeatureAddressType* dest_addr,
      const CmdType* cmd,
      uint64_t* msg_cnt
  ) = 0;
  virtual EebusError CallSubscribe(
      SenderObject* self,
      const FeatureAddressType* sender_addr,
      const FeatureAddressType* dest_addr,
      FeatureTypeType server_feature_type
  ) = 0;
  virtual EebusError
  CallUnsubscribe(SenderObject* self, const FeatureAddressType* sender_addr, const FeatureAddressType* dest_addr)
      = 0;
  virtual EebusError CallBind(
      SenderObject* self,
      const FeatureAddressType* sender_addr,
      const FeatureAddressType* dest_addr,
      FeatureTypeType server_feature_type
  ) = 0;
  virtual EebusError
  CallUnbind(SenderObject* self, const FeatureAddressType* sender_addr, const FeatureAddressType* dest_addr)
      = 0;
  virtual EebusError
  ResultSuccess(SenderObject* self, const HeaderType* request_header, const FeatureAddressType* sender_addr)
      = 0;
  virtual EebusError ResultError(
      SenderObject* self,
      const HeaderType* request_header,
      const FeatureAddressType* sender_addr,
      const ErrorType* err
  ) = 0;
};

class SenderGMock : public SenderGMockInterface {
 public:
  virtual ~SenderGMock() {};
  MOCK_METHOD1(Destruct, void(SenderObject*));
  MOCK_METHOD5(
      Read,
      EebusError(SenderObject*, const FeatureAddressType*, const FeatureAddressType*, const CmdType*, uint64_t*)
  );
  MOCK_METHOD4(Reply, EebusError(SenderObject*, const HeaderType*, const FeatureAddressType*, const CmdType*));
  MOCK_METHOD4(Notify, EebusError(SenderObject*, const FeatureAddressType*, const FeatureAddressType*, const CmdType*));
  MOCK_METHOD5(
      Write,
      EebusError(SenderObject*, const FeatureAddressType*, const FeatureAddressType*, const CmdType*, uint64_t*)
  );
  MOCK_METHOD4(
      CallSubscribe,
      EebusError(SenderObject*, const FeatureAddressType*, const FeatureAddressType*, FeatureTypeType)
  );
  MOCK_METHOD3(CallUnsubscribe, EebusError(SenderObject*, const FeatureAddressType*, const FeatureAddressType*));
  MOCK_METHOD4(
      CallBind,
      EebusError(SenderObject*, const FeatureAddressType*, const FeatureAddressType*, FeatureTypeType)
  );
  MOCK_METHOD3(CallUnbind, EebusError(SenderObject*, const FeatureAddressType*, const FeatureAddressType*));
  MOCK_METHOD3(ResultSuccess, EebusError(SenderObject*, const HeaderType*, const FeatureAddressType*));
  MOCK_METHOD4(ResultError, EebusError(SenderObject*, const HeaderType*, const FeatureAddressType*, const ErrorType*));
};

typedef struct SenderMock {
  /** Implements the Sender Interface */
  SenderObject obj;
  SenderGMock* gmock;
} SenderMock;

#define SENDER_MOCK(obj) ((SenderMock*)(obj))

SenderMock* SenderMockCreate(void);

static inline void SenderMockDelete(SenderMock* self) {
  if (self != nullptr) {
    SENDER_DESTRUCT(SENDER_OBJECT(self));
    EEBUS_FREE(self);
  }
}

#endif  // TESTS_SRC_MOCKS_SPINE_DEVICE_SENDER_MOCK_H_
