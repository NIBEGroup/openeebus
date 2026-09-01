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
 * @brief Measurement Common functionality
 */

#ifndef SRC_USE_CASE_SPECIALIZATION_MEASUREMENT_MEASUREMENT_COMMON_H_
#define SRC_USE_CASE_SPECIALIZATION_MEASUREMENT_MEASUREMENT_COMMON_H_

#include "src/spine/feature/feature_local.h"
#include "src/spine/feature/feature_remote.h"
#include "src/spine/model/measurement_types.h"
#include "src/use_case/specialization/helper.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct MeasurementCommon MeasurementCommon;

struct MeasurementCommon {
    FeatureLocalObject* feature_local;
    FeatureRemoteObject* feature_remote;
};

/**
 * @brief Constructs a MeasurementCommon instance
 *
 * @param self Pointer to the MeasurementCommon instance to be constructed
 * @param feature_local Pointer to the FeatureLocalObject associated with this measurement
 * @param feature_remote Pointer to the FeatureRemoteObject associated with this measurement
 */
void MeasurementCommonConstruct(
    MeasurementCommon* self,
    FeatureLocalObject* feature_local,
    FeatureRemoteObject* feature_remote
);

/**
 * @brief Get the measurement description list helper
 * @param self Pointer to the MeasurementCommon instance to retrieve the descriptions from
 * @return Pointer to the measurement description list
 */
static inline const MeasurementDescriptionListDataType* MeasurementCommonGetDescriptions(
    const MeasurementCommon* self
) {
    return (
        const MeasurementDescriptionListDataType*
    )HelperGetFeatureData(self->feature_local, self->feature_remote, kFunctionTypeMeasurementDescriptionListData);
}

/**
 * @brief Get the measurement list helper
 * @param self Pointer to the MeasurementCommon instance to retrieve the measurements from
 * @return Pointer to the measurement list
 */
static inline const MeasurementListDataType* MeasurementCommonGetMeasurements(const MeasurementCommon* self) {
    return (
        const MeasurementListDataType*
    )HelperGetFeatureData(self->feature_local, self->feature_remote, kFunctionTypeMeasurementListData);
}

/**
 * @brief Check if the measurement list contains a measurement that matches the given filter
 * @param self Pointer to the MeasurementCommon instance to check against
 * @param measurements_list Pointer to the MeasurementListDataType containing the measurements to check
 * @param filter Pointer to the MeasurementDescriptionDataType filter to match against
 * @return true if a matching measurement is found, false otherwise
 */
bool MeasurementCommonCheckMeasurementWithFilter(
    const MeasurementCommon* self,
    const MeasurementListDataType* measurements_list,
    const MeasurementDescriptionDataType* filter
);

/**
 * @brief Get the measurement description with a specific ID
 * @param self Pointer to the MeasurementCommon instance to retrieve the description from
 * @param measurement_id The ID of the measurement description to retrieve
 * @return Pointer to the MeasurementDescriptionDataType matching the ID, or NULL if not found
 */
const MeasurementDescriptionDataType*
MeasurementCommonGetMeasurementDescriptionWithId(const MeasurementCommon* self, MeasurementIdType measurement_id);

/**
 * @brief Get the measurement description with a specific filter
 * @param self Pointer to the MeasurementCommon instance to retrieve the description from
 * @param filter Pointer to the MeasurementDescriptionDataType filter to match against
 * @return Pointer to the MeasurementDescriptionDataType matching the filter, or NULL if not found
 */
const MeasurementDescriptionDataType* MeasurementCommonGetMeasurementDescriptionWithFilter(
    const MeasurementCommon* self,
    const MeasurementDescriptionDataType* filter
);

/**
 * @brief Get the measurement constraints with a specific filter
 * @param self Pointer to the MeasurementCommon instance to retrieve the constraints from
 * @param filter Pointer to the MeasurementConstraintsDataType filter to match against
 * @return Pointer to the MeasurementConstraintsDataType matching the filter, or NULL if not found
 */
const MeasurementConstraintsDataType* MeasurementCommonGetMeasurementConstraintsWithFilter(
    const MeasurementCommon* self,
    const MeasurementConstraintsDataType* filter
);

/**
 * @brief Get the measurement data with a specific ID
 * @param self Pointer to the MeasurementCommon instance to retrieve the measurement from
 * @param measurement_id The ID of the measurement to retrieve
 * @return Pointer to the MeasurementDataType matching the ID, or NULL if not found
 */
const MeasurementDataType*
MeasurementCommonGetMeasurementWithId(const MeasurementCommon* self, MeasurementIdType measurement_id);

/**
 * @brief Get the measurement data with a specific filter
 * @param self Pointer to the MeasurementCommon instance to retrieve the measurement from
 * @param filter Pointer to the MeasurementDescriptionDataType filter to match against
 * @return Pointer to the MeasurementDataType matching the filter, or NULL if not found
 */
const MeasurementDataType*
MeasurementCommonGetMeasurementWithFilter(const MeasurementCommon* self, const MeasurementDescriptionDataType* filter);

/**
 * @brief Retrieves the first measurement data that matches the given filter criteria
 *
 * @param self Pointer to the MeasurementCommon instance
 * @param filter Pointer to measurement description filter criteria
 * @param it Pointer to iterator that will be set to first matching measurement
 *
 * @note The iterator will be positioned at the first measurement entry matching the filter,
 *       or at the end if no match is found
 */
void MeasurementCommonGetMeasurementDescriptionMatchFirst(
    const MeasurementCommon* self,
    const MeasurementDescriptionDataType* filter,
    EebusDataListMatchIterator* it
);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_USE_CASE_SPECIALIZATION_MEASUREMENT_MEASUREMENT_COMMON_H_
