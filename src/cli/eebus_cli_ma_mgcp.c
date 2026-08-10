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
 * @brief EEBUS CLI MA MGCP commands handling implementation
 */

#include <stdio.h>
#include <string.h>

#include "src/cli/eebus_cli_ma_mgcp.h"
#include "src/cli/eebus_cli_remote_arg.h"
#include "src/use_case/model/mgcp_types.h"
#include "src/use_case/model/scaled_value.h"

typedef struct MaMgcpCli MaMgcpCli;

struct MaMgcpCli {
  /** Implements the Eebus Cli Handler Interface */
  EebusCliHandlerObject obj;

  /** MA MGCP instance to deal with */
  MaMgcpUseCaseObject* ma_mgcp;
  /** Connected remote entity address list (not owned — pointer into caller's storage) */
  const EntityAddressList* addr_list;
};

#define MA_MGCP_CLI(obj) ((MaMgcpCli*)(obj))

static void Destruct(EebusCliHandlerObject* self);
static void HandleCmd(const EebusCliHandlerObject* self, const char* const* tokens, size_t num_tokens);

static const EebusCliHandlerInterface ma_mgcp_cli_methods = {
    .destruct   = Destruct,
    .handle_cmd = HandleCmd,
};

static EebusError
MaMgcpCliConstruct(MaMgcpCli* self, MaMgcpUseCaseObject* ma_mgcp, const EntityAddressList* addr_list);

static void HandleCmdList(const MaMgcpCli* self);
static void HandleCmdGet(
    const MaMgcpCli*         self,
    const EntityAddressType* entity_addr,
    const char* const*       tokens,
    size_t                   num_tokens
);

static EebusError
MaMgcpCliConstruct(MaMgcpCli* self, MaMgcpUseCaseObject* ma_mgcp, const EntityAddressList* addr_list) {
  EEBUS_CLI_HANDLER_INTERFACE(self) = &ma_mgcp_cli_methods;

  self->ma_mgcp   = NULL;
  self->addr_list = NULL;

  if ((ma_mgcp == NULL) || (addr_list == NULL)) {
    return kEebusErrorInputArgumentNull;
  }

  self->ma_mgcp   = ma_mgcp;
  self->addr_list = addr_list;

  return kEebusErrorOk;
}

EebusCliHandlerObject* MaMgcpCliCreate(MaMgcpUseCaseObject* ma_mgcp, const EntityAddressList* addr_list) {
  MaMgcpCli* const ma_mgcp_cli = (MaMgcpCli*)EEBUS_MALLOC(sizeof(MaMgcpCli));
  if (ma_mgcp_cli == NULL) {
    return NULL;
  }

  if (MaMgcpCliConstruct(ma_mgcp_cli, ma_mgcp, addr_list) != kEebusErrorOk) {
    MaMgcpCliDelete(EEBUS_CLI_HANDLER_OBJECT(ma_mgcp_cli));
    return NULL;
  }

  return EEBUS_CLI_HANDLER_OBJECT(ma_mgcp_cli);
}

static void Destruct(EebusCliHandlerObject* self) {
  MaMgcpCli* const ma_mgcp_cli = MA_MGCP_CLI(self);
  ma_mgcp_cli->addr_list       = NULL;
}

//-------------------------------------------------------------------------------------------//
//
// MA MGCP List Handling
//
//-------------------------------------------------------------------------------------------//
static void HandleCmdList(const MaMgcpCli* self) {
  const size_t count = EntityAddressListGetSize(self->addr_list);
  if (count == 0) {
    printf("ma_mgcp: no remotes connected\n");
    return;
  }
  printf("ma_mgcp connected remotes (%zu):\n", count);
  for (size_t i = 0; i < count; i++) {
    char formatted[EEBUS_CLI_ENTITY_ADDR_STR_MAX];
    CliFormatEntityAddress(EntityAddressListGet(self->addr_list, i), formatted, sizeof(formatted));
    printf("  %s\n", formatted);
  }
}

//-------------------------------------------------------------------------------------------//
//
// MA MGCP Getters Handling
//
//-------------------------------------------------------------------------------------------//
static void HandleCmdGet(
    const MaMgcpCli*         self,
    const EntityAddressType* entity_addr,
    const char* const*       tokens,
    size_t                   num_tokens
) {
  if (num_tokens != 3) {
    printf("Insufficient arguments for ma_mgcp get command\n");
    return;
  }

  const char* const name = tokens[2];

  if (strcmp(name, "pv_curtailment_limit_factor") == 0) {
    ScaledValue value = {0};
    if (MaMgcpGetPvCurtailmentLimitFactor(self->ma_mgcp, entity_addr, &value) != kEebusErrorOk) {
      printf("Getting MA MGCP pv_curtailment_limit_factor failed\n");
      return;
    }

    printf("MA MGCP pv_curtailment_limit_factor: ");
    ScaledValuePrint("value=%s\n", &value);
    return;
  }

  const GcpMeasurementNameId* const name_id = GcpMgcpMeasurementGetNameId(name);
  if (name_id == NULL) {
    printf("Unknown measurement name for ma_mgcp get: %s\n", name);
    return;
  }

  ScaledValue value = {0};
  if (MaMgcpGetMeasurementData(self->ma_mgcp, *name_id, entity_addr, &value) != kEebusErrorOk) {
    printf("Getting MA MGCP measurement value failed\n");
    return;
  }

  printf("MA MGCP measurement %s: ", name);
  ScaledValuePrint("value=%s\n", &value);
}

static void HandleCmd(const EebusCliHandlerObject* self, const char* const* tokens, size_t num_tokens) {
  const MaMgcpCli* const ma_mgcp_cli = MA_MGCP_CLI(self);

  if (num_tokens < 2) {
    printf("Insufficient arguments for ma_mgcp command\n");
    return;
  }

  if (strcmp(tokens[1], "list") == 0) {
    HandleCmdList(ma_mgcp_cli);
    return;
  }

  const char* adjusted[10];
  size_t      adjusted_count                 = 0;
  const EntityAddressType* const remote_addr = CliExtractRemoteArg(
      tokens, num_tokens, ma_mgcp_cli->addr_list, "ma_mgcp", adjusted, &adjusted_count
  );
  if (remote_addr == NULL) {
    return;
  }

  if (adjusted_count < 2) {
    printf("Insufficient arguments for ma_mgcp command\n");
    return;
  }

  if (strcmp(adjusted[1], "get") == 0) {
    HandleCmdGet(ma_mgcp_cli, remote_addr, adjusted, adjusted_count);
  } else {
    printf("Unknown subcommand for ma_mgcp: %s\n", adjusted[1]);
  }
}
