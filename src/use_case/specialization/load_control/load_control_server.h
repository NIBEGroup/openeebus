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
 * @brief Load Control Server functionality
 */

#ifndef SRC_USE_CASE_FEATURE_INFO_LOAD_CONTROL_LOAD_CONTROL_SERVER_H_
#define SRC_USE_CASE_FEATURE_INFO_LOAD_CONTROL_LOAD_CONTROL_SERVER_H_

#include "src/spine/entity/entity_local.h"
#include "src/spine/model/loadcontrol_types.h"
#include "src/use_case/specialization/feature_info_server.h"
#include "src/use_case/specialization/load_control/load_control_common.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct LoadControlServer LoadControlServer;

struct LoadControlServer {
    FeatureInfoServer feature_info_server;
    LoadControlCommon load_control_common;
};

/**
 * @brief Constructs a LoadControlServer instance.
 *
 * This function initializes a LoadControlServer instance by associating it with a local entity.
 * It sets up the necessary structures for managing load control server functionality, enabling
 * communication and data handling for load control operations.
 *
 * @param self A pointer to the LoadControlServer instance to be initialized.
 * @param local_entity A pointer to the local entity object to associate with the server.
 * @return An EebusError indicating the success or failure of the operation.
 *         - kEebusErrorOk: If the construction was successful.
 *         - Other error codes: If the construction failed.
 */
EebusError LoadControlServerConstruct(LoadControlServer* self, EntityLocalObject* local_entity);

/**
 * @brief Adds a new limit description data set and returns the limit ID.
 *
 * This function adds a new limit description data set to the load control server. If the operation
 * is successful, the function will return the assigned limit ID through the limit_id parameter.
 * The limit_id may not be provided (can be NULL) if the caller does not require it.
 *
 * @param self A pointer to the LoadControlServer instance.
 * @param description A pointer to the LoadControlLimitDescriptionDataType structure containing the description to be
 * added.
 * @param limit_id A pointer to a LoadControlLimitIdType variable where the assigned limit ID will be stored (can be
 * NULL).
 * @return An EebusError code indicating the success or failure of the operation.
 *         - kEebusErrorOk: If the data set was successfully added.
 *         - Other error codes: If the operation failed.
 */
EebusError LoadControlServerAddLimitDescription(
    LoadControlServer* self,
    LoadControlLimitDescriptionDataType* description,
    LoadControlLimitIdType* limit_id
);

/**
 * @brief Sets or updates limit data for a specified limit ID.
 *
 * This function sets or updates the data set for the given limit_id in the load control server.
 * If a data set with the specified ID exists, it will be updated with the provided data; otherwise,
 * a new data set may be created. This function is transformed from eebus-go UpdateLimitDataForIds().
 *
 * @param self A pointer to the LoadControlServer instance.
 * @param limt_data A pointer to the LoadControlLimitDataType structure containing the data to be set or updated.
 * @param limit_id The limit ID for which the data set is to be set or updated.
 * @return An EebusError code indicating the success or failure of the operation.
 *         - kEebusErrorOk: If the data set was successfully set or updated.
 *         - Other error codes: If the operation failed.
 */
EebusError LoadControlServerUpdateLimitWithId(
    LoadControlServer* self,
    LoadControlLimitDataType* limt_data,
    LoadControlLimitIdType limit_id
);

/**
 * @brief Sets or updates limit data for entries matching a filter.
 *
 * This function sets or updates the data set for entries in the load control server that match the
 * specified filter criteria. Before the update, items matching delete_selectors will be removed
 * from the data set, and the fields to be removed can be limited using delete_elements.
 * This function is transformed from eebus-go UpdateLimitDataForFilters().
 *
 * @param self A pointer to the LoadControlServer instance.
 * @param limit_data A pointer to the LoadControlLimitDataType structure containing the data to be set or updated.
 * @param filter A pointer to the LoadControlLimitDescriptionDataType structure specifying the filter criteria.
 * @param delete_selectors A pointer to the LoadControlLimitListDataSelectorsType structure specifying which items to
 * remove before the update.
 * @param delete_elements A pointer to the LoadControlLimitDataElementsType structure specifying which fields to remove
 * from the selected items.
 * @return An EebusError code indicating the success or failure of the operation.
 *         - kEebusErrorOk: If the data set was successfully set or updated.
 *         - Other error codes: If the operation failed.
 */
EebusError LoadControlServerUpdateLimitWithFilter(
    const LoadControlServer* self,
    const LoadControlLimitDataType* limit_data,
    const LoadControlLimitDescriptionDataType* filter,
    const LoadControlLimitListDataSelectorsType* delete_selectors,
    const LoadControlLimitDataElementsType* delete_elements
);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_USE_CASE_FEATURE_INFO_LOAD_CONTROL_LOAD_CONTROL_SERVER_H_
