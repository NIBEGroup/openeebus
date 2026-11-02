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
 * @brief Data Reader Mock "class"
 */

#ifndef TESTS_UNIT_TESTS_MOCKS_SHIP_API_DATA_READER_MOCK_H_
#define TESTS_UNIT_TESTS_MOCKS_SHIP_API_DATA_READER_MOCK_H_

#include <gmock/gmock.h>

#include <memory>

#include "src/common/eebus_malloc.h"
#include "src/ship/api/data_reader_interface.h"

class DataReaderGMockInterface {
 public:
  virtual ~DataReaderGMockInterface() {};
  virtual void Destruct(DataReaderObject* self)                          = 0;
  virtual void HandleMessage(DataReaderObject* self, MessageBuffer* msg) = 0;
};

class DataReaderGMock : public DataReaderGMockInterface {
 public:
  virtual ~DataReaderGMock() {};
  MOCK_METHOD1(Destruct, void(DataReaderObject*));
  MOCK_METHOD2(HandleMessage, void(DataReaderObject*, MessageBuffer*));
};

typedef struct DataReaderMock {
  /** Implements the Data Reader Interface */
  DataReaderObject obj;
  DataReaderGMock* gmock;
} DataReaderMock;

#define DATA_READER_MOCK(obj) ((DataReaderMock*)(obj))

DataReaderMock* DataReaderMockCreate(void);

static inline void DataReaderMockDelete(DataReaderMock* self) {
  if (self != nullptr) {
    DATA_READER_DESTRUCT(DATA_READER_OBJECT(self));
    EEBUS_FREE(self);
  }
}

#endif  // TESTS_UNIT_TESTS_MOCKS_SHIP_API_DATA_READER_MOCK_H_
