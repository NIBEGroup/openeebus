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
 * @brief EEBUS CLI Compressor OHPCF commands handling implementation
 */

#include <stdio.h>
#include <string.h>

#include "src/cli/eebus_cli_compressor_ohpcf.h"
#include "src/common/eebus_arguments.h"
#include "src/common/eebus_bool/eebus_bool.h"
#include "src/common/eebus_date_time/eebus_date_time.h"
#include "src/use_case/model/scaled_value.h"

typedef struct CompressorOhpcfCli CompressorOhpcfCli;

struct CompressorOhpcfCli {
  /** Implements the Eebus Cli Handler Interface */
  EebusCliHandlerObject obj;

  /** Compressor OHPCF instance to deal with */
  CompressorOhpcfUseCaseObject* compressor_ohpcf;
  /** Compressor OHPCF related MU MPC instance to deal with */
  MuMpcUseCaseObject* mu_mpc;
};

#define COMPRESSOR_OHPCF_CLI(obj) ((CompressorOhpcfCli*)(obj))

static void Destruct(EebusCliHandlerObject* self);
static void HandleCmd(const EebusCliHandlerObject* self, const char* const* tokens, size_t num_tokens);

static const EebusCliHandlerInterface compressor_ohpcf_cli_methods = {
    .destruct   = Destruct,
    .handle_cmd = HandleCmd,
};

static EebusError CompressorOhpcfCliConstruct(
    CompressorOhpcfCli* self,
    CompressorOhpcfUseCaseObject* compressor_ohpcf,
    MuMpcUseCaseObject* mu_mpc
);

static void HandleCmdSet(const CompressorOhpcfCli* self, const char* const* tokens, size_t num_tokens);
static void HandleCmdGet(const CompressorOhpcfCli* self, const char* const* tokens, size_t num_tokens);
static void HandleCmdAnnounce(const CompressorOhpcfCli* self, const char* const* tokens, size_t num_tokens);
static void HandleCmdReportState(const CompressorOhpcfCli* self, const char* const* tokens, size_t num_tokens);
static void HandleCmdClearProcess(const CompressorOhpcfCli* self, const char* const* tokens, size_t num_tokens);

EebusError CompressorOhpcfCliConstruct(
    CompressorOhpcfCli* self,
    CompressorOhpcfUseCaseObject* compressor_ohpcf,
    MuMpcUseCaseObject* mu_mpc
) {
  // Override "virtual functions table"
  EEBUS_CLI_HANDLER_INTERFACE(self) = &compressor_ohpcf_cli_methods;

  self->compressor_ohpcf = compressor_ohpcf;
  self->mu_mpc           = mu_mpc;

  if (compressor_ohpcf == NULL) {
    return kEebusErrorInputArgumentNull;
  }

  return kEebusErrorOk;
}

EebusCliHandlerObject*
CompressorOhpcfCliCreate(CompressorOhpcfUseCaseObject* compressor_ohpcf, MuMpcUseCaseObject* mu_mpc) {
  CompressorOhpcfCli* const compressor_ohpcf_cli = (CompressorOhpcfCli*)EEBUS_MALLOC(sizeof(CompressorOhpcfCli));
  if (compressor_ohpcf_cli == NULL) {
    return NULL;
  }

  if (CompressorOhpcfCliConstruct(compressor_ohpcf_cli, compressor_ohpcf, mu_mpc) != kEebusErrorOk) {
    CompressorOhpcfCliDelete(EEBUS_CLI_HANDLER_OBJECT(compressor_ohpcf_cli));
    return NULL;
  }

  return EEBUS_CLI_HANDLER_OBJECT(compressor_ohpcf_cli);
}

void Destruct(EebusCliHandlerObject* self) {
  UNUSED(self);

  // Nothing to be deallocated yet
}

//-------------------------------------------------------------------------------------------//
//
// Compressor OHPCF Getters Handling
//
//-------------------------------------------------------------------------------------------//
void HandleCmdSetPowerTotal(const CompressorOhpcfCli* self, const char* const* tokens, size_t num_tokens) {
  if (num_tokens != 4) {
    printf("Insufficient arguments for compressor_ohpcf set power_total command\n");
    return;
  }

  if (self->mu_mpc == NULL) {
    printf("Compressor OHPCF MU MPC instance is NULL\n");
    return;
  }

  ScaledValue power_total = {0};
  if (ScaledValueParse(tokens[3], &power_total) != kEebusErrorOk) {
    printf("Compressor OHPCF invalid power total value: %s\n", tokens[3]);
    return;
  }

  EebusError err = MuMpcSetMeasurementDataCache(self->mu_mpc, kMpcPowerTotal, &power_total, NULL, NULL);
  if (err != kEebusErrorOk) {
    printf("Setting Compressor OHPCF power total measurement cache failed: %d\n", err);
    return;
  }

  err = MuMpcUpdate(self->mu_mpc);
  if (err != kEebusErrorOk) {
    printf("Updating Compressor OHPCF power total measurement failed: %d\n", err);
    return;
  }

  ScaledValuePrint("Compressor OHPCF power total set to %s W\n", &power_total);
}

