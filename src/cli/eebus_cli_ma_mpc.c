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
#include "src/use_case/model/scaled_value.h"

typedef struct MaMpcCli MaMpcCli;

struct MaMpcCli {
    /** Implements the Eebus Cli Handler Interface */
    EebusCliHandlerObject obj;

    /** MA MPC instance to deal with */
    MaMpcUseCaseObject* ma_mpc;
    /** MA MPC remote entity address to communicate with */
    const EntityAddressType* entity_addr;
};

#define MA_MPC_CLI(obj) ((MaMpcCli*)(obj))

static void Destruct(EebusCliHandlerObject* self);
static void HandleCmd(const EebusCliHandlerObject* self, const char* const* tokens, size_t num_tokens);

static const EebusCliHandlerInterface ma_mpc_cli_methods = {
    .destruct   = Destruct,
    .handle_cmd = HandleCmd,
};

static EebusError MaMpcCliConstruct(MaMpcCli* self, MaMpcUseCaseObject* ma_mpc, const EntityAddressType* entity_addr);

static void HandleCmdMaMpcGet(const MaMpcCli* self, const char* const* tokens, size_t num_tokens);

EebusError MaMpcCliConstruct(MaMpcCli* self, MaMpcUseCaseObject* ma_mpc, const EntityAddressType* entity_addr) {
    // Override "virtual functions table"
    EEBUS_CLI_HANDLER_INTERFACE(self) = &ma_mpc_cli_methods;

    self->ma_mpc      = ma_mpc;
    self->entity_addr = NULL;

    if ((ma_mpc == NULL) || (entity_addr == NULL)) {
        return kEebusErrorInputArgumentNull;
    }

    self->entity_addr = EntityAddressCopy(entity_addr);
    if (self->entity_addr == NULL) {
        return kEebusErrorMemoryAllocate;
    }

    return kEebusErrorOk;
}

EebusCliHandlerObject* MaMpcCliCreate(MaMpcUseCaseObject* ma_mpc, const EntityAddressType* entity_addr) {
    MaMpcCli* ma_mpc_cli = (MaMpcCli*)EEBUS_MALLOC(sizeof(MaMpcCli));
    if (ma_mpc_cli == NULL) {
        return NULL;
    }

    if (MaMpcCliConstruct(ma_mpc_cli, ma_mpc, entity_addr) != kEebusErrorOk) {
        MaMpcCliDelete(EEBUS_CLI_HANDLER_OBJECT(ma_mpc_cli));
        return NULL;
    }

    return EEBUS_CLI_HANDLER_OBJECT(ma_mpc_cli);
}

void Destruct(EebusCliHandlerObject* self) {
    MaMpcCli* ma_mpc_cli = MA_MPC_CLI(self);

    EntityAddressDelete((EntityAddressType*)ma_mpc_cli->entity_addr);
    ma_mpc_cli->entity_addr = NULL;
}

//-------------------------------------------------------------------------------------------//
//
// MA MPC Getters Handling
//
//-------------------------------------------------------------------------------------------//
void HandleCmdMaMpcGet(const MaMpcCli* self, const char* const* tokens, size_t num_tokens) {
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
    if (MaMpcGetMeasurementData(self->ma_mpc, *name_id, self->entity_addr, &value) != kEebusErrorOk) {
        printf("Getting MA MPC measurement value failed\n");
        return;
    }

    printf("MA MPC measurement %s: ", name);
    ScaledValuePrint("value=%s\n", &value);
}

void HandleCmd(const EebusCliHandlerObject* self, const char* const* tokens, size_t num_tokens) {
    const MaMpcCli* const ma_mpc_cli = MA_MPC_CLI(self);

    if (num_tokens < 2) {
        printf("Insufficient arguments for ma_mpc command\n");
        return;
    }

    if (strcmp(tokens[1], "get") == 0) {
        HandleCmdMaMpcGet(ma_mpc_cli, tokens, num_tokens);
    } else {
        printf("Unknown subcommand for ma_mpc: %s\n", tokens[1]);
    }
}
