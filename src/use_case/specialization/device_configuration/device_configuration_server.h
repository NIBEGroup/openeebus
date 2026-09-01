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
 * @brief Device Configuration Server functionality
 */

#ifndef SRC_USE_CASE_FEATURE_INFO_DEVICE_CONFIGURATION_DEVICE_CONFIGURATION_SERVER_H_
#define SRC_USE_CASE_FEATURE_INFO_DEVICE_CONFIGURATION_DEVICE_CONFIGURATION_SERVER_H_

#include "src/common/eebus_errors.h"
#include "src/use_case/specialization/device_configuration/device_configuration_common.h"
#include "src/use_case/specialization/feature_info_server.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct DeviceConfigurationServer DeviceConfigurationServer;

struct DeviceConfigurationServer {
    FeatureInfoServer feature_info_server;

    DeviceConfigurationCommon device_cfg_common;
};

/**
 * @brief Constructs a DeviceConfigurationServer instance.
 *
 * This function initializes a DeviceConfigurationServer instance by associating it
 * with a local entity. It sets up the necessary structures for managing device
 * configuration communication and handling server-side operations.
 *
 * @param self A pointer to the DeviceConfigurationServer instance to be initialized.
 * @param local_entity A pointer to the local entity object to associate with the server.
 * @return An EebusError indicating the success or failure of the operation.
 *         - kEebusErrorOk: If the construction was successful.
 *         - Other error codes: If the construction failed.
 */
EebusError DeviceConfigurationServerConstruct(DeviceConfigurationServer* self, EntityLocalObject* local_entity);

/**
 * @brief Adds a key-value description to the device configuration server.
 *
 * This function adds a new key-value description to the device configuration server.
 * The description defines metadata about a key-value pair that can be used for
 * device configuration purposes.
 *
 * @param self A pointer to the DeviceConfigurationServer instance.
 * @param description A pointer to the DeviceConfigurationKeyValueDescriptionDataType structure
 *                    containing the key-value description to be added.
 * @return An EebusError indicating the success or failure of the operation.
 *         - kEebusErrorOk: If the key-value description was successfully added.
 *         - Other error codes: If the operation failed.
 */
EebusError DeviceConfigurationServerAddKeyValueDescription(
    const DeviceConfigurationServer* self,
    const DeviceConfigurationKeyValueDescriptionDataType* description
);

/**
 * @brief Updates a key-value entry for a given key ID in the device configuration server.
 *
 * This function updates an existing key-value entry in the device configuration server
 * that matches the specified key ID. Elements provided in the delete_elements parameter
 * will be removed from the key-value entry before the update is applied.
 *
 * @param self A pointer to the DeviceConfigurationServer instance.
 * @param key_value A pointer to the DeviceConfigurationKeyValueDataType structure
 *                  containing the new key-value data to be updated.
 * @param delete_elements A pointer to the DeviceConfigurationKeyValueDataElementsType structure
 *                        specifying the elements to be removed from the key-value entry.
 * @param key_id The key ID of the key-value entry to be updated.
 * @return An EebusError indicating the success or failure of the operation.
 *         - kEebusErrorOk: If the key-value entry was successfully updated.
 *         - Other error codes: If the operation failed.
 */
EebusError DeviceConfigurationServerUpdateKeyValueWithKeyId(
    DeviceConfigurationServer* self,
    const DeviceConfigurationKeyValueDataType* key_value,
    const DeviceConfigurationKeyValueDataElementsType* delete_elements,
    DeviceConfigurationKeyIdType key_id
);

/**
 * @brief Updates a key-value entry in the device configuration server using a filter.
 *
 * This function updates an existing key-value entry in the device configuration server
 * that matches the specified filter criteria. Elements provided in the delete_elements
 * parameter will be removed from the key-value entry before the update is applied.
 *
 * @param self A pointer to the DeviceConfigurationServer instance.
 * @param key_value A pointer to the DeviceConfigurationKeyValueDataType structure
 *                  containing the new key-value data to be updated.
 * @param delete_elements A pointer to the DeviceConfigurationKeyValueDataElementsType structure
 *                        specifying the elements to be removed from the key-value entry.
 * @param filter A pointer to the DeviceConfigurationKeyValueDescriptionDataType structure
 *               defining the filter criteria to identify the key-value entry to be updated.
 * @return An EebusError indicating the success or failure of the operation.
 *         - kEebusErrorOk: If the key-value entry was successfully updated.
 *         - Other error codes: If the operation failed.
 */
EebusError DeviceConfigurationServerUpdateKeyValueWithFilter(
    const DeviceConfigurationServer* self,
    const DeviceConfigurationKeyValueDataType* key_value,
    const DeviceConfigurationKeyValueDataElementsType* delete_elements,
    const DeviceConfigurationKeyValueDescriptionDataType* filter
);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_USE_CASE_FEATURE_INFO_DEVICE_CONFIGURATION_DEVICE_CONFIGURATION_SERVER_H_
