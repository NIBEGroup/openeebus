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
 * @brief EEBUS CLI GCP MGCP commands handling implementation
 */

#include <stdio.h>
#include <string.h>

#include "src/cli/eebus_cli_gcp_mgcp.h"
#include "src/use_case/actor/gcp/mgcp/gcp_mgcp.h"
#include "src/use_case/model/mgcp_types.h"
#include "src/use_case/model/scaled_value.h"

typedef struct GcpMgcpCli GcpMgcpCli;

struct GcpMgcpCli {
    /** Implements the Eebus Cli Handler Interface */
    EebusCliHandlerObject obj;

    /** GCP MGCP instance to deal with */
    GcpMgcpUseCaseObject* gcp_mgcp;
};

#define GCP_MGCP_CLI(obj) ((GcpMgcpCli*)(obj))

static void Destruct(EebusCliHandlerObject* self);
static void HandleCmd(const EebusCliHandlerObject* self, const char* const* tokens, size_t num_tokens);

static const EebusCliHandlerInterface gcp_mgcp_cli_methods = {
    .destruct   = Destruct,
    .handle_cmd = HandleCmd,
};

static EebusError GcpMgcpCliConstruct(GcpMgcpCli* self, GcpMgcpUseCaseObject* gcp_mgcp);

static void HandleCmdSet(const GcpMgcpCli* self, const char* const* tokens, size_t num_tokens);
static void HandleCmdGet(const GcpMgcpCli* self, const char* const* tokens, size_t num_tokens);

static EebusError GcpMgcpCliConstruct(GcpMgcpCli* self, GcpMgcpUseCaseObject* gcp_mgcp) {
    EEBUS_CLI_HANDLER_INTERFACE(self) = &gcp_mgcp_cli_methods;

    self->gcp_mgcp = gcp_mgcp;
    if (gcp_mgcp == NULL) {
        return kEebusErrorInputArgumentNull;
    }

    return kEebusErrorOk;
}

EebusCliHandlerObject* GcpMgcpCliCreate(GcpMgcpUseCaseObject* gcp_mgcp) {
    GcpMgcpCli* const gcp_mgcp_cli = (GcpMgcpCli*)EEBUS_MALLOC(sizeof(GcpMgcpCli));
    if (gcp_mgcp_cli == NULL) {
        return NULL;
    }

    if (GcpMgcpCliConstruct(gcp_mgcp_cli, gcp_mgcp) != kEebusErrorOk) {
        GcpMgcpCliDelete(EEBUS_CLI_HANDLER_OBJECT(gcp_mgcp_cli));
        return NULL;
    }

    return EEBUS_CLI_HANDLER_OBJECT(gcp_mgcp_cli);
}

static void Destruct(EebusCliHandlerObject* self) {
    // Nothing to deallocate
    (void)self;
}

//-------------------------------------------------------------------------------------------//
//
// GCP MGCP Setters Handling
//
//-------------------------------------------------------------------------------------------//
static void HandleCmdSet(const GcpMgcpCli* self, const char* const* tokens, size_t num_tokens) {
    if (num_tokens != 4) {
        printf("Insufficient arguments for gcp_mgcp set command\n");
        return;
    }

    const char* const name = tokens[2];

    if (strcmp(name, "pv_curtailment_limit_factor") == 0) {
        ScaledValue value = {0};
        if (ScaledValueParse(tokens[3], &value) != kEebusErrorOk) {
            printf("Parsing GCP MGCP pv_curtailment_limit_factor value failed\n");
            return;
        }

        if (GcpMgcpSetPvCurtailmentLimitFactor(self->gcp_mgcp, &value) != kEebusErrorOk) {
            printf("Setting GCP MGCP pv_curtailment_limit_factor failed\n");
            return;
        }

        printf("Setting GCP MGCP pv_curtailment_limit_factor succeeded\n");
        return;
    }

    const GcpMeasurementNameId* const name_id = GcpMgcpMeasurementGetNameId(name);
    if (name_id == NULL) {
        printf("Unknown measurement name for gcp_mgcp set: %s\n", name);
        return;
    }

    ScaledValue value = {0};
    if (ScaledValueParse(tokens[3], &value) != kEebusErrorOk) {
        printf("Parsing GCP MGCP measurement value failed\n");
        return;
    }

    if (GcpMgcpSetMeasurementDataCache(self->gcp_mgcp, *name_id, &value, NULL, NULL) != kEebusErrorOk) {
        printf("Setting GCP MGCP measurement cache failed\n");
        return;
    }

    if (GcpMgcpUpdate(self->gcp_mgcp) != kEebusErrorOk) {
        printf("Updating GCP MGCP failed\n");
        return;
    }

    printf("Setting GCP MGCP measurement data succeeded\n");
}

//-------------------------------------------------------------------------------------------//
//
// GCP MGCP Getters Handling
//
//-------------------------------------------------------------------------------------------//
static void HandleCmdGet(const GcpMgcpCli* self, const char* const* tokens, size_t num_tokens) {
    if (num_tokens != 3) {
        printf("Insufficient arguments for gcp_mgcp get command\n");
        return;
    }

    const char* const name = tokens[2];

    if (strcmp(name, "pv_curtailment_limit_factor") == 0) {
        ScaledValue value = {0};
        if (GcpMgcpGetPvCurtailmentLimitFactor(self->gcp_mgcp, &value) != kEebusErrorOk) {
            printf("Getting GCP MGCP pv_curtailment_limit_factor failed\n");
            return;
        }

        printf("GCP MGCP pv_curtailment_limit_factor: ");
        ScaledValuePrint("value=%s\n", &value);
        return;
    }

    const GcpMeasurementNameId* const name_id = GcpMgcpMeasurementGetNameId(name);
    if (name_id == NULL) {
        printf("Unknown measurement name for gcp_mgcp get: %s\n", name);
        return;
    }

    ScaledValue value = {0};
    if (GcpMgcpGetMeasurementData(self->gcp_mgcp, *name_id, &value) != kEebusErrorOk) {
        printf("Getting GCP MGCP measurement value failed\n");
        return;
    }

    printf("GCP MGCP measurement %s: ", name);
    ScaledValuePrint("value=%s\n", &value);
}

static void HandleCmd(const EebusCliHandlerObject* self, const char* const* tokens, size_t num_tokens) {
    const GcpMgcpCli* const gcp_mgcp_cli = GCP_MGCP_CLI(self);

    if (num_tokens < 2) {
        printf("Insufficient arguments for gcp_mgcp command\n");
        return;
    }

    if (strcmp(tokens[1], "set") == 0) {
        HandleCmdSet(gcp_mgcp_cli, tokens, num_tokens);
    } else if (strcmp(tokens[1], "get") == 0) {
        HandleCmdGet(gcp_mgcp_cli, tokens, num_tokens);
    } else {
        printf("Unknown subcommand for gcp_mgcp: %s\n", tokens[1]);
    }
}
