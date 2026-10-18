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
 * @brief mDNS entry implementation
 * MdnsEntryParseTxtRecord() receives txt record that has format as following:
 *
 *   0                                 ....                   txt_record_size - 1
 * ______________________________________________________________________________
 * | l0 | key0=val0 | l1 | key1=val1 | ... | li | keyi=vali | ...               |
 * ------------------------------------------------------------------------------
 *
 * Where:
 *   l0, l1, li - length of record
 *   key0, key1, keyi - keys
 *   val0, val1, vali - values
 * E.g., txt record having a txvers and path specified:
 *
 * txt_record      = "\011txtvers=1\013path=/ship/"
 * txt_record_size = strlen(txt_record) = 20
 *
 * l0   = strlen("txtvers=1") = 011 octal = 9 decimal
 * key0 = "txtvers"
 * val0 = "1"
 *
 * l1   = strlen("path=/ship/") = 013 octal = 11 decimal
 * key1 = "path"
 * val1 = "/ship/"
 *
 * Note: in practice txt record like this is incomplete as doesn't contain all
 * of the mandatory fields. Applicable for format example only
 */

#include "src/ship/api/mdns_entry.h"

#include <stdint.h>

#include "src/common/array_util.h"
#include "src/common/debug.h"
#include "src/common/eebus_errors.h"
#include "src/common/string_util.h"
#include "src/common/struct_util.h"

/** Set MDNS_ENTRY_DEBUG 1 to enable debug prints */
#ifndef MDNS_ENTRY_DEBUG
#define MDNS_ENTRY_DEBUG 0
#endif

/** mDNS Entry debug printf(), enabled whith MDNS_DEBUG = 1 */
#if MDNS_ENTRY_DEBUG
#define MDNS_ENTRY_DEBUG_PRINTF(fmt, ...) DebugPrintf(fmt, ##__VA_ARGS__)
#else
#define MDNS_ENTRY_DEBUG_PRINTF(fmt, ...)
#endif  // MDNS_ENTRY_DEBUG

typedef struct {
  const char* key;
  size_t offset;
} MdnsEntryMapping;

#define MDNS_ENTRY_FIELD_OFFSET(field) STRUCT_MEMBER_OFFSET(MdnsEntry, field)
#define MDNS_ENTRY_MAPPING(key, field) {key, MDNS_ENTRY_FIELD_OFFSET(field)}

static void MdnsEntryConstruct(MdnsEntry* entry, const char* name, const char* domain, uint32_t iface);

void MdnsEntryConstruct(MdnsEntry* entry, const char* name, const char* domain, uint32_t iface) {
  // Service location fields
  entry->name   = StringCopy(name);
  entry->host   = NULL;
  entry->domain = StringCopy(domain);
  entry->port   = 0;
  entry->iface  = iface;

  // SHIP TXT Record fields
  entry->txtvers = NULL;
  entry->id      = NULL;
  entry->path    = NULL;
  entry->ski     = NULL;
  entry->reg     = NULL;
  entry->brand   = NULL;
  entry->type    = NULL;
  entry->model   = NULL;
}

MdnsEntry* MdnsEntryCreate(const char* name, const char* domain, uint32_t iface) {
  MdnsEntry* new_entry = (MdnsEntry*)EEBUS_MALLOC(sizeof(MdnsEntry));
  if (new_entry == NULL) {
    return NULL;
  }

  MdnsEntryConstruct(new_entry, name, domain, iface);

  return new_entry;
}

void MdnsEntryDestruct(MdnsEntry* entry) {
  // Service location fields
  StringDelete((char*)entry->name);
  entry->name = NULL;
  StringDelete((char*)entry->host);
  entry->host = NULL;
  StringDelete((char*)entry->domain);
  entry->domain = NULL;

  // SHIP TXT Record fields
  StringDelete((char*)entry->txtvers);
  entry->txtvers = NULL;
  StringDelete((char*)entry->id);
  entry->id = NULL;
  StringDelete((char*)entry->path);
  entry->path = NULL;
  StringDelete((char*)entry->ski);
  entry->ski = NULL;
  StringDelete((char*)entry->reg);
  entry->reg = NULL;
  StringDelete((char*)entry->brand);
  entry->brand = NULL;
  StringDelete((char*)entry->type);
  entry->type = NULL;
  StringDelete((char*)entry->model);
  entry->model = NULL;
}

void MdnsEntryDeallocator(void* p) {
  MdnsEntryDelete((MdnsEntry*)p);
}

MdnsEntry* MdnsEntryCopy(const MdnsEntry* src) {
  if (src == NULL) {
    return NULL;
  }

  MdnsEntry* new_entry = (MdnsEntry*)EEBUS_MALLOC(sizeof(MdnsEntry));
  if (new_entry == NULL) {
    return NULL;
  }

  // Service location fields
  new_entry->name   = StringCopy(src->name);
  new_entry->host   = StringCopy(src->host);
  new_entry->domain = StringCopy(src->domain);
  new_entry->port   = src->port;
  new_entry->iface  = src->iface;

  // SHIP TXT Record fields
  new_entry->txtvers = StringCopy(src->txtvers);
  new_entry->id      = StringCopy(src->id);
  new_entry->path    = StringCopy(src->path);
  new_entry->ski     = StringCopy(src->ski);
  new_entry->reg     = StringCopy(src->reg);
  new_entry->brand   = StringCopy(src->brand);
  new_entry->type    = StringCopy(src->type);
  new_entry->model   = StringCopy(src->model);

  return new_entry;
}

