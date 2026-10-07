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
 * @brief EEBUS CLI CEM OHPCF commands handling implementation
 */

#include <stdio.h>
#include <string.h>

#include "src/cli/eebus_cli_cem_ohpcf.h"
#include "src/cli/eebus_cli_handler_interface.h"
#include "src/cli/eebus_cli_remote_arg.h"
#include "src/common/eebus_arguments.h"
#include "src/common/eebus_bool/eebus_bool.h"
#include "src/common/eebus_date_time/eebus_date_time.h"
#include "src/spine/api/message.h"
#include "src/spine/model/result_types.h"
#include "src/use_case/model/scaled_value.h"

typedef struct CemOhpcfCli CemOhpcfCli;
struct CemOhpcfCli {
  /** Implements the Eebus Cli Handler Interface */
  EebusCliHandlerObject obj;

  /** CEM OHPCF instance to deal with */
  CemOhpcfUseCaseObject* cem_ohpcf;
  /** List of remote entity addresses to communicate with */
  const EntityAddressList* addr_list;
};

#define CEM_OHPCF_CLI(obj) ((CemOhpcfCli*)(obj))

static void Destruct(EebusCliHandlerObject* self);
static void HandleCmd(const EebusCliHandlerObject* self, const char* const* tokens, size_t num_tokens);

static const EebusCliHandlerInterface cem_ohpcf_cli_methods = {
    .destruct   = Destruct,
    .handle_cmd = HandleCmd,
};

static EebusError
CemOhpcfCliConstruct(CemOhpcfCli* self, CemOhpcfUseCaseObject* cem_ohpcf, const EntityAddressList* addr_list);

static void HandleCmdGetAnnounced(const CemOhpcfCli* self, const EntityAddressType* entity_addr);
static void HandleCmdGetState(const CemOhpcfCli* self, const EntityAddressType* entity_addr);
static void HandleCmdGet(
    const CemOhpcfCli* self,
    const EntityAddressType* entity_addr,
    const char* const* tokens,
    size_t num_tokens
);
static void OnScheduleResult(
    const ResultMessage* result_msg,
    const FeatureAddressType* remote_feature_addr,
    EebusError err,
    void* ctx
);
static void HandleCmdSchedule(
    const CemOhpcfCli* self,
    const EntityAddressType* entity_addr,
    const char* const* tokens,
    size_t num_tokens
);
static void OnWriteCommandResult(
    const ResultMessage* result_msg,
    const FeatureAddressType* remote_feature_addr,
    EebusError err,
    void* ctx
);
static void HandleCmdWriteCommand(
    const CemOhpcfCli* self,
    const EntityAddressType* entity_addr,
    const char* const* tokens,
    size_t num_tokens
);

EebusError
CemOhpcfCliConstruct(CemOhpcfCli* self, CemOhpcfUseCaseObject* cem_ohpcf, const EntityAddressList* addr_list) {
  // Override "virtual functions table"
  EEBUS_CLI_HANDLER_INTERFACE(self) = &cem_ohpcf_cli_methods;

  self->cem_ohpcf = cem_ohpcf;
  self->addr_list = NULL;

  if ((cem_ohpcf == NULL) || (addr_list == NULL)) {
    return kEebusErrorInputArgumentNull;
  }

  self->addr_list = addr_list;

  return kEebusErrorOk;
}

EebusCliHandlerObject* CemOhpcfCliCreate(CemOhpcfUseCaseObject* cem_ohpcf, const EntityAddressList* addr_list) {
  CemOhpcfCli* const cem_ohpcf_cli = (CemOhpcfCli*)EEBUS_MALLOC(sizeof(CemOhpcfCli));
  if (cem_ohpcf_cli == NULL) {
    return NULL;
  }

  if (CemOhpcfCliConstruct(cem_ohpcf_cli, cem_ohpcf, addr_list) != kEebusErrorOk) {
    CemOhpcfCliDelete(EEBUS_CLI_HANDLER_OBJECT(cem_ohpcf_cli));
    return NULL;
  }

  return EEBUS_CLI_HANDLER_OBJECT(cem_ohpcf_cli);
}

