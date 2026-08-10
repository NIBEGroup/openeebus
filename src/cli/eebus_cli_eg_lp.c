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
 * @brief EEBUS CLI EG Limitation of Power commands handling implementation
 */

#include <stdio.h>
#include <string.h>

#include "src/cli/eebus_cli_eg_lp.h"
#include "src/cli/eebus_cli_remote_arg.h"
#include "src/common/eebus_arguments.h"
#include "src/common/eebus_bool/eebus_bool.h"
#include "src/common/eebus_date_time/eebus_date_time.h"
#include "src/use_case/model/scaled_value.h"

typedef struct EgLpCli EgLpCli;

struct EgLpCli {
  /** Implements the Eebus Cli Handler Interface */
  EebusCliHandlerObject obj;

  /** EG LP instance to deal with */
  EgLpUseCaseObject* eg_lp;
  /** Connected remote entity address list (not owned — pointer into caller's storage) */
  const EntityAddressList* addr_list;
  /** Energy direction (consume = LPC, produce = LPP) */
  EnergyDirectionType energy_direction;
  /** Command name ("eg_lpc" or "eg_lpp") */
  const char* cmd_name;
  /** Command name in uppercase ("EG LPC" or "EG LPP") */
  const char* cmd_name_caps;
};

#define EG_LP_CLI(obj) ((EgLpCli*)(obj))

static void Destruct(EebusCliHandlerObject* self);
static void HandleCmd(const EebusCliHandlerObject* self, const char* const* tokens, size_t num_tokens);

static const EebusCliHandlerInterface eg_lp_cli_methods = {
    .destruct   = Destruct,
    .handle_cmd = HandleCmd,
};

static EebusError EgLpCliConstruct(
    EgLpCli*                 self,
    EnergyDirectionType      energy_direction,
    EgLpUseCaseObject*       eg_lp,
    const EntityAddressList* addr_list
);

static void HandleCmdList(const EgLpCli* self);
static void HandleCmdGetPowerLimit(
    const EgLpCli* self, const EntityAddressType* entity_addr, const char* const* tokens, size_t num_tokens
);
static void HandleCmdGetFailsafeLimit(
    const EgLpCli* self, const EntityAddressType* entity_addr, const char* const* tokens, size_t num_tokens
);
static void HandleCmdGetFailsafeDuration(
    const EgLpCli* self, const EntityAddressType* entity_addr, const char* const* tokens, size_t num_tokens
);
static void HandleCmdGetPowerNominalMax(
    const EgLpCli* self, const EntityAddressType* entity_addr, const char* const* tokens, size_t num_tokens
);
static void HandleCmdGet(
    const EgLpCli* self, const EntityAddressType* entity_addr, const char* const* tokens, size_t num_tokens
);
static void OnSetPowerLimitResult(
    const ResultMessage*      result_msg,
    const FeatureAddressType* remote_feature_addr,
    EebusError                err,
    void*                     ctx
);
static void HandleCmdSetPowerLimit(
    const EgLpCli* self, const EntityAddressType* entity_addr, const char* const* tokens, size_t num_tokens
);
static void OnSetFailsafeLimitResult(
    const ResultMessage*      result_msg,
    const FeatureAddressType* remote_feature_addr,
    EebusError                err,
    void*                     ctx
);
static void HandleCmdSetFailsafeLimit(
    const EgLpCli* self, const EntityAddressType* entity_addr, const char* const* tokens, size_t num_tokens
);
static void OnSetFailsafeDurationResult(
    const ResultMessage*      result_msg,
    const FeatureAddressType* remote_feature_addr,
    EebusError                err,
    void*                     ctx
);
static void HandleCmdSetFailsafeDuration(
    const EgLpCli* self, const EntityAddressType* entity_addr, const char* const* tokens, size_t num_tokens
);
static void HandleCmdSet(
    const EgLpCli* self, const EntityAddressType* entity_addr, const char* const* tokens, size_t num_tokens
);
static void HandleCmdStart(const EgLpCli* self, const char* const* tokens, size_t num_tokens);
static void HandleCmdStop(const EgLpCli* self, const char* const* tokens, size_t num_tokens);

