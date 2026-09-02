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
 * @brief EEBUS CLI CS Limitation of Power commands handling implementation
 */

#include <stdio.h>
#include <string.h>

#include "src/cli/eebus_cli_cs_lp.h"
#include "src/common/eebus_arguments.h"
#include "src/common/eebus_bool/eebus_bool.h"
#include "src/common/eebus_date_time/eebus_date_time.h"
#include "src/use_case/model/scaled_value.h"

typedef struct CsLpCli CsLpCli;

struct CsLpCli {
  /** Implements the Eebus Cli Handler Interface */
  EebusCliHandlerObject obj;

  /** CS LP instance to deal with */
  CsLpUseCaseObject* cs_lp;
  /** Command name ("cs_lpc" or "cs_lpp") */
  const char* cmd_name;
  /** Command name in uppercase ("CS LPC" or "CS LPP") */
  const char* cmd_name_caps;
};

#define CS_LP_CLI(obj) ((CsLpCli*)(obj))

static void Destruct(EebusCliHandlerObject* self);
static void HandleCmd(const EebusCliHandlerObject* self, const char* const* tokens, size_t num_tokens);

static const EebusCliHandlerInterface cs_lp_cli_methods = {
    .destruct   = Destruct,
    .handle_cmd = HandleCmd,
};

static EebusError CsLpCliConstruct(CsLpCli* self, EnergyDirectionType energy_direction, CsLpUseCaseObject* cs_lp);

static void HandleCmdGetPowerLimit(const CsLpCli* self, const char* const* tokens, size_t num_tokens);
static void HandleCmdGetFailsafeLimit(const CsLpCli* self, const char* const* tokens, size_t num_tokens);
static void HandleCmdGetFailsafeDuration(const CsLpCli* self, const char* const* tokens, size_t num_tokens);
static void HandleCmdGetNominalMax(const CsLpCli* self, const char* const* tokens, size_t num_tokens);
static void HandleCmdGet(const CsLpCli* self, const char* const* tokens, size_t num_tokens);
static void HandleCmdSetPowerLimit(const CsLpCli* self, const char* const* tokens, size_t num_tokens);
static void HandleCmdSetFailsafeLimit(const CsLpCli* self, const char* const* tokens, size_t num_tokens);
static void HandleCmdSetFailsafeDuration(const CsLpCli* self, const char* const* tokens, size_t num_tokens);
static void HandleCmdSetNominalMax(const CsLpCli* self, const char* const* tokens, size_t num_tokens);
static void HandleCmdSet(const CsLpCli* self, const char* const* tokens, size_t num_tokens);
static void HandleCmdStart(const CsLpCli* self, const char* const* tokens, size_t num_tokens);
static void HandleCmdStop(const CsLpCli* self, const char* const* tokens, size_t num_tokens);

EebusError CsLpCliConstruct(CsLpCli* self, EnergyDirectionType energy_direction, CsLpUseCaseObject* cs_lp) {
  // Override "virtual functions table"
  EEBUS_CLI_HANDLER_INTERFACE(self) = &cs_lp_cli_methods;

  self->cs_lp         = cs_lp;
  self->cmd_name      = NULL;
  self->cmd_name_caps = NULL;

  if (cs_lp == NULL) {
    return kEebusErrorInputArgumentNull;
  }

  if (energy_direction == kEnergyDirectionTypeConsume) {
    self->cmd_name      = "cs_lpc";
    self->cmd_name_caps = "CS LPC";
  } else {
    self->cmd_name      = "cs_lpp";
    self->cmd_name_caps = "CS LPP";
  }

  return kEebusErrorOk;
}

EebusCliHandlerObject* CsLpCliCreate(EnergyDirectionType energy_direction, CsLpUseCaseObject* cs_lp) {
  CsLpCli* const cli_cs_lp = (CsLpCli*)EEBUS_MALLOC(sizeof(CsLpCli));
  if (cli_cs_lp == NULL) {
    return NULL;
  }

  if (CsLpCliConstruct(cli_cs_lp, energy_direction, cs_lp) != kEebusErrorOk) {
    CsLpCliDelete(EEBUS_CLI_HANDLER_OBJECT(cli_cs_lp));
    return NULL;
  }

  return EEBUS_CLI_HANDLER_OBJECT(cli_cs_lp);
}

