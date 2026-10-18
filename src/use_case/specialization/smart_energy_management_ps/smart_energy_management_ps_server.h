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
 * @brief Smart Energy Management PS Server functionality
 */

#ifndef SRC_USE_CASE_SPECIALIZATION_SMART_ENERGY_MANAGEMENT_PS_SMART_ENERGY_MANAGEMENT_PS_SERVER_H_
#define SRC_USE_CASE_SPECIALIZATION_SMART_ENERGY_MANAGEMENT_PS_SMART_ENERGY_MANAGEMENT_PS_SERVER_H_

#include "src/spine/entity/entity_local.h"
#include "src/spine/model/smart_energy_management_ps_types.h"
#include "src/use_case/specialization/feature_info_server.h"
#include "src/use_case/specialization/smart_energy_management_ps/smart_energy_management_ps_common.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct SmartEnergyManagementPsServer SmartEnergyManagementPsServer;

struct SmartEnergyManagementPsServer {
  FeatureInfoServer feature_info_server;
  SmartEnergyManagementPsCommon smart_energy_management_ps_common;
};

/**
 * @brief Constructs a SmartEnergyManagementPsServer instance.
 *
 * This function initializes a SmartEnergyManagementPsServer instance by associating it with a local entity.
 * It sets up the necessary structures for managing smart energy management PS server functionality, enabling
 * communication and data handling for smart energy management operations.
 *
 * @param self A pointer to the SmartEnergyManagementPsServer instance to be initialized.
 * @param local_entity A pointer to the local entity object to associate with the server.
 * @return An EebusError indicating the success or failure of the operation.
 *         - kEebusErrorOk: If the construction was successful.
 *         - Other error codes: If the construction failed.
 */
EebusError SmartEnergyManagementPsServerConstruct(SmartEnergyManagementPsServer* self, EntityLocalObject* local_entity);

/**
 * @brief Update the Smart Energy Management PS server data
 * @param self A pointer to the SmartEnergyManagementPsServer instance
 * @param data Pointer to the SmartEnergyManagementPsDataType to be written
 * @return An EebusError indicating the success or failure of the operation.
 *         - kEebusErrorOk: If the request was successfully sent.
 *         - Other error codes: If the request failed.
 */
EebusError
SmartEnergyManagementPsServerUpdate(SmartEnergyManagementPsServer* self, const SmartEnergyManagementPsDataType* data);

/**
 * @brief Update by write partial the Smart Energy Management PS server data
 * @param self A pointer to the SmartEnergyManagementPsServer instance
 * @param alternatives_id Pointer to the AlternativesIdType to update, can be NULL
 * @param node_schedule Pointer to the PowerSequenceNodeScheduleInformationDataType to update, can be NULL
 * @param power_sequence Pointer to the SmartEnergyManagementPsPowerSequenceType to update, can be NULL
 * @return An EebusError indicating the success or failure of the operation.
 *         - kEebusErrorOk: If the request was successfully sent.
 *         - Other error codes: If the request failed.
 */
EebusError SmartEnergyManagementPsServerUpdatePartial(
    SmartEnergyManagementPsServer* self,
    const AlternativesIdType* alternatives_id,
    const PowerSequenceNodeScheduleInformationDataType* node_schedule,
    const SmartEnergyManagementPsPowerSequenceType* power_sequence
);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_USE_CASE_SPECIALIZATION_SMART_ENERGY_MANAGEMENT_PS_SMART_ENERGY_MANAGEMENT_PS_SERVER_H_
