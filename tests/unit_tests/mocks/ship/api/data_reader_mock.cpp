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
 * @brief Data Reader mock implementation
 */

#include "data_reader_mock.h"

#include <gmock/gmock.h>

#include "src/common/eebus_errors.h"
#include "src/ship/api/data_reader_interface.h"

static void Destruct(DataReaderObject* self);
static void HandleMessage(DataReaderObject* self, MessageBuffer* msg);

static const DataReaderInterface data_reader_methods = {
    .destruct       = Destruct,
    .handle_message = HandleMessage,
};

static EebusError DataReaderMockConstruct(DataReaderMock* self);

EebusError DataReaderMockConstruct(DataReaderMock* self) {
  // Override "virtual functions table"
  DATA_READER_INTERFACE(self) = &data_reader_methods;

  self->gmock = new DataReaderGMock();
  if (self->gmock == nullptr) {
    return kEebusErrorMemoryAllocate;
  }

  return kEebusErrorOk;
}

DataReaderMock* DataReaderMockCreate(void) {
  DataReaderMock* const mock = (DataReaderMock*)EEBUS_MALLOC(sizeof(DataReaderMock));
  if (mock == nullptr) {
    return nullptr;
  }

  if (DataReaderMockConstruct(mock) != kEebusErrorOk) {
    DataReaderMockDelete(mock);
    return nullptr;
  }

  return mock;
}

void Destruct(DataReaderObject* self) {
  DataReaderMock* const mock = DATA_READER_MOCK(self);
  mock->gmock->Destruct(self);
  delete mock->gmock;
}

void HandleMessage(DataReaderObject* self, MessageBuffer* msg) {
  DataReaderMock* const mock = DATA_READER_MOCK(self);
  mock->gmock->HandleMessage(self, msg);
}