void Destruct(EebusCliHandlerObject* self) {
  UNUSED(self);
  // Nothing to be deallocated yet
}

//-------------------------------------------------------------------------------------------//
//
// CS LP Getters Handling
//
//-------------------------------------------------------------------------------------------//
void HandleCmdGetPowerLimit(const CsLpCli* self, const char* const* tokens, size_t num_tokens) {
  UNUSED(tokens);
  UNUSED(num_tokens);

  LoadLimit limit = {0};
  if (CsLpGetActivePowerLimit(self->cs_lp, &limit) != kEebusErrorOk) {
    printf("%s getting Active Power Limit failed\n", self->cmd_name_caps);
    return;
  }

  printf("%s ", self->cmd_name_caps);
  ScaledValuePrint("Active Power Limit: value=%s, ", &limit.value);
  EebusDurationPrint("duration=%s, ", &limit.duration);
  printf("is active=%s\n", EebusBoolToString(limit.is_active));
}

void HandleCmdGetFailsafeLimit(const CsLpCli* self, const char* const* tokens, size_t num_tokens) {
  UNUSED(tokens);
  UNUSED(num_tokens);

  ScaledValue power_limit = {0};
  bool is_changeable      = false;
  if (CsLpGetFailsafeActivePowerLimit(self->cs_lp, &power_limit, &is_changeable) != kEebusErrorOk) {
    printf("%s getting Failsafe Active Power Limit failed\n", self->cmd_name_caps);
    return;
  }

  printf("%s ", self->cmd_name_caps);
  ScaledValuePrint("Failsafe Active Power Limit: value=%s, ", &power_limit);
  printf("is changeable=%s\n", EebusBoolToString(is_changeable));
}

void HandleCmdGetFailsafeDuration(const CsLpCli* self, const char* const* tokens, size_t num_tokens) {
  UNUSED(tokens);
  UNUSED(num_tokens);

  DurationType duration = {0};
  bool is_changeable    = false;
  if (CsLpGetFailsafeDurationMinimum(self->cs_lp, &duration, &is_changeable) != kEebusErrorOk) {
    printf("%s getting Failsafe Duration Minimum failed\n", self->cmd_name_caps);
    return;
  }

  printf("%s ", self->cmd_name_caps);
  EebusDurationPrint("Failsafe Duration Minimum: %s, ", &duration);
  printf("is changeable=%s\n", EebusBoolToString(is_changeable));
}

void HandleCmdGetNominalMax(const CsLpCli* self, const char* const* tokens, size_t num_tokens) {
  UNUSED(tokens);
  UNUSED(num_tokens);

  ScaledValue nominal_max = {0};
  if (CsLpGetNominalMax(self->cs_lp, &nominal_max) != kEebusErrorOk) {
    printf("%s getting Nominal Max failed\n", self->cmd_name_caps);
    return;
  }

  printf("%s ", self->cmd_name_caps);
  ScaledValuePrint("Nominal Max: value=%s\n", &nominal_max);
}

void HandleCmdGet(const CsLpCli* self, const char* const* tokens, size_t num_tokens) {
  if (num_tokens != 3) {
    printf("Insufficient arguments for %s get command\n", self->cmd_name);
    return;
  }

  const char* subcommand = tokens[2];
  if (strcmp(subcommand, "power_limit") == 0) {
    HandleCmdGetPowerLimit(self, tokens, num_tokens);
  } else if (strcmp(subcommand, "failsafe_limit") == 0) {
    HandleCmdGetFailsafeLimit(self, tokens, num_tokens);
  } else if (strcmp(subcommand, "failsafe_duration") == 0) {
    HandleCmdGetFailsafeDuration(self, tokens, num_tokens);
  } else if (strcmp(subcommand, "nominal_max") == 0) {
    HandleCmdGetNominalMax(self, tokens, num_tokens);
  } else {
    printf("Unknown subcommand for %s get: %s\n", self->cmd_name, subcommand);
  }
}

