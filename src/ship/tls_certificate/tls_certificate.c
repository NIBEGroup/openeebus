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
 * @brief Tls Certificate implementation
 */

#include <openssl/asn1.h>
#include <openssl/crypto.h>
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <stdbool.h>
#include <stdint.h>

#include "src/common/eebus_arguments.h"
#include "src/common/eebus_errors.h"
#include "src/common/eebus_malloc.h"
#include "src/common/string_util.h"
#include "src/ship/tls_certificate/tls_certificate.h"

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
// Private methods
static X509* PemCertRead(const char* cert_file);
static const char* CalcSubjectKeyIdString(X509* cert);
static bool CheckSki(const X509* cert, const char* ski);
static EebusError LoadX509Certificate(TlsCertificate* self, const char* cert_file);
static EVP_PKEY* PemPrivateKeyRead(const char* key_file);
static EebusError LoadX509PrivateKey(TlsCertificate* self, const char* key_file);

void TlsCertificateConstruct(TlsCertificate* self) {
  // Override "virtual functions table"
  TLS_CERTIFICATE_INTERFACE(self) = &tls_certificate_methods;

  self->cert      = NULL;
  self->cert_size = 0;
  self->pkey      = NULL;
  self->pkey_size = 0;
  self->ski       = NULL;
}

X509* PemCertRead(const char* cert_file) {
  FILE* const fp = fopen(cert_file, "r");
  if (fp == NULL) {
    return NULL;
  }

  X509* const cert = PEM_read_X509(fp, NULL, NULL, NULL);

  fclose(fp);

  return cert;
}

const char* CalcSubjectKeyIdString(X509* cert) {
  EVP_PKEY* pkey = X509_get_pubkey(cert);
  if (!pkey) {
    return NULL;
  }

  // Get the SubjectPublicKeyInfo
  const unsigned char* pk = NULL;
  X509_ALGOR* alg         = NULL;
  X509_PUBKEY* pub_key    = NULL;
  if (X509_PUBKEY_set(&pub_key, pkey) != 1) {
    EVP_PKEY_free(pkey);
    return NULL;
  }

  int pk_len = 0;
  X509_PUBKEY_get0_param(NULL, &pk, &pk_len, &alg, pub_key);
  if ((pk == NULL) || (pk_len <= 0)) {
    EVP_PKEY_free(pkey);
    X509_PUBKEY_free(pub_key);
    return NULL;
  }

  // Hash the public key bit string
  unsigned char sha1[SHA_DIGEST_LENGTH];
  SHA1(pk, pk_len, sha1);

  const char* ski = StringWithHex(sha1, SHA_DIGEST_LENGTH);
  EVP_PKEY_free(pkey);
  X509_PUBKEY_free(pub_key);

  return ski;
}

bool CheckSki(const X509* cert, const char* ski) {
  if ((cert == NULL) || (ski == NULL)) {
    return false;
  }

  const int32_t loc = X509_get_ext_by_NID(cert, NID_subject_key_identifier, -1);
  if (loc < 0) {
    return false;
  }

  X509_EXTENSION* const ext = X509_get_ext(cert, loc);
  if (ext == NULL) {
    return false;
  }

  ASN1_OCTET_STRING* const value = X509_EXTENSION_get_data(ext);
  if (value == NULL) {
    return false;
  }

  const uint8_t* const asn1_string = ASN1_STRING_get0_data(value);
  const int32_t asn1_string_len    = ASN1_STRING_length(value);

  if ((asn1_string == NULL) || (asn1_string_len < 2)) {
    return false;
  }

  // asn1_string[0] - type = 4 (Octet String)
  // asn1_string[1] - length = 20 (Actual number of bytes in value)
  // asn1_string[2] - Start value bytes
  const char* const ski_stored = StringWithHex(&asn1_string[2], asn1_string_len - 2);
  if (ski_stored == NULL) {
    return false;
  }

  const bool ok = !strcmp(ski, ski_stored);
  StringDelete((char*)ski_stored);
  return ok;
}

