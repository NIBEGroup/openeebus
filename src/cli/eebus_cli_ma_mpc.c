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
 * @brief EEBUS CLI MA MPC commands handling implementation
 */

#include <stdio.h>
#include <string.h>

#include "src/cli/eebus_cli_ma_mpc.h"
#include "src/cli/eebus_cli_remote_arg.h"
#include "src/use_case/model/scaled_value.h"

typedef struct MaMpcCli MaMpcCli;

struct MaMpcCli {
  /** Implements the Eebus Cli Handler Interface */
  EebusCliHandlerObject obj;

  /** MA MPC instance to deal with */
  MaMpcUseCaseObject* ma_mpc;
  /** Connected remote entity address list (not owned — pointer into caller's storage) */
  const EntityAddressList* addr_list;
};

#define MA_MPC_CLI(obj) ((MaMpcCli*)(obj))

static void Destruct(EebusCliHandlerObject* self);
static void HandleCmd(const EebusCliHandlerObject* self, const char* const* tokens, size_t num_tokens);

static const EebusCliHandlerInterface ma_mpc_cli_methods = {
    .destruct   = Destruct,
    .handle_cmd = HandleCmd,
};

static EebusError
MaMpcCliConstruct(MaMpcCli* self, MaMpcUseCaseObject* ma_mpc, const EntityAddressList* addr_list);

static void HandleCmdList(const MaMpcCli* self);
static void HandleCmdMaMpcGet(
    const MaMpcCli*          self,
    const EntityAddressType* entity_addr,
    const char* const*       tokens,
    size_t                   num_tokens
);

static EebusError
MaMpcCliConstruct(MaMpcCli* self, MaMpcUseCaseObject* ma_mpc, const EntityAddressList* addr_list) {
  EEBUS_CLI_HANDLER_INTERFACE(self) = &ma_mpc_cli_methods;

  self->ma_mpc    = NULL;
  self->addr_list = NULL;

  if ((ma_mpc == NULL) || (addr_list == NULL)) {
    return kEebusErrorInputArgumentNull;
  }

  self->ma_mpc    = ma_mpc;
  self->addr_list = addr_list;

  return kEebusErrorOk;
}

EebusCliHandlerObject* MaMpcCliCreate(MaMpcUseCaseObject* ma_mpc, const EntityAddressList* addr_list) {
  MaMpcCli* ma_mpc_cli = (MaMpcCli*)EEBUS_MALLOC(sizeof(MaMpcCli));
  if (ma_mpc_cli == NULL) {
    return NULL;
  }

  if (MaMpcCliConstruct(ma_mpc_cli, ma_mpc, addr_list) != kEebusErrorOk) {
    MaMpcCliDelete(EEBUS_CLI_HANDLER_OBJECT(ma_mpc_cli));
    return NULL;
  }

  return EEBUS_CLI_HANDLER_OBJECT(ma_mpc_cli);
}

static void Destruct(EebusCliHandlerObject* self) {
  MaMpcCli* ma_mpc_cli = MA_MPC_CLI(self);
  ma_mpc_cli->addr_list = NULL;
}

//-------------------------------------------------------------------------------------------//
//
// MA MPC List Handling
//
//-------------------------------------------------------------------------------------------//
static void HandleCmdList(const MaMpcCli* self) {
  const size_t count = EntityAddressListGetSize(self->addr_list);
  if (count == 0) {
    printf("ma_mpc: no remotes connected\n");
    return;
  }
  printf("ma_mpc connected remotes (%zu):\n", count);
  for (size_t i = 0; i < count; i++) {
    char formatted[EEBUS_CLI_ENTITY_ADDR_STR_MAX];
    CliFormatEntityAddress(EntityAddressListGet(self->addr_list, i), formatted, sizeof(formatted));
    printf("  %s\n", formatted);
  }
}

//-------------------------------------------------------------------------------------------//
//
// MA MPC Getters Handling
//
//-------------------------------------------------------------------------------------------//
static void HandleCmdMaMpcGet(
    const MaMpcCli*          self,
    const EntityAddressType* entity_addr,
    const char* const*       tokens,
    size_t                   num_tokens
) {
  if (num_tokens != 3) {
    printf("Insufficient arguments for ma_mpc get command\n");
    return;
  }

  const char* const name = tokens[2];

  const MuMpcMeasurementNameId* const name_id = MuMpcMeasurementGetNameId(name);
  if (name_id == NULL) {
    printf("Unknown measurement name for ma_mpc get: %s\n", name);
    return;
  }

  ScaledValue value = {0};
  if (MaMpcGetMeasurementData(self->ma_mpc, *name_id, entity_addr, &value) != kEebusErrorOk) {
    printf("Getting MA MPC measurement value failed\n");
    return;
  }

  printf("MA MPC measurement %s: ", name);
  ScaledValuePrint("value=%s\n", &value);
}

static void HandleCmd(const EebusCliHandlerObject* self, const char* const* tokens, size_t num_tokens) {
  const MaMpcCli* const ma_mpc_cli = MA_MPC_CLI(self);

  if (num_tokens < 2) {
    printf("Insufficient arguments for ma_mpc command\n");
    return;
  }

  if (strcmp(tokens[1], "list") == 0) {
    HandleCmdList(ma_mpc_cli);
    return;
  }

  const char* adjusted[10];
  size_t      adjusted_count                  = 0;
  const EntityAddressType* const remote_addr  = CliExtractRemoteArg(
      tokens, num_tokens, ma_mpc_cli->addr_list, "ma_mpc", adjusted, &adjusted_count
  );
  if (remote_addr == NULL) {
    return;
  }

  if (adjusted_count < 2) {
    printf("Insufficient arguments for ma_mpc command\n");
    return;
  }

  if (strcmp(adjusted[1], "get") == 0) {
    HandleCmdMaMpcGet(ma_mpc_cli, remote_addr, adjusted, adjusted_count);
  } else {
    printf("Unknown subcommand for ma_mpc: %s\n", adjusted[1]);
  }
}
