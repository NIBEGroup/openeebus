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
 * @brief Data Writer Mock "class"
 */

#ifndef TESTS_UNIT_TESTS_MOCKS_SHIP_SHIP_CONNECTION_DATA_WRITER_MOCK_H_
#define TESTS_UNIT_TESTS_MOCKS_SHIP_SHIP_CONNECTION_DATA_WRITER_MOCK_H_

#include <gmock/gmock.h>

#include <memory>

#include "src/common/eebus_malloc.h"
#include "src/ship/api/data_writer_interface.h"

class DataWriterGMockInterface {
 public:
  virtual ~DataWriterGMockInterface() {};
  virtual void Destruct(DataWriterObject* self)                                          = 0;
  virtual void WriteMessage(DataWriterObject* self, const uint8_t* msg, size_t msg_size) = 0;
};

class DataWriterGMock : public DataWriterGMockInterface {
 public:
  virtual ~DataWriterGMock() {};
  MOCK_METHOD1(Destruct, void(DataWriterObject*));
  MOCK_METHOD3(WriteMessage, void(DataWriterObject*, const uint8_t*, size_t));
};

typedef struct DataWriterMock {
  /** Implements the Data Writer Interface */
  DataWriterObject obj;
  DataWriterGMock* gmock;
} DataWriterMock;

#define DATA_WRITER_MOCK(obj) ((DataWriterMock*)(obj))

DataWriterMock* DataWriterMockCreate(void);

static inline void DataWriterMockDelete(DataWriterMock* self) {
  if (self != nullptr) {
    DATA_WRITER_DESTRUCT(DATA_WRITER_OBJECT(self));
    EEBUS_FREE(self);
  }
}

#endif  // TESTS_UNIT_TESTS_MOCKS_SHIP_SHIP_CONNECTION_DATA_WRITER_MOCK_H_
