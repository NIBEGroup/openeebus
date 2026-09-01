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
 * @brief Device Configuration Common functionality
 */

#ifndef SRC_USE_CASE_FEATURE_INFO_DEVICE_CONFIGURATION_DEVICE_CONFIGURATION_COMMON_H_
#define SRC_USE_CASE_FEATURE_INFO_DEVICE_CONFIGURATION_DEVICE_CONFIGURATION_COMMON_H_

#include "src/common/eebus_data/eebus_data_list.h"
#include "src/spine/feature/feature_local.h"
#include "src/spine/feature/feature_remote.h"
#include "src/spine/model/device_configuration_types.h"
#include "src/use_case/specialization/helper.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct DeviceConfigurationCommon DeviceConfigurationCommon;

struct DeviceConfigurationCommon {
    FeatureLocalObject* feature_local;
    FeatureRemoteObject* feature_remote;
};

/**
 * @brief Constructs a DeviceConfigurationCommon instance.
 *
 * This function initializes a DeviceConfigurationCommon instance by associating it
 * with a local feature object and a remote feature object. It sets up the necessary
 * structures for managing device configuration communication between the local and
 * remote features.
 *
 * @param self A pointer to the DeviceConfigurationCommon instance to be initialized.
 * @param feature_local A pointer to the local feature object.
 * @param feature_remote A pointer to the remote feature object.
 */
void DeviceConfigurationCommonConstruct(
    DeviceConfigurationCommon* self,
    FeatureLocalObject* feature_local,
    FeatureRemoteObject* feature_remote
);

/**
 * @brief Retrieves the list of key-value descriptions for the device configuration.
 *
 * This function returns a pointer to the list of key-value descriptions associated
 * with the given DeviceConfigurationCommon instance.
 *
 * @param self A pointer to the DeviceConfigurationCommon instance.
 * @return A pointer to the DeviceConfigurationKeyValueDescriptionListDataType structure
 *         containing the key-value descriptions, or NULL if no data is available.
 */
static inline const DeviceConfigurationKeyValueDescriptionListDataType*
DeviceConfigurationCommonGetKeyValueDescriptionList(const DeviceConfigurationCommon* self) {
    return HelperGetFeatureData(
        self->feature_local,
        self->feature_remote,
        kFunctionTypeDeviceConfigurationKeyValueDescriptionListData
    );
}

/**
 * @brief Checks if a key-value entry matches the given filter criteria.
 *
 * This function evaluates whether a key-value entry from the provided data list
 * matches the specified description filter. It is used to determine if a key-value
 * pair satisfies the filtering criteria defined in the description.
 *
 * @param self A pointer to the DeviceConfigurationCommon instance.
 * @param data A pointer to the DeviceConfigurationKeyValueListDataType structure
 *             containing the key-value entries to be checked.
 * @param description A pointer to the DeviceConfigurationKeyValueDescriptionDataType structure
 *                    that defines the filtering criteria.
 * @return true if a matching key-value entry is found, false otherwise.
 */
bool DeviceConfigurationCommonCheckKeyValueWithFilter(
    const DeviceConfigurationCommon* self,
    const DeviceConfigurationKeyValueListDataType* data,
    const DeviceConfigurationKeyValueDescriptionDataType* description
);

/**
 * @brief Get the description for a given key-value combination based on filter criteria
 *
 * @param self Pointer to the DeviceConfigurationCommon instance
 * @param filter Filter criteria containing key-value pair to match
 * @return const DeviceConfigurationKeyValueDescriptionDataType* Pointer to matching description if found, NULL
 * otherwise
 * @return A pointer to the DeviceConfigurationKeyValueDescriptionDataType structure
 *         containing the matching key-value description, or NULL if no match is found.
 */
const DeviceConfigurationKeyValueDescriptionDataType* DeviceConfigurationCommonGetKeyValueDescriptionWithFilter(
    const DeviceConfigurationCommon* self,
    const DeviceConfigurationKeyValueDescriptionDataType* filter
);

/**
 * @brief Retrieves a key-value description for a given key ID.
 *
 * This function searches the key-value descriptions in the device configuration
 * and returns a pointer to the description that matches the specified key ID.
 *
 * @param self A pointer to the DeviceConfigurationCommon instance to search within.
 * @param key_id The key ID for which the key-value description is to be retrieved.
 * @return A pointer to the DeviceConfigurationKeyValueDescriptionDataType structure
 *         containing the matching key-value description, or NULL if no match is found.
 */
const DeviceConfigurationKeyValueDescriptionDataType* DeviceConfigurationCommonGetKeyValueDescriptionWithKeyId(
    const DeviceConfigurationCommon* self,
    DeviceConfigurationKeyIdType key_id
);

/**
 * @brief Initialise the iterator pointing to the first entry that matches
 * the specified filter criteria. The iterator provides possibility to go through all of
 * the matches using EebusDataListMatchIteratorNext() and EebusDataListMatchIteratorIsDone().
 *
 * @param self A pointer to the DeviceConfigurationCommon instance to search within.
 * @param filter A pointer to the DeviceConfigurationKeyValueDescriptionDataType structure
 *               that defines the filtering criteria.
 * @param it A pointer to the match iterator which will refer to the first matching entry.
 *           On caller side EebusDataListMatchIteratorIsDone(it) will return true if no matches found
 */
void DeviceConfigurationCommonKeyValueDescriptionMatchFirst(
    const DeviceConfigurationCommon* self,
    const DeviceConfigurationKeyValueDescriptionDataType* filter,
    EebusDataListMatchIterator* it
);

/**
 * @brief Retrieves a key-value entry for a given key ID.
 *
 * This function searches the key-value data in the device configuration
 * and returns a pointer to the key-value entry that matches the specified key ID.
 *
 * @param self A pointer to the DeviceConfigurationCommon instance to search within.
 * @param key_id The key ID for which the key-value entry is to be retrieved.
 * @return A pointer to the DeviceConfigurationKeyValueDataType structure
 *         containing the matching key-value entry, or NULL if no match is found.
 */
const DeviceConfigurationKeyValueDataType* DeviceConfigurationCommonGetKeyValueWithKeyId(
    const DeviceConfigurationCommon* self,
    DeviceConfigurationKeyIdType key_id
);

/**
 * @brief Retrieves a key-value entry that matches the given filter criteria.
 *
 * This function searches the key-value data in the device configuration
 * and returns a pointer to the first key-value entry that satisfies the specified
 * filter criteria. If no match is found, the function returns NULL.
 *
 * @param self A pointer to the DeviceConfigurationCommon instance to search within.
 * @param filter A pointer to the DeviceConfigurationKeyValueDescriptionDataType structure
 *               that defines the filtering criteria.
 * @return A pointer to the DeviceConfigurationKeyValueDataType structure
 *         containing the matching key-value entry, or NULL if no match is found.
 */
const DeviceConfigurationKeyValueDataType* DeviceConfigurationCommonGetKeyValueWithFilter(
    const DeviceConfigurationCommon* self,
    const DeviceConfigurationKeyValueDescriptionDataType* filter
);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_USE_CASE_FEATURE_INFO_DEVICE_CONFIGURATION_DEVICE_CONFIGURATION_COMMON_H_
