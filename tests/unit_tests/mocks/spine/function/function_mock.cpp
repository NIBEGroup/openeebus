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
 * @brief Function mock implementation
 */

#include "function_mock.h"

#include <gmock/gmock.h>

#include "src/spine/api/function_interface.h"

static void Destruct(FunctionObject* self);
static CmdType* CreateReadCmd(const FunctionObject* self, const FilterType* filter_partial);
static FunctionType GetFunctionType(const FunctionObject* self);
static const void* GetData(const FunctionObject* self);
static CmdType* CreateReplyCmd(const FunctionObject* self);
static CmdType* CreateNotifyCmd(
    const FunctionObject* self,
    const void* new_data,
    const FilterType* filter_partial,
    const FilterType* filter_delete
);
static CmdType* CreateWriteCmd(
    const FunctionObject* self,
    const void* new_data,
    const FilterType* filter_partial,
    const FilterType* filter_delete
);
static void* DataCopy(const FunctionObject* self);
static EebusError UpdateData(
    FunctionObject* self,
    const void* new_data,
    const FilterType* filter_partial,
    const FilterType* filter_delete,
    bool wr_remote,
    bool persist
);
static const OperationsObject* GetOperations(const FunctionObject* self);
static void SetOperations(FunctionObject* self, bool read, bool read_partial, bool write, bool write_partial);

static const FunctionInterface function_methods = {
    .destruct          = Destruct,
    .create_read_cmd   = CreateReadCmd,
    .get_function_type = GetFunctionType,
    .get_data          = GetData,
    .create_reply_cmd  = CreateReplyCmd,
    .create_notify_cmd = CreateNotifyCmd,
    .create_write_cmd  = CreateWriteCmd,
    .data_copy         = DataCopy,
    .update_data       = UpdateData,
    .get_operations    = GetOperations,
    .set_operations    = SetOperations,
};

static EebusError FunctionMockConstruct(FunctionMock* self);

EebusError FunctionMockConstruct(FunctionMock* self) {
  // Override "virtual functions table"
  FUNCTION_INTERFACE(self) = &function_methods;

  self->gmock = new FunctionGMock();
  if (self->gmock == nullptr) {
    return kEebusErrorMemoryAllocate;
  }

  return kEebusErrorOk;
}

FunctionMock* FunctionMockCreate(void) {
  FunctionMock* const mock = (FunctionMock*)EEBUS_MALLOC(sizeof(FunctionMock));
  if (mock == nullptr) {
    return nullptr;
  }

  if (FunctionMockConstruct(mock) != kEebusErrorOk) {
    FunctionMockDelete(mock);
    return nullptr;
  }

  return mock;
}

void Destruct(FunctionObject* self) {
  FunctionMock* const mock = FUNCTION_MOCK(self);
  mock->gmock->Destruct(self);
  delete mock->gmock;
}

CmdType* CreateReadCmd(const FunctionObject* self, const FilterType* filter_partial) {
  FunctionMock* const mock = FUNCTION_MOCK(self);
  return mock->gmock->CreateReadCmd(self, filter_partial);
}

FunctionType GetFunctionType(const FunctionObject* self) {
  FunctionMock* const mock = FUNCTION_MOCK(self);
  return mock->gmock->GetFunctionType(self);
}

const void* GetData(const FunctionObject* self) {
  FunctionMock* const mock = FUNCTION_MOCK(self);
  return mock->gmock->GetData(self);
}

CmdType* CreateReplyCmd(const FunctionObject* self) {
  FunctionMock* const mock = FUNCTION_MOCK(self);
  return mock->gmock->CreateReplyCmd(self);
}

CmdType* CreateNotifyCmd(
    const FunctionObject* self,
    const void* new_data,
    const FilterType* filter_partial,
    const FilterType* filter_delete
) {
  FunctionMock* const mock = FUNCTION_MOCK(self);
  return mock->gmock->CreateNotifyCmd(self, new_data, filter_partial, filter_delete);
}

CmdType* CreateWriteCmd(
    const FunctionObject* self,
    const void* new_data,
    const FilterType* filter_partial,
    const FilterType* filter_delete
) {
  FunctionMock* const mock = FUNCTION_MOCK(self);
  return mock->gmock->CreateWriteCmd(self, new_data, filter_partial, filter_delete);
}

void* DataCopy(const FunctionObject* self) {
  FunctionMock* const mock = FUNCTION_MOCK(self);
  return mock->gmock->DataCopy(self);
}

EebusError UpdateData(
    FunctionObject* self,
    const void* new_data,
    const FilterType* filter_partial,
    const FilterType* filter_delete,
    bool wr_remote,
    bool persist
) {
  FunctionMock* const mock = FUNCTION_MOCK(self);
  return mock->gmock->UpdateData(self, new_data, filter_partial, filter_delete, wr_remote, persist);
}

const OperationsObject* GetOperations(const FunctionObject* self) {
  FunctionMock* const mock = FUNCTION_MOCK(self);
  return mock->gmock->GetOperations(self);
}

void SetOperations(FunctionObject* self, bool read, bool read_partial, bool write, bool write_partial) {
  FunctionMock* const mock = FUNCTION_MOCK(self);
  mock->gmock->SetOperations(self, read, read_partial, write, write_partial);
}
