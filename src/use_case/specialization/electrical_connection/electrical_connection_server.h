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
 * @brief Electrical Connection Server functionality
 */

#ifndef SRC_USE_CASE_SPECIALIZATION_ELECTRICAL_CONNECTION_ELECTRICAL_CONNECTION_SERVER_H_
#define SRC_USE_CASE_SPECIALIZATION_ELECTRICAL_CONNECTION_ELECTRICAL_CONNECTION_SERVER_H_

#include "src/spine/entity/entity_local.h"
#include "src/spine/model/electrical_connection_types.h"
#include "src/use_case/specialization/electrical_connection/electrical_connection_common.h"
#include "src/use_case/specialization/feature_info_server.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct ElectricalConnectionServer ElectricalConnectionServer;

struct ElectricalConnectionServer {
    FeatureInfoServer feature_info_server;
    ElectricalConnectionCommon el_connection_common;
};

/**
 * @brief Constructs an ElectricalConnectionServer instance.
 * @param self A pointer to the ElectricalConnectionServer instance to be constructed.
 * @param local_entity A pointer to the local entity object to associate with the server.
 * @return An EebusError indicating the success or failure of the operation.
 *         - kEebusErrorOk: If the construction was successful.
 *         - Other error codes: If the construction failed.
 */
EebusError ElectricalConnectionServerGetOrAddIdForDescription(
    ElectricalConnectionServer* self,
    const ElectricalConnectionDescriptionDataType* filter,
    ElectricalConnectionIdType* electrical_connection_id
);

/**
 * @brief Constructs an ElectricalConnectionServer instance.
 *
 * This function initializes an ElectricalConnectionServer instance by associating it
 * with a local entity. It sets up the necessary structures for managing electrical
 * connection server functionality, enabling communication and data handling for
 * electrical connections.
 *
 * @param self A pointer to the ElectricalConnectionServer instance to be initialized.
 * @param local_entity A pointer to the local entity object to associate with the server.
 * @return An EebusError indicating the success or failure of the operation.
 *         - kEebusErrorOk: If the construction was successful.
 *         - Other error codes: If the construction failed.
 */
EebusError ElectricalConnectionServerConstruct(ElectricalConnectionServer* self, EntityLocalObject* local_entity);

/**
 * @brief Adds a new electrical connection description with a specified ID.
 * @param self A pointer to the ElectricalConnectionServer instance to add the description.
 * @param description A pointer to the ElectricalConnectionDescriptionDataType structure
 *                    containing the description to be added.
 * @param electrical_connection_id The ID of the electrical connection to which the description belongs.
 * @return An EebusError indicating the success or failure of the operation.
 *         - kEebusErrorOk: If the description was successfully added.
 *         - Other error codes: If the construction failed.
 */
EebusError ElectricalConnectionServerAddDescriptionWithId(
    ElectricalConnectionServer* self,
    const ElectricalConnectionDescriptionDataType* description,
    ElectricalConnectionIdType electrical_connection_id
);

/**
 * @brief Adds a new parameter description to the electrical connection server.
 *
 * This function adds a new parameter description to the electrical connection server
 * and returns the ID of the added parameter.
 *
 * @param self A pointer to the ElectricalConnectionServer instance.
 * @param description A pointer to the ElectricalConnectionParameterDescriptionDataType structure
 *                    containing the parameter description to be added.
 * @param parameter_id A pointer to store the ID of the added parameter.
 * @return An EebusError indicating the success or failure of the operation.
 *         - kEebusErrorOk: If the parameter was successfully added.
 *         - Other error codes: If the operation failed.
 */
EebusError ElectricalConnectionServerAddParameterDescription(
    ElectricalConnectionServer* self,
    const ElectricalConnectionParameterDescriptionDataType* description,
    ElectricalConnectionParameterIdType* parameter_id
);

/**
 * @brief Adds a characteristic to the electrical connection server.
 *
 * This function adds a new characteristic to the electrical connection server.
 * The characteristic provides detailed information about a specific property
 * of the electrical connection.
 *
 * @param self A pointer to the ElectricalConnectionServer instance.
 * @param characteristic A pointer to the ElectricalConnectionCharacteristicDataType structure
 *                       containing the characteristic to be added.
 * @return An EebusError indicating the success or failure of the operation.
 *         - kEebusErrorOk: If the characteristic was successfully added.
 *         - Other error codes: If the operation failed.
 */
EebusError ElectricalConnectionServerAddCharacteristic(
    ElectricalConnectionServer* self,
    const ElectricalConnectionCharacteristicDataType* characteristic
);

/**
 * @brief Update data set for a filter.
 *
 * This function updates the data set for a specific filter by adding new
 * elements and removing specified elements from the data set. Elements
 * provided in delete_elements will be removed from the data set before
 * the update is applied.
 *
 * @note ElectricalConnectionId, ParameterId, and CharacteristicId must be set.
 *
 * @param self Pointer to the ElectricalConnectionServer instance
 * @param data Pointer to the data structure containing the new characteristic
 *             data to be added to the data set
 * @param delete_elements Pointer to the data structure containing the elements
 *                        to be removed from the data set before the update
 *
 * @return Will return an error of type EebusError if the data set could not
 *         be updated successfully
 */
EebusError ElectricalConnectionServerUpdateCharacteristic(
    ElectricalConnectionServer* self,
    const ElectricalConnectionCharacteristicDataType* data,
    const ElectricalConnectionCharacteristicDataElementsType* delete_elements
);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_USE_CASE_SPECIALIZATION_ELECTRICAL_CONNECTION_ELECTRICAL_CONNECTION_SERVER_H_
