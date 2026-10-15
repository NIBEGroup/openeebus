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
 * @brief shippairing service instance implementation
 *
 * ShipPairingEntryParseTxtRecord() takes a TXT record in the DNS-SD wire form,
 * a sequence of length prefixed strings, as documented for
 * MdnsEntryParseTxtRecord(). E.g.:
 *
 * txt_record      = "\011txtvers=1\020parType=fpSha256"
 * txt_record_size = 27
 */

#include "src/ship/api/ship_pairing_entry.h"

#include <stdint.h>

#include "src/common/array_util.h"
#include "src/common/debug.h"
#include "src/common/struct_util.h"
#include "src/ship/tls_certificate/tls_certificate.h"

/** Set SHIP_PAIRING_ENTRY_DEBUG 1 to enable debug prints */
#ifndef SHIP_PAIRING_ENTRY_DEBUG
#define SHIP_PAIRING_ENTRY_DEBUG 0
#endif

/** shippairing entry debug printf(), enabled with SHIP_PAIRING_ENTRY_DEBUG = 1 */
#if SHIP_PAIRING_ENTRY_DEBUG
#define SHIP_PAIRING_ENTRY_DEBUG_PRINTF(fmt, ...) DebugPrintf(fmt, ##__VA_ARGS__)
#else
#define SHIP_PAIRING_ENTRY_DEBUG_PRINTF(fmt, ...)
#endif  // SHIP_PAIRING_ENTRY_DEBUG

typedef struct {
  const char* key;
  size_t offset;
} ShipPairingEntryMapping;

#define SHIP_PAIRING_ENTRY_FIELD_OFFSET(field) STRUCT_MEMBER_OFFSET(ShipPairingEntry, field)
#define SHIP_PAIRING_ENTRY_MAPPING(key, field) {key, SHIP_PAIRING_ENTRY_FIELD_OFFSET(field)}

/**
 * @brief The keys of table 1, in the order the digest message needs them
 *
 * Sized explicitly so that adding a key without also raising
 * SHIP_PAIRING_TXT_PAIR_COUNT, which backends use to size their own arrays,
 * fails to compile.
 */
static const ShipPairingEntryMapping ship_pairing_entry_lut[SHIP_PAIRING_TXT_PAIR_COUNT] = {
    SHIP_PAIRING_ENTRY_MAPPING("txtvers", txtvers),
    SHIP_PAIRING_ENTRY_MAPPING("parType", par_type),
    SHIP_PAIRING_ENTRY_MAPPING("forId", for_id),
    SHIP_PAIRING_ENTRY_MAPPING("forPar", for_par),
    SHIP_PAIRING_ENTRY_MAPPING("trustId", trust_id),
    SHIP_PAIRING_ENTRY_MAPPING("trustPar", trust_par),
    SHIP_PAIRING_ENTRY_MAPPING("trustCurve", trust_curve),
    SHIP_PAIRING_ENTRY_MAPPING("type", type),
    SHIP_PAIRING_ENTRY_MAPPING("trustNonce", trust_nonce),
    SHIP_PAIRING_ENTRY_MAPPING("alg", alg),
    SHIP_PAIRING_ENTRY_MAPPING("digest", digest),
};

static void ShipPairingEntryConstruct(ShipPairingEntry* entry, const char* name, const char* domain, uint32_t iface);
static bool ShipPairingEntryIsUppercaseHex(const char* s, size_t digits);

void ShipPairingEntryConstruct(ShipPairingEntry* entry, const char* name, const char* domain, uint32_t iface) {
  // Service instance fields
  entry->name   = StringCopy(name);
  entry->domain = StringCopy(domain);
  entry->iface  = iface;

  // shippairing TXT record fields
  entry->txtvers     = NULL;
  entry->par_type    = NULL;
  entry->for_id      = NULL;
  entry->for_par     = NULL;
  entry->trust_id    = NULL;
  entry->trust_par   = NULL;
  entry->trust_curve = NULL;
  entry->type        = NULL;
  entry->trust_nonce = NULL;
  entry->alg         = NULL;
  entry->digest      = NULL;

  entry->txtvers_is_first = false;
}

ShipPairingEntry* ShipPairingEntryCreate(const char* name, const char* domain, uint32_t iface) {
  ShipPairingEntry* new_entry = (ShipPairingEntry*)EEBUS_MALLOC(sizeof(ShipPairingEntry));
  if (new_entry == NULL) {
    return NULL;
  }

  ShipPairingEntryConstruct(new_entry, name, domain, iface);

  return new_entry;
}

