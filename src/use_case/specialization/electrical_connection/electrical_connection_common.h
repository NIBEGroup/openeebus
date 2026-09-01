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
 * @brief Electrical Connection Common functionality
 */

#ifndef SRC_USE_CASE_SPECIALIZATION_ELECTRICAL_CONNECTION_ELECTRICAL_CONNECTION_COMMON_H_
#define SRC_USE_CASE_SPECIALIZATION_ELECTRICAL_CONNECTION_ELECTRICAL_CONNECTION_COMMON_H_

#include "src/common/eebus_data/eebus_data_list.h"
#include "src/spine/feature/feature_local.h"
#include "src/spine/feature/feature_remote.h"
#include "src/spine/model/electrical_connection_types.h"
#include "src/use_case/specialization/helper.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct ElectricalConnectionCommon ElectricalConnectionCommon;

struct ElectricalConnectionCommon {
    FeatureLocalObject* feature_local;
    FeatureRemoteObject* feature_remote;
};

/**
 * @brief Constructs an ElectricalConnectionCommon instance.
 *
 * This function initializes an ElectricalConnectionCommon instance by associating it
 * with a local feature object and a remote feature object. It sets up the necessary
 * structures for managing electrical connection communication between the local and
 * remote features.
 *
 * @param self A pointer to the ElectricalConnectionCommon instance to be initialized.
 * @param feature_local A pointer to the local feature object.
 * @param feature_remote A pointer to the remote feature object.
 */
void ElectricalConnectionCommonConstruct(
    ElectricalConnectionCommon* self,
    FeatureLocalObject* feature_local,
    FeatureRemoteObject* feature_remote
);

/**
 * @brief Retrieves the list of electrical connection characteristics.
 *
 * This function returns a pointer to the ElectricalConnectionCharacteristicListDataType
 * structure that contains the list of characteristics associated with the electrical
 * connection. These characteristics provide detailed information about the electrical
 * connection's properties.
 *
 * @param self A pointer to the ElectricalConnectionCommon instance.
 * @return A pointer to the ElectricalConnectionCharacteristicListDataType structure
 *         containing the list of characteristics, or NULL if no data is available.
 */
static inline const ElectricalConnectionCharacteristicListDataType* ElectricalConnectionCommonGetCharacteristicList(
    const ElectricalConnectionCommon* self
) {
    return (const ElectricalConnectionCharacteristicListDataType*)HelperGetFeatureData(
        self->feature_local,
        self->feature_remote,
        kFunctionTypeElectricalConnectionCharacteristicListData
    );
}

/**
 * @brief Retrieves the list of electrical connection parameter descriptions.
 *
 * This function returns a pointer to the ElectricalConnectionParameterDescriptionListDataType
 * structure that contains the list of parameter descriptions associated with the electrical
 * connection. These parameter descriptions provide metadata about the parameters used in
 * the electrical connection.
 *
 * @param self A pointer to the ElectricalConnectionCommon instance.
 * @return A pointer to the ElectricalConnectionParameterDescriptionListDataType structure
 *         containing the list of parameter descriptions, or NULL if no data is available.
 */
static inline const ElectricalConnectionParameterDescriptionListDataType*
ElectricalConnectionCommonGetParameterDescriptionList(const ElectricalConnectionCommon* self) {
    return (const ElectricalConnectionParameterDescriptionListDataType*)HelperGetFeatureData(
        self->feature_local,
        self->feature_remote,
        kFunctionTypeElectricalConnectionParameterDescriptionListData
    );
}

/**
 * @brief Retrieves a parameter description that matches the given filter criteria.
 *
 * This function searches through the electrical connection's parameter descriptions
 * and returns a pointer to the unique matching entry that satisfies the specified
 * filter criteria. If no match is found or there are multiple matches, the function returns NULL.
 *
 * @param self A pointer to the ElectricalConnectionCommon instance to search within.
 * @param filter A pointer to the ElectricalConnectionParameterDescriptionDataType structure
 *               that defines the filtering criteria.
 * @return A pointer to the matching ElectricalConnectionParameterDescriptionDataType, or
 *         NULL if no matches are found or there are multiple matches.
 */
const ElectricalConnectionParameterDescriptionDataType* ElectricalConnectionCommonGetParameterDescriptionWithFilter(
    const ElectricalConnectionCommon* self,
    const ElectricalConnectionParameterDescriptionDataType* filter
);

