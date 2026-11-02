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
 * @brief Service Reader Mock "class"
 */

#ifndef TESTS_UNIT_TESTS_MOCKS_SERVICE_API_SERVICE_READER_MOCK_H_
#define TESTS_UNIT_TESTS_MOCKS_SERVICE_API_SERVICE_READER_MOCK_H_

#include <gmock/gmock.h>

#include <memory>

#include "src/common/eebus_malloc.h"
#include "src/service/api/service_reader_interface.h"

class ServiceReaderGMockInterface {
 public:
  virtual ~ServiceReaderGMockInterface() {};
  virtual void Destruct(ServiceReaderObject* self)                                                              = 0;
  virtual void OnRemoteSkiConnected(ServiceReaderObject* self, EebusServiceObject* service, const char* ski)    = 0;
  virtual void OnRemoteSkiDisconnected(ServiceReaderObject* self, EebusServiceObject* service, const char* ski) = 0;
  virtual void OnRemoteServicesUpdate(ServiceReaderObject* self, EebusServiceObject* service, const Vector* entries)
      = 0;
  virtual void OnShipIdUpdate(ServiceReaderObject* self, const char* ski, const char* shipd_id) = 0;
  virtual void OnShipStateUpdate(ServiceReaderObject* self, const char* ski, SmeState state)    = 0;
  virtual bool IsWaitingForTrustAllowed(const ServiceReaderObject* self, const char* ski)       = 0;
};

class ServiceReaderGMock : public ServiceReaderGMockInterface {
 public:
  virtual ~ServiceReaderGMock() {};
  MOCK_METHOD1(Destruct, void(ServiceReaderObject*));
  MOCK_METHOD3(OnRemoteSkiConnected, void(ServiceReaderObject*, EebusServiceObject*, const char*));
  MOCK_METHOD3(OnRemoteSkiDisconnected, void(ServiceReaderObject*, EebusServiceObject*, const char*));
  MOCK_METHOD3(OnRemoteServicesUpdate, void(ServiceReaderObject*, EebusServiceObject*, const Vector*));
  MOCK_METHOD3(OnShipIdUpdate, void(ServiceReaderObject*, const char*, const char*));
  MOCK_METHOD3(OnShipStateUpdate, void(ServiceReaderObject*, const char*, SmeState));
  MOCK_METHOD2(IsWaitingForTrustAllowed, bool(const ServiceReaderObject*, const char*));
};

typedef struct ServiceReaderMock {
  /** Implements the Service Reader Interface */
  ServiceReaderObject obj;
  ServiceReaderGMock* gmock;
} ServiceReaderMock;

#define SERVICE_READER_MOCK(obj) ((ServiceReaderMock*)(obj))

ServiceReaderMock* ServiceReaderMockCreate(void);

static inline void ServiceReaderMockDelete(ServiceReaderMock* self) {
  if (self != nullptr) {
    SERVICE_READER_DESTRUCT(SERVICE_READER_OBJECT(self));
    EEBUS_FREE(self);
  }
}

#endif  // TESTS_UNIT_TESTS_MOCKS_SERVICE_API_SERVICE_READER_MOCK_H_
