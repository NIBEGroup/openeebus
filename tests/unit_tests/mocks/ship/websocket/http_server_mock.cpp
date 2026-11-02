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
 * @brief Http Server mock implementation
 */

#include "http_server_mock.h"

#include <gmock/gmock.h>

#include "src/ship/api/http_server_interface.h"

static void Destruct(HttpServerObject* self);
static EebusError Start(HttpServerObject* self);
static void Stop(HttpServerObject* self);

static const HttpServerInterface http_server_methods = {
    .destruct = Destruct,
    .start    = Start,
    .stop     = Stop,
};

static EebusError HttpServerMockConstruct(HttpServerMock* self);

EebusError HttpServerMockConstruct(HttpServerMock* self) {
  // Override "virtual functions table"
  HTTP_SERVER_INTERFACE(self) = &http_server_methods;

  self->gmock = new HttpServerGMock();
  if (self->gmock == nullptr) {
    return kEebusErrorMemoryAllocate;
  }

  return kEebusErrorOk;
}

HttpServerMock* HttpServerMockCreate(void) {
  HttpServerMock* const mock = (HttpServerMock*)EEBUS_MALLOC(sizeof(HttpServerMock));
  if (mock == nullptr) {
    return nullptr;
  }

  if (HttpServerMockConstruct(mock) != kEebusErrorOk) {
    HttpServerMockDelete(mock);
    return nullptr;
  }

  return mock;
}

void Destruct(HttpServerObject* self) {
  HttpServerMock* const mock = HTTP_SERVER_MOCK(self);
  mock->gmock->Destruct(self);
  delete mock->gmock;
}

EebusError Start(HttpServerObject* self) {
  HttpServerMock* const mock = HTTP_SERVER_MOCK(self);
  return mock->gmock->Start(self);
}

void Stop(HttpServerObject* self) {
  HttpServerMock* const mock = HTTP_SERVER_MOCK(self);
  mock->gmock->Stop(self);
}