EebusError EgLpCliConstruct(
    EgLpCli*                 self,
    EnergyDirectionType      energy_direction,
    EgLpUseCaseObject*       eg_lp,
    const EntityAddressList* addr_list
) {
  EEBUS_CLI_HANDLER_INTERFACE(self) = &eg_lp_cli_methods;

  self->eg_lp            = NULL;
  self->addr_list        = NULL;
  self->energy_direction = energy_direction;
  self->cmd_name         = NULL;
  self->cmd_name_caps    = NULL;

  if ((eg_lp == NULL) || (addr_list == NULL)) {
    return kEebusErrorInputArgumentNull;
  }

  self->eg_lp     = eg_lp;
  self->addr_list = addr_list;

  if (energy_direction == kEnergyDirectionTypeConsume) {
    self->cmd_name      = "eg_lpc";
    self->cmd_name_caps = "EG LPC";
  } else {
    self->cmd_name      = "eg_lpp";
    self->cmd_name_caps = "EG LPP";
  }

  return kEebusErrorOk;
}

EebusCliHandlerObject*
EgLpCliCreate(EnergyDirectionType energy_direction, EgLpUseCaseObject* eg_lp, const EntityAddressList* addr_list) {
  EgLpCli* cli_eg_lp = (EgLpCli*)EEBUS_MALLOC(sizeof(EgLpCli));
  if (cli_eg_lp == NULL) {
    return NULL;
  }

  if (EgLpCliConstruct(cli_eg_lp, energy_direction, eg_lp, addr_list) != kEebusErrorOk) {
    EgLpCliDelete(EEBUS_CLI_HANDLER_OBJECT(cli_eg_lp));
    return NULL;
  }

  return EEBUS_CLI_HANDLER_OBJECT(cli_eg_lp);
}

void Destruct(EebusCliHandlerObject* self) {
  EgLpCli* const eg_lp_cli = EG_LP_CLI(self);
  eg_lp_cli->addr_list     = NULL;
}

//-------------------------------------------------------------------------------------------//
//
// EG LP List Handling
//
//-------------------------------------------------------------------------------------------//
static void HandleCmdList(const EgLpCli* self) {
  const size_t count = EntityAddressListGetSize(self->addr_list);
  if (count == 0) {
    printf("%s: no remotes connected\n", self->cmd_name);
    return;
  }
  printf("%s connected remotes (%zu):\n", self->cmd_name, count);
  for (size_t i = 0; i < count; i++) {
    char formatted[EEBUS_CLI_ENTITY_ADDR_STR_MAX];
    CliFormatEntityAddress(EntityAddressListGet(self->addr_list, i), formatted, sizeof(formatted));
    printf("  %s\n", formatted);
  }
}

//-------------------------------------------------------------------------------------------//
//
// EG LP Getters Handling
//
//-------------------------------------------------------------------------------------------//
static void HandleCmdGetPowerLimit(
    const EgLpCli*           self,
    const EntityAddressType* entity_addr,
    const char* const*       tokens,
    size_t                   num_tokens
) {
  UNUSED(tokens);
  UNUSED(num_tokens);

  LoadLimit limit = {0};
  if (EgLpGetActivePowerLimit(self->eg_lp, entity_addr, &limit) != kEebusErrorOk) {
    printf("%s getting power limit failed\n", self->cmd_name_caps);
    return;
  }

  printf("%s ", self->cmd_name_caps);
  ScaledValuePrint("Active Power Limit: value=%s, ", &limit.value);
  EebusDurationPrint("duration=%s, ", &limit.duration);
  printf("active=%s\n", EebusBoolToString(limit.is_active));
}

static void HandleCmdGetFailsafeLimit(
    const EgLpCli*           self,
    const EntityAddressType* entity_addr,
    const char* const*       tokens,
    size_t                   num_tokens
) {
  UNUSED(tokens);
  UNUSED(num_tokens);

  ScaledValue power_limit = {0};
  if (EgLpGetFailsafeActivePowerLimit(self->eg_lp, entity_addr, &power_limit) != kEebusErrorOk) {
    printf("%s getting failsafe limit failed\n", self->cmd_name_caps);
    return;
  }

  printf("%s ", self->cmd_name_caps);
  ScaledValuePrint("Failsafe Active Power Limit: value=%s\n", &power_limit);
}

