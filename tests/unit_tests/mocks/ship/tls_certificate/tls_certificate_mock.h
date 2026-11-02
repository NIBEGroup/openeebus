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
 * @brief Tls Certificate Mock "class"
 */

#ifndef TESTS_UNIT_TESTS_MOCKS_SHIP_TLS_CERTIFICATE_TLS_CERTIFICATE_MOCK_H_
#define TESTS_UNIT_TESTS_MOCKS_SHIP_TLS_CERTIFICATE_TLS_CERTIFICATE_MOCK_H_

#include <gmock/gmock.h>

#include <memory>

#include "src/common/eebus_malloc.h"
#include "src/ship/api/tls_certificate_interface.h"

class TlsCertificateGMockInterface {
 public:
  virtual ~TlsCertificateGMockInterface() {};
  virtual void Destruct(TlsCertificateObject* self)                    = 0;
  virtual const char* GetSki(const TlsCertificateObject* self)         = 0;
  virtual const void* GetCertificate(const TlsCertificateObject* self) = 0;
  virtual size_t GetCertificateSize(const TlsCertificateObject* self)  = 0;
  virtual const void* GetPrivateKey(const TlsCertificateObject* self)  = 0;
  virtual size_t GetPrivateKeySize(const TlsCertificateObject* self)   = 0;
};

class TlsCertificateGMock : public TlsCertificateGMockInterface {
 public:
  virtual ~TlsCertificateGMock() {};
  MOCK_METHOD1(Destruct, void(TlsCertificateObject*));
  MOCK_METHOD1(GetSki, const char*(const TlsCertificateObject*));
  MOCK_METHOD1(GetCertificate, const void*(const TlsCertificateObject*));
  MOCK_METHOD1(GetCertificateSize, size_t(const TlsCertificateObject*));
  MOCK_METHOD1(GetPrivateKey, const void*(const TlsCertificateObject*));
  MOCK_METHOD1(GetPrivateKeySize, size_t(const TlsCertificateObject*));
};

typedef struct TlsCertificateMock {
  /** Implements the Tls Certificate Interface */
  TlsCertificateObject obj;
  TlsCertificateGMock* gmock;
} TlsCertificateMock;

#define TLS_CERTIFICATE_MOCK(obj) ((TlsCertificateMock*)(obj))

TlsCertificateMock* TlsCertificateMockCreate(void);

static inline void TlsCertificateMockDelete(TlsCertificateMock* self) {
  if (self != nullptr) {
    TLS_CERTIFICATE_DESTRUCT(TLS_CERTIFICATE_OBJECT(self));
    EEBUS_FREE(self);
  }
}

#endif  // TESTS_UNIT_TESTS_MOCKS_SHIP_TLS_CERTIFICATE_TLS_CERTIFICATE_MOCK_H_
