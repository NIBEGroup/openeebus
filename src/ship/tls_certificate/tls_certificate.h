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
 * @brief Tls Certificate implementation declarations
 */

#ifndef SRC_SHIP_TLS_CERTIFICATE_TLS_CERTIFICATE_H_
#define SRC_SHIP_TLS_CERTIFICATE_TLS_CERTIFICATE_H_

#include <stdint.h>

#include "src/common/eebus_errors.h"
#include "src/ship/api/tls_certificate_interface.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

TlsCertificateObject* TlsCertificateLoadX509KeyPair(const char* cert_file, const char* key_file);

TlsCertificateObject*
TlsCertificateParseX509KeyPair(const char* cert_buf, size_t cert_buf_size, const char* key_buf, size_t key_buf_size);

static inline void TlsCertificateDelete(TlsCertificateObject* tls_certificate) {
  if (tls_certificate != NULL) {
    TLS_CERTIFICATE_DESTRUCT(tls_certificate);
    EEBUS_FREE(tls_certificate);
  }
}

const char* TlsCertificateCalcPublicKeySki(const uint8_t* cert, size_t cert_size);

/** @brief Size in octets of a SHA-256 digest, and of an HMAC-SHA256 output */
#define TLS_CERTIFICATE_SHA256_SIZE 32

/**
 * @defgroup TlsCertificateCurveNames Elliptic curve names
 *
 * The public key curves permitted for SHIP certificates. secp256r1 is the only
 * curve of [SHIP] 1.0.x; the brainpool curves were added by [SHIP] 1.1.0. The
 * spellings are those required on the wire by the SHIP Pairing Service TXT
 * record key "trustCurve" (SHIP Pairing Service TS 1.0.0, section 5.4), and are
 * deliberately not taken from the underlying crypto library, whose own naming
 * differs between backends and versions.
 * @{
 */
#define TLS_CERTIFICATE_CURVE_SECP256R1 "secp256r1"
#define TLS_CERTIFICATE_CURVE_BRAINPOOLP256R1 "brainpoolP256r1"
#define TLS_CERTIFICATE_CURVE_BRAINPOOLP384R1 "brainpoolP384r1"
/** @} */

/**
 * @brief Calculates the SHA-256 fingerprint of a certificate
 *
 * The fingerprint is the SHA-256 value of the certificate in DER format
 * (SHIP Pairing Service TS 1.0.0, section 6.2). It identifies a certificate the
 * way an SKI identifies a public key, and is the authentication parameter used
 * by the SHIP Pairing Service TXT record keys "forPar" and "trustPar".
 *
 * The certificate is parsed before it is hashed, so a buffer holding more than
 * the certificate itself still yields the fingerprint of that certificate.
 *
 * @param cert DER encoded certificate
 * @param cert_size Size of @p cert in octets
 * @return Dynamically allocated uppercase hexadecimal string of 64 digits, or
 *         NULL on error. The caller is responsible for deallocating it with
 *         StringDelete().
 */
const char* TlsCertificateCalcFingerprintSha256(const uint8_t* cert, size_t cert_size);

/**
 * @brief Gets the elliptic curve of a certificate's public key
 *
 * @param cert DER encoded certificate
 * @param cert_size Size of @p cert in octets
 * @return One of the @ref TlsCertificateCurveNames string constants, or NULL if
 *         the certificate has no elliptic curve public key or uses a curve that
 *         is not permitted for SHIP. The returned string is static and MUST NOT
 *         be deallocated.
 */
const char* TlsCertificateGetCurveName(const uint8_t* cert, size_t cert_size);

/**
 * @brief Fills a buffer with cryptographically secure random octets
 *
 * Used for values that an adversary must not be able to predict or derive, such
 * as a devA-secret or a "trustNonce" (SHIP Pairing Service TS 1.0.0, section 6.3).
 *
 * @note Backends differ in whether they can satisfy this on their own. The
 *       OpenSSL backend uses the library's own seeded generator. The mbedTLS
 *       backend seeds a CTR_DRBG from the mbedTLS entropy pool, which on a bare
 *       metal target has no source unless the integrator has registered one with
 *       mbedtls_entropy_add_source(). When no entropy is available this fails
 *       rather than returning predictable octets, so a caller MUST check the
 *       return value and MUST NOT use @p buf if it is not kEebusErrorOk.
 *
 * @param buf Buffer to fill
 * @param buf_size Number of octets to write to @p buf
 * @return kEebusErrorOk on success, an error otherwise
 */
EebusError TlsCertificateRandomBytes(uint8_t* buf, size_t buf_size);

/**
 * @brief Calculates an HMAC-SHA256 over a message
 *
 * HMAC according to [RFC 2104] with the hash function SHA-256, the algorithm
 * denoted "hmacSha256" by the SHIP Pairing Service TXT record key "alg"
 * (SHIP Pairing Service TS 1.0.0, section 7.2).
 *
 * @param key HMAC key K
 * @param key_size Size of @p key in octets
 * @param msg Message M to authenticate
 * @param msg_size Size of @p msg in octets
 * @param digest Buffer of TLS_CERTIFICATE_SHA256_SIZE octets receiving the result
 * @return kEebusErrorOk on success, an error otherwise
 */
EebusError
TlsCertificateHmacSha256(const uint8_t* key, size_t key_size, const uint8_t* msg, size_t msg_size, uint8_t* digest);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SHIP_TLS_CERTIFICATE_TLS_CERTIFICATE_H_