void ShipPairingEntryDestruct(ShipPairingEntry* entry) {
  // Service instance fields
  StringDelete((char*)entry->name);
  entry->name = NULL;
  StringDelete((char*)entry->domain);
  entry->domain = NULL;

  // shippairing TXT record fields
  for (size_t i = 0; i < ARRAY_SIZE(ship_pairing_entry_lut); ++i) {
    const char** const value = (const char**)((uint8_t*)entry + ship_pairing_entry_lut[i].offset);
    StringDelete((char*)*value);
    *value = NULL;
  }
}

void ShipPairingEntryDeallocator(void* p) {
  ShipPairingEntryDelete((ShipPairingEntry*)p);
}

ShipPairingEntry* ShipPairingEntryCopy(const ShipPairingEntry* src) {
  if (src == NULL) {
    return NULL;
  }

  ShipPairingEntry* new_entry = (ShipPairingEntry*)EEBUS_MALLOC(sizeof(ShipPairingEntry));
  if (new_entry == NULL) {
    return NULL;
  }

  // Service instance fields
  new_entry->name   = StringCopy(src->name);
  new_entry->domain = StringCopy(src->domain);
  new_entry->iface  = src->iface;

  // shippairing TXT record fields
  for (size_t i = 0; i < ARRAY_SIZE(ship_pairing_entry_lut); ++i) {
    const size_t offset                           = ship_pairing_entry_lut[i].offset;
    const char* const* const value                = (const char* const*)((const uint8_t*)src + offset);
    *(const char**)((uint8_t*)new_entry + offset) = StringCopy(*value);
  }

  new_entry->txtvers_is_first = src->txtvers_is_first;

  return new_entry;
}

EebusError ShipPairingEntrySetValue(
    ShipPairingEntry* entry,
    const char* key_ptr,
    size_t key_size,
    const char* value_ptr,
    size_t value_size
) {
  if ((entry == NULL) || (key_ptr == NULL) || (key_size == 0)) {
    return kEebusErrorParse;
  }

  if ((value_ptr == NULL) || (value_size == 0)) {
    return kEebusErrorParse;
  }

  const char** value = NULL;
  for (size_t i = 0; i < ARRAY_SIZE(ship_pairing_entry_lut); ++i) {
    // The whole key has to match, not just its beginning, so that a truncated
    // key is treated as the unknown key it is rather than as the one it is a
    // prefix of.
    if ((strlen(ship_pairing_entry_lut[i].key) == key_size)
        && (strncmp(ship_pairing_entry_lut[i].key, key_ptr, key_size) == 0)) {
      value = (const char**)((uint8_t*)entry + ship_pairing_entry_lut[i].offset);
      break;
    }
  }

  if (value == NULL) {
    return kEebusErrorParse;
  }

  // A key repeated within one record keeps its first value, so that a second
  // occurrence cannot overwrite what the digest was calculated over.
  if (*value != NULL) {
    return kEebusErrorOk;
  }

  *value = StringNCopy(value_ptr, value_size);

  return kEebusErrorOk;
}

EebusError ShipPairingEntryParseTxtRecord(ShipPairingEntry* entry, const char* txt_record, uint16_t txt_record_size) {
  if ((entry == NULL) || (txt_record == NULL)) {
    return kEebusErrorParse;
  }

  const char* record_ptr = txt_record;
  size_t bytes_left      = txt_record_size;
  bool is_first_key      = true;

  while (bytes_left > 0) {
    const size_t record_size = *(const uint8_t*)record_ptr;
    if ((record_size >= bytes_left) || (record_size < 3)) {
      return kEebusErrorParse;
    }

    const char* const key_ptr    = record_ptr + 1;
    const char* const assign_ptr = memchr(key_ptr, '=', record_size);
    if (assign_ptr == NULL) {
      SHIP_PAIRING_ENTRY_DEBUG_PRINTF("%s, Warning! Skipping record: %.*s\n", __func__, (int)record_size, key_ptr);
      bytes_left -= record_size + 1;
      record_ptr += record_size + 1;
      is_first_key = false;
      continue;
    }

    const char* const value_ptr = assign_ptr + 1;

    const size_t key_size   = assign_ptr - key_ptr;
    const size_t value_size = record_size - key_size - 1;
    if ((key_size == 0) || (value_size == 0)) {
      return kEebusErrorParse;
    }

    if (is_first_key) {
      entry->txtvers_is_first = (key_size == strlen("txtvers")) && (strncmp("txtvers", key_ptr, key_size) == 0);
      is_first_key            = false;
    }

    // A key outside table 1 is ignored rather than rejected (section 5.4).
    if (ShipPairingEntrySetValue(entry, key_ptr, key_size, value_ptr, value_size) != kEebusErrorOk) {
      SHIP_PAIRING_ENTRY_DEBUG_PRINTF(
          "%s, Warning! Unsupported key: %.*s; value: %.*s\n",
          __func__,
          (int)key_size,
          key_ptr,
          (int)value_size,
          value_ptr
      );
    }

    bytes_left -= record_size + 1;
    record_ptr += record_size + 1;
  }

  return kEebusErrorOk;
}