EebusError
MdnsEntrySetValue(MdnsEntry* entry, const char* key_ptr, size_t key_size, const char* value_ptr, size_t value_size) {
  if ((key_ptr == NULL) || (key_size == 0)) {
    return kEebusErrorParse;
  }

  if ((value_ptr == NULL) || (value_size == 0)) {
    return kEebusErrorParse;
  }

  static const MdnsEntryMapping mdns_entry_lut[] = {
      MDNS_ENTRY_MAPPING("txtvers", txtvers),
      MDNS_ENTRY_MAPPING("id", id),
      MDNS_ENTRY_MAPPING("path", path),
      MDNS_ENTRY_MAPPING("ski", ski),
      MDNS_ENTRY_MAPPING("register", reg),
      MDNS_ENTRY_MAPPING("brand", brand),
      MDNS_ENTRY_MAPPING("type", type),
      MDNS_ENTRY_MAPPING("model", model),
  };

  const char** value = NULL;
  for (size_t i = 0; i < ARRAY_SIZE(mdns_entry_lut); ++i) {
    if (!strncmp(mdns_entry_lut[i].key, key_ptr, key_size)) {
      value = (const char**)((const uint8_t*)entry + mdns_entry_lut[i].offset);
      break;
    }
  }

  if (value == NULL) {
    return kEebusErrorParse;
  };

  *value = StringNCopy(value_ptr, value_size);

  return kEebusErrorOk;
}

EebusError MdnsEntryParseTxtRecord(MdnsEntry* entry, const char* txt_record, uint16_t txt_record_size) {
  const char* record_ptr = txt_record;
  size_t bytes_left      = txt_record_size;

  while (bytes_left > 0) {
    const size_t record_size = *(const uint8_t*)record_ptr;
    if ((record_size >= bytes_left) || (record_size < 3)) {
      return kEebusErrorParse;
    }

    const char* const key_ptr    = record_ptr + 1;
    const char* const assign_ptr = memchr(key_ptr, '=', record_size);
    if (assign_ptr == NULL) {
      MDNS_ENTRY_DEBUG_PRINTF("%s, Warning! Skipping record: %.*s\n", __func__, (int)record_size, key_ptr);
      bytes_left -= record_size + 1;
      record_ptr += record_size + 1;
      continue;
    }

    const char* const value_ptr = assign_ptr + 1;

    const size_t key_size   = assign_ptr - key_ptr;
    const size_t value_size = record_size - key_size - 1;
    if ((key_size == 0) || (value_size == 0)) {
      return kEebusErrorParse;
    }

    const EebusError ret = MdnsEntrySetValue(entry, key_ptr, key_size, value_ptr, value_size);
    if (ret != kEebusErrorOk) {
      MDNS_ENTRY_DEBUG_PRINTF(
          "%s, Warning! Unsupported key: %.*s; value: %.*s\n",
          __func__,
          key_size,
          key_ptr,
          value_size,
          value_ptr
      );
    }

    bytes_left -= record_size + 1;
    record_ptr += record_size + 1;
  }

  return (bytes_left == 0) ? kEebusErrorOk : kEebusErrorParse;
}

EebusError MdnsEntrySetHost(MdnsEntry* entry, const char* host) {
  entry->host = StringCopy(host);
  return (entry->host == NULL) ? kEebusErrorMemoryAllocate : kEebusErrorOk;
}

const char* MdnsEntryToUri(const MdnsEntry* entry) {
  const char* const host = entry->host;

  size_t len = host ? strlen(host) : 0;
  if (len == 0) {
    return NULL;
  }

  if (host[len - 1] == '.') {
    --len;
  }

  return StringFmtSprintf("wss://%.*s:%d%s", (int)len, host, entry->port, entry->path);
}

void MdnsEntrySetPort(MdnsEntry* entry, int port) {
  entry->port = port;
}

bool MdnsEntryIsValid(const MdnsEntry* entry) {
  bool is_valid = true;

  // Check service location
  is_valid = is_valid && !StringIsEmpty(entry->name);
  is_valid = is_valid && !StringIsEmpty(entry->host);
  is_valid = is_valid && (entry->port != 0);

  // Check mandatory SHIP Txt Record related fields
  is_valid = is_valid && !StringIsEmpty(entry->txtvers);
  is_valid = is_valid && !StringIsEmpty(entry->id);
  is_valid = is_valid && !StringIsEmpty(entry->path);
  is_valid = is_valid && !StringIsEmpty(entry->ski);
  is_valid = is_valid && !StringIsEmpty(entry->reg) && (!strcmp(entry->reg, "true") || !strcmp(entry->reg, "false"));

  return is_valid;
}
