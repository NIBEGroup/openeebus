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
 * @brief Data Writer mock implementation
 */

#include "data_writer_mock.h"

#include <gmock/gmock.h>

#include "src/common/eebus_errors.h"
#include "src/ship/api/data_writer_interface.h"

static void Destruct(DataWriterObject* self);
static void WriteMessage(DataWriterObject* self, const uint8_t* msg, size_t msg_size);

static const DataWriterInterface data_writer_methods = {
    .destruct      = Destruct,
    .write_message = WriteMessage,
};

static EebusError DataWriterMockConstruct(DataWriterMock* self);

EebusError DataWriterMockConstruct(DataWriterMock* self) {
  // Override "virtual functions table"
  DATA_WRITER_INTERFACE(self) = &data_writer_methods;

  self->gmock = new DataWriterGMock();
  if (self->gmock == nullptr) {
    return kEebusErrorMemoryAllocate;
  }

  return kEebusErrorOk;
}

DataWriterMock* DataWriterMockCreate(void) {
  DataWriterMock* const mock = (DataWriterMock*)EEBUS_MALLOC(sizeof(DataWriterMock));
  if (mock == nullptr) {
    return nullptr;
  }

  if (DataWriterMockConstruct(mock) != kEebusErrorOk) {
    DataWriterMockDelete(mock);
    return nullptr;
  }

  return mock;
}

void Destruct(DataWriterObject* self) {
  DataWriterMock* const mock = DATA_WRITER_MOCK(self);
  mock->gmock->Destruct(self);
  delete mock->gmock;
}

void WriteMessage(DataWriterObject* self, const uint8_t* msg, size_t msg_size) {
  DataWriterMock* const mock = DATA_WRITER_MOCK(self);
  mock->gmock->WriteMessage(self, msg, msg_size);
}