//-------------------------------------------------------------------------------------------//
//
// CS LP Setters Handling
//
//-------------------------------------------------------------------------------------------//
void HandleCmdSetPowerLimit(const CsLpCli* self, const char* const* tokens, size_t num_tokens) {
  if (num_tokens != 6) {
    printf("Insufficient arguments for %s set power_limit command\n", self->cmd_name);
    return;
  }

  ScaledValue power_limit = {0};
  if (ScaledValueParse(tokens[3], &power_limit) != kEebusErrorOk) {
    printf("%s invalid limit value: %s\n", self->cmd_name_caps, tokens[3]);
    return;
  }

  bool is_active = false;
  if (EebusBoolParse(tokens[4], &is_active) != kEebusErrorOk) {
    printf("%s invalid active flag value: %s\n", self->cmd_name_caps, tokens[4]);
    return;
  }

  bool is_changeable = false;
  if (EebusBoolParse(tokens[5], &is_changeable) != kEebusErrorOk) {
    printf("%s invalid changeable flag value: %s\n", self->cmd_name_caps, tokens[5]);
    return;
  }

  if (CsLpSetActivePowerLimit(self->cs_lp, &power_limit, is_active, is_changeable) != kEebusErrorOk) {
    printf("%s setting Active Power Limit failed\n", self->cmd_name_caps);
    return;
  }

  printf("%s setting Active Power Limit succeeded\n", self->cmd_name_caps);
}

void HandleCmdSetFailsafeLimit(const CsLpCli* self, const char* const* tokens, size_t num_tokens) {
  if (num_tokens != 5) {
    printf("Insufficient arguments for %s set failsafe_limit command\n", self->cmd_name);
    return;
  }

  ScaledValue power_limit = {0};
  if (ScaledValueParse(tokens[3], &power_limit) != kEebusErrorOk) {
    printf("%s invalid value for Failsafe Limit: %s\n", self->cmd_name_caps, tokens[3]);
    return;
  }

  bool is_changeable = false;
  if (EebusBoolParse(tokens[4], &is_changeable) != kEebusErrorOk) {
    printf("%s invalid changeable flag value: %s\n", self->cmd_name_caps, tokens[4]);
    return;
  }

  if (CsLpSetFailsafeActivePowerLimit(self->cs_lp, &power_limit, is_changeable) != kEebusErrorOk) {
    printf("%s setting Failsafe Limit failed\n", self->cmd_name_caps);
    return;
  }

  printf("%s setting Failsafe Limit succeeded\n", self->cmd_name_caps);
}

void HandleCmdSetFailsafeDuration(const CsLpCli* self, const char* const* tokens, size_t num_tokens) {
  if (num_tokens != 5) {
    printf("Insufficient arguments for %s set failsafe_duration command\n", self->cmd_name);
    return;
  }

  DurationType duration = {0};
  if (EebusDurationParse(tokens[3], &duration) != kEebusErrorOk) {
    printf("%s invalid value for Failsafe Duration: %s\n", self->cmd_name_caps, tokens[3]);
    return;
  }

  bool is_changeable = false;
  if (EebusBoolParse(tokens[4], &is_changeable) != kEebusErrorOk) {
    printf("%s invalid changeable flag value: %s\n", self->cmd_name_caps, tokens[4]);
    return;
  }

  if (CsLpSetFailsafeDurationMinimum(self->cs_lp, &duration, is_changeable) != kEebusErrorOk) {
    printf("%s setting Failsafe Duration failed\n", self->cmd_name_caps);
    return;
  }

  printf("%s setting Failsafe Duration minimum succeeded\n", self->cmd_name_caps);
}

