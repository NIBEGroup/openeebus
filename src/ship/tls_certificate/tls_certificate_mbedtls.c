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
 * @brief Tls Certificate mbedTLS based implementation
 */

#include <mbedtls/x509.h>
#include <mbedtls/x509_crt.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#if defined(MBEDTLS_VERSION_MAJOR) && (MBEDTLS_VERSION_MAJOR >= 4)
// mbedTLS 4.x provides hashing through PSA only
#include <psa/crypto.h>
#endif

#include "src/common/array_util.h"
#include "src/common/debug.h"
#include "src/common/eebus_errors.h"
#include "src/common/eebus_malloc.h"
#include "src/common/string_util.h"
#include "src/ship/tls_certificate/tls_certificate.h"

/** Set TLS_CERTIFICATE_MBEDTLS_DEBUG 1 to enable debug prints */
#ifndef TLS_CERTIFICATE_MBEDTLS_DEBUG
#define TLS_CERTIFICATE_MBEDTLS_DEBUG 0
#endif

/** Tls Certificate mbedTLS debug printf(), enabled whith TLS_CERTIFICATE_MBEDTLS_DEBUG = 1 */
#if TLS_CERTIFICATE_MBEDTLS_DEBUG
#define TLS_CERTIFICATE_MBEDTLS_DEBUG_PRINTF(fmt, ...) DebugPrintf(fmt, ##__VA_ARGS__)
#else
#define TLS_CERTIFICATE_MBEDTLS_DEBUG_PRINTF(fmt, ...)
#endif  // TLS_CERTIFICATE_MBEDTLS_DEBUG

#define PKEY_BUF_SIZE 2048

typedef struct TlsCertificate TlsCertificate;

struct TlsCertificate {
  /** Implements the Tls Certificate Interface */
  TlsCertificateObject obj;

  const uint8_t* cert;
  int32_t cert_size;

  const uint8_t* pkey;
  int32_t pkey_size;

  const char* ski;
};

#define TLS_CERTIFICATE(obj) ((TlsCertificate*)(obj))

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

static void TlsCertificateConstruct(TlsCertificate* self);
static const char* CalcSubjectKeyIdStringWithBuf(const mbedtls_x509_crt* cert, unsigned char* buf, size_t buf_size);
static const char* CalcSubjectKeyIdString(const mbedtls_x509_crt* cert);
static bool CheckSki(const mbedtls_x509_crt* cert, const char* ski);
static EebusError ParseX509Certificate(TlsCertificate* self, const char* cert_buf, size_t cert_buf_size);
static EebusError ParseX509PrivateKey(TlsCertificate* self, const char* key_buf, size_t key_buf_size);

void TlsCertificateConstruct(TlsCertificate* self) {
  // Override "virtual functions table"
  TLS_CERTIFICATE_INTERFACE(self) = &tls_certificate_methods;

  self->cert      = NULL;
  self->cert_size = 0;
  self->pkey      = NULL;
  self->pkey_size = 0;
  self->ski       = NULL;
}

const char* CalcSubjectKeyIdStringWithBuf(const mbedtls_x509_crt* cert, unsigned char* buf, size_t buf_size) {
  const mbedtls_pk_context* const pk = &cert->pk;

  int len = mbedtls_pk_write_pubkey_der(pk, buf, buf_size);
  if (len < 0) {
    TLS_CERTIFICATE_MBEDTLS_DEBUG_PRINTF("mbedtls_pk_write_pubkey_der failed: -0x%04X\n", -len);
    return NULL;
  }

  // Since mbedtls_pk_write_pubkey_der writes data at the end of the buffer
  unsigned char* p = buf + buf_size - len;

  // Verify and skip the outer SEQUENCE
  if ((len < 4) || (p[0] != 0x30)) {
    TLS_CERTIFICATE_MBEDTLS_DEBUG_PRINTF("Invalid outer ASN.1 sequence\n");
    return NULL;
  }

  // Skip the outer sequence header
  int header_size = 2;
  if (p[1] & 0x80) {
    header_size = 2 + (p[1] & 0x7F);
  }

  p += header_size;
  len -= header_size;

  // Skip the AlgorithmIdentifier sequence
  if ((len < 2) || (p[0] != 0x30)) {
    TLS_CERTIFICATE_MBEDTLS_DEBUG_PRINTF("Invalid AlgorithmIdentifier sequence\n");
    return NULL;
  }

  int alg_len = p[1] + 2;  // Include sequence header
  p += alg_len;
  len -= alg_len;

  // Skip the BIT STRING tag and length
  if ((len < 2) || (p[0] != 0x03)) {
    TLS_CERTIFICATE_MBEDTLS_DEBUG_PRINTF("Invalid BIT STRING tag\n");
    return NULL;
  }

  header_size = 2;
  if (p[1] & 0x80) {
    header_size = 2 + (p[1] & 0x7F);
  }

  p += header_size;
  len -= header_size;

  // Skip unused bits byte
  p += 1;
  len -= 1;

  // Calculate SHA1 hash of the remaining raw key data
  unsigned char sha1[20];
#if MBEDTLS_VERSION_MAJOR >= 4
  // mbedTLS 4.x removed the legacy hash API, use PSA
  size_t sha1_len = 0;
  int ret         = (int)psa_hash_compute(PSA_ALG_SHA_1, p, len, sha1, sizeof(sha1), &sha1_len);
#else
  int ret = mbedtls_sha1(p, len, sha1);
#endif
  if (ret != 0) {
    TLS_CERTIFICATE_MBEDTLS_DEBUG_PRINTF("mbedtls_sha1 failed: -0x%04X\n", -ret);
    return NULL;
  }

  return StringWithHex(sha1, sizeof(sha1));
}