void Destruct(EebusCliHandlerObject* self) {
  CemOhpcfCli* const cem_ohpcf_cli = CEM_OHPCF_CLI(self);

  cem_ohpcf_cli->addr_list = NULL;
}

//-------------------------------------------------------------------------------------------//
//
// CEM OHPCF Getters Handling
//
//-------------------------------------------------------------------------------------------//
void HandleCmdGetAnnounced(const CemOhpcfCli* self, const EntityAddressType* entity_addr) {
  OptionalPowerConsumption optional_power_consumption;
  const EebusError err
      = CemOhpcfGetAnnouncedOptionalPowerConsumption(self->cem_ohpcf, entity_addr, &optional_power_consumption);

  if (err != kEebusErrorOk) {
    printf("CEM OHPCF failed to get announced optional power consumption, error code: %d\n", err);
    return;
  }

  printf("CEM OHPCF Announced Optional Power Consumption:\n");
  OptionalPowerConsumptionPrint(&optional_power_consumption);
  printf("\n");
}

void HandleCmdGetState(const CemOhpcfCli* self, const EntityAddressType* entity_addr) {
  const CompressorOhpcfState state = CemOhpcfGetCompressorState(self->cem_ohpcf, entity_addr);

  const char* const state_name = CompressorOhpcfStateGetName(state);
  if (state_name != NULL) {
    printf("CEM OHPCF Compressor State: %s\n", state_name);
    return;
  }

  printf("CEM OHPCF Compressor State: %d\n", state);
}

void HandleCmdGet(
    const CemOhpcfCli* self,
    const EntityAddressType* entity_addr,
    const char* const* tokens,
    size_t num_tokens
) {
  if (num_tokens < 3) {
    printf("Insufficient arguments for cem_ohpcf get command\n");
    return;
  }

  if (strcmp(tokens[2], "announced") == 0) {
    HandleCmdGetAnnounced(self, entity_addr);
  } else if (strcmp(tokens[2], "state") == 0) {
    HandleCmdGetState(self, entity_addr);
  } else {
    printf("Unknown get subcommand for cem_ohpcf: %s\n", tokens[2]);
  }
}

//-------------------------------------------------------------------------------------------//
//
// CEM OHPCF Schedule Handling
//
//-------------------------------------------------------------------------------------------//
void OnScheduleResult(
    const ResultMessage* result_msg,
    const FeatureAddressType* remote_feature_addr,
    EebusError err,
    void* ctx
) {
  UNUSED(err);
  (void)ctx;
  const ResultDataType* const result_data = result_msg->result_data;

  if ((result_data == NULL) || (result_data->error_number == NULL)) {
    FeatureAddressPrint("CEM OHPCF schedule optional power consumption result missing from %s\n", remote_feature_addr);
    return;
  }

  if (*result_data->error_number != kErrorNumberTypeNoError) {
    printf("CEM OHPCF schedule optional power consumption result error: %u from ", *result_data->error_number);
    FeatureAddressPrint("%s\n", remote_feature_addr);
  }
}

void HandleCmdSchedule(
    const CemOhpcfCli* self,
    const EntityAddressType* entity_addr,
    const char* const* tokens,
    size_t num_tokens
) {
  if (num_tokens < 3) {
    printf("Insufficient arguments for cem_ohpcf schedule command\n");
    return;
  }

  const char* const duration_str = tokens[2];
  EebusDuration start_time;
  if (EebusDurationParse(duration_str, &start_time) != kEebusErrorOk) {
    printf("CEM OHPCF invalid duration format: %s\n", duration_str);
    return;
  }

  const EebusError err
      = CemOhpcfScheduleOptionalPowerConsumption(self->cem_ohpcf, entity_addr, &start_time, OnScheduleResult, NULL);
  if (err != kEebusErrorOk) {
    printf("CEM OHPCF failed to schedule optional power consumption, error code: %d\n", err);
    return;
  }

  printf("CEM OHPCF successfully scheduled optional power consumption for duration: %s\n", duration_str);
}

