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
 * @brief Load Control Client functionality
 */

#ifndef SRC_USE_CASE_ENERGY_GUARD_LOAD_CONTROL_CLIENT_H_
#define SRC_USE_CASE_ENERGY_GUARD_LOAD_CONTROL_CLIENT_H_

#include "src/spine/api/feature_local_interface.h"
#include "src/spine/model/loadcontrol_types.h"
#include "src/use_case/specialization/feature_info_client.h"
#include "src/use_case/specialization/load_control/load_control_common.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct LoadControlClient LoadControlClient;

struct LoadControlClient {
    FeatureInfoClient feature_info_client;

    LoadControlCommon load_control_common;
};

/**
 * @brief Initializes a LoadControlClient instance.
 *
 * This function initializes a LoadControlClient instance by associating it with a local entity
 * and a remote entity. The local entity must have a feature with the client role, and the remote
 * entity must have a feature with the server role. It sets up the necessary structures for managing
 * load control communication between the entities.
 *
 * @param self A pointer to the LoadControlClient instance to be initialized.
 * @param local_entity A pointer to the local entity object (client role).
 * @param remote_entity A pointer to the remote entity object (server role).
 * @return An EebusError indicating the success or failure of the operation.
 *         - kEebusErrorOk: If the initialization was successful.
 *         - Other error codes: If the initialization failed.
 */
EebusError
LoadControlClientConstruct(LoadControlClient* self, EntityLocalObject* local_entity, EntityRemoteObject* remote_entity);

/**
 * @brief Requests limit description data from a remote device.
 *
 * This function sends a request to a remote device to retrieve limit description data
 * (FunctionTypeLoadControlLimitDescriptionListData) based on the specified selectors and elements.
 * The response will contain the matching limit descriptions.
 *
 * @param self A pointer to the LoadControlClient instance making the request.
 * @param selectors A pointer to the LoadControlLimitDescriptionListDataSelectorsType structure
 *                  that specifies the criteria for selecting the limit descriptions.
 * @param elements A pointer to the LoadControlLimitDescriptionDataElementsType structure
 *                 that specifies the elements to be included in the response.
 * @return An EebusError indicating the success or failure of the operation.
 *         - kEebusErrorOk: If the request was successfully sent.
 *         - Other error codes: If the request failed.
 */
EebusError LoadControlClientRequestLimitDescriptions(
    LoadControlClient* self,
    const LoadControlLimitDescriptionListDataSelectorsType* selectors,
    const LoadControlLimitDescriptionDataElementsType* elements
);

/**
 * @brief Requests limit constraints data from a remote device.
 *
 * This function sends a request to a remote device to retrieve limit constraints data
 * (FunctionTypeLoadControlLimitConstraintsListData) based on the specified selectors and elements.
 * The response will contain the matching limit constraints.
 *
 * @param self A pointer to the LoadControlClient instance making the request.
 * @param selectors A pointer to the LoadControlLimitConstraintsListDataSelectorsType structure
 *                  that specifies the criteria for selecting the limit constraints.
 * @param elements A pointer to the LoadControlLimitConstraintsDataElementsType structure
 *                 that specifies the elements to be included in the response.
 * @return An EebusError indicating the success or failure of the operation.
 *         - kEebusErrorOk: If the request was successfully sent.
 *         - Other error codes: If the request failed.
 */
EebusError LoadControlClientRequestLimitConstraints(
    LoadControlClient* self,
    const LoadControlLimitConstraintsListDataSelectorsType* selectors,
    const LoadControlLimitConstraintsDataElementsType* elements
);

EebusError LoadControlClientReadLimit(
    LoadControlClient* self,
    const LoadControlLimitDescriptionDataType* filter,
    ReplyMessageCallback cb,
    void* ctx
);

/**
 * @brief Writes load control limit list to a remote device.
 *
 * This function sends new or updated load control limit data to a remote device.
 * It can also remove existing limit data entries specified by the delete selectors and elements.
 * The function returns an error if the operation fails.
 *
 * @param self A pointer to the LoadControlClient instance making the request.
 * @param limit_list A pointer to a LoadControlLimitListDataType structure containing the limit data to be written.
 * @param delete_selectors A pointer to the LoadControlLimitListDataSelectorsType structure specifying which limit
 * data entries to delete.
 * @param deleteElements A pointer to the LoadControlLimitDataElementsType structure specifying which elements to
 * delete from the selected entries.
 * @return An EebusError indicating the success or failure of the operation.
 *         - kEebusErrorOk: If the operation was successful.
 *         - Other error codes: If the operation failed.
 */
EebusError LoadControlClientWriteLimitList(
    LoadControlClient* self,
    const LoadControlLimitListDataType* limit_list,
    const LoadControlLimitListDataSelectorsType* delete_selectors,
    const LoadControlLimitDataElementsType* delete_elements,
    ResultMessageCallback cb,
    void* ctx
);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_USE_CASE_ENERGY_GUARD_LOAD_CONTROL_CLIENT_H_
