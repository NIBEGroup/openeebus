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
 * @brief Electrical Connection Server functionality implementation
 */

#include "src/use_case/specialization/electrical_connection/electrical_connection_server.h"
#include "src/common/array_util.h"
#include "src/spine/model/filter.h"
#include "src/spine/model/model.h"
#include "src/use_case/specialization/helper.h"

static const FunctionType description_fcn           = kFunctionTypeElectricalConnectionDescriptionListData;
static const FunctionType characteristic_fcn        = kFunctionTypeElectricalConnectionCharacteristicListData;
static const FunctionType parameter_description_fcn = kFunctionTypeElectricalConnectionParameterDescriptionListData;

EebusError ElectricalConnectionServerConstruct(ElectricalConnectionServer* self, EntityLocalObject* local_entity) {
    const EebusError err
        = FeatureInfoServerConstruct(&self->feature_info_server, kFeatureTypeTypeElectricalConnection, local_entity);
    if (err != kEebusErrorOk) {
        return err;
    }

    ElectricalConnectionCommonConstruct(&self->el_connection_common, self->feature_info_server.local_feature, NULL);
    return kEebusErrorOk;
}

EebusError ElectricalConnectionServerAddDescriptionWithId(
    ElectricalConnectionServer* self,
    const ElectricalConnectionDescriptionDataType* description,
    ElectricalConnectionIdType electrical_connection_id
) {
    if (description == NULL) {
        return kEebusErrorInputArgumentNull;
    }

    ElectricalConnectionDescriptionDataType description_tmp = *description;

    description_tmp.electrical_connection_id = &electrical_connection_id;

    const ElectricalConnectionDescriptionDataType* const description_data[] = {&description_tmp};
    const ElectricalConnectionDescriptionListDataType descriptions_list_tmp
        = {description_data, ARRAY_SIZE(description_data)};

    const FilterType filter_partial = FILTER_PARTIAL(description_fcn, NULL, NULL, NULL);

    FeatureLocalObject* const fl = self->feature_info_server.local_feature;
    return FEATURE_LOCAL_UPDATE_DATA(fl, description_fcn, &descriptions_list_tmp, &filter_partial, NULL);
}

ElectricalConnectionParameterIdType
GetNextParameterId(const ElectricalConnectionServer* self, const ElectricalConnectionIdType* electrical_connection_id) {
    const ElectricalConnectionParameterDescriptionDataType filter
        = {.electrical_connection_id = electrical_connection_id};

    const ElectricalConnectionParameterDescriptionListDataType* const parameter_descriptions_list
        = HelperGetFeatureData(self->feature_info_server.local_feature, NULL, parameter_description_fcn);

    ElectricalConnectionParameterIdType next_id = (ElectricalConnectionParameterIdType)0;

    EebusDataListMatchIterator it = {0};
    HelperListMatchFirst(parameter_description_fcn, parameter_descriptions_list, &filter, &it);
    for (; !EebusDataListMatchIteratorIsDone(&it); EebusDataListMatchIteratorNext(&it)) {
        const ElectricalConnectionParameterDescriptionDataType* description = EebusDataListMatchIteratorGet(&it);
        if ((description->parameter_id != NULL) && (*description->parameter_id >= next_id)) {
            next_id = *description->parameter_id + 1;
        }
    }

    return next_id;
}

EebusError ElectricalConnectionServerAddParameterDescription(
    ElectricalConnectionServer* self,
    const ElectricalConnectionParameterDescriptionDataType* description,
    ElectricalConnectionParameterIdType* parameter_id
) {
    if (description == NULL || parameter_id == NULL) {
        return kEebusErrorInputArgumentNull;
    }

    if ((description->electrical_connection_id == NULL) || (description->parameter_id != NULL)) {
        return kEebusErrorInputArgument;
    }

    *parameter_id = GetNextParameterId(self, description->electrical_connection_id);

    ElectricalConnectionParameterDescriptionDataType description_tmp = *description;

    description_tmp.parameter_id = parameter_id;

    const FilterType filter_partial = FILTER_PARTIAL(parameter_description_fcn, NULL, NULL, NULL);

    const ElectricalConnectionParameterDescriptionDataType* const description_data[] = {&description_tmp};
    const ElectricalConnectionParameterDescriptionListDataType descriptions_list
        = {description_data, ARRAY_SIZE(description_data)};

    FeatureLocalObject* const fl = self->feature_info_server.local_feature;

    return FEATURE_LOCAL_UPDATE_DATA(fl, parameter_description_fcn, &descriptions_list, &filter_partial, NULL);
}

