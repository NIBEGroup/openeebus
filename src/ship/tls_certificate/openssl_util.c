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
 * @brief OpenSSL utility functions for TLS certificates implementation
 */

#include <openssl/x509.h>
#include <stdint.h>

#include "src/common/string_util.h"

const char* OpenSslGetSubjectKeyIdString(const X509* cert) {
    const int32_t loc = X509_get_ext_by_NID(cert, NID_subject_key_identifier, -1);
    if (loc < 0) {
        return NULL;
    }

    X509_EXTENSION* const ext = X509_get_ext(cert, loc);
    if (ext == NULL) {
        return NULL;
    }

    ASN1_OCTET_STRING* const value = X509_EXTENSION_get_data(ext);
    if (value == NULL) {
        return NULL;
    }

    const uint8_t* const asn1_string = ASN1_STRING_get0_data(value);
    const int32_t asn1_string_len    = ASN1_STRING_length(value);

    if (asn1_string_len < 2) {
        return NULL;
    }

    // asn1_string[0] - type = 4 (Octet String)
    // asn1_string[1] - length = 20 (Actual number of bytes in value)
    // asn1_string[2] - Start value bytes
    return StringWithHex(&asn1_string[2], asn1_string_len - 2);
}
