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
 * @brief EEBUS CLI MU MPC commands handling implementation
 */

#include <stdio.h>
#include <string.h>

#include "src/cli/eebus_cli_mu_mpc.h"
#include "src/common/eebus_arguments.h"
#include "src/use_case/actor/mu/mpc/mu_mpc.h"

typedef struct MuMpcCli MuMpcCli;

struct MuMpcCli {
    /** Implements the Eebus Cli Handler Interface */
    EebusCliHandlerObject obj;

    /** MU MPC instance to deal with */
    MuMpcUseCaseObject* mu_mpc;
};

#define MU_MPC_CLI(obj) ((MuMpcCli*)(obj))

static void Destruct(EebusCliHandlerObject* self);
static void HandleCmd(const EebusCliHandlerObject* self, const char* const* tokens, size_t num_tokens);

static const EebusCliHandlerInterface mu_mpc_cli_methods = {
    .destruct   = Destruct,
    .handle_cmd = HandleCmd,
};

EebusError MuMpcCliConstruct(MuMpcCli* self, MuMpcUseCaseObject* mu_mpc);

static void HandleCmdSet(const MuMpcCli* self, const char* const* tokens, size_t num_tokens);
static void HandleCmdGet(const MuMpcCli* self, const char* const* tokens, size_t num_tokens);

EebusError MuMpcCliConstruct(MuMpcCli* self, MuMpcUseCaseObject* mu_mpc) {
    // Override "virtual functions table"
    EEBUS_CLI_HANDLER_INTERFACE(self) = &mu_mpc_cli_methods;

    self->mu_mpc = mu_mpc;
    if (mu_mpc == NULL) {
        return kEebusErrorInputArgumentNull;
    }

    return kEebusErrorOk;
}

EebusCliHandlerObject* MuMpcCliCreate(MuMpcUseCaseObject* mu_mpc) {
    MuMpcCli* mu_mpc_cli = (MuMpcCli*)EEBUS_MALLOC(sizeof(MuMpcCli));
    if (mu_mpc_cli == NULL) {
        return NULL;
    }

    if (MuMpcCliConstruct(mu_mpc_cli, mu_mpc) != kEebusErrorOk) {
        MuMpcCliDelete(EEBUS_CLI_HANDLER_OBJECT(mu_mpc_cli));
        return NULL;
    }

    return EEBUS_CLI_HANDLER_OBJECT(mu_mpc_cli);
}

void Destruct(EebusCliHandlerObject* self) {
    UNUSED(self);
    // Nothing to be deallocated yet
}

void HandleCmdSet(const MuMpcCli* self, const char* const* tokens, size_t num_tokens) {
    if (num_tokens != 4) {
        printf("Insufficient arguments for mu_mpc set command\n");
        return;
    }

    const char* const name = tokens[2];

    const MuMpcMeasurementNameId* name_id = MuMpcMeasurementGetNameId(name);

    if (name_id == NULL) {
        printf("Unknown measurement name for mu_mpc set: %s\n", name);
        return;
    }

    ScaledValue value = {0};
    if (ScaledValueParse(tokens[3], &value) != kEebusErrorOk) {
        printf("Parsing MU MPC measurement value failed\n");
        return;
    }

    if (MuMpcSetMeasurementDataCache(self->mu_mpc, *name_id, &value, NULL, NULL) != kEebusErrorOk) {
        printf("Setting MU MPC measurement cache failed\n");
        return;
    }

    if (MuMpcUpdate(self->mu_mpc) != kEebusErrorOk) {
        printf("Updating MU MPC failed\n");
        return;
    }

    printf("Setting MU MPC measurement data succeeded\n");
}

void HandleCmdGet(const MuMpcCli* self, const char* const* tokens, size_t num_tokens) {
    if (num_tokens != 3) {
        printf("Insufficient arguments for mu_mpc get command\n");
        return;
    }

    const char* const name = tokens[2];

    const MuMpcMeasurementNameId* const name_id = MuMpcMeasurementGetNameId(name);
    if (name_id == NULL) {
        printf("Unknown measurement name for mu_mpc get: %s\n", name);
        return;
    }

    ScaledValue value = {0};
    if (MuMpcGetMeasurementData(self->mu_mpc, *name_id, &value) != kEebusErrorOk) {
        printf("Getting MU MPC measurement value failed\n");
        return;
    }

    printf("MU MPC measurement %s: ", name);
    ScaledValuePrint("value=%s\n", &value);
}

void HandleCmd(const EebusCliHandlerObject* self, const char* const* tokens, size_t num_tokens) {
    const MuMpcCli* mu_mpc_cli = MU_MPC_CLI(self);

    if (num_tokens < 2) {
        printf("Insufficient arguments for MU MPC command\n");
        return;
    }

    if (strcmp(tokens[1], "set") == 0) {
        HandleCmdSet(mu_mpc_cli, tokens, num_tokens);
    } else if (strcmp(tokens[1], "get") == 0) {
        HandleCmdGet(mu_mpc_cli, tokens, num_tokens);
    } else {
        printf("Unknown subcommand for MU MPC: %s\n", tokens[1]);
    }
}