ElectricalConnectionCharacteristicIdType GetNextCharacteristicId(const ElectricalConnectionServer* self) {
    FeatureLocalObject* const fl = self->feature_info_server.local_feature;

    const ElectricalConnectionCharacteristicListDataType* const characteristics_list
        = FEATURE_LOCAL_GET_DATA(fl, characteristic_fcn);

    if ((characteristics_list == NULL) || (characteristics_list->electrical_connection_characteristic_data == NULL)) {
        return (ElectricalConnectionCharacteristicIdType)0;
    }

    ElectricalConnectionCharacteristicIdType next_id = (ElectricalConnectionCharacteristicIdType)0;

    for (size_t i = 0; i < characteristics_list->electrical_connection_characteristic_data_size; ++i) {
        const ElectricalConnectionCharacteristicDataType* const characteristic
            = characteristics_list->electrical_connection_characteristic_data[i];

        if ((characteristic->characteristic_id != NULL) && (*characteristic->characteristic_id >= next_id)) {
            next_id = *characteristic->characteristic_id + 1;
        }
    }

    return next_id;
}

EebusError ElectricalConnectionServerAddCharacteristic(
    ElectricalConnectionServer* self,
    const ElectricalConnectionCharacteristicDataType* characteristic
) {
    if ((characteristic->electrical_connection_id == NULL) || (characteristic->parameter_id == NULL)) {
        return kEebusErrorInputArgument;
    }

    if (characteristic->characteristic_id != NULL) {
        return kEebusErrorInputArgument;
    }

    ElectricalConnectionCharacteristicDataType characteristic_tmp = *characteristic;
    characteristic_tmp.characteristic_id = &(ElectricalConnectionCharacteristicIdType){GetNextCharacteristicId(self)};

    const ElectricalConnectionCharacteristicDataType* const characteristic_data[] = {&characteristic_tmp};

    ElectricalConnectionCharacteristicListDataType data_list = {
        .electrical_connection_characteristic_data      = characteristic_data,
        .electrical_connection_characteristic_data_size = ARRAY_SIZE(characteristic_data),
    };

    const FilterType filter_partial = FILTER_PARTIAL(characteristic_fcn, NULL, NULL, NULL);

    FeatureLocalObject* const fl = self->feature_info_server.local_feature;

    return FEATURE_LOCAL_UPDATE_DATA(fl, characteristic_fcn, &data_list, &filter_partial, NULL);
}

EebusError ElectricalConnectionServerUpdateCharacteristic(
    ElectricalConnectionServer* self,
    const ElectricalConnectionCharacteristicDataType* data,
    const ElectricalConnectionCharacteristicDataElementsType* delete_elements
) {
    if (data == NULL) {
        return kEebusErrorInputArgumentNull;
    }

    if ((data->characteristic_id == NULL) || (data->electrical_connection_id == NULL) || (data->parameter_id == NULL)) {
        return kEebusErrorInputArgument;
    }

    const ElectricalConnectionCharacteristicDataType filter = {
        .electrical_connection_id = data->electrical_connection_id,
        .parameter_id             = data->parameter_id,
        .characteristic_id        = data->characteristic_id,
    };

    const ElectricalConnectionCharacteristicDataType* const characteristic
        = ElectricalConnectionCommonGetCharacteristicWithFilter(&self->el_connection_common, &filter);

    if (characteristic == NULL) {
        return kEebusErrorNoChange;
    }

    FilterType partial = FILTER_PARTIAL(characteristic_fcn, NULL, NULL, NULL);

    ElectricalConnectionCharacteristicListDataSelectorsType selectors = {
        .characteristic_id = data->characteristic_id,
    };

    FilterType* delete_filter
        = (delete_elements != NULL) ? &FILTER_DELETE(characteristic_fcn, NULL, &selectors, delete_elements) : NULL;

    const ElectricalConnectionCharacteristicDataType* const characteristic_data[]
        = {(ElectricalConnectionCharacteristicDataType*)data};

    ElectricalConnectionCharacteristicListDataType data_list = {
        .electrical_connection_characteristic_data      = characteristic_data,
        .electrical_connection_characteristic_data_size = ARRAY_SIZE(characteristic_data),
    };

    FeatureLocalObject* const fl = self->feature_info_server.local_feature;

    return FEATURE_LOCAL_UPDATE_DATA(fl, characteristic_fcn, &data_list, &partial, delete_filter);
}

// TODO: UpdatePermittedValueSetForIds() and UpdatePermittedValueSetForFilters() from eebus-go