static void HandleCmdGetFailsafeDuration(
    const EgLpCli*           self,
    const EntityAddressType* entity_addr,
    const char* const*       tokens,
    size_t                   num_tokens
) {
  UNUSED(tokens);
  UNUSED(num_tokens);

  DurationType duration = {0};
  if (EgLpGetFailsafeDurationMinimum(self->eg_lp, entity_addr, &duration) != kEebusErrorOk) {
    printf("%s getting failsafe duration failed\n", self->cmd_name_caps);
    return;
  }

  printf("%s ", self->cmd_name_caps);
  EebusDurationPrint("Failsafe Duration Minimum: %s\n", &duration);
}

static void HandleCmdGetPowerNominalMax(
    const EgLpCli*           self,
    const EntityAddressType* entity_addr,
    const char* const*       tokens,
    size_t                   num_tokens
) {
  UNUSED(tokens);
  UNUSED(num_tokens);

  ScaledValue power_limit = {0};
  const EebusError err    = EgLpGetPowerNominalMax(self->eg_lp, entity_addr, &power_limit);

  if (err != kEebusErrorOk) {
    printf("%s getting Power Nominal Max failed\n", self->cmd_name_caps);
    return;
  }

  printf("%s ", self->cmd_name_caps);
  ScaledValuePrint("Power Nominal Max: value=%s\n", &power_limit);
}

static void HandleCmdGet(
    const EgLpCli*           self,
    const EntityAddressType* entity_addr,
    const char* const*       tokens,
    size_t                   num_tokens
) {
  if (num_tokens != 3) {
    printf("Insufficient arguments for %s get command\n", self->cmd_name);
    return;
  }

  const char* subcommand = tokens[2];
  if (strcmp(subcommand, "power_limit") == 0) {
    HandleCmdGetPowerLimit(self, entity_addr, tokens, num_tokens);
  } else if (strcmp(subcommand, "failsafe_limit") == 0) {
    HandleCmdGetFailsafeLimit(self, entity_addr, tokens, num_tokens);
  } else if (strcmp(subcommand, "failsafe_duration") == 0) {
    HandleCmdGetFailsafeDuration(self, entity_addr, tokens, num_tokens);
  } else if (strcmp(subcommand, "power_nominal_max") == 0) {
    HandleCmdGetPowerNominalMax(self, entity_addr, tokens, num_tokens);
  } else {
    printf("Unknown subcommand for %s get: %s\n", self->cmd_name, subcommand);
  }
}

//-------------------------------------------------------------------------------------------//
//
// EG LP Setters Handling
//
//-------------------------------------------------------------------------------------------//
static void OnSetPowerLimitResult(
    const ResultMessage*      result_msg,
    const FeatureAddressType* remote_feature_addr,
    EebusError                err,
    void*                     ctx
) {
  UNUSED(err);
  const char* const cmd_name_caps         = (const char*)ctx;
  const ResultDataType* const result_data = result_msg->result_data;

  if ((result_data == NULL) || (result_data->error_number == NULL)) {
    printf("%s Active Power Limit result missing from ", cmd_name_caps);
    FeatureAddressPrint("%s\n", remote_feature_addr);
    return;
  }

  if (*result_data->error_number != kErrorNumberTypeNoError) {
    printf("%s Active Power Limit result error: %u from ", cmd_name_caps, *result_data->error_number);
    FeatureAddressPrint("%s\n", remote_feature_addr);
  }
}

static void HandleCmdSetPowerLimit(
    const EgLpCli*           self,
    const EntityAddressType* entity_addr,
    const char* const*       tokens,
    size_t                   num_tokens
) {
  if (num_tokens != 6) {
    printf("Insufficient arguments for %s set power_limit command\n", self->cmd_name);
    return;
  }

  LoadLimit limit = {0};
  if (ScaledValueParse(tokens[3], &limit.value) != kEebusErrorOk) {
    printf("%s invalid Active Power Limit value: %s\n", self->cmd_name_caps, tokens[3]);
    return;
  }

  if (EebusDurationParse(tokens[4], &limit.duration) != kEebusErrorOk) {
    printf("%s invalid Active Power Limit duration value: %s\n", self->cmd_name_caps, tokens[4]);
    return;
  }

  if (EebusBoolParse(tokens[5], &limit.is_active) != kEebusErrorOk) {
    printf("%s invalid is_active flag value: %s\n", self->cmd_name_caps, tokens[5]);
    return;
  }

  if (EgLpSetActivePowerLimit(self->eg_lp, entity_addr, &limit, OnSetPowerLimitResult, (void*)self->cmd_name_caps)
      != kEebusErrorOk) {
    printf("%s setting Active Power Limit failed\n", self->cmd_name_caps);
    return;
  }

  printf("%s setting Active Power Limit succeeded\n", self->cmd_name_caps);
}

