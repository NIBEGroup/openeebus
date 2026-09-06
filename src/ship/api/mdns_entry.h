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
 * @brief mDNSResponder browse entry buffer
 */

#ifndef SRC_SHIP_API_MDNS_ENTRY_H_
#define SRC_SHIP_API_MDNS_ENTRY_H_

#include <stdint.h>
#include "src/common/eebus_errors.h"
#include "src/common/string_util.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct MdnsEntry MdnsEntry;

struct MdnsEntry {
  /**
   * @defgroup ServiceNameLocation Service name and location fields
   * @{
   */

  /**
   * @brief Service name
   * Example: DemoBrand-EVSE-234567890
   */
  const char* name;
  /**
   * @brief The target hostname of the machine providing the service.
   * This name can be passed to functions like gethostbyname()
   * to identify the host's IP address.
   * Example: "host=DESKTOP-IAKQS71.local.".
   */
  const char* host;
  /**
   * @brief The domain of the service instance.
   * Example: "domain=local.".
   */
  const char* domain;
  /**
   * @brief The port, in network byte order, on which connections are accepted
   * for this service.
   * Example: port=4711
   */
  uint16_t port;
  /**
   * @brief The interface index used by the service
   * Example: iface=0.
   */
  uint32_t iface;
  /** @} */

  /**
   * @defgroup ShipTxtRecord SHIP TXT Record
   */

  /**
   * @brief Version number, e.g. txtvers=1
   * Mandatory
   */
  const char* txtvers;
  /**
   * @brief Identifier, contains a globally unique ID of the SHIP node
   * and has a maximum length of 63 bytes.
   * Example: id=DemoBrand-EVSE-234567890.
   * Mandatory
   */
  const char* id;
  /**
   * @brief String with wss path, e.g. path=/ship/,
   * Mandatory
   */
  const char* path;
  /**
   * @brief 40 byte hexadecimal digits representing the 160 bit SKI value.
   * Example: ski=41c98b1bbe5fc7657ce311981951f12d304ab419.
   * Mandatory
   */
  const char* ski;
  /**
   * @brief String value indicating whether auto accept is active in the SHIP node.
   * Example: register=false.
   * Mandatory
   */
  const char* reg;
  /**
   * @brief String with brand.
   * Example: brand=DemoBrand.
   * Optional
   */
  const char* brand;
  /**
   * @brief String with device type.
   * Example: type=ChargingStation.
   * Optional
   */
  const char* type;
  /**
   * @brief String with model.
   * Example: model=EVSE.
   * Optional
   */
  const char* model;
  /** @} */
};

MdnsEntry* MdnsEntryCreate(const char* name, const char* domain, uint32_t iface);

void MdnsEntryDestruct(MdnsEntry* entry);

static inline void MdnsEntryDelete(MdnsEntry* entry) {
  if (entry != NULL) {
    MdnsEntryDestruct(entry);
    EEBUS_FREE(entry);
  }
}

/**
 * @brief Creates a deep copy of the given MdnsEntry.
 * @param src The source MdnsEntry to copy.
 * @return A pointer to the newly created MdnsEntry copy, or NULL if memory allocation fails.
 */
MdnsEntry* MdnsEntryCopy(const MdnsEntry* src);

void MdnsEntryDeallocator(void* p);

EebusError MdnsEntrySetHost(MdnsEntry* entry, const char* host);

void MdnsEntrySetPort(MdnsEntry* entry, int port);

EebusError MdnsEntryParseTxtRecord(MdnsEntry* entry, const char* txt_record, uint16_t txt_record_size);

EebusError
MdnsEntrySetValue(MdnsEntry* entry, const char* key_ptr, size_t key_size, const char* value_ptr, size_t value_size);

bool MdnsEntryIsValid(const MdnsEntry* entry);

const char* MdnsEntryToUri(const MdnsEntry* entry);

static inline const char* MdnsEntryGetName(const MdnsEntry* entry) { return entry->name; }

static inline const char* MdnsEntryGetHost(const MdnsEntry* entry) { return entry->host; }

static inline const char* MdnsEntryGetDomain(const MdnsEntry* entry) { return entry->domain; }

static inline uint16_t MdnsEntryGetPort(const MdnsEntry* entry) { return entry->port; }

static inline uint32_t MdnsEntryGetInterface(const MdnsEntry* entry) { return entry->iface; }

static inline const char* MdnsEntryGetTxtVers(const MdnsEntry* entry) { return entry->txtvers; }

static inline const char* MdnsEntryGetId(const MdnsEntry* entry) { return entry->id; }

static inline const char* MdnsEntryGetPath(const MdnsEntry* entry) { return entry->path; }

static inline const char* MdnsEntryGetSki(const MdnsEntry* entry) { return entry->ski; }

static inline const char* MdnsEntryGetRegister(const MdnsEntry* entry) { return entry->reg; }

static inline const char* MdnsEntryGetBrand(const MdnsEntry* entry) { return entry->brand; }

static inline const char* MdnsEntryGetType(const MdnsEntry* entry) { return entry->type; }

static inline const char* MdnsEntryGetModel(const MdnsEntry* entry) { return entry->model; }

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SHIP_API_MDNS_ENTRY_H_
