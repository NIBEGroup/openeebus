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
 * @brief EEBUS CLI implementation
 */

#include <stdio.h>

#include "src/cli/eebus_cli.h"
#include "src/cli/eebus_cli_cs_lp.h"
#include "src/cli/eebus_cli_eg_lp.h"
#include "src/cli/eebus_cli_gcp_mgcp.h"
#include "src/cli/eebus_cli_ma_mgcp.h"
#include "src/cli/eebus_cli_ma_mpc.h"
#include "src/cli/eebus_cli_mu_mpc.h"
#include "src/common/array_util.h"
#include "src/common/eebus_errors.h"
#include "src/common/eebus_malloc.h"
#include "src/common/string_util.h"

typedef struct EebusCli EebusCli;

struct EebusCli {
    /** Implements the EEBUS CLI Interface */
    EebusCliObject obj;

    /** CS LPC CLI instance to deal with */
    EebusCliHandlerObject* cs_lpc_cli;
    /** CS LPP CLI instance to deal with */
    EebusCliHandlerObject* cs_lpp_cli;
    /** EG LPC CLI instance to deal with */
    EebusCliHandlerObject* eg_lpc_cli;
    /** EG LPP CLI instance to deal with */
    EebusCliHandlerObject* eg_lpp_cli;
    /** MA MPC CLI instance to deal with */
    EebusCliHandlerObject* ma_mpc_cli;
    /** MU MPC CLI instance to deal with */
    EebusCliHandlerObject* mu_mpc_cli;
    /** GCP MGCP CLI instance to deal with */
    EebusCliHandlerObject* gcp_mgcp_cli;
    /** MA MGCP CLI instance to deal with */
    EebusCliHandlerObject* ma_mgcp_cli;
};

#define EEBUS_CLI(obj) ((EebusCli*)(obj))

static void Destruct(EebusCliObject* self);
static void SetCsLpc(EebusCliObject* self, CsLpUseCaseObject* cs_lpc_use_case);
static void SetCsLpp(EebusCliObject* self, CsLpUseCaseObject* cs_lpp_use_case);
static void
SetEgLpc(EebusCliObject* self, EgLpUseCaseObject* eg_lpc_use_case, const EntityAddressType* remote_entity_address);
static void SetMuMpc(EebusCliObject* self, MuMpcUseCaseObject* mu_mpc_use_case);
static void
SetEgLpp(EebusCliObject* self, EgLpUseCaseObject* eg_lpp_use_case, const EntityAddressType* remote_entity_address);
static void
SetMaMpc(EebusCliObject* self, MaMpcUseCaseObject* ma_mpc_use_case, const EntityAddressType* remote_entity_address);
static void SetGcpMgcp(EebusCliObject* self, GcpMgcpUseCaseObject* gcp_mgcp_use_case);
static void
SetMaMgcp(EebusCliObject* self, MaMgcpUseCaseObject* ma_mgcp_use_case, const EntityAddressType* remote_entity_address);
static void HandleCmd(const EebusCliObject* self, char* cmd);

static const EebusCliInterface eebus_cli_methods = {
    .destruct     = Destruct,
    .set_cs_lpc   = SetCsLpc,
    .set_cs_lpp   = SetCsLpp,
    .set_eg_lpc   = SetEgLpc,
    .set_eg_lpp   = SetEgLpp,
    .set_mu_mpc   = SetMuMpc,
    .set_ma_mpc   = SetMaMpc,
    .set_gcp_mgcp = SetGcpMgcp,
    .set_ma_mgcp  = SetMaMgcp,
    .handle_cmd   = HandleCmd,
};

static EebusError EebusCliConstruct(EebusCli* self);

EebusError EebusCliConstruct(EebusCli* self) {
    // Override "virtual functions table"
    EEBUS_CLI_INTERFACE(self) = &eebus_cli_methods;

    self->cs_lpc_cli   = NULL;
    self->cs_lpp_cli   = NULL;
    self->eg_lpc_cli   = NULL;
    self->eg_lpp_cli   = NULL;
    self->mu_mpc_cli   = NULL;
    self->ma_mpc_cli   = NULL;
    self->gcp_mgcp_cli = NULL;
    self->ma_mgcp_cli  = NULL;

    return kEebusErrorOk;
}

EebusCliObject* EebusCliCreate(void) {
    EebusCli* const eebus_cli = (EebusCli*)EEBUS_MALLOC(sizeof(EebusCli));
    if (eebus_cli == NULL) {
        return NULL;
    }

    if (EebusCliConstruct(eebus_cli) != kEebusErrorOk) {
        EebusCliDelete(EEBUS_CLI_OBJECT(eebus_cli));
        return NULL;
    }

    return EEBUS_CLI_OBJECT(eebus_cli);
}