static void OnSetFailsafeLimitResult(
    const ResultMessage*      result_msg,
    const FeatureAddressType* remote_feature_addr,
    EebusError                err,
    void*                     ctx
) {
  UNUSED(err);
  const char* const cmd_name_caps         = (const char*)ctx;
  const ResultDataType* const result_data = result_msg->result_data;

  if ((result_data == NULL) || (result_data->error_number == NULL)) {
    printf("%s Failsafe Active Power Limit result missing from ", cmd_name_caps);
    FeatureAddressPrint("%s\n", remote_feature_addr);
    return;
  }

  if (*result_data->error_number != kErrorNumberTypeNoError) {
    printf("%s Failsafe Active Power Limit result error: %u from ", cmd_name_caps, *result_data->error_number);
    FeatureAddressPrint("%s\n", remote_feature_addr);
  }
}

static void HandleCmdSetFailsafeLimit(
    const EgLpCli*           self,
    const EntityAddressType* entity_addr,
    const char* const*       tokens,
    size_t                   num_tokens
) {
  if (num_tokens != 4) {
    printf("Insufficient arguments for %s set failsafe_limit command\n", self->cmd_name);
    return;
  }

  ScaledValue power_limit = {0};
  if (ScaledValueParse(tokens[3], &power_limit) != kEebusErrorOk) {
    printf("%s invalid value for Failsafe Active Power Limit: %s\n", self->cmd_name_caps, tokens[3]);
    return;
  }

  if (EgLpSetFailsafeActivePowerLimit(
          self->eg_lp,
          entity_addr,
          &power_limit,
          OnSetFailsafeLimitResult,
          (void*)self->cmd_name_caps
      )
      != kEebusErrorOk) {
    printf("%s setting Failsafe Active Power Limit failed\n", self->cmd_name_caps);
    return;
  }

  printf("%s setting Failsafe Active Power Limit succeeded\n", self->cmd_name_caps);
}

static void OnSetFailsafeDurationResult(
    const ResultMessage*      result_msg,
    const FeatureAddressType* remote_feature_addr,
    EebusError                err,
    void*                     ctx
) {
  UNUSED(err);
  const char* const cmd_name_caps         = (const char*)ctx;
  const ResultDataType* const result_data = result_msg->result_data;

  if ((result_data == NULL) || (result_data->error_number == NULL)) {
    printf("%s Failsafe Duration Minimum result missing from ", cmd_name_caps);
    FeatureAddressPrint("%s\n", remote_feature_addr);
    return;
  }

  if (*result_data->error_number != kErrorNumberTypeNoError) {
    printf("%s Failsafe Duration Minimum result error: %u from ", cmd_name_caps, *result_data->error_number);
    FeatureAddressPrint("%s\n", remote_feature_addr);
  }
}

static void HandleCmdSetFailsafeDuration(
    const EgLpCli*           self,
    const EntityAddressType* entity_addr,
    const char* const*       tokens,
    size_t                   num_tokens
) {
  if (num_tokens != 4) {
    printf("Insufficient arguments for %s set failsafe_duration command\n", self->cmd_name);
    return;
  }

  DurationType duration = {0};
  if (EebusDurationParse(tokens[3], &duration) != kEebusErrorOk) {
    printf("%s invalid value for Failsafe Duration: %s\n", self->cmd_name_caps, tokens[3]);
    return;
  }

  if (EgLpSetFailsafeDurationMinimum(
          self->eg_lp,
          entity_addr,
          &duration,
          OnSetFailsafeDurationResult,
          (void*)self->cmd_name_caps
      )
      != kEebusErrorOk) {
    printf("%s setting Failsafe Duration failed\n", self->cmd_name_caps);
    return;
  }

  printf("%s setting Failsafe Duration succeeded\n", self->cmd_name_caps);
}