/**
 * @brief Check if payload contains data for a given filter.
 *
 * Data type will be checked for ElectricalConnectionPermittedValueSetListDataType,
 * filter type will be checked for ElectricalConnectionParameterDescriptionDataType.
 *
 * @param self Pointer to the ElectricalConnectionCommon instance.
 * @param data Pointer to the ElectricalConnectionPermittedValueSetListDataType data to check.
 * @param filter Pointer to the ElectricalConnectionParameterDescriptionDataType filter to apply.
 * @return true if the data matches the filter, false otherwise.
 */
bool ElectricalConnectionCommonCheckEventPayloadDataForFilter(
    const ElectricalConnectionCommon* self,
    const ElectricalConnectionPermittedValueSetListDataType* data,
    const ElectricalConnectionParameterDescriptionDataType* filter
);

const ElectricalConnectionDescriptionDataType* ElectricalConnectionCommonGetDescriptionWithId(
    const ElectricalConnectionCommon* self,
    ElectricalConnectionIdType electrical_connection_id
);

/**
 * @brief Get an electrical connection description that matches the filter criteria
 *
 * This function searches through electrical connection descriptions and returns the unique one
 * that matches all the criteria specified in the filter parameter. If no match is found or there are
 * multiple matches, returns NULL.
 *
 * Trnsformed from eebus-go GetDescriptionsForFilter()
 *
 * @param self Pointer to the ElectricalConnectionCommon instance
 * @param filter Pointer to the description filter criteria to match against
 * @return Pointer to the matching electrical connection description,
 *         or NULL if no match is found or there are several matches
 */
const ElectricalConnectionDescriptionDataType* ElectricalConnectionCommonGetDescriptionWithFilter(
    const ElectricalConnectionCommon* self,
    const ElectricalConnectionDescriptionDataType* filter
);

/**
 * @brief Retrieves the electrical connection description that matches the given parameter description filter.
 *
 * This function searches for and returns a pointer to an ElectricalConnectionDescriptionDataType
 * that matches the specified filter criteria. The filter is provided as a parameter description
 * data type.
 *
 * @param self A pointer to the ElectricalConnectionCommon instance.
 * @param filter A pointer to the ElectricalConnectionParameterDescriptionDataType that specifies
 *               the filter criteria for the search.
 * @return A pointer to the ElectricalConnectionDescriptionDataType that matches the filter, or
 *         NULL if no matching description is found.
 */
const ElectricalConnectionDescriptionDataType* ElectricalConnectionCommonGetDescriptionWithParameterDescriptionFilter(
    const ElectricalConnectionCommon* self,
    const ElectricalConnectionParameterDescriptionDataType* filter
);

/**
 * @brief Retrieves a permitted value set that matches the given filter criteria.
 *
 * This function searches through the electrical connection's permitted value sets
 * and returns a pointer to the unique matching entry that satisfies the specified
 * filter criteria. If no match is found or there are multiple matches, the function returns NULL.
 *
 * Transformed from eebus-go GetPermittedValueSetForFilter()
 *
 * @param self A pointer to the ElectricalConnectionCommon instance to search within.
 * @param filter A pointer to the ElectricalConnectionPermittedValueSetDataType structure
 *               that defines the filtering criteria.
 * @return A pointer to the matching ElectricalConnectionPermittedValueSetDataType, or
 *         NULL if no matches are found or there are multiple matches.
 */
const ElectricalConnectionPermittedValueSetDataType* ElectricalConnectionCommonGetPermittedValueSetWithFilter(
    const ElectricalConnectionCommon* self,
    const ElectricalConnectionPermittedValueSetDataType* filter
);

// TODO: Port GetPermittedValueDataForFilter(), GetPhaseCurrentLimits() and
// AdjustValueToBeWithinPermittedValuesForParameterId() (not used in LPC/MPC)

/**
 * @brief Retrieves an electrical connection parameter description based on a filter.
 *
 * This function searches through the electrical connection parameters using the provided filter
 * criteria and returns the unique matching parameter description.
 *
 * @param self Pointer to the ElectricalConnectionCommon instance
 * @param filter Filter criteria for parameter description selection
 * @return Pointer to matching parameter description, If no matching characteristic is found
 * or there are multiple matches, returns NULL
 */
const ElectricalConnectionCharacteristicDataType* ElectricalConnectionCommonGetCharacteristicWithFilter(
    const ElectricalConnectionCommon* self,
    const ElectricalConnectionCharacteristicDataType* filter
);

bool ElectricalConnectionCommonCheckCharacteristicWithFilter(
    const ElectricalConnectionCharacteristicListDataType* data,
    const ElectricalConnectionCharacteristicDataType* filter
);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_USE_CASE_SPECIALIZATION_ELECTRICAL_CONNECTION_ELECTRICAL_CONNECTION_COMMON_H_
