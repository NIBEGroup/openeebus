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
 * @brief Measurement Server functionality implementation
 */

#include "src/use_case/specialization/measurement/measurement_server.h"
#include "src/common/array_util.h"
#include "src/common/eebus_arguments.h"
#include "src/spine/model/filter.h"
#include "src/spine/model/measurement_types.h"

static const FunctionType measurement_fcn = kFunctionTypeMeasurementListData;
static const FunctionType description_fcn = kFunctionTypeMeasurementDescriptionListData;
static const FunctionType constraints_fcn = kFunctionTypeMeasurementConstraintsListData;

EebusError MeasurementServerConstruct(MeasurementServer* self, EntityLocalObject* local_entity) {
    const EebusError err
        = FeatureInfoServerConstruct(&self->feature_info_server, kFeatureTypeTypeMeasurement, local_entity);

    if (err != kEebusErrorOk) {
        return err;
    }

    MeasurementCommonConstruct(&self->measurement_common, self->feature_info_server.local_feature, NULL);
    return kEebusErrorOk;
}

MeasurementIdType GetNextMeasurementId(const MeasurementServer* self) {
    const MeasurementDescriptionListDataType* const descriptions_list
        = MeasurementCommonGetDescriptions(&self->measurement_common);

    if ((descriptions_list == NULL) || (descriptions_list->measurement_description_data == NULL)) {
        return 0;
    }

    MeasurementIdType next_id = (MeasurementIdType)0;
    for (size_t i = 0; i < descriptions_list->measurement_description_data_size; ++i) {
        const MeasurementDescriptionDataType* description = descriptions_list->measurement_description_data[i];
        if ((description->measurement_id != NULL) && (*description->measurement_id >= next_id)) {
            next_id = *description->measurement_id + 1;
        }
    }

    return next_id;
}

EebusError MeasurementServerAddDescription(
    const MeasurementServer* self,
    const MeasurementDescriptionDataType* description,
    MeasurementIdType* measurement_id
) {
    if ((description == NULL) || (measurement_id == NULL)) {
        return kEebusErrorInputArgumentNull;
    }

    if (description->measurement_id != NULL) {
        return kEebusErrorInputArgument;
    }

    *measurement_id = GetNextMeasurementId(self);

    MeasurementDescriptionDataType description_tmp = *description;

    description_tmp.measurement_id = measurement_id;

    const FilterType filter_partial = FILTER_PARTIAL(description_fcn, NULL, NULL, NULL);

    const MeasurementDescriptionDataType* const description_data[] = {&description_tmp};
    const MeasurementDescriptionListDataType measurements_description
        = {description_data, ARRAY_SIZE(description_data)};

    FeatureLocalObject* const fl = self->feature_info_server.local_feature;

    return FEATURE_LOCAL_UPDATE_DATA(fl, description_fcn, &measurements_description, &filter_partial, NULL);
}

EebusError MeasurementServerUpdateMeasurementWithId(
    MeasurementServer* self,
    const MeasurementDataType* measurement_data,
    MeasurementIdType measurement_id
) {
    const MeasurementDescriptionDataType filter = {.measurement_id = &measurement_id};
    return MeasurementServerUpdateMeasurementWithFilter(self, measurement_data, &filter, NULL, NULL);
}

EebusError MeasurementServerUpdateMeasurements(
    MeasurementServer* self,
    const MeasurementListDataType* measurements_list,
    const MeasurementListDataSelectorsType* delete_selectors,
    const MeasurementDataElementsType* delete_elements
) {
    const FilterType filter_partial = FILTER_PARTIAL(measurement_fcn, NULL, NULL, NULL);
    const FilterType* filter_delete = &FILTER_DELETE(measurement_fcn, NULL, delete_selectors, delete_elements);
    if ((delete_selectors == NULL) && (delete_elements == NULL)) {
        filter_delete = NULL;
    }

    FeatureLocalObject* const fl = self->feature_info_server.local_feature;

    return FEATURE_LOCAL_UPDATE_DATA(fl, measurement_fcn, measurements_list, &filter_partial, filter_delete);
}

EebusError MeasurementServerUpdateMeasurementWithFilter(
    MeasurementServer* self,
    const MeasurementDataType* measurement,
    const MeasurementDescriptionDataType* filter,
    const MeasurementListDataSelectorsType* delete_selectors,
    const MeasurementDataElementsType* delete_elements
) {
    UNUSED(filter);
    const MeasurementDataType* measurement_data[]   = {measurement};
    const MeasurementListDataType measurements_list = {measurement_data, ARRAY_SIZE(measurement_data)};

    // TODO: get the data id with filter

    return MeasurementServerUpdateMeasurements(self, &measurements_list, delete_selectors, delete_elements);
}

EebusError MeasurementServerUpdateMeasurementConstraints(
    MeasurementServer* self,
    const MeasurementConstraintsListDataType* constraints_list,
    const MeasurementListDataSelectorsType* delete_selectors,
    const MeasurementDataElementsType* delete_elements
) {
    const FilterType filter_partial = FILTER_PARTIAL(constraints_fcn, NULL, NULL, NULL);
    const FilterType* filter_delete = &FILTER_DELETE(constraints_fcn, NULL, delete_selectors, delete_elements);
    if ((delete_selectors == NULL) && (delete_elements == NULL)) {
        filter_delete = NULL;
    }

    FeatureLocalObject* const fl = self->feature_info_server.local_feature;

    return FEATURE_LOCAL_UPDATE_DATA(fl, constraints_fcn, constraints_list, &filter_partial, filter_delete);
}
