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
 * @brief Info Provider Mock "class"
 */

#ifndef TESTS_UNIT_TESTS_MOCKS_SHIP_API_INFO_PROVIDER_MOCK_H_
#define TESTS_UNIT_TESTS_MOCKS_SHIP_API_INFO_PROVIDER_MOCK_H_

#include <gmock/gmock.h>

#include <memory>

#include "src/common/eebus_malloc.h"
#include "src/ship/api/info_provider_interface.h"

class InfoProviderMockInterface {
 public:
  virtual ~InfoProviderMockInterface() {};
  virtual void Destruct(InfoProviderObject* self)                                                                = 0;
  virtual bool IsRemoteServiceForSkiPaired(InfoProviderObject* self, const char* ski)                            = 0;
  virtual void HandleConnectionClosed(InfoProviderObject* self, ShipConnectionObject* sc, bool had_error)        = 0;
  virtual void ReportServiceShipId(InfoProviderObject* self, const char* service_id, const char* ship_id)        = 0;
  virtual bool IsWaitingForTrustAllowed(InfoProviderObject*, const char* ski)                                    = 0;
  virtual void HandleShipStateUpdate(InfoProviderObject* self, const char* ski, SmeState state, const char* err) = 0;
  virtual DataReaderObject* SetupRemoteDevice(InfoProviderObject* self, const char* ski, DataWriterObject* data_writer)
      = 0;
};

class InfoProviderGMock : public InfoProviderMockInterface {
 public:
  virtual ~InfoProviderGMock() {};
  MOCK_METHOD1(Destruct, void(InfoProviderObject*));
  MOCK_METHOD2(IsRemoteServiceForSkiPaired, bool(InfoProviderObject*, const char*));
  MOCK_METHOD3(HandleConnectionClosed, void(InfoProviderObject*, ShipConnectionObject*, bool));
  MOCK_METHOD3(ReportServiceShipId, void(InfoProviderObject*, const char*, const char*));
  MOCK_METHOD2(IsWaitingForTrustAllowed, bool(InfoProviderObject*, const char*));
  MOCK_METHOD4(HandleShipStateUpdate, void(InfoProviderObject*, const char*, SmeState, const char*));
  MOCK_METHOD3(
      SetupRemoteDevice,
      DataReaderObject*(InfoProviderObject* self, const char* ski, DataWriterObject* data_writer)
  );
};

typedef struct InfoProviderMock {
  /** Implements the Info Provider Interface */
  InfoProviderObject obj;
  InfoProviderGMock* gmock;
} InfoProviderMock;

#define INFO_PROVIDER_MOCK(obj) ((InfoProviderMock*)(obj))

InfoProviderMock* InfoProviderMockCreate(void);

static inline void InfoProviderMockDelete(InfoProviderMock* self) {
  if (self != nullptr) {
    INFO_PROVIDER_DESTRUCT(INFO_PROVIDER_OBJECT(self));
    EEBUS_FREE(self);
  }
}

#endif  // TESTS_UNIT_TESTS_MOCKS_SHIP_API_INFO_PROVIDER_MOCK_H_