static void HandleCmdSet(
    const EgLpCli*           self,
    const EntityAddressType* entity_addr,
    const char* const*       tokens,
    size_t                   num_tokens
) {
  if (num_tokens < 3) {
    printf("Insufficient arguments for %s set command\n", self->cmd_name);
    return;
  }

  const char* subcommand = tokens[2];
  if (strcmp(subcommand, "power_limit") == 0) {
    HandleCmdSetPowerLimit(self, entity_addr, tokens, num_tokens);
  } else if (strcmp(subcommand, "failsafe_limit") == 0) {
    HandleCmdSetFailsafeLimit(self, entity_addr, tokens, num_tokens);
  } else if (strcmp(subcommand, "failsafe_duration") == 0) {
    HandleCmdSetFailsafeDuration(self, entity_addr, tokens, num_tokens);
  } else {
    printf("Unknown subcommand for %s set: %s\n", self->cmd_name, subcommand);
  }
}

//-------------------------------------------------------------------------------------------//
//
// EG LP Start/Stop Handling
//
//-------------------------------------------------------------------------------------------//
static void HandleCmdStart(const EgLpCli* self, const char* const* tokens, size_t num_tokens) {
  if (num_tokens < 3) {
    printf("Insufficient arguments for %s start command\n", self->cmd_name);
    return;
  }

  const char* const subcommand = tokens[2];
  if (strcmp(subcommand, "heartbeat") == 0) {
    EgLpStartHeartbeat(self->eg_lp);
    printf("%s heartbeat started\n", self->cmd_name_caps);
  } else {
    printf("Unknown subcommand for %s start: %s\n", self->cmd_name, subcommand);
  }
}

static void HandleCmdStop(const EgLpCli* self, const char* const* tokens, size_t num_tokens) {
  if (num_tokens < 3) {
    printf("Insufficient arguments for %s stop command\n", self->cmd_name);
    return;
  }

  const char* const subcommand = tokens[2];
  if (strcmp(subcommand, "heartbeat") == 0) {
    EgLpStopHeartbeat(self->eg_lp);
    printf("%s heartbeat stopped\n", self->cmd_name_caps);
  } else {
    printf("Unknown subcommand for %s stop: %s\n", self->cmd_name, subcommand);
  }
}

static void HandleCmd(const EebusCliHandlerObject* self, const char* const* tokens, size_t num_tokens) {
  const EgLpCli* const eg_lp_cli = EG_LP_CLI(self);

  if (num_tokens < 2) {
    printf("Insufficient arguments for %s command\n", eg_lp_cli->cmd_name);
    return;
  }

  if (strcmp(tokens[1], "list") == 0) {
    HandleCmdList(eg_lp_cli);
    return;
  }

  // start/stop commands don't target a remote address — dispatch before --remote extraction
  if (strcmp(tokens[1], "start") == 0) {
    HandleCmdStart(eg_lp_cli, tokens, num_tokens);
    return;
  }
  if (strcmp(tokens[1], "stop") == 0) {
    HandleCmdStop(eg_lp_cli, tokens, num_tokens);
    return;
  }

  const char* adjusted[10];
  size_t      adjusted_count                 = 0;
  const EntityAddressType* const remote_addr = CliExtractRemoteArg(
      tokens, num_tokens, eg_lp_cli->addr_list, eg_lp_cli->cmd_name, adjusted, &adjusted_count
  );
  if (remote_addr == NULL) {
    return;
  }

  if (adjusted_count < 2) {
    printf("Insufficient arguments for %s command\n", eg_lp_cli->cmd_name);
    return;
  }

  if (strcmp(adjusted[1], "set") == 0) {
    HandleCmdSet(eg_lp_cli, remote_addr, adjusted, adjusted_count);
  } else if (strcmp(adjusted[1], "get") == 0) {
    HandleCmdGet(eg_lp_cli, remote_addr, adjusted, adjusted_count);
  } else {
    printf("Unknown subcommand for %s: %s\n", eg_lp_cli->cmd_name, adjusted[1]);
  }
}