size_t ShipPairingEntryGetTxtPairCount(void) {
  return ARRAY_SIZE(ship_pairing_entry_lut);
}

const char* ShipPairingEntryGetTxtKey(size_t index) {
  if (index >= ARRAY_SIZE(ship_pairing_entry_lut)) {
    return NULL;
  }

  return ship_pairing_entry_lut[index].key;
}

const char* ShipPairingEntryGetTxtValue(const ShipPairingEntry* entry, size_t index) {
  if ((entry == NULL) || (index >= ARRAY_SIZE(ship_pairing_entry_lut))) {
    return NULL;
  }

  return *(const char* const*)((const uint8_t*)entry + ship_pairing_entry_lut[index].offset);
}

bool ShipPairingEntryIsUppercaseHex(const char* s, size_t digits) {
  if (StringIsEmpty(s) || (strlen(s) != digits)) {
    return false;
  }

  for (size_t i = 0; i < digits; ++i) {
    const char c = s[i];
    if (((c < '0') || (c > '9')) && ((c < 'A') || (c > 'F'))) {
      return false;
    }
  }

  return true;
}

bool ShipPairingEntryIsValid(const ShipPairingEntry* entry) {
  if (entry == NULL) {
    return false;
  }

  // Section 5.4: "txtvers" has to be the first key of the record.
  bool is_valid = entry->txtvers_is_first;

  // The values this version of the specification defines are single valued, so
  // a mandatory key being present and being permitted is one check.
  is_valid = is_valid && !StringIsEmpty(entry->txtvers) && (strcmp(entry->txtvers, SHIP_PAIRING_TXTVERS) == 0);
  is_valid
      = is_valid && !StringIsEmpty(entry->par_type) && (strcmp(entry->par_type, SHIP_PAIRING_PAR_TYPE_FP_SHA256) == 0);
  is_valid = is_valid && !StringIsEmpty(entry->type) && (strcmp(entry->type, SHIP_PAIRING_TYPE_ADD_CU) == 0);
  is_valid = is_valid && !StringIsEmpty(entry->alg) && (strcmp(entry->alg, SHIP_PAIRING_ALG_HMAC_SHA256) == 0);

  // Section 5.4: the SHIP IDs are UTF-8 and must not be empty. Their internal
  // structure belongs to [SRIP] and is not checked here.
  is_valid = is_valid && !StringIsEmpty(entry->for_id);
  is_valid = is_valid && !StringIsEmpty(entry->trust_id);

  // Section 5.4: fingerprints and the digest are 64 uppercase hexadecimal
  // digits, the nonce is 32.
  is_valid = is_valid && ShipPairingEntryIsUppercaseHex(entry->for_par, SHIP_PAIRING_FINGERPRINT_SIZE * 2);
  is_valid = is_valid && ShipPairingEntryIsUppercaseHex(entry->trust_par, SHIP_PAIRING_FINGERPRINT_SIZE * 2);
  is_valid = is_valid && ShipPairingEntryIsUppercaseHex(entry->digest, SHIP_PAIRING_DIGEST_SIZE * 2);
  is_valid = is_valid && ShipPairingEntryIsUppercaseHex(entry->trust_nonce, SHIP_PAIRING_NONCE_SIZE * 2);

  // Section 5.4: one of the curves permitted for a SHIP certificate. Whether the
  // receiver supports it is a separate question (section 9, step 1).
  is_valid = is_valid && !StringIsEmpty(entry->trust_curve)
             && ((strcmp(entry->trust_curve, TLS_CERTIFICATE_CURVE_SECP256R1) == 0)
                 || (strcmp(entry->trust_curve, TLS_CERTIFICATE_CURVE_BRAINPOOLP256R1) == 0)
                 || (strcmp(entry->trust_curve, TLS_CERTIFICATE_CURVE_BRAINPOOLP384R1) == 0));

  return is_valid;
}