void HandleCmdSet(const CompressorOhpcfCli* self, const char* const* tokens, size_t num_tokens) {
  if (num_tokens < 3) {
    printf("Insufficient arguments for compressor_ohpcf set command\n");
    return;
  }

  const char* subcommand = tokens[2];
  if (strcmp(subcommand, "power_total") == 0) {
    HandleCmdSetPowerTotal(self, tokens, num_tokens);
  } else {
    printf("Unknown subcommand for compressor_ohpcf set: %s\n", subcommand);
  }
}
void HandleCmdGet(const CompressorOhpcfCli* self, const char* const* tokens, size_t num_tokens) {
  if (num_tokens != 3) {
    printf("Insufficient arguments for compressor_ohpcf get command\n");
    return;
  }

  const char* subcommand = tokens[2];
  if (strcmp(subcommand, "state") != 0) {
    printf("Unknown subcommand for compressor_ohpcf get: %s\n", subcommand);
    return;
  }

  const CompressorOhpcfState state = CompressorOhpcfGetState(self->compressor_ohpcf);
  const char* const state_name     = CompressorOhpcfStateGetName(state);

  if (state_name == NULL) {
    printf("Unknown compressor_ohpcf state: %d\n", state);
    return;
  }

  printf("Compressor OHPCF state: %s\n", state_name);
}

//-------------------------------------------------------------------------------------------//
//
// Compressor OHPCF Setters Handling
//
//-------------------------------------------------------------------------------------------//
void HandleCmdAnnounce(const CompressorOhpcfCli* self, const char* const* tokens, size_t num_tokens) {
  if (num_tokens < 8) {
    printf("Insufficient arguments for compressor_ohpcf announce command\n");
    return;
  }

  OptionalPowerConsumption optional_power_consumption = {0};
  if (ScaledValueParse(tokens[2], &optional_power_consumption.max_power_w) != kEebusErrorOk) {
    printf("Compressor OHPCF invalid power consumption value: %s\n", tokens[2]);
    return;
  }

  if (EebusDurationParse(tokens[3], &optional_power_consumption.earliest_start_time) != kEebusErrorOk) {
    printf("Compressor OHPCF invalid min duration value: %s\n", tokens[3]);
    return;
  }

  if (EebusDurationParse(tokens[4], &optional_power_consumption.latest_end_time) != kEebusErrorOk) {
    printf("Compressor OHPCF invalid max duration value: %s\n", tokens[4]);
    return;
  }

  if (EebusDurationParse(tokens[5], &optional_power_consumption.active_duration_min) != kEebusErrorOk) {
    printf("Compressor OHPCF invalid active duration value: %s\n", tokens[5]);
    return;
  }

  if (EebusBoolParse(tokens[6], &optional_power_consumption.is_stoppable) != kEebusErrorOk) {
    printf("Compressor OHPCF invalid is_stoppable flag value: %s\n", tokens[6]);
    return;
  }

  if (EebusBoolParse(tokens[7], &optional_power_consumption.is_pausable) != kEebusErrorOk) {
    printf("Compressor OHPCF invalid is_pausable flag value: %s\n", tokens[7]);
    return;
  }

  EebusError err = CompressorOhpcfAnnounce(self->compressor_ohpcf, &optional_power_consumption);
  if (err != kEebusErrorOk) {
    printf("Compressor OHPCF announce process failed with error: %d\n", err);
    return;
  }

  printf("Compressor OHPCF announce process succeeded\n");
}

void HandleCmdReportState(const CompressorOhpcfCli* self, const char* const* tokens, size_t num_tokens) {
  if (num_tokens < 3) {
    printf("Compressor OHPCF insufficient arguments for compressor_ohpcf report_state command\n");
    return;
  }

  const CompressorOhpcfState* const state = CompressorOhpcfStateGetStateWithName(tokens[2]);
  if (state == NULL) {
    printf("Unknown compressor_ohpcf state for report_state: %s\n", tokens[2]);
    return;
  }

  EebusDuration scheduled_start_time          = {0};
  const EebusDuration* p_scheduled_start_time = NULL;
  if (*state == kCompressorOhpcfStateScheduled) {
    if (num_tokens != 4) {
      printf("Missing start time for scheduled state\n");
      return;
    }

    if (EebusDurationParse(tokens[3], &scheduled_start_time) != kEebusErrorOk) {
      printf("Invalid start time value: %s\n", tokens[3]);
      return;
    }

    p_scheduled_start_time = &scheduled_start_time;
  }

  if (CompressorOhpcfReportState(self->compressor_ohpcf, *state, p_scheduled_start_time) != kEebusErrorOk) {
    printf("Compressor OHPCF report state process failed\n");
    return;
  }

  printf("Compressor OHPCF report state process succeeded\n");
}

void HandleCmdClearProcess(const CompressorOhpcfCli* self, const char* const* tokens, size_t num_tokens) {
  UNUSED(tokens);
  UNUSED(num_tokens);

  if (CompressorOhpcfClearProcess(self->compressor_ohpcf) != kEebusErrorOk) {
    printf("Compressor OHPCF clear process failed\n");
    return;
  }

  printf("Compressor OHPCF clear process succeeded\n");
}

void HandleCmd(const EebusCliHandlerObject* self, const char* const* tokens, size_t num_tokens) {
  const CompressorOhpcfCli* const compressor_ohpcf_cli = COMPRESSOR_OHPCF_CLI(self);

  if (num_tokens < 2) {
    printf("Insufficient arguments for compressor_ohpcf command\n");
    return;
  }

  if (strcmp(tokens[1], "set") == 0) {
    HandleCmdSet(compressor_ohpcf_cli, tokens, num_tokens);
  } else if (strcmp(tokens[1], "get") == 0) {
    HandleCmdGet(compressor_ohpcf_cli, tokens, num_tokens);
  } else if (strcmp(tokens[1], "announce") == 0) {
    HandleCmdAnnounce(compressor_ohpcf_cli, tokens, num_tokens);
  } else if (strcmp(tokens[1], "report_state") == 0) {
    HandleCmdReportState(compressor_ohpcf_cli, tokens, num_tokens);
  } else if (strcmp(tokens[1], "clear_process") == 0) {
    HandleCmdClearProcess(compressor_ohpcf_cli, tokens, num_tokens);
  } else {
    printf("Unknown subcommand for compressor_ohpcf: %s\n", tokens[1]);
  }
}
