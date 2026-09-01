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
#include "src/use_case/model/mgcp_types.h"
#include "src/use_case/model/scaled_value.h"

typedef struct MaMgcpCli MaMgcpCli;

struct MaMgcpCli {
    /** Implements the Eebus Cli Handler Interface */
    EebusCliHandlerObject obj;

    /** MA MGCP instance to deal with */
    MaMgcpUseCaseObject* ma_mgcp;
    /** MA MGCP remote entity address to communicate with */
    const EntityAddressType* entity_addr;
};

#define MA_MGCP_CLI(obj) ((MaMgcpCli*)(obj))

static void Destruct(EebusCliHandlerObject* self);
static void HandleCmd(const EebusCliHandlerObject* self, const char* const* tokens, size_t num_tokens);

static const EebusCliHandlerInterface ma_mgcp_cli_methods = {
    .destruct   = Destruct,
    .handle_cmd = HandleCmd,
};

static EebusError
MaMgcpCliConstruct(MaMgcpCli* self, MaMgcpUseCaseObject* ma_mgcp, const EntityAddressType* entity_addr);

static void HandleCmdGet(const MaMgcpCli* self, const char* const* tokens, size_t num_tokens);

static EebusError
MaMgcpCliConstruct(MaMgcpCli* self, MaMgcpUseCaseObject* ma_mgcp, const EntityAddressType* entity_addr) {
    EEBUS_CLI_HANDLER_INTERFACE(self) = &ma_mgcp_cli_methods;

    self->ma_mgcp     = ma_mgcp;
    self->entity_addr = NULL;

    if ((ma_mgcp == NULL) || (entity_addr == NULL)) {
        return kEebusErrorInputArgumentNull;
    }

    self->entity_addr = EntityAddressCopy(entity_addr);
    if (self->entity_addr == NULL) {
        return kEebusErrorMemoryAllocate;
    }

    return kEebusErrorOk;
}

EebusCliHandlerObject* MaMgcpCliCreate(MaMgcpUseCaseObject* ma_mgcp, const EntityAddressType* entity_addr) {
    MaMgcpCli* const ma_mgcp_cli = (MaMgcpCli*)EEBUS_MALLOC(sizeof(MaMgcpCli));
    if (ma_mgcp_cli == NULL) {
        return NULL;
    }

    if (MaMgcpCliConstruct(ma_mgcp_cli, ma_mgcp, entity_addr) != kEebusErrorOk) {
        MaMgcpCliDelete(EEBUS_CLI_HANDLER_OBJECT(ma_mgcp_cli));
        return NULL;
    }

    return EEBUS_CLI_HANDLER_OBJECT(ma_mgcp_cli);
}

static void Destruct(EebusCliHandlerObject* self) {
    MaMgcpCli* const ma_mgcp_cli = MA_MGCP_CLI(self);

    EntityAddressDelete((EntityAddressType*)ma_mgcp_cli->entity_addr);
    ma_mgcp_cli->entity_addr = NULL;
}

//-------------------------------------------------------------------------------------------//
//
// MA MGCP Getters Handling
//
//-------------------------------------------------------------------------------------------//
static void HandleCmdGet(const MaMgcpCli* self, const char* const* tokens, size_t num_tokens) {
    if (num_tokens != 3) {
        printf("Insufficient arguments for ma_mgcp get command\n");
        return;
    }

    const char* const name = tokens[2];

    if (strcmp(name, "pv_curtailment_limit_factor") == 0) {
        ScaledValue value = {0};
        if (MaMgcpGetPvCurtailmentLimitFactor(self->ma_mgcp, self->entity_addr, &value) != kEebusErrorOk) {
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
    if (MaMgcpGetMeasurementData(self->ma_mgcp, *name_id, self->entity_addr, &value) != kEebusErrorOk) {
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

    if (strcmp(tokens[1], "get") == 0) {
        HandleCmdGet(ma_mgcp_cli, tokens, num_tokens);
    } else {
        printf("Unknown subcommand for ma_mgcp: %s\n", tokens[1]);
    }
}
