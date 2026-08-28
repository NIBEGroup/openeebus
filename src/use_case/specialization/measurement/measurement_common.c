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
 * @brief Measurement Common functionality implementation
 */

#include "src/use_case/specialization/measurement/measurement_common.h"

#include "src/common/eebus_data/eebus_data_list.h"
#include "src/spine/model/measurement_types.h"
#include "src/spine/model/model.h"
#include "src/use_case/specialization/helper.h"

void MeasurementCommonConstruct(
    MeasurementCommon* self,
    FeatureLocalObject* feature_local,
    FeatureRemoteObject* feature_remote
) {
    self->feature_local  = feature_local;
    self->feature_remote = feature_remote;
}

bool MeasurementIdMatch(const MeasurementIdType* id_a, const MeasurementIdType* id_b) {
    if ((id_a == NULL) || (id_b == NULL)) {
        return false;
    }

    return *id_a == *id_b;
}

bool MeasurementCommonCheckMeasurementWithFilter(
    const MeasurementCommon* self,
    const MeasurementListDataType* measurements_list,
    const MeasurementDescriptionDataType* filter
) {
    if ((measurements_list == NULL) || (filter == NULL)) {
        return false;
    }

    const MeasurementDescriptionListDataType* const descriptions_list
        = HelperGetFeatureData(self->feature_local, self->feature_remote, kFunctionTypeMeasurementListData);

    EebusDataListMatchIterator it = {0};
    HelperListMatchFirst(kFunctionTypeMeasurementListData, descriptions_list, filter, &it);

    for (; !EebusDataListMatchIteratorIsDone(&it); EebusDataListMatchIteratorNext(&it)) {
        const MeasurementDescriptionDataType* description = EebusDataListMatchIteratorGet(&it);
        if (description->measurement_id == NULL) {
            for (size_t i = 0; i < measurements_list->measurement_data_size; ++i) {
                const MeasurementDataType* const item = measurements_list->measurement_data[i];
                if (MeasurementIdMatch(item->measurement_id, description->measurement_id)) {
                    return true;
                }
            }
        }
    }

    return false;
}

const MeasurementDescriptionDataType*
MeasurementCommonGetMeasurementDescriptionWithId(const MeasurementCommon* self, MeasurementIdType measurement_id) {
    const MeasurementDescriptionDataType filter = {.measurement_id = &measurement_id};
    return MeasurementCommonGetMeasurementDescriptionWithFilter(self, &filter);
}

const MeasurementDescriptionDataType* MeasurementCommonGetMeasurementDescriptionWithFilter(
    const MeasurementCommon* self,
    const MeasurementDescriptionDataType* filter
) {
    const MeasurementDescriptionListDataType* const descriptions_list = MeasurementCommonGetDescriptions(self);
    return HelperGetListUniqueMatch(kFunctionTypeMeasurementDescriptionListData, descriptions_list, filter);
}

const MeasurementConstraintsDataType* MeasurementCommonGetMeasurementConstraintsWithFilter(
    const MeasurementCommon* self,
    const MeasurementConstraintsDataType* filter
) {
    const MeasurementConstraintsListDataType* const constraints_list
        = HelperGetFeatureData(self->feature_local, self->feature_remote, kFunctionTypeMeasurementConstraintsListData);

    return HelperGetListUniqueMatch(kFunctionTypeMeasurementConstraintsListData, constraints_list, filter);
}

const MeasurementDataType*
MeasurementCommonGetMeasurementWithId(const MeasurementCommon* self, MeasurementIdType measurement_id) {
    const MeasurementDescriptionDataType filter = {.measurement_id = &measurement_id};
    return MeasurementCommonGetMeasurementWithFilter(self, &filter);
}

const MeasurementDataType*
MeasurementCommonGetMeasurementWithFilter(const MeasurementCommon* self, const MeasurementDescriptionDataType* filter) {
    const MeasurementDescriptionDataType* const description
        = MeasurementCommonGetMeasurementDescriptionWithFilter(self, filter);

    const MeasurementListDataType* const measurements_list
        = HelperGetFeatureData(self->feature_local, self->feature_remote, kFunctionTypeMeasurementListData);

    const MeasurementDataType measurements_filter = {.measurement_id = description->measurement_id};

    return HelperGetListUniqueMatch(kFunctionTypeMeasurementListData, measurements_list, &measurements_filter);
}

void MeasurementCommonGetMeasurementDescriptionMatchFirst(
    const MeasurementCommon* self,
    const MeasurementDescriptionDataType* filter,
    EebusDataListMatchIterator* it
) {
    const MeasurementDescriptionListDataType* const descriptions_list
        = HelperGetFeatureData(self->feature_local, self->feature_remote, kFunctionTypeMeasurementDescriptionListData);

    HelperListMatchFirst(kFunctionTypeMeasurementDescriptionListData, descriptions_list, filter, it);
}
