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
 * @brief Helper to extract and resolve the --remote <entity_address> CLI argument
 */

#ifndef SRC_CLI_EEBUS_CLI_REMOTE_ARG_H_
#define SRC_CLI_EEBUS_CLI_REMOTE_ARG_H_

#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "src/common/entity_address_list.h"
#include "src/spine/model/entity_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Maximum length of a formatted entity address string (device + entity IDs). */
#define EEBUS_CLI_ENTITY_ADDR_STR_MAX 128

/**
 * @brief Format an EntityAddressType as a CLI string: "<device>/<id0>/<id1>/..."
 *
 * Example: device "d:_n:HeatPump_123" with entity [1] → "d:_n:HeatPump_123/1"
 *
 * @param addr      Address to format.
 * @param buf       Output buffer.
 * @param buf_size  Size of @p buf.
 */
static inline void CliFormatEntityAddress(const EntityAddressType* addr, char* buf, size_t buf_size) {
  if (buf_size == 0) {
    return;
  }
  buf[0] = '\0';

  size_t pos = 0;
  int    n   = snprintf(buf, buf_size, "%s", addr->device != NULL ? addr->device : "");
  if (n > 0) {
    pos = (size_t)n < buf_size ? (size_t)n : buf_size - 1;
  }

  for (size_t i = 0; i < addr->entity_size && pos < buf_size - 1; i++) {
    if (addr->entity[i] != NULL) {
      n = snprintf(buf + pos, buf_size - pos, "/%u", *addr->entity[i]);
      if (n > 0) {
        pos += (size_t)n < (buf_size - pos) ? (size_t)n : (buf_size - pos - 1);
      }
    }
  }
}

/**
 * @brief Extract --remote <entity_address> from a CLI token array, resolve it against a
 *        connected-remote list, and return the matching EntityAddressType*.
 *
 * The entity address format is "<device>/<entity_id0>/<entity_id1>/..." —
 * e.g. "d:_n:HeatPump_123456789/1". The token pair "--remote <addr>" is omitted
 * from @p out_tokens so the calling handler sees the same token count it would
 * without multi-remote support.
 *
 * Resolution rules:
 * - "--remote <addr>" present: look it up in @p list; return it or print an error.
 * - "--remote" absent, list size == 1: return the single entry implicitly.
 * - "--remote" absent, list size >  1: print a disambiguation message and return NULL.
 * - "--remote" absent, list size == 0: print "no remotes connected" and return NULL.
 *
 * @param tokens          Input token array.
 * @param num_tokens      Number of tokens.
 * @param list            Connected entity address list to resolve against.
 * @param cmd_name        Command name used in error messages.
 * @param out_tokens      Output buffer (must hold at least @p num_tokens entries).
 * @param out_num_tokens  Set to the number of tokens written to @p out_tokens.
 * @return Resolved address pointer, or NULL (error message already printed).
 */
static inline const EntityAddressType* CliExtractRemoteArg(
    const char* const*       tokens,
    size_t                   num_tokens,
    const EntityAddressList* list,
    const char*              cmd_name,
    const char*              out_tokens[],
    size_t*                  out_num_tokens
) {
  const char* remote_str = NULL;
  *out_num_tokens        = 0;

  for (size_t i = 0; i < num_tokens; i++) {
    if (strcmp(tokens[i], "--remote") == 0) {
      if (i + 1 >= num_tokens) {
        printf("Missing address after --remote in %s command\n", cmd_name);
        return NULL;
      }
      remote_str = tokens[++i];
    } else {
      out_tokens[(*out_num_tokens)++] = tokens[i];
    }
  }

  const size_t count = EntityAddressListGetSize(list);

  if (remote_str == NULL) {
    if (count == 0) {
      printf("No remotes connected for %s\n", cmd_name);
      return NULL;
    }
    if (count == 1) {
      return EntityAddressListGet(list, 0);
    }
    printf("Multiple remotes connected; specify one with --remote <entity_address>:\n");
    for (size_t i = 0; i < count; i++) {
      char formatted[EEBUS_CLI_ENTITY_ADDR_STR_MAX];
      CliFormatEntityAddress(EntityAddressListGet(list, i), formatted, sizeof(formatted));
      printf("  %s\n", formatted);
    }
    return NULL;
  }

  for (size_t i = 0; i < count; i++) {
    const EntityAddressType* addr = EntityAddressListGet(list, i);
    char                     formatted[EEBUS_CLI_ENTITY_ADDR_STR_MAX];
    CliFormatEntityAddress(addr, formatted, sizeof(formatted));
    if (strcmp(formatted, remote_str) == 0) {
      return addr;
    }
  }

  printf("Remote '%s' not connected for %s\n", remote_str, cmd_name);
  return NULL;
}

#ifdef __cplusplus
}
#endif

#endif  // SRC_CLI_EEBUS_CLI_REMOTE_ARG_H_