//-------------------------------------------------------------------------------------------//
//
// CEM OHPCF Write Command Handling
//
//-------------------------------------------------------------------------------------------//
void OnWriteCommandResult(
    const ResultMessage* result_msg,
    const FeatureAddressType* remote_feature_addr,
    EebusError err,
    void* ctx
) {
  UNUSED(err);
  const char* const command_str           = (const char*)ctx;
  const ResultDataType* const result_data = result_msg->result_data;

  if ((result_data == NULL) || (result_data->error_number == NULL)) {
    printf("CEM OHPCF %s command result missing from ", command_str);
    FeatureAddressPrint("%s\n", remote_feature_addr);
    return;
  }

  if (*result_data->error_number != kErrorNumberTypeNoError) {
    printf("CEM OHPCF %s command result error: %u from ", command_str, *result_data->error_number);
    FeatureAddressPrint("%s\n", remote_feature_addr);
  }
}

void HandleCmdWriteCommand(
    const CemOhpcfCli* self,
    const EntityAddressType* entity_addr,
    const char* const* tokens,
    size_t num_tokens
) {
  if (num_tokens < 3) {
    printf("Insufficient arguments for cem_ohpcf write_command command\n");
    return;
  }

  const char* const command_str = tokens[2];

  EebusError err = kEebusErrorOk;
  if (strcmp(command_str, "stop") == 0) {
    err = CemOhpcfWriteStopCommand(self->cem_ohpcf, entity_addr, OnWriteCommandResult, "stop");
  } else if (strcmp(command_str, "pause") == 0) {
    err = CemOhpcfWritePauseCommand(self->cem_ohpcf, entity_addr, OnWriteCommandResult, "pause");
  } else if (strcmp(command_str, "resume") == 0) {
    err = CemOhpcfWriteResumeCommand(self->cem_ohpcf, entity_addr, OnWriteCommandResult, "resume");
  } else {
    printf("Unknown subcommand for cem_ohpcf write command: %s\n", command_str);
    return;
  }

  if (err != kEebusErrorOk) {
    printf("CEM OHPCF failed to write %s, error code: %d\n", command_str, err);
    return;
  }

  printf("CEM OHPCF successfully wrote %s command\n", command_str);
}

void HandleCmd(const EebusCliHandlerObject* self, const char* const* tokens, size_t num_tokens) {
  const CemOhpcfCli* const cem_ohpcf_cli = CEM_OHPCF_CLI(self);

  if (num_tokens < 2) {
    printf("Insufficient arguments for cem_ohpcf command\n");
    return;
  }

  const char* adjusted[10];
  size_t adjusted_count = 0;
  const EntityAddressType* const entity_addr
      = CliExtractRemoteArg(tokens, num_tokens, cem_ohpcf_cli->addr_list, "cem_ohpcf", adjusted, &adjusted_count);
  if (entity_addr == NULL) {
    return;
  }

  if (adjusted_count < 2) {
    printf("Insufficient arguments for cem_ohpcf command\n");
    return;
  }

  if (strcmp(adjusted[1], "get") == 0) {
    HandleCmdGet(cem_ohpcf_cli, entity_addr, adjusted, adjusted_count);
  } else if (strcmp(adjusted[1], "schedule") == 0) {
    HandleCmdSchedule(cem_ohpcf_cli, entity_addr, adjusted, adjusted_count);
  } else if (strcmp(adjusted[1], "write_command") == 0) {
    HandleCmdWriteCommand(cem_ohpcf_cli, entity_addr, adjusted, adjusted_count);
  } else {
    printf("Unknown subcommand for cem_ohpcf: %s\n", adjusted[1]);
  }
}
