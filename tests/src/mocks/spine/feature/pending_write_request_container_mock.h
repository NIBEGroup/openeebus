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
 * @brief Pending Write Request Container Mock "class"
 */

#ifndef TESTS_SRC_MOCKS_SPINE_FEATURE_PENDING_WRITE_REQUEST_CONTAINER_MOCK_H_
#define TESTS_SRC_MOCKS_SPINE_FEATURE_PENDING_WRITE_REQUEST_CONTAINER_MOCK_H_

#include <gmock/gmock.h>

#include <memory>

#include "src/common/eebus_malloc.h"
#include "src/spine/api/pending_write_request_container_interface.h"

class PendingWriteRequestContainerGMockInterface {
 public:
  virtual ~PendingWriteRequestContainerGMockInterface() {};
  virtual void Destruct(PendingWriteRequestContainerObject* self)                                = 0;
  virtual EebusError Add(PendingWriteRequestContainerObject* self, const Message* msg)           = 0;
  virtual void Remove(PendingWriteRequestContainerObject* self, PendingWriteRequestObject* item) = 0;
  virtual PendingWriteRequestObject*
  Find(PendingWriteRequestContainerObject* self, const char* ski, MsgCounterType msg_cnt)
      = 0;
  virtual size_t GetSize(PendingWriteRequestContainerObject* self)                    = 0;
  virtual void Tick(PendingWriteRequestContainerObject* self, FeatureLocalObject* fl) = 0;
  virtual void SetExpiredCallback(PendingWriteRequestContainerObject* self, PendingWriteRequestExpiredCb cb, void* ctx)
      = 0;
};

class PendingWriteRequestContainerGMock : public PendingWriteRequestContainerGMockInterface {
 public:
  virtual ~PendingWriteRequestContainerGMock() {};
  MOCK_METHOD1(Destruct, void(PendingWriteRequestContainerObject*));
  MOCK_METHOD2(Add, EebusError(PendingWriteRequestContainerObject*, const Message*));
  MOCK_METHOD2(Remove, void(PendingWriteRequestContainerObject*, PendingWriteRequestObject*));
  MOCK_METHOD3(Find, PendingWriteRequestObject*(PendingWriteRequestContainerObject*, const char*, MsgCounterType));
  MOCK_METHOD1(GetSize, size_t(PendingWriteRequestContainerObject*));
  MOCK_METHOD2(Tick, void(PendingWriteRequestContainerObject*, FeatureLocalObject*));
  MOCK_METHOD3(SetExpiredCallback, void(PendingWriteRequestContainerObject*, PendingWriteRequestExpiredCb, void*));
};

typedef struct PendingWriteRequestContainerMock {
  /** Implements the Pending Write Request Container Interface */
  PendingWriteRequestContainerObject obj;
  PendingWriteRequestContainerGMock* gmock;
} PendingWriteRequestContainerMock;

#define PENDING_WRITE_REQUEST_CONTAINER_MOCK(obj) ((PendingWriteRequestContainerMock*)(obj))

PendingWriteRequestContainerMock* PendingWriteRequestContainerMockCreate(void);

static inline void PendingWriteRequestContainerMockDelete(PendingWriteRequestContainerMock* self) {
  if (self != nullptr) {
    PENDING_WRITE_REQUEST_CONTAINER_DESTRUCT(PENDING_WRITE_REQUEST_CONTAINER_OBJECT(self));
    EEBUS_FREE(self);
  }
}

#endif  // TESTS_SRC_MOCKS_SPINE_FEATURE_PENDING_WRITE_REQUEST_CONTAINER_MOCK_H_