void Destruct(EebusCliObject* self) {
    EebusCli* const eebus_cli = EEBUS_CLI(self);

    MaMgcpCliDelete(eebus_cli->ma_mgcp_cli);
    eebus_cli->ma_mgcp_cli = NULL;

    GcpMgcpCliDelete(eebus_cli->gcp_mgcp_cli);
    eebus_cli->gcp_mgcp_cli = NULL;

    MaMpcCliDelete(eebus_cli->ma_mpc_cli);
    eebus_cli->ma_mpc_cli = NULL;

    MuMpcCliDelete(eebus_cli->mu_mpc_cli);
    eebus_cli->mu_mpc_cli = NULL;

    EgLpCliDelete(eebus_cli->eg_lpp_cli);
    eebus_cli->eg_lpp_cli = NULL;

    EgLpCliDelete(eebus_cli->eg_lpc_cli);
    eebus_cli->eg_lpc_cli = NULL;

    CsLpCliDelete(eebus_cli->cs_lpp_cli);
    eebus_cli->cs_lpp_cli = NULL;

    CsLpCliDelete(eebus_cli->cs_lpc_cli);
    eebus_cli->cs_lpc_cli = NULL;
}

void SetCsLpc(EebusCliObject* self, CsLpUseCaseObject* cs_lpc_use_case) {
    EebusCli* const eebus_cli = EEBUS_CLI(self);

    // Free the existing CLI instance before creating a new one — this function may be
    // called multiple times (e.g. on use case re-registration), so the old instance
    // must be released first to prevent a memory leak.
    CsLpCliDelete(eebus_cli->cs_lpc_cli);
    eebus_cli->cs_lpc_cli = CsLpCliCreate(kEnergyDirectionTypeConsume, cs_lpc_use_case);
}

void SetCsLpp(EebusCliObject* self, CsLpUseCaseObject* cs_lpp_use_case) {
    EebusCli* const eebus_cli = EEBUS_CLI(self);

    // Free the existing CLI instance before creating a new one — this function may be
    // called multiple times (e.g. on use case re-registration), so the old instance
    // must be released first to prevent a memory leak.
    CsLpCliDelete(eebus_cli->cs_lpp_cli);
    eebus_cli->cs_lpp_cli = CsLpCliCreate(kEnergyDirectionTypeProduce, cs_lpp_use_case);
}

void SetEgLpc(
    EebusCliObject* self,
    EgLpUseCaseObject* eg_lpc_use_case,
    const EntityAddressType* remote_entity_address
) {
    EebusCli* const eebus_cli = EEBUS_CLI(self);

    // Always tear down the existing CLI instance — this function may be called
    // multiple times (e.g. on reconnection), so the old instance must be released
    // first to prevent a memory leak. A NULL remote address signals disconnection,
    // in which case no new instance will be created below.
    EgLpCliDelete(eebus_cli->eg_lpc_cli);
    eebus_cli->eg_lpc_cli = NULL;

    // Create a new CLI instance if remote entity address is not NULL
    if (remote_entity_address != NULL) {
        eebus_cli->eg_lpc_cli = EgLpCliCreate(kEnergyDirectionTypeConsume, eg_lpc_use_case, remote_entity_address);
    }
}

static void SetMuMpc(EebusCliObject* self, MuMpcUseCaseObject* mu_mpc_use_case) {
    EebusCli* const eebus_cli = EEBUS_CLI(self);

    // Free the existing CLI instance before creating a new one — this function may be
    // called multiple times (e.g. on use case re-registration), so the old instance
    // must be released first to prevent a memory leak.
    MuMpcCliDelete(eebus_cli->mu_mpc_cli);
    eebus_cli->mu_mpc_cli = MuMpcCliCreate(mu_mpc_use_case);
}

void SetEgLpp(
    EebusCliObject* self,
    EgLpUseCaseObject* eg_lpp_use_case,
    const EntityAddressType* remote_entity_address
) {
    EebusCli* const eebus_cli = EEBUS_CLI(self);

    // Always tear down the existing CLI instance — this function may be called
    // multiple times (e.g. on reconnection), so the old instance must be released
    // first to prevent a memory leak. A NULL remote address signals disconnection,
    // in which case no new instance will be created below.
    EgLpCliDelete(eebus_cli->eg_lpp_cli);
    eebus_cli->eg_lpp_cli = NULL;

    // Create a new CLI instance if remote entity address is not NULL
    if (remote_entity_address != NULL) {
        eebus_cli->eg_lpp_cli = EgLpCliCreate(kEnergyDirectionTypeProduce, eg_lpp_use_case, remote_entity_address);
    }
}