const char* CalcSubjectKeyIdString(const mbedtls_x509_crt* cert) {
  static const size_t kBufSize = 2048;

  unsigned char* buf = EEBUS_MALLOC(kBufSize);
  if (buf == NULL) {
    TLS_CERTIFICATE_MBEDTLS_DEBUG_PRINTF("Failed to allocate memory for Subject Key Identifier buffer\n");
    return NULL;
  }

  const char* const ski = CalcSubjectKeyIdStringWithBuf(cert, buf, kBufSize);
  EEBUS_FREE(buf);
  return ski;
}

bool CheckSki(const mbedtls_x509_crt* cert, const char* ski) {
  if ((cert == NULL) || (ski == NULL)) {
    return false;
  }

  if ((cert->subject_key_id.p == NULL) || (cert->subject_key_id.len == 0)) {
    TLS_CERTIFICATE_MBEDTLS_DEBUG_PRINTF("Subject Key Identifier is empty\n");
    return false;
  }

  const char* const ski_stored = StringWithHex(cert->subject_key_id.p, cert->subject_key_id.len);
  if (ski_stored == NULL) {
    TLS_CERTIFICATE_MBEDTLS_DEBUG_PRINTF("Failed to allocate memory for stored Subject Key Identifier\n");
    return false;
  }

  const bool ok = !strcmp(ski, ski_stored);
  StringDelete((char*)ski_stored);
  return ok;
}

EebusError ParseX509Certificate(TlsCertificate* self, const char* cert_buf, size_t cert_buf_size) {
  char* pem_buf = EEBUS_MALLOC(cert_buf_size + 1);
  if (pem_buf == NULL) {
    return kEebusErrorMemory;
  }

  memcpy(pem_buf, cert_buf, cert_buf_size);
  pem_buf[cert_buf_size] = '\0';  // Null-terminate

  mbedtls_x509_crt cert;
  mbedtls_x509_crt_init(&cert);
  int ret = mbedtls_x509_crt_parse(&cert, (void*)pem_buf, cert_buf_size + 1);
  EEBUS_FREE(pem_buf);
  if (ret != 0) {
    TLS_CERTIFICATE_MBEDTLS_DEBUG_PRINTF("mbedtls_x509_crt_parse failed: -0x%04X\n", -ret);
    return kEebusErrorInit;
  }

  // DER data is available in cert.raw.p and cert.raw.len
  self->cert      = ArrayCopy(cert.raw.p, cert.raw.len, sizeof(uint8_t));
  self->cert_size = cert.raw.len;
  if (self->cert == NULL) {
    TLS_CERTIFICATE_MBEDTLS_DEBUG_PRINTF("Failed to allocate memory for certificate\n");
    mbedtls_x509_crt_free(&cert);
    return kEebusErrorMemoryAllocate;
  }

  self->ski     = CalcSubjectKeyIdString(&cert);
  const bool ok = CheckSki(&cert, self->ski);
  if (!ok) {
    TLS_CERTIFICATE_MBEDTLS_DEBUG_PRINTF("Invalid Subject Key Identifier\n");
  }

  mbedtls_x509_crt_free(&cert);
  return ok ? kEebusErrorOk : kEebusErrorInit;
}

