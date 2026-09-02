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
 * @brief Measurement Server functionality
 */

#ifndef SRC_USE_CASE_SPECIALIZATION_MEASUREMENT_MEASUREMENT_CLIENT_H_
#define SRC_USE_CASE_SPECIALIZATION_MEASUREMENT_MEASUREMENT_CLIENT_H_

#include "src/spine/api/feature_local_interface.h"
#include "src/use_case/specialization/feature_info_client.h"
#include "src/use_case/specialization/measurement/measurement_common.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct MeasurementClient MeasurementClient;

struct MeasurementClient {
  FeatureInfoClient feature_info_client;
  MeasurementCommon measurement_common;
};

/**
 * @brief Constructs a Measurement Client object
 *
 * @param self Pointer to the MeasurementClient instance to be constructed
 * @param local_entity Pointer to the local entity object
 * @param remote_entity Pointer to the remote entity object
 *
 * @return EebusError Error code indicating success or failure of the construction
 *
 * @note This function initializes a new Measurement Client instance with the provided
 *       local and remote entity objects.
 */
EebusError
MeasurementClientConstruct(MeasurementClient* self, EntityLocalObject* local_entity, EntityRemoteObject* remote_entity);

/**
 * @brief Requests Function Type Measurement Description List Data from a remote device
 * This function sends a request to a remote device to retrieve measurement descriptions
 * based on the provided selectors and elements parameters
 *
 * @param self Pointer to MeasurementClient instance
 * @param selectors Pointer to measurement description list data selectors
 * @param elements Pointer to measurement description data elements
 * @return EebusError Returns error code indicating success or failure of the request
 *
 * This function sends a request to a remote device to retrieve measurement descriptions
 * based on the provided selectors and elements parameters.
 */
EebusError MeasurementClientRequestDescriptions(
    MeasurementClient* self,
    const MeasurementDescriptionListDataSelectorsType* selectors,
    const MeasurementDescriptionDataElementsType* elements
);

/**
 * @brief Requests FunctionTypeMeasurementConstraintsListData from a remote entity
 * This function sends a request to get measurement constraints list data
 * from a remote EEBUS entity based on provided selectors and elements filters
 *
 * @param self Pointer to MeasurementClient instance
 * @param selector Pointer to measurement constraints list data selectors
 * @param elements Pointer to measurement constraints data elements
 *
 * @return EebusError Error code indicating success or failure of the request
 */
EebusError MeasurementClientRequestConstraints(
    MeasurementClient* self,
    const MeasurementConstraintsListDataSelectorsType* selector,
    const MeasurementConstraintsDataElementsType* elements
);

/**
 * @brief Requests measurement list data from a remote entity
 * This function sends a request to obtain measurement data based on the provided
 * selectors and elements configuration from a remote EEBUS entity
 *
 * @param self Pointer to the MeasurementClient instance
 * @param selector Pointer to measurement list data selectors configuration
 * @param elements Pointer to measurement data elements configuration
 *
 * @return EebusError Error code indicating success or failure of the request
 */
// request FunctionTypeMeasurementListData from a remote entity
EebusError MeasurementClientRequestData(
    MeasurementClient* self,
    const MeasurementListDataSelectorsType* selector,
    const MeasurementDataElementsType* elements,
    ReplyMessageCallback cb,
    void* ctx
);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_USE_CASE_SPECIALIZATION_MEASUREMENT_MEASUREMENT_CLIENT_H_