EebusError LoadX509Certificate(TlsCertificate* self, const char* cert_file) {
  X509* const pem_cert = PemCertRead(cert_file);
  if (pem_cert == NULL) {
    return kEebusErrorFileSystemNoFile;
  }

  // Transform PEM -> DER
  uint8_t* cert = NULL;

  self->cert_size = i2d_X509(pem_cert, &cert);

  self->cert = cert;
  if ((self->cert == NULL) || (self->cert_size == 0)) {
    X509_free(pem_cert);
    return kEebusErrorMemoryAllocate;
  }

  self->ski     = CalcSubjectKeyIdString(pem_cert);
  const bool ok = CheckSki(pem_cert, self->ski);

  X509_free(pem_cert);

  return ok ? kEebusErrorOk : kEebusErrorInit;
}

EVP_PKEY* PemPrivateKeyRead(const char* key_file) {
  FILE* const fp = fopen(key_file, "r");
  if (fp == NULL) {
    return NULL;
  }

  EVP_PKEY* const pkey = PEM_read_PrivateKey(fp, NULL, NULL, NULL);

  fclose(fp);

  return pkey;
}

EebusError LoadX509PrivateKey(TlsCertificate* self, const char* key_file) {
  EVP_PKEY* const pem_pkey = PemPrivateKeyRead(key_file);
  if (pem_pkey == NULL) {
    return kEebusErrorFileSystemNoFile;
  }

  uint8_t* pkey = NULL;

  self->pkey_size = i2d_PrivateKey(pem_pkey, &pkey);

  self->pkey = pkey;
  if ((self->pkey == NULL) || (self->pkey_size == 0)) {
    EVP_PKEY_free(pem_pkey);
    return kEebusErrorMemoryAllocate;
  }

  EVP_PKEY_free(pem_pkey);
  return kEebusErrorOk;
}

TlsCertificateObject* TlsCertificateLoadX509KeyPair(const char* cert_file, const char* key_file) {
  TlsCertificate* const tls_certificate = (TlsCertificate*)EEBUS_MALLOC(sizeof(TlsCertificate));
  if (tls_certificate == NULL) {
    return NULL;
  }

  TlsCertificateConstruct(tls_certificate);

  if ((LoadX509Certificate(tls_certificate, cert_file) != kEebusErrorOk)
      || (LoadX509PrivateKey(tls_certificate, key_file) != kEebusErrorOk)) {
    TlsCertificateDelete(TLS_CERTIFICATE_OBJECT(tls_certificate));
    return NULL;
  }

  return TLS_CERTIFICATE_OBJECT(tls_certificate);
}

TlsCertificateObject*
TlsCertificateParseX509KeyPair(const char* cert_buf, size_t cert_buf_size, const char* key_buf, size_t key_buf_size) {
  UNUSED(cert_buf);
  UNUSED(cert_buf_size);
  UNUSED(key_buf);
  UNUSED(key_buf_size);

  return NULL;  // Not implemented
}

void Destruct(TlsCertificateObject* self) {
  TlsCertificate* const tls_cert = TLS_CERTIFICATE(self);

  if (tls_cert->pkey != NULL) {
    if (tls_cert->pkey_size > 0) {
      OPENSSL_cleanse((uint8_t*)tls_cert->pkey, tls_cert->pkey_size);
    }

    OPENSSL_free((uint8_t*)tls_cert->pkey);
    tls_cert->pkey = NULL;
  }

  if (tls_cert->cert != NULL) {
    if (tls_cert->cert_size > 0) {
      OPENSSL_cleanse((uint8_t*)tls_cert->cert, tls_cert->cert_size);
    }

    OPENSSL_free((uint8_t*)tls_cert->cert);
    tls_cert->cert = NULL;
  }

  if (tls_cert->ski != NULL) {
    EEBUS_FREE((char*)tls_cert->ski);
    tls_cert->ski = NULL;
  }
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

  X509* x509_cert = d2i_X509(NULL, &p, cert_size);
  if (x509_cert == NULL) {
    return NULL;
  }

  const char* ski = CalcSubjectKeyIdString(x509_cert);
  X509_free(x509_cert);
  return ski;
}