void SetMaMpc(
    EebusCliObject* self,
    MaMpcUseCaseObject* ma_mpc_use_case,
    const EntityAddressType* remote_entity_address
) {
    EebusCli* const eebus_cli = EEBUS_CLI(self);

    // Always tear down the existing CLI instance — this function may be called
    // multiple times (e.g. on reconnection), so the old instance must be released
    // first to prevent a memory leak. A NULL remote address signals disconnection,
    // in which case no new instance will be created below.
    MaMpcCliDelete(eebus_cli->ma_mpc_cli);
    eebus_cli->ma_mpc_cli = NULL;

    // Create a new CLI instance if remote entity address is not NULL
    if (remote_entity_address != NULL) {
        eebus_cli->ma_mpc_cli = MaMpcCliCreate(ma_mpc_use_case, remote_entity_address);
    }
}

static void SetGcpMgcp(EebusCliObject* self, GcpMgcpUseCaseObject* gcp_mgcp_use_case) {
    EebusCli* const eebus_cli = EEBUS_CLI(self);

    // Free the existing CLI instance before creating a new one — this function may be
    // called multiple times (e.g. on use case re-registration), so the old instance
    // must be released first to prevent a memory leak.
    GcpMgcpCliDelete(eebus_cli->gcp_mgcp_cli);
    eebus_cli->gcp_mgcp_cli = GcpMgcpCliCreate(gcp_mgcp_use_case);
}

static void
SetMaMgcp(EebusCliObject* self, MaMgcpUseCaseObject* ma_mgcp_use_case, const EntityAddressType* remote_entity_address) {
    EebusCli* const eebus_cli = EEBUS_CLI(self);

    // Always tear down the existing CLI instance — this function may be called
    // multiple times (e.g. on reconnection), so the old instance must be released
    // first to prevent a memory leak. A NULL remote address signals disconnection,
    // in which case no new instance will be created below.
    MaMgcpCliDelete(eebus_cli->ma_mgcp_cli);
    eebus_cli->ma_mgcp_cli = NULL;

    // Create a new CLI instance if remote entity address is not NULL
    if (remote_entity_address != NULL) {
        eebus_cli->ma_mgcp_cli = MaMgcpCliCreate(ma_mgcp_use_case, remote_entity_address);
    }
}

void HandleCmd(const EebusCliObject* self, char* cmd) {
    const EebusCli* const eebus_cli = EEBUS_CLI(self);

    static const char delimiters[] = " \t\n";

    const char* tokens[10] = {NULL};
    size_t num_tokens      = 0;
    char* p                = NULL;

    for (char* token = StringToken(cmd, delimiters, &p); token != NULL; token = StringToken(NULL, delimiters, &p)) {
        if (num_tokens >= ARRAY_SIZE(tokens)) {
            printf("Too many arguments specified!\n");
            return;
        }

        tokens[num_tokens++] = token;
    }

    if (tokens[0] == NULL) {
        return;
    }

    const EebusCliHandlerObject* handler = NULL;
    if (strcmp(tokens[0], "cs_lpc") == 0) {
        handler = eebus_cli->cs_lpc_cli;
    } else if (strcmp(tokens[0], "cs_lpp") == 0) {
        handler = eebus_cli->cs_lpp_cli;
    } else if (strcmp(tokens[0], "eg_lpc") == 0) {
        handler = eebus_cli->eg_lpc_cli;
    } else if (strcmp(tokens[0], "eg_lpp") == 0) {
        handler = eebus_cli->eg_lpp_cli;
    } else if (strcmp(tokens[0], "mu_mpc") == 0) {
        handler = eebus_cli->mu_mpc_cli;
    } else if (strcmp(tokens[0], "ma_mpc") == 0) {
        handler = eebus_cli->ma_mpc_cli;
    } else if (strcmp(tokens[0], "gcp_mgcp") == 0) {
        handler = eebus_cli->gcp_mgcp_cli;
    } else if (strcmp(tokens[0], "ma_mgcp") == 0) {
        handler = eebus_cli->ma_mgcp_cli;
    } else {
        printf("Unknown command: %s\n", tokens[0]);
        return;
    }

    if (handler == NULL) {
        printf("%s use case not set in CLI handler\n", tokens[0]);
        return;
    }

    EEBUS_CLI_HANDLER_HANDLE_CMD(handler, tokens, num_tokens);
}
