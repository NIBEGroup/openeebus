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
 * @brief Tls Certificate mock implementation
 */

#include "tls_certificate_mock.h"

#include <gmock/gmock.h>

#include "src/common/eebus_errors.h"
#include "src/ship/api/tls_certificate_interface.h"

static void Destruct(TlsCertificateObject* self);
static const char* GetSki(const TlsCertificateObject* self);
static const void* GetCertificate(const TlsCertificateObject* self);
static size_t GetCertificateSize(const TlsCertificateObject* self);
static const void* GetPrivateKey(const TlsCertificateObject* self);
static size_t GetPrivateKeySize(const TlsCertificateObject* self);

static const TlsCertificateInterface tls_certificate_methods = {
    .destruct             = Destruct,
    .get_ski              = GetSki,
    .get_certificate      = GetCertificate,
    .get_certificate_size = GetCertificateSize,
    .get_private_key      = GetPrivateKey,
    .get_private_key_size = GetPrivateKeySize,
};

static EebusError TlsCertificateMockConstruct(TlsCertificateMock* self);

EebusError TlsCertificateMockConstruct(TlsCertificateMock* self) {
  // Override "virtual functions table"
  TLS_CERTIFICATE_INTERFACE(self) = &tls_certificate_methods;

  self->gmock = new TlsCertificateGMock();
  if (self->gmock == nullptr) {
    return kEebusErrorMemoryAllocate;
  }

  return kEebusErrorOk;
}

TlsCertificateMock* TlsCertificateMockCreate(void) {
  TlsCertificateMock* const mock = (TlsCertificateMock*)EEBUS_MALLOC(sizeof(TlsCertificateMock));
  if (mock == nullptr) {
    return nullptr;
  }

  if (TlsCertificateMockConstruct(mock) != kEebusErrorOk) {
    TlsCertificateMockDelete(mock);
    return nullptr;
  }

  return mock;
}

void Destruct(TlsCertificateObject* self) {
  TlsCertificateMock* const mock = TLS_CERTIFICATE_MOCK(self);
  mock->gmock->Destruct(self);
  delete mock->gmock;
}

const char* GetSki(const TlsCertificateObject* self) {
  TlsCertificateMock* const mock = TLS_CERTIFICATE_MOCK(self);
  return mock->gmock->GetSki(self);
}

const void* GetCertificate(const TlsCertificateObject* self) {
  TlsCertificateMock* const mock = TLS_CERTIFICATE_MOCK(self);
  return mock->gmock->GetCertificate(self);
}

size_t GetCertificateSize(const TlsCertificateObject* self) {
  TlsCertificateMock* const mock = TLS_CERTIFICATE_MOCK(self);
  return mock->gmock->GetCertificateSize(self);
}

const void* GetPrivateKey(const TlsCertificateObject* self) {
  TlsCertificateMock* const mock = TLS_CERTIFICATE_MOCK(self);
  return mock->gmock->GetPrivateKey(self);
}

size_t GetPrivateKeySize(const TlsCertificateObject* self) {
  TlsCertificateMock* const mock = TLS_CERTIFICATE_MOCK(self);
  return mock->gmock->GetPrivateKeySize(self);
}
