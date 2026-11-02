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
 * @brief Http Server Mock "class"
 */

#ifndef TESTS_UNIT_TESTS_MOCKS_SHIP_WEBSOCKET_HTTP_SERVER_MOCK_H_
#define TESTS_UNIT_TESTS_MOCKS_SHIP_WEBSOCKET_HTTP_SERVER_MOCK_H_

#include <gmock/gmock.h>

#include <memory>

#include "src/common/eebus_malloc.h"
#include "src/ship/api/http_server_interface.h"

class HttpServerGMockInterface {
 public:
  virtual ~HttpServerGMockInterface() {};
  virtual void Destruct(HttpServerObject* self)    = 0;
  virtual EebusError Start(HttpServerObject* self) = 0;
  virtual void Stop(HttpServerObject* self)        = 0;
};

class HttpServerGMock : public HttpServerGMockInterface {
 public:
  virtual ~HttpServerGMock() {};
  MOCK_METHOD1(Destruct, void(HttpServerObject*));
  MOCK_METHOD1(Start, EebusError(HttpServerObject*));
  MOCK_METHOD1(Stop, void(HttpServerObject*));
};

typedef struct HttpServerMock {
  /** Implements the Http Server Interface */
  HttpServerObject obj;
  HttpServerGMock* gmock;
} HttpServerMock;

#define HTTP_SERVER_MOCK(obj) ((HttpServerMock*)(obj))

HttpServerMock* HttpServerMockCreate(void);

static inline void HttpServerMockDelete(HttpServerMock* self) {
  if (self != nullptr) {
    HTTP_SERVER_DESTRUCT(HTTP_SERVER_OBJECT(self));
    EEBUS_FREE(self);
  }
}

#endif  // TESTS_UNIT_TESTS_MOCKS_SHIP_WEBSOCKET_HTTP_SERVER_MOCK_H_
