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
 * @brief Smart Energy Management PS Client functionality
 */

#ifndef SRC_USE_CASE_SPECIALIZATION_SMART_ENERGY_MANAGEMENT_PS_SMART_ENERGY_MANAGEMENT_PS_CLIENT_H_
#define SRC_USE_CASE_SPECIALIZATION_SMART_ENERGY_MANAGEMENT_PS_SMART_ENERGY_MANAGEMENT_PS_CLIENT_H_

#include "src/spine/api/feature_local_interface.h"
#include "src/spine/model/loadcontrol_types.h"
#include "src/use_case/specialization/feature_info_client.h"
#include "src/use_case/specialization/smart_energy_management_ps/smart_energy_management_ps_common.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct SmartEnergyManagementPsClient SmartEnergyManagementPsClient;

struct SmartEnergyManagementPsClient {
  FeatureInfoClient feature_info_client;

  SmartEnergyManagementPsCommon sem_ps_common;
};

/**
 * @brief Initializes a SmartEnergyManagementPsClient instance.
 *
 * This function initializes a SmartEnergyManagementPsClient instance by associating it with a local entity
 * and a remote entity. The local entity must have a feature with the client role, and the remote
 * entity must have a feature with the server role. It sets up the necessary structures for managing
 * Smart Energy Management PS communication between the entities.
 *
 * @param self A pointer to the SmartEnergyManagementPsClient instance to be initialized.
 * @param local_entity A pointer to the local entity object (client role).
 * @param remote_entity A pointer to the remote entity object (server role).
 * @return An EebusError indicating the success or failure of the operation.
 *         - kEebusErrorOk: If the initialization was successful.
 *         - Other error codes: If the initialization failed.
 */
EebusError SmartEnergyManagementPsClientConstruct(
    SmartEnergyManagementPsClient* self,
    EntityLocalObject* local_entity,
    EntityRemoteObject* remote_entity
);

/**
 * @brief Requests SmartEnergyManagementPsData from the remote entity.
 *
 * This function sends a request to the remote entity associated with the SmartEnergyManagementPsClient
 * instance to retrieve the Smart Energy Management PS data. It initiates the data retrieval process,
 * allowing the client to receive the latest information from the server.
 *
 * @param self A pointer to the SmartEnergyManagementPsClient instance.
 * @return An EebusError indicating the success or failure of the operation.
 *         - kEebusErrorOk: If the request was successfully sent.
 *         - Other error codes: If the request failed.
 */
EebusError
SmartEnergyManagementPsClientRequestData(SmartEnergyManagementPsClient* self, ReplyMessageCallback cb, void* ctx);

/**
 * @brief Writes partial SmartEnergyManagementPsData with single
 * power sequence element to the remote entity.
 *
 * This function sends a partial update of the power sequence data to the remote entity
 * associated with the SmartEnergyManagementPsClient instance. It allows updating specific
 * fields of the power sequence element without sending the entire data structure.
 *
 * @param self A pointer to the SmartEnergyManagementPsClient instance.
 * @param alternative_id The identifier for the alternatives to which the power sequence belongs.
 * @param power_sequence A pointer to the SmartEnergyManagementPsPowerSequenceType structure
 *                       containing the partial data to be written.
 * @return An EebusError indicating the success or failure of the operation.
 *         - kEebusErrorOk: If the write operation was successful.
 *         - Other error codes: If the write operation failed.
 */
EebusError SmartEnergyManagementPsClientWritePartial(
    SmartEnergyManagementPsClient* self,
    const AlternativesIdType* alternative_id,
    const SmartEnergyManagementPsPowerSequenceType* power_sequence,
    ResultMessageCallback cb,
    void* ctx
);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_USE_CASE_SPECIALIZATION_SMART_ENERGY_MANAGEMENT_PS_SMART_ENERGY_MANAGEMENT_PS_CLIENT_H_
