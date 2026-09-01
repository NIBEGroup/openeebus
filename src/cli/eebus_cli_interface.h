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
 * @brief EEBUS CLI interface declarations
 */

#ifndef SRC_CLI_EEBUS_CLI_INTERFACE_H_
#define SRC_CLI_EEBUS_CLI_INTERFACE_H_

#include "src/common/eebus_malloc.h"
#include "src/spine/model/entity_types.h"
#include "src/use_case/actor/cs/cs_lp.h"
#include "src/use_case/actor/eg/eg_lp.h"
#include "src/use_case/actor/gcp/mgcp/gcp_mgcp.h"
#include "src/use_case/actor/ma/mgcp/ma_mgcp.h"
#include "src/use_case/actor/ma/mpc/ma_mpc.h"
#include "src/use_case/actor/mu/mpc/mu_mpc.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @brief EEBUS CLI Interface
 * (EEBUS CLI "virtual functions table" declaration)
 */
typedef struct EebusCliInterface EebusCliInterface;

/**
 * @brief EEBUS CLI Object type definition
 * ("abstract class", has no members but only pointer to
 * "virtual functions table")
 */
typedef struct EebusCliObject EebusCliObject;

/**
 * @brief EEBUS CLI Interface Structure
 */
struct EebusCliInterface {
    /**
     * @brief EEBUS CLI Destructor
     * @param self Pointer to the EEBUS CLI handler instance to be destructed
     */
    void (*destruct)(EebusCliObject* self);
    /**
     * @brief Set the CS LPC use case instance to be used by the CLI handler
     * @param self Pointer to the EEBUS CLI handler instance
     * @param cs_lpc_use_case CS LPC use case instance to be used by the CLI handler
     */
    void (*set_cs_lpc)(EebusCliObject* self, CsLpUseCaseObject* cs_lpc_use_case);
    /**
     * @brief Set the CS LPP use case instance to be used by the CLI handler
     * @param self Pointer to the EEBUS CLI handler instance
     * @param cs_lpp_use_case CS LPP use case instance to be used by the CLI handler
     */
    void (*set_cs_lpp)(EebusCliObject* self, CsLpUseCaseObject* cs_lpp_use_case);
    /**
     * @brief Set the EG LPC use case instance to be used by the CLI handler
     * @param self Pointer to the EEBUS CLI handler instance
     * @param eg_lpc_use_case EG LPC use case instance to be used by the CLI handler
     * @param remote_entity_address EG LPC remote entity address to be used by the CLI handler
     */
    void (*set_eg_lpc)(
        EebusCliObject* self,
        EgLpUseCaseObject* eg_lpc_use_case,
        const EntityAddressType* remote_entity_address
    );
    /**
     * @brief Set the EG LPP use case instance to be used by the CLI handler
     * @param self Pointer to the EEBUS CLI handler instance
     * @param eg_lpp_use_case EG LPP use case instance to be used by the CLI handler
     * @param remote_entity_address EG LPP remote entity address to be used by the CLI handler
     */
    void (*set_eg_lpp)(
        EebusCliObject* self,
        EgLpUseCaseObject* eg_lpp_use_case,
        const EntityAddressType* remote_entity_address
    );
    /**
     * @brief Set the MU MPC use case instance to be used by the CLI handler
     * @param self Pointer to the EEBUS CLI handler instance
     * @param mu_mpc_use_case MU MPC use case instance to be used by the CLI handler
     */
    void (*set_mu_mpc)(EebusCliObject* self, MuMpcUseCaseObject* mu_mpc_use_case);
    /**
     * @brief Set the MA MPC use case instance to be used by the CLI handler
     * @param self Pointer to the EEBUS CLI handler instance
     * @param ma_mpc_use_case MA MPC use case instance to be used by the CLI handler
     * @param remote_entity_address MA MPC remote entity address to be used by the CLI handler
     */
    void (*set_ma_mpc)(
        EebusCliObject* self,
        MaMpcUseCaseObject* ma_mpc_use_case,
        const EntityAddressType* remote_entity_address
    );
    /**
     * @brief Set the GCP MGCP use case instance to be used by the CLI handler
     * @param self Pointer to the EEBUS CLI handler instance
     * @param gcp_mgcp_use_case GCP MGCP use case instance to be used by the CLI handler
     */
    void (*set_gcp_mgcp)(EebusCliObject* self, GcpMgcpUseCaseObject* gcp_mgcp_use_case);
    /**
     * @brief Set the MA MGCP use case instance to be used by the CLI handler
     * @param self Pointer to the EEBUS CLI handler instance
     * @param ma_mgcp_use_case MA MGCP use case instance to be used by the CLI handler
     * @param remote_entity_address MA MGCP remote entity address to be used by the CLI handler
     */
    void (*set_ma_mgcp)(
        EebusCliObject* self,
        MaMgcpUseCaseObject* ma_mgcp_use_case,
        const EntityAddressType* remote_entity_address
    );
    /**
     * @brief Handle the command passed as a string
     * @param self Pointer to the EEBUS CLI handler instance
     * @param cmd Command string to be handled
     * @note The command string is modified during the processing
     */
    void (*handle_cmd)(const EebusCliObject* self, char* cmd);
};

