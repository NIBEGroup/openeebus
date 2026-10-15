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
 * @brief shippairing service instance and its TXT record
 *
 * A shippairing service instance announces a request to establish SHIP trust,
 * authenticated with a secret of the receiving node rather than confirmed by a
 * user (SHIP Pairing Service TS 1.0.0). Its TXT record has a schema of its own,
 * unrelated to the SHIP TXT record of MdnsEntry, so it is a separate type.
 *
 * The host name and port of the service are deliberately absent: this version
 * of the specification defines no protocol over that port, and section 5.3
 * forbids the receiver from sending anything to it or relating the host name to
 * a SHIP service.
 */

#ifndef SRC_SHIP_API_SHIP_PAIRING_ENTRY_H_
#define SRC_SHIP_API_SHIP_PAIRING_ENTRY_H_

#include <stdbool.h>
#include <stdint.h>

#include "src/common/eebus_errors.h"
#include "src/common/eebus_malloc.h"
#include "src/common/string_util.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @defgroup ShipPairingServiceName Service name and domain
 *
 * The service name "shippairing" is registered with the IANA for TCP, and the
 * domain is the link local one (section 5.2).
 * @{
 */
#define SHIP_PAIRING_SERVICE_TYPE "_shippairing._tcp"
#define SHIP_PAIRING_DOMAIN "local."
/** @} */

/**
 * @defgroup ShipPairingTxtValues Permitted TXT record values
 *
 * The only values this version of the specification defines
 * (SHIP Pairing Service TS 1.0.0, table 1).
 * @{
 */
/** @brief Version of the TXT record format */
#define SHIP_PAIRING_TXTVERS "1"
/** @brief Authentication parameter is a SHA-256 certificate fingerprint */
#define SHIP_PAIRING_PAR_TYPE_FP_SHA256 "fpSha256"
/** @brief Digest algorithm is HMAC-SHA256 */
#define SHIP_PAIRING_ALG_HMAC_SHA256 "hmacSha256"
/** @brief Request to trust a control unit, an "addCu-request" */
#define SHIP_PAIRING_TYPE_ADD_CU "addCu"
/** @} */

/**
 * @defgroup ShipPairingSizes Sizes of the cryptographic values
 * @{
 */
/** @brief Octets of a devA-secret, an integer of the range [0, 2^128 - 1] */
#define SHIP_PAIRING_SECRET_SIZE 16
/** @brief Octets of a trustNonce, an integer of the range [0, 2^128 - 1] */
#define SHIP_PAIRING_NONCE_SIZE 16
/** @brief Octets of a SHA-256 certificate fingerprint */
#define SHIP_PAIRING_FINGERPRINT_SIZE 32
/** @brief Octets of a digest */
#define SHIP_PAIRING_DIGEST_SIZE 32
/** @} */

typedef struct ShipPairingEntry ShipPairingEntry;

struct ShipPairingEntry {
  /**
   * @defgroup ShipPairingServiceInstance Service instance fields
   * @{
   */

  /**
   * @brief Service instance name
   * Its structure is transparent to the receiver, which does not need to relate
   * it to a SHIP service instance name (section 5.2).
   * Example: Control Unit ExampleCompany C8277H008F#1
   */
  const char* name;
  /**
   * @brief The domain of the service instance
   * Example: domain=local.
   */
  const char* domain;
  /**
   * @brief The interface index the service was seen on
   */
  uint32_t iface;
  /** @} */

  /**
   * @defgroup ShipPairingTxtRecord shippairing TXT record
   *
   * Every key of table 1 is mandatory. A key not listed there is ignored on
   * reception, so unknown keys have no field here.
   * @{
   */

  /**
   * @brief Version of the TXT record format, always "1"
   * Example: txtvers=1
   */
  const char* txtvers;
  /**
   * @brief Kind of authentication parameter, always "fpSha256"
   * Example: parType=fpSha256
   */
  const char* par_type;
  /**
   * @brief SHIP ID of the node the request is addressed to (devA)
   * Example: forId=i:983327_u:C8277H008F-3
   */
  const char* for_id;
  /**
   * @brief SHA-256 fingerprint of the addressed node's certificate,
   * 64 uppercase hexadecimal digits
   * Example: forPar=C74B7855D3479415F62CC01E5F6D9A93EBC676057D85417ADA16FD1384338943
   */
  const char* for_par;
  /**
   * @brief SHIP ID of the node to be trusted (devZ)
   * Example: trustId=i:46925_u:43652bk-2-gt1
   */
  const char* trust_id;
  /**
   * @brief SHA-256 fingerprint of the certificate of the node to be trusted,
   * 64 uppercase hexadecimal digits
   * Example: trustPar=2CC72E781F7A7D2A08D50196C50FEDF0F7BA583F43F76C8C0DDEC9EEF0D005B4
   */
  const char* trust_par;
  /**
   * @brief Curve of the public key of the node to be trusted
   * One of the TLS_CERTIFICATE_CURVE_* names.
   * Example: trustCurve=secp256r1
   */
  const char* trust_curve;
  /**
   * @brief Kind of request, always "addCu" in this version
   * Example: type=addCu
   */
  const char* type;
  /**
   * @brief Random nonce of the requesting node, 32 uppercase hexadecimal digits
   * Example: trustNonce=BDCEE427FA7208DF3C1F2A749BA6F4D4
   */
  const char* trust_nonce;
  /**
   * @brief Algorithm of the digest, always "hmacSha256"
   * Example: alg=hmacSha256
   */
  const char* alg;
  /**
   * @brief Digest over the other keys, 64 uppercase hexadecimal digits
   * Example: digest=BCBB62B2176DA2CEE545784CEB1F2A55E049451B12A549C98E8CA213F001DA25
   */
  const char* digest;
  /** @} */

  /**
   * @brief Whether "txtvers" was the first key of the parsed TXT record
   *
   * Section 5.4 requires it to be first and section 9 makes a record that is not
   * ordered that way invalid, so the order has to be remembered while the keys
   * themselves are being parsed out of it.
   */
  bool txtvers_is_first;
};

/**
 * @brief Dynamically allocates and initialises a shippairing entry
 * @param name Service instance name
 * @param domain Domain of the service instance
 * @param iface Interface index the service was seen on
 * @return New entry on success, NULL on error
 */
ShipPairingEntry* ShipPairingEntryCreate(const char* name, const char* domain, uint32_t iface);

/**
 * @brief Releases everything the entry owns, without releasing the entry
 * @param entry Entry to destruct
 */
void ShipPairingEntryDestruct(ShipPairingEntry* entry);

/**
 * @brief Destructs the entry and deallocates it
 * @param entry Entry to delete, may be NULL
 */
static inline void ShipPairingEntryDelete(ShipPairingEntry* entry) {
  if (entry != NULL) {
    ShipPairingEntryDestruct(entry);
    EEBUS_FREE(entry);
  }
}

/**
 * @brief Creates a deep copy of an entry
 * @param src Entry to copy
 * @return New entry on success, NULL on error
 */
ShipPairingEntry* ShipPairingEntryCopy(const ShipPairingEntry* src);

/**
 * @brief Deletes an entry held in a Vector
 * @param p Entry to delete
 */
void ShipPairingEntryDeallocator(void* p);

/**
 * @brief Parses a DNS-SD TXT record into an entry
 *
 * Keys that table 1 does not list are ignored, as section 5.4 requires. A
 * malformed record, as opposed to an unexpected key, is reported as an error.
 *
 * @param entry Entry to fill
 * @param txt_record TXT record, a sequence of length prefixed key=value strings
 * @param txt_record_size Size of @p txt_record in octets
 * @return kEebusErrorOk on success, kEebusErrorParse on a malformed record
 */
EebusError ShipPairingEntryParseTxtRecord(ShipPairingEntry* entry, const char* txt_record, uint16_t txt_record_size);

/**
 * @brief Stores one key-value pair of a TXT record in an entry
 * @param entry Entry to fill
 * @param key_ptr Key, not NUL terminated
 * @param key_size Size of @p key_ptr
 * @param value_ptr Value, not NUL terminated
 * @param value_size Size of @p value_ptr
 * @return kEebusErrorOk if the key belongs to the schema, kEebusErrorParse otherwise
 */
EebusError ShipPairingEntrySetValue(
    ShipPairingEntry* entry,
    const char* key_ptr,
    size_t key_size,
    const char* value_ptr,
    size_t value_size
);

/**
 * @brief Number of key-value pairs a shippairing TXT record holds
 *
 * Available at compile time for the backends that need to size an array of
 * them. ShipPairingEntryGetTxtPairCount() returns the same number and is
 * checked against this.
 */
#define SHIP_PAIRING_TXT_PAIR_COUNT 11

/**
 * @brief Number of key-value pairs a shippairing TXT record holds
 *
 * Every key of table 1 is mandatory, so this is both how many an announcement
 * writes and how many a complete record has.
 */
size_t ShipPairingEntryGetTxtPairCount(void);

/**
 * @brief Gets the name of a TXT record key, in the order table 1 lists them
 * @param index Index below ShipPairingEntryGetTxtPairCount()
 * @return Key name, or NULL if @p index is out of range. The returned string is
 *         static and MUST NOT be deallocated.
 */
const char* ShipPairingEntryGetTxtKey(size_t index);

/**
 * @brief Gets the value an entry holds for a TXT record key
 * @param entry Entry to read
 * @param index Index below ShipPairingEntryGetTxtPairCount()
 * @return Value, or NULL if @p index is out of range or the key is unset. The
 *         returned string belongs to @p entry.
 */
const char* ShipPairingEntryGetTxtValue(const ShipPairingEntry* entry, size_t index);

/**
 * @brief Reports whether the TXT record of an entry conforms to the specification
 *
 * Covers the rules of section 5.4 that hold for any receiver: every mandatory
 * key is present, "txtvers" was the first of them, and every value matches its
 * permitted form. Whether the request is addressed to this node, and whether
 * this node supports the curve it names, are properties of the receiver rather
 * than of the record, and are evaluated separately (section 9, step 1).
 *
 * @param entry Entry to check
 * @return true if the record is valid
 */
bool ShipPairingEntryIsValid(const ShipPairingEntry* entry);

static inline const char* ShipPairingEntryGetName(const ShipPairingEntry* entry) {
  return entry->name;
}

static inline const char* ShipPairingEntryGetDomain(const ShipPairingEntry* entry) {
  return entry->domain;
}

static inline uint32_t ShipPairingEntryGetInterface(const ShipPairingEntry* entry) {
  return entry->iface;
}

static inline const char* ShipPairingEntryGetTxtVers(const ShipPairingEntry* entry) {
  return entry->txtvers;
}

static inline const char* ShipPairingEntryGetParType(const ShipPairingEntry* entry) {
  return entry->par_type;
}

static inline const char* ShipPairingEntryGetForId(const ShipPairingEntry* entry) {
  return entry->for_id;
}

static inline const char* ShipPairingEntryGetForPar(const ShipPairingEntry* entry) {
  return entry->for_par;
}

static inline const char* ShipPairingEntryGetTrustId(const ShipPairingEntry* entry) {
  return entry->trust_id;
}

static inline const char* ShipPairingEntryGetTrustPar(const ShipPairingEntry* entry) {
  return entry->trust_par;
}

static inline const char* ShipPairingEntryGetTrustCurve(const ShipPairingEntry* entry) {
  return entry->trust_curve;
}

static inline const char* ShipPairingEntryGetType(const ShipPairingEntry* entry) {
  return entry->type;
}

static inline const char* ShipPairingEntryGetTrustNonce(const ShipPairingEntry* entry) {
  return entry->trust_nonce;
}

static inline const char* ShipPairingEntryGetAlg(const ShipPairingEntry* entry) {
  return entry->alg;
}

static inline const char* ShipPairingEntryGetDigest(const ShipPairingEntry* entry) {
  return entry->digest;
}

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SHIP_API_SHIP_PAIRING_ENTRY_H_
