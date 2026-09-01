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
 * @brief Load Control Common functionality
 */

#ifndef SRC_USE_CASE_ENERGY_GUARD_LOAD_CONTROL_COMMON_H_
#define SRC_USE_CASE_ENERGY_GUARD_LOAD_CONTROL_COMMON_H_

#include "src/spine/feature/feature_local.h"
#include "src/spine/feature/feature_remote.h"
#include "src/spine/model/loadcontrol_types.h"
#include "src/use_case/specialization/helper.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct LoadControlCommon LoadControlCommon;

struct LoadControlCommon {
    FeatureLocalObject* feature_local;
    FeatureRemoteObject* feature_remote;
};

/**
 * @brief Constructs a LoadControlCommon instance for local and remote features.
 *
 * This function initializes a LoadControlCommon instance by associating it with a local feature object
 * and a remote feature object. It sets up the necessary structures for managing load control communication
 * between the local and remote features.
 *
 * @param self A pointer to the LoadControlCommon instance to be initialized.
 * @param feature_local A pointer to the local feature object.
 * @param feature_remote A pointer to the remote feature object.
 */
void LocalLoadControlCommonConstruct(
    LoadControlCommon* self,
    FeatureLocalObject* feature_local,
    FeatureRemoteObject* feature_remote
);

/**
 * @brief Retrieves the list of load control limit descriptions.
 *
 * This function returns a pointer to the LoadControlLimitDescriptionListDataType structure
 * that contains the list of limit descriptions associated with the load control instance.
 * These descriptions provide metadata about the available load control limits.
 *
 * @param self A pointer to the LoadControlCommon instance.
 * @return A pointer to the LoadControlLimitDescriptionListDataType structure containing
 *         the list of limit descriptions, or NULL if no data is available.
 */
static inline const LoadControlLimitDescriptionListDataType* LoadControlCommonGetLimitDescriptionList(
    const LoadControlCommon* self
) {
    return HelperGetFeatureData(
        self->feature_local,
        self->feature_remote,
        kFunctionTypeLoadControlLimitDescriptionListData
    );
}

/**
 * @brief Checks if the load control limit data contains an entry matching the given filter.
 *
 * This function evaluates whether the provided load control limit data contains at least one entry
 * that matches the specified filter criteria. It is used to determine if the data set includes
 * a limit that satisfies the filtering conditions.
 *
 * @param self A pointer to the LoadControlCommon instance.
 * @param data A pointer to the LoadControlLimitListDataType structure containing the data to be checked.
 * @param filter A pointer to the LoadControlLimitDescriptionDataType structure specifying the filter criteria.
 * @return true if a matching entry is found, false otherwise.
 */
bool LoadControlCommonCheckLimitWithFilter(
    const LoadControlCommon* self,
    const LoadControlLimitListDataType* data,
    const LoadControlLimitDescriptionDataType* filter
);

/**
 * @brief Retrieves a limit description that matches the given filter criteria.
 *
 * This function searches the list of load control limit descriptions associated with the given
 * LoadControlCommon instance and returns a pointer to the first entry that matches the specified
 * filter criteria. If no match is found, the function returns NULL.
 *
 * @param self A pointer to the LoadControlCommon instance.
 * @param filter A pointer to the LoadControlLimitDescriptionDataType structure specifying the filter criteria.
 * @return A pointer to the matching LoadControlLimitDescriptionDataType structure, or NULL if no match is found.
 */
const LoadControlLimitDescriptionDataType* LoadControlCommonGetLimitDescriptionWithFilter(
    const LoadControlCommon* self,
    const LoadControlLimitDescriptionDataType* filter
);

/**
 * @brief Retrieves a limit description with the specified limit ID.
 *
 * This function searches the list of load control limit descriptions associated with the given
 * LoadControlCommon instance and returns a pointer to the entry that matches the specified
 * limit_id. If no matching description is found, the function returns NULL.
 *
 * Transformed from eebus-go GetLimitDescriptionForId().
 *
 * @param self A pointer to the LoadControlCommon instance.
 * @param limit_id The limit ID for which the description is to be retrieved.
 * @return A pointer to the matching LoadControlLimitDescriptionDataType structure, or NULL if no match is found.
 */
const LoadControlLimitDescriptionDataType*
LoadControlCommonGetLimitDescriptionWithId(LoadControlCommon* self, LoadControlLimitIdType limit_id);

/**
 * @brief Retrieves limit data with the specified limit ID.
 *
 * This function searches the list of load control limit data associated with the given
 * LoadControlCommon instance and returns a pointer to the entry that matches the specified
 * limit_id. If no matching data is found, the function returns NULL.
 *
 * @param self A pointer to the LoadControlCommon instance.
 * @param limit_id The limit ID for which the data is to be retrieved.
 * @return A pointer to the matching LoadControlLimitDataType structure, or NULL if no match is found.
 */
const LoadControlLimitDataType*
LoadControlCommonGetLimitWithId(const LoadControlCommon* self, LoadControlLimitIdType limit_id);

/**
 * @brief Retrieves limit data that matches the given filter criteria.
 *
 * This function searches the list of load control limit data associated with the given
 * LoadControlCommon instance and returns a pointer to the first entry that matches the specified
 * filter criteria. If no matching data is found, the function returns NULL.
 *
 * @param self A pointer to the LoadControlCommon instance.
 * @param filter A pointer to the LoadControlLimitDescriptionDataType structure specifying the filter criteria.
 * @return A pointer to the matching LoadControlLimitDataType structure, or NULL if no match is found.
 */
const LoadControlLimitDataType*
LoadControlCommonGetLimitWithFilter(const LoadControlCommon* self, const LoadControlLimitDescriptionDataType* filter);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_USE_CASE_ENERGY_GUARD_LOAD_CONTROL_COMMON_H_