/**
 * @brief EEBUS CLI Object Structure
 */
struct EebusCliObject {
    const EebusCliInterface* interface_;
};

/**
 * @brief EEBUS CLI pointer typecast
 */
#define EEBUS_CLI_OBJECT(obj) ((EebusCliObject*)(obj))

/**
 * @brief EEBUS CLI Interface class pointer typecast
 */
#define EEBUS_CLI_INTERFACE(obj) (EEBUS_CLI_OBJECT(obj)->interface_)

/**
 * @brief EEBUS CLI Destruct caller definition
 */
#define EEBUS_CLI_DESTRUCT(obj) (EEBUS_CLI_INTERFACE(obj)->destruct(obj))

/**
 * @brief EEBUS CLI Set CS LPC caller definition
 */
#define EEBUS_CLI_SET_CS_LPC(obj, cs_lpc_use_case) (EEBUS_CLI_INTERFACE(obj)->set_cs_lpc(obj, cs_lpc_use_case))

/**
 * @brief EEBUS CLI Set CS LPP caller definition
 */
#define EEBUS_CLI_SET_CS_LPP(obj, cs_lpp_use_case) (EEBUS_CLI_INTERFACE(obj)->set_cs_lpp(obj, cs_lpp_use_case))

/**
 * @brief EEBUS CLI Set EG LPC caller definition
 */
#define EEBUS_CLI_SET_EG_LPC(obj, eg_lpc_use_case, remote_entity_address) \
    (EEBUS_CLI_INTERFACE(obj)->set_eg_lpc(obj, eg_lpc_use_case, remote_entity_address))

/**
 * @brief EEBUS CLI Set EG LPP caller definition
 */
#define EEBUS_CLI_SET_EG_LPP(obj, eg_lpp_use_case, remote_entity_address) \
    (EEBUS_CLI_INTERFACE(obj)->set_eg_lpp(obj, eg_lpp_use_case, remote_entity_address))

/**
 * @brief EEBUS CLI Set MU MPC caller definition
 */
#define EEBUS_CLI_SET_MU_MPC(obj, mu_mpc_use_case) (EEBUS_CLI_INTERFACE(obj)->set_mu_mpc(obj, mu_mpc_use_case))

/**
 * @brief EEBUS CLI Set Ma Mpc caller definition
 */
#define EEBUS_CLI_SET_MA_MPC(obj, ma_mpc_use_case, remote_entity_address) \
    (EEBUS_CLI_INTERFACE(obj)->set_ma_mpc(obj, ma_mpc_use_case, remote_entity_address))

/**
 * @brief EEBUS CLI Set GCP MGCP caller definition
 */
#define EEBUS_CLI_SET_GCP_MGCP(obj, gcp_mgcp_use_case) (EEBUS_CLI_INTERFACE(obj)->set_gcp_mgcp(obj, gcp_mgcp_use_case))

/**
 * @brief EEBUS CLI Set MA MGCP caller definition
 */
#define EEBUS_CLI_SET_MA_MGCP(obj, ma_mgcp_use_case, remote_entity_address) \
    (EEBUS_CLI_INTERFACE(obj)->set_ma_mgcp(obj, ma_mgcp_use_case, remote_entity_address))

/**
 * @brief EEBUS CLI Handle Cmd caller definition
 */
#define EEBUS_CLI_HANDLE_CMD(obj, cmd) (EEBUS_CLI_INTERFACE(obj)->handle_cmd(obj, cmd))

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_CLI_EEBUS_CLI_INTERFACE_H_