EebusError ParseX509PrivateKey(TlsCertificate* self, const char* key_buf, size_t key_buf_size) {
  char* pem_buf = EEBUS_MALLOC(key_buf_size + 1);
  if (pem_buf == NULL) {
    return kEebusErrorMemory;
  }

  memcpy(pem_buf, key_buf, key_buf_size);
  pem_buf[key_buf_size] = '\0';  // Null-terminate

  mbedtls_pk_context pk;
  mbedtls_pk_init(&pk);
#if MBEDTLS_VERSION_MAJOR >= 4
  // mbedTLS 4.x dropped the RNG arguments (PSA provides RNG)
  int ret = mbedtls_pk_parse_key(&pk, (void*)pem_buf, key_buf_size + 1, NULL, 0);
#else
  int ret = mbedtls_pk_parse_key(&pk, (void*)pem_buf, key_buf_size + 1, NULL, 0, NULL, NULL);
#endif
  EEBUS_FREE(pem_buf);

  if (ret != 0) {
    TLS_CERTIFICATE_MBEDTLS_DEBUG_PRINTF("mbedtls_pk_parse_key failed: -0x%04X\n", -ret);
    return kEebusErrorInit;
  }

  // DER data is available in pk.raw.p and pk.raw.len
  uint8_t* pkey_buf = EEBUS_MALLOC(PKEY_BUF_SIZE);
  if (pkey_buf == NULL) {
    return kEebusErrorMemoryAllocate;
  }

  memset(pkey_buf, 0, PKEY_BUF_SIZE);
  self->pkey_size = mbedtls_pk_write_key_der(&pk, pkey_buf, PKEY_BUF_SIZE);
  if (self->pkey_size == 0) {
    TLS_CERTIFICATE_MBEDTLS_DEBUG_PRINTF("mbedtls_pk_write_key_der failed\n");
    EEBUS_FREE(pkey_buf);
    return kEebusErrorInit;
  }

  const int offset = PKEY_BUF_SIZE - self->pkey_size;
  self->pkey       = ArrayCopy(pkey_buf + offset, self->pkey_size, sizeof(uint8_t));
  EEBUS_FREE(pkey_buf);
  if (self->pkey == NULL) {
    TLS_CERTIFICATE_MBEDTLS_DEBUG_PRINTF("Failed to allocate memory for private key\n");
    return kEebusErrorMemoryAllocate;
  }

  return kEebusErrorOk;
}

TlsCertificateObject* TlsCertificateLoadX509KeyPair(const char* cert_file, const char* key_file) {
  return NULL;  // Not implemented
}

TlsCertificateObject*
TlsCertificateParseX509KeyPair(const char* cert_buf, size_t cert_buf_size, const char* key_buf, size_t key_buf_size) {
  TlsCertificate* const tls_certificate = (TlsCertificate*)EEBUS_MALLOC(sizeof(TlsCertificate));
  if (tls_certificate == NULL) {
    return NULL;
  }

  TlsCertificateConstruct(tls_certificate);

  if ((ParseX509Certificate(tls_certificate, cert_buf, cert_buf_size) != kEebusErrorOk)
      || (ParseX509PrivateKey(tls_certificate, key_buf, key_buf_size) != kEebusErrorOk)) {
    TlsCertificateDelete(TLS_CERTIFICATE_OBJECT(tls_certificate));
    return NULL;
  }

  return TLS_CERTIFICATE_OBJECT(tls_certificate);
}

void Destruct(TlsCertificateObject* self) {
  TlsCertificate* const tls_cert = TLS_CERTIFICATE(self);

  if (tls_cert->pkey != NULL) {
    EEBUS_FREE((uint8_t*)tls_cert->pkey);
    tls_cert->pkey = NULL;
  }

  if (tls_cert->cert != NULL) {
    EEBUS_FREE((uint8_t*)tls_cert->cert);
    tls_cert->cert = NULL;
  }

  StringDelete((char*)tls_cert->ski);
  tls_cert->ski = NULL;
}

const char* GetSki(const TlsCertificateObject* self) {
  return TLS_CERTIFICATE(self)->ski;
}

const void* GetCertificate(const TlsCertificateObject* self) {
  return TLS_CERTIFICATE(self)->cert;
}

size_t GetCertificateSize(const TlsCertificateObject* self) {
  return TLS_CERTIFICATE(self)->cert_size;
}

const void* GetPrivateKey(const TlsCertificateObject* self) {
  return TLS_CERTIFICATE(self)->pkey;
}

size_t GetPrivateKeySize(const TlsCertificateObject* self) {
  return TLS_CERTIFICATE(self)->pkey_size;
}

const char* TlsCertificateCalcPublicKeySki(const uint8_t* cert, size_t cert_size) {
  if ((cert == NULL) || (cert_size == 0)) {
    return NULL;
  }

  const unsigned char* p = cert;
  mbedtls_x509_crt x509_cert;
  mbedtls_x509_crt_init(&x509_cert);
  int ret = mbedtls_x509_crt_parse_der(&x509_cert, p, cert_size);
  if (ret != 0) {
    TLS_CERTIFICATE_MBEDTLS_DEBUG_PRINTF("mbedtls_x509_crt_parse_der failed: -0x%04X\n", -ret);
    return NULL;
  }

  const char* const ski = CalcSubjectKeyIdString(&x509_cert);
  mbedtls_x509_crt_free(&x509_cert);
  return ski;
}