void HandleCmdSetNominalMax(const CsLpCli* self, const char* const* tokens, size_t num_tokens) {
  if (num_tokens != 4) {
    printf("Insufficient arguments for %s set nominal_max command\n", self->cmd_name);
    return;
  }

  ScaledValue nominal_max = {0};
  if (ScaledValueParse(tokens[3], &nominal_max) != kEebusErrorOk) {
    printf("%s invalid value for Nominal Max: %s\n", self->cmd_name_caps, tokens[3]);
    return;
  }

  if (CsLpSetNominalMax(self->cs_lp, &nominal_max) != kEebusErrorOk) {
    printf("%s setting Nominal Max failed\n", self->cmd_name_caps);
    return;
  }

  printf("%s setting Nominal Max succeeded\n", self->cmd_name_caps);
}

void HandleCmdSet(const CsLpCli* self, const char* const* tokens, size_t num_tokens) {
  if (num_tokens < 3) {
    printf("Insufficient arguments for %s set command\n", self->cmd_name);
    return;
  }

  const char* subcommand = tokens[2];
  if (strcmp(subcommand, "power_limit") == 0) {
    HandleCmdSetPowerLimit(self, tokens, num_tokens);
  } else if (strcmp(subcommand, "failsafe_limit") == 0) {
    HandleCmdSetFailsafeLimit(self, tokens, num_tokens);
  } else if (strcmp(subcommand, "failsafe_duration") == 0) {
    HandleCmdSetFailsafeDuration(self, tokens, num_tokens);
  } else if (strcmp(subcommand, "nominal_max") == 0) {
    HandleCmdSetNominalMax(self, tokens, num_tokens);
  } else {
    printf("Unknown subcommand for %s set: %s\n", self->cmd_name, subcommand);
  }
}

//-------------------------------------------------------------------------------------------//
//
// CS LP Start/Stop Handling
//
//-------------------------------------------------------------------------------------------//
void HandleCmdStart(const CsLpCli* self, const char* const* tokens, size_t num_tokens) {
  if (num_tokens < 3) {
    printf("Insufficient arguments for %s start command\n", self->cmd_name);
    return;
  }

  const char* const subcommand = tokens[2];
  if (strcmp(subcommand, "heartbeat") == 0) {
    CsLpStartHeartbeat(self->cs_lp);
    printf("%s heartbeat started\n", self->cmd_name_caps);
  } else {
    printf("Unknown subcommand for %s start: %s\n", self->cmd_name, subcommand);
  }
}

void HandleCmdStop(const CsLpCli* self, const char* const* tokens, size_t num_tokens) {
  if (num_tokens < 3) {
    printf("Insufficient arguments for %s stop command\n", self->cmd_name);
    return;
  }

  const char* const subcommand = tokens[2];
  if (strcmp(subcommand, "heartbeat") == 0) {
    CsLpStopHeartbeat(self->cs_lp);
    printf("%s heartbeat stopped\n", self->cmd_name_caps);
  } else {
    printf("Unknown subcommand for %s stop: %s\n", self->cmd_name, subcommand);
  }
}

void HandleCmd(const EebusCliHandlerObject* self, const char* const* tokens, size_t num_tokens) {
  const CsLpCli* cs_lp_cli = CS_LP_CLI(self);

  if (num_tokens < 2) {
    printf("Insufficient arguments for %s command\n", cs_lp_cli->cmd_name);
    return;
  }

  if (strcmp(tokens[1], "set") == 0) {
    HandleCmdSet(cs_lp_cli, tokens, num_tokens);
  } else if (strcmp(tokens[1], "get") == 0) {
    HandleCmdGet(cs_lp_cli, tokens, num_tokens);
  } else if (strcmp(tokens[1], "start") == 0) {
    HandleCmdStart(cs_lp_cli, tokens, num_tokens);
  } else if (strcmp(tokens[1], "stop") == 0) {
    HandleCmdStop(cs_lp_cli, tokens, num_tokens);
  } else {
    printf("Unknown subcommand for %s: %s\n", cs_lp_cli->cmd_name, tokens[1]);
  }
}
