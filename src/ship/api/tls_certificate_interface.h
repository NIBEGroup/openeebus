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
 * @brief Tls Certificate interface declarations
 */

#ifndef SRC_SHIP_API_TLS_CERTIFICATE_INTERFACE_H_
#define SRC_SHIP_API_TLS_CERTIFICATE_INTERFACE_H_

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @brief Tls Certificate Interface
 * (Tls Certificate "virtual functions table" declaration)
 */
typedef struct TlsCertificateInterface TlsCertificateInterface;

/**
 * @brief Tls Certificate Object type definition
 * ("abstract class", has no members but only pointer to
 * "virtual functions table")
 */
typedef struct TlsCertificateObject TlsCertificateObject;

/**
 * @brief TlsCertificate Interface Structure
 */
struct TlsCertificateInterface {
    void (*destruct)(TlsCertificateObject* self);
    const char* (*get_ski)(const TlsCertificateObject* self);
    const void* (*get_certificate)(const TlsCertificateObject* self);
    size_t (*get_certificate_size)(const TlsCertificateObject* self);
    const void* (*get_private_key)(const TlsCertificateObject* self);
    size_t (*get_private_key_size)(const TlsCertificateObject* self);
};

/**
 * @brief Tls Certificate Object Structure
 */
struct TlsCertificateObject {
    const TlsCertificateInterface* interface_;
};

/**
 * @brief Tls Certificate pointer typecast
 */
#define TLS_CERTIFICATE_OBJECT(obj) ((TlsCertificateObject*)(obj))

/**
 * @brief Tls Certificate Interface class pointer typecast
 */
#define TLS_CERTIFICATE_INTERFACE(obj) (TLS_CERTIFICATE_OBJECT(obj)->interface_)

/**
 * @brief Tls Certificate Destruct caller definition
 */
#define TLS_CERTIFICATE_DESTRUCT(obj) (TLS_CERTIFICATE_INTERFACE(obj)->destruct(obj))

/**
 * @brief Tls Certificate Get Ski caller definition
 */
#define TLS_CERTIFICATE_GET_SKI(obj) (TLS_CERTIFICATE_INTERFACE(obj)->get_ski(obj))

/**
 * @brief Tls Certificate Get Certificate caller definition
 */
#define TLS_CERTIFICATE_GET_CERTIFICATE(obj) (TLS_CERTIFICATE_INTERFACE(obj)->get_certificate(obj))

/**
 * @brief Tls Certificate Get Certificate Size caller definition
 */
#define TLS_CERTIFICATE_GET_CERTIFICATE_SIZE(obj) (TLS_CERTIFICATE_INTERFACE(obj)->get_certificate_size(obj))

/**
 * @brief Tls Certificate Get Private Key caller definition
 */
#define TLS_CERTIFICATE_GET_PRIVATE_KEY(obj) (TLS_CERTIFICATE_INTERFACE(obj)->get_private_key(obj))

/**
 * @brief Tls Certificate Get Private Key Size caller definition
 */
#define TLS_CERTIFICATE_GET_PRIVATE_KEY_SIZE(obj) (TLS_CERTIFICATE_INTERFACE(obj)->get_private_key_size(obj))

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SHIP_API_TLS_CERTIFICATE_INTERFACE_H_
