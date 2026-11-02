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
 * @brief Function Mock "class"
 */

#ifndef TESTS_UNIT_TESTS_MOCKS_SPINE_FUNCTION_FUNCTION_MOCK_H_
#define TESTS_UNIT_TESTS_MOCKS_SPINE_FUNCTION_FUNCTION_MOCK_H_

#include <gmock/gmock.h>

#include <memory>

#include "src/common/eebus_malloc.h"
#include "src/spine/api/function_interface.h"

class FunctionGMockInterface {
 public:
  virtual ~FunctionGMockInterface() {};
  virtual void Destruct(FunctionObject* self)                                                  = 0;
  virtual CmdType* CreateReadCmd(const FunctionObject* self, const FilterType* filter_partial) = 0;
  virtual FunctionType GetFunctionType(const FunctionObject* self)                             = 0;
  virtual const void* GetData(const FunctionObject* self)                                      = 0;
  virtual CmdType* CreateReplyCmd(const FunctionObject* self)                                  = 0;
  virtual CmdType* CreateNotifyCmd(
      const FunctionObject* self,
      const void* new_data,
      const FilterType* filter_partial,
      const FilterType* filter_delete
  ) = 0;
  virtual CmdType* CreateWriteCmd(
      const FunctionObject* self,
      const void* new_data,
      const FilterType* filter_partial,
      const FilterType* filter_delete
  )                                                  = 0;
  virtual void* DataCopy(const FunctionObject* self) = 0;
  virtual EebusError UpdateData(
      FunctionObject* self,
      const void* new_data,
      const FilterType* filter_partial,
      const FilterType* filter_delete,
      bool wr_remote,
      bool persist
  )                                                                                                              = 0;
  virtual const OperationsObject* GetOperations(const FunctionObject* self)                                      = 0;
  virtual void SetOperations(FunctionObject* self, bool read, bool read_partial, bool write, bool write_partial) = 0;
};

class FunctionGMock : public FunctionGMockInterface {
 public:
  virtual ~FunctionGMock() {};
  MOCK_METHOD1(Destruct, void(FunctionObject*));
  MOCK_METHOD2(CreateReadCmd, CmdType*(const FunctionObject*, const FilterType*));
  MOCK_METHOD1(GetFunctionType, FunctionType(const FunctionObject*));
  MOCK_METHOD1(GetData, const void*(const FunctionObject*));
  MOCK_METHOD1(CreateReplyCmd, CmdType*(const FunctionObject*));
  MOCK_METHOD4(CreateNotifyCmd, CmdType*(const FunctionObject*, const void*, const FilterType*, const FilterType*));
  MOCK_METHOD4(CreateWriteCmd, CmdType*(const FunctionObject*, const void*, const FilterType*, const FilterType*));
  MOCK_METHOD1(DataCopy, void*(const FunctionObject*));
  MOCK_METHOD6(UpdateData, EebusError(FunctionObject*, const void*, const FilterType*, const FilterType*, bool, bool));
  MOCK_METHOD1(GetOperations, const OperationsObject*(const FunctionObject*));
  MOCK_METHOD5(SetOperations, void(FunctionObject*, bool, bool, bool, bool));
};

typedef struct FunctionMock {
  /** Implements the Function Interface */
  FunctionObject obj;
  FunctionGMock* gmock;
} FunctionMock;

#define FUNCTION_MOCK(obj) ((FunctionMock*)(obj))

FunctionMock* FunctionMockCreate(void);

static inline void FunctionMockDelete(FunctionMock* self) {
  if (self != nullptr) {
    FUNCTION_DESTRUCT(FUNCTION_OBJECT(self));
    EEBUS_FREE(self);
  }
}

#endif  // TESTS_UNIT_TESTS_MOCKS_SPINE_FUNCTION_FUNCTION_MOCK_H_
