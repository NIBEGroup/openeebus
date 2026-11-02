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
 * @brief Pending Write Request Mock "class"
 */

#ifndef TESTS_UNIT_TESTS_MOCKS_SPINE_FEATURE_PENDING_WRITE_REQUEST_MOCK_H_
#define TESTS_UNIT_TESTS_MOCKS_SPINE_FEATURE_PENDING_WRITE_REQUEST_MOCK_H_

#include <gmock/gmock.h>

#include <memory>

#include "src/common/eebus_malloc.h"
#include "src/spine/api/pending_write_request_interface.h"

class PendingWriteRequestGMockInterface {
 public:
  virtual ~PendingWriteRequestGMockInterface() {};
  virtual void Destruct(PendingWriteRequestObject* self)                                               = 0;
  virtual const char* GetSki(PendingWriteRequestObject* self)                                          = 0;
  virtual uint64_t GetMessageCounter(PendingWriteRequestObject* self)                                  = 0;
  virtual EebusError GetMessage(PendingWriteRequestObject* self, FeatureLocalObject* fl, Message* msg) = 0;
  virtual uint64_t GetNumberOfApprovals(PendingWriteRequestObject* self)                               = 0;
  virtual void AddApproval(PendingWriteRequestObject* self)                                            = 0;
  virtual uint32_t GetRemainingTime(PendingWriteRequestObject* self)                                   = 0;
  virtual void UpdateRemainingTime(PendingWriteRequestObject* self)                                    = 0;
  virtual bool HasExpired(PendingWriteRequestObject* self)                                             = 0;
};

class PendingWriteRequestGMock : public PendingWriteRequestGMockInterface {
 public:
  virtual ~PendingWriteRequestGMock() {};
  MOCK_METHOD1(Destruct, void(PendingWriteRequestObject*));
  MOCK_METHOD1(GetSki, const char*(PendingWriteRequestObject*));
  MOCK_METHOD1(GetMessageCounter, uint64_t(PendingWriteRequestObject*));
  MOCK_METHOD3(GetMessage, EebusError(PendingWriteRequestObject*, FeatureLocalObject*, Message*));
  MOCK_METHOD1(GetNumberOfApprovals, uint64_t(PendingWriteRequestObject*));
  MOCK_METHOD1(AddApproval, void(PendingWriteRequestObject*));
  MOCK_METHOD1(GetRemainingTime, uint32_t(PendingWriteRequestObject*));
  MOCK_METHOD1(UpdateRemainingTime, void(PendingWriteRequestObject*));
  MOCK_METHOD1(HasExpired, bool(PendingWriteRequestObject*));
};

typedef struct PendingWriteRequestMock {
  /** Implements the Pending Write Request Interface */
  PendingWriteRequestObject obj;
  PendingWriteRequestGMock* gmock;
} PendingWriteRequestMock;

#define PENDING_WRITE_REQUEST_MOCK(obj) ((PendingWriteRequestMock*)(obj))

PendingWriteRequestMock* PendingWriteRequestMockCreate(void);

static inline void PendingWriteRequestMockDelete(PendingWriteRequestMock* self) {
  if (self != nullptr) {
    PENDING_WRITE_REQUEST_DESTRUCT(PENDING_WRITE_REQUEST_OBJECT(self));
    EEBUS_FREE(self);
  }
}

#endif  // TESTS_UNIT_TESTS_MOCKS_SPINE_FEATURE_PENDING_WRITE_REQUEST_MOCK_H_
