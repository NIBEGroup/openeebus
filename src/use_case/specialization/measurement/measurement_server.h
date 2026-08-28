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

#ifndef SRC_USE_CASE_SPECIALIZATION_MEASUREMENT_MEASUREMENT_SERVER_H_
#define SRC_USE_CASE_SPECIALIZATION_MEASUREMENT_MEASUREMENT_SERVER_H_

#include "src/use_case/specialization/feature_info_server.h"
#include "src/use_case/specialization/measurement/measurement_common.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct MeasurementServer MeasurementServer;

struct MeasurementServer {
    FeatureInfoServer feature_info_server;
    MeasurementCommon measurement_common;
};

/**
 * @brief Constructs a MeasurementServer instance
 * @param self The MeasurementServer instance to be constructed
 * @param local_entity The local entity for which to construct the MeasurementServer
 * @return EebusError indicating success or failure of the operation
 */
EebusError MeasurementServerConstruct(MeasurementServer* self, EntityLocalObject* local_entity);

/**
 * @brief Add a new measurement description to the server
 * @param self The MeasurementServer instance to add the description to
 * @param description The measurement description data to add
 * @param measurement_id Pointer to store the ID of the added measurement
 * @return EebusError indicating success or failure of the operation
 */
EebusError MeasurementServerAddDescription(
    const MeasurementServer* self,
    const MeasurementDescriptionDataType* description,
    MeasurementIdType* measurement_id
);

/**
 * @brief Update the measurement with a specific ID
 * @param self The MeasurementServer instance to update the measurement for
 * @param measurement_data The measurement data to update
 * @param measurement_id The ID of the measurement to update
 * @return EebusError indicating success or failure of the operation
 */
EebusError MeasurementServerUpdateMeasurementWithId(
    MeasurementServer* self,
    const MeasurementDataType* measurement_data,
    MeasurementIdType measurement_id
);

/**
 * @brief Update the measurements list
 * @param self The MeasurementServer instance to update the measurements for
 * @param measurements_list The list of measurements to update
 * @param delete_selectors The selectors for measurements to delete
 * @param delete_elements The elements to delete from the measurements
 * @return EebusError indicating success or failure of the operation
 */
EebusError MeasurementServerUpdateMeasurements(
    MeasurementServer* self,
    const MeasurementListDataType* measurements_list,
    const MeasurementListDataSelectorsType* delete_selectors,
    const MeasurementDataElementsType* delete_elements
);

/**
 * @brief Update the measurement with a specific filter
 * @param self The MeasurementServer instance to update the measurement for
 * @param measurement The measurement data to update
 * @param filter The filter to match the measurement against
 * @param delete_selectors The selectors for measurements to delete
 * @param delete_elements The elements to delete from the measurements
 * @return EebusError indicating success or failure of the operation
 */
EebusError MeasurementServerUpdateMeasurementWithFilter(
    MeasurementServer* self,
    const MeasurementDataType* measurement,
    const MeasurementDescriptionDataType* filter,
    const MeasurementListDataSelectorsType* delete_selectors,
    const MeasurementDataElementsType* delete_elements
);

/**
 * @brief Update the measurement constraints
 * @param self The MeasurementServer instance to update the constraints for
 * @param constraints_list The list of measurement constraints to update
 * @param delete_selectors The selectors for measurements to delete
 * @param delete_elements The elements to delete from the measurements
 * @return EebusError indicating success or failure of the operation
 */
EebusError MeasurementServerUpdateMeasurementConstraints(
    MeasurementServer* self,
    const MeasurementConstraintsListDataType* constraints_list,
    const MeasurementListDataSelectorsType* delete_selectors,
    const MeasurementDataElementsType* delete_elements
);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_USE_CASE_SPECIALIZATION_MEASUREMENT_MEASUREMENT_SERVER_H_
