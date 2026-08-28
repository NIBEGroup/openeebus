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
 * @brief Device Configuration Client functionality
 */

#ifndef SRC_USE_CASE_ENERGY_GUARD_DEVICE_CONFIGURATION_CLIENT_H_
#define SRC_USE_CASE_ENERGY_GUARD_DEVICE_CONFIGURATION_CLIENT_H_

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

#include "src/spine/api/feature_local_interface.h"
#include "src/spine/model/device_configuration_types.h"
#include "src/use_case/specialization/device_configuration/device_configuration_common.h"
#include "src/use_case/specialization/feature_info_client.h"

typedef struct DeviceConfigurationClient DeviceConfigurationClient;

struct DeviceConfigurationClient {
    FeatureInfoClient feature_info_client;

    DeviceConfigurationCommon device_cfg_common;
};

/**
 * @brief Constructs a DeviceConfigurationClient instance.
 *
 * This function initializes a DeviceConfigurationClient instance by associating it
 * with a local entity and a remote entity. It sets up the necessary structures
 * for managing device configuration communication between the entities.
 *
 * @param self A pointer to the DeviceConfigurationClient instance to be initialized.
 * @param local_entity A pointer to the local entity object.
 * @param remote_entity A pointer to the remote entity object.
 * @return An EebusError indicating the success or failure of the operation.
 *         - kEebusErrorOk: If the construction was successful.
 *         - Other error codes: If the construction failed.
 */
EebusError DeviceConfigurationClientConstruct(
    DeviceConfigurationClient* self,
    EntityLocalObject* local_entity,
    EntityRemoteObject* remote_entity
);

/**
 * @brief Requests key-value description data from a remote entity.
 *
 * This function sends a request to a remote entity to retrieve key-value description
 * data based on the specified selectors and elements. The response will contain
 * the matching key-value descriptions.
 *
 * @param self A pointer to the DeviceConfigurationClient instance making the request.
 * @param selectors A pointer to the DeviceConfigurationKeyValueDescriptionListDataSelectorsType
 *                  structure that specifies the criteria for selecting the key-value descriptions.
 * @param elements A pointer to the DeviceConfigurationKeyValueDescriptionDataElementsType
 *                 structure that specifies the elements to be included in the response.
 * @return An EebusError indicating the success or failure of the operation.
 *         - kEebusErrorOk: If the request was successfully sent.
 *         - Other error codes: If the request failed.
 */
EebusError DeviceConfigurationClientRequestKeyValueDescription(
    DeviceConfigurationClient* self,
    const DeviceConfigurationKeyValueDescriptionListDataSelectorsType* selectors,
    const DeviceConfigurationKeyValueDescriptionDataElementsType* elements
);

/**
 * @brief Requests key-value data from a remote entity.
 *
 * This function sends a request to a remote entity to retrieve key-value data
 * based on the specified selectors and elements. The response will contain
 * the matching key-value data entries.
 *
 * @param self A pointer to the DeviceConfigurationClient instance making the request.
 * @param selectors A pointer to the DeviceConfigurationKeyValueListDataSelectorsType
 *                  structure that specifies the criteria for selecting the key-value data.
 * @param elements A pointer to the DeviceConfigurationKeyValueDataElementsType
 *                 structure that specifies the elements to be included in the response.
 * @return An EebusError indicating the success or failure of the operation.
 *         - kEebusErrorOk: If the request was successfully sent.
 *         - Other error codes: If the request failed.
 */
EebusError DeviceConfigurationClientRequestKeyValue(
    DeviceConfigurationClient* self,
    const DeviceConfigurationKeyValueListDataSelectorsType* selectors,
    const DeviceConfigurationKeyValueDataElementsType* elements,
    ReplyMessageCallback cb,
    void* ctx
);

/**
 * @brief Writes key-value data to a remote entity.
 *
 * This function sends a request to a remote entity to write key-value data.
 * The data is specified in the provided DeviceConfigurationKeyValueDataType structure.
 *
 * @param self A pointer to the DeviceConfigurationClient instance writing the data.
 * @param key_value_list A pointer to the DeviceConfigurationKeyValueListDataType structure
 *             containing the key-value data to be written.
 * @return An EebusError indicating the success or failure of the operation.
 *         - kEebusErrorOk: If the write operation was successful.
 *         - Other error codes: If the write operation failed.
 */
EebusError DeviceConfigurationClientWriteKeyValueList(
    DeviceConfigurationClient* self,
    const DeviceConfigurationKeyValueListDataType* key_value_list,
    ResultMessageCallback cb,
    void* ctx
);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_USE_CASE_ENERGY_GUARD_DEVICE_CONFIGURATION_CLIENT_H_
