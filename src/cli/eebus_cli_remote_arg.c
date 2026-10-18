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
#include "src/cli/eebus_cli_remote_arg.h"

#include <stdio.h>
#include <string.h>

void CliFormatEntityAddress(const EntityAddressType* addr, char* buf, size_t buf_size) {
  if (buf_size == 0) {
    return;
  }

  buf[0] = '\0';

  size_t pos = 0;

  int n = snprintf(buf, buf_size, "%s", addr->device != NULL ? addr->device : "");
  if (n > 0) {
    pos = (size_t)n < buf_size ? (size_t)n : buf_size - 1;
  }

  for (size_t i = 0; i < addr->entity_size && pos < buf_size - 1; ++i) {
    if (addr->entity[i] != NULL) {
      n = snprintf(buf + pos, buf_size - pos, "/%u", *addr->entity[i]);
      if (n > 0) {
        pos += (size_t)n < (buf_size - pos) ? (size_t)n : (buf_size - pos - 1);
      }
    }
  }
}

const EntityAddressType* CliExtractRemoteArg(
    const char* const* tokens,
    size_t num_tokens,
    const EntityAddressList* list,
    const char* cmd_name,
    const char* out_tokens[],
    size_t* out_num_tokens
) {
  const char* remote_str = NULL;

  *out_num_tokens = 0;

  for (size_t i = 0; i < num_tokens; ++i) {
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
    for (size_t i = 0; i < count; ++i) {
      char formatted[EEBUS_CLI_ENTITY_ADDR_STR_MAX];
      CliFormatEntityAddress(EntityAddressListGet(list, i), formatted, sizeof(formatted));
      printf("  %s\n", formatted);
    }

    return NULL;
  }

  for (size_t i = 0; i < count; ++i) {
    const EntityAddressType* addr = EntityAddressListGet(list, i);
    char formatted[EEBUS_CLI_ENTITY_ADDR_STR_MAX];
    CliFormatEntityAddress(addr, formatted, sizeof(formatted));
    if (strcmp(formatted, remote_str) == 0) {
      return addr;
    }
  }

  printf("Remote '%s' not connected for %s\n", remote_str, cmd_name);
  return NULL;
}
