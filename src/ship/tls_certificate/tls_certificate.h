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

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SHIP_TLS_CERTIFICATE_TLS_CERTIFICATE_H_
