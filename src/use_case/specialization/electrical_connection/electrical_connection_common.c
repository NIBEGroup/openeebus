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
 * @brief Electrical Connection Common functionality implementation
 */

#include "src/use_case/specialization/electrical_connection/electrical_connection_common.h"
#include "src/spine/model/model.h"
#include "src/use_case/specialization/helper.h"

static const FunctionType description_fcn           = kFunctionTypeElectricalConnectionDescriptionListData;
static const FunctionType parameter_description_fcn = kFunctionTypeElectricalConnectionParameterDescriptionListData;
static const FunctionType permitted_value_set_fcn   = kFunctionTypeElectricalConnectionPermittedValueSetListData;
static const FunctionType characteristic_fcn        = kFunctionTypeElectricalConnectionCharacteristicListData;

void ElectricalConnectionCommonConstruct(
    ElectricalConnectionCommon* self,
    FeatureLocalObject* feature_local,
    FeatureRemoteObject* feature_remote
) {
    self->feature_local  = feature_local;
    self->feature_remote = feature_remote;
}

const ElectricalConnectionParameterDescriptionDataType* ElectricalConnectionCommonGetParameterDescriptionWithFilter(
    const ElectricalConnectionCommon* self,
    const ElectricalConnectionParameterDescriptionDataType* filter
) {
    if ((self == NULL) || (filter == NULL)) {
        return NULL;
    }

    const ElectricalConnectionParameterDescriptionListDataType* const parameters_description_list
        = ElectricalConnectionCommonGetParameterDescriptionList(self);

    return HelperGetListUniqueMatch(parameter_description_fcn, parameters_description_list, filter);
}

bool ParameterIdMatch(
    const ElectricalConnectionParameterDescriptionDataType* decription,
    const ElectricalConnectionPermittedValueSetDataType* data
) {
    if ((decription == NULL) || (data == NULL)) {
        return false;
    }

    if ((data->parameter_id == NULL) || (decription->parameter_id == NULL)) {
        return false;
    }

    return *data->parameter_id == *decription->parameter_id;
}

bool ElectricalConnectionCommonCheckEventPayloadDataForFilter(
    const ElectricalConnectionCommon* self,
    const ElectricalConnectionPermittedValueSetListDataType* data,
    const ElectricalConnectionParameterDescriptionDataType* filter
) {
    if ((data == NULL || filter == NULL)) {
        return false;
    }

    const ElectricalConnectionParameterDescriptionListDataType* parameters_description_list
        = ElectricalConnectionCommonGetParameterDescriptionList(self);

    EebusDataListMatchIterator it = {0};
    HelperListMatchFirst(parameter_description_fcn, parameters_description_list, filter, &it);

    for (; !EebusDataListMatchIteratorIsDone(&it); EebusDataListMatchIteratorNext(&it)) {
        const ElectricalConnectionParameterDescriptionDataType* const description = EebusDataListMatchIteratorGet(&it);
        for (size_t j = 0; j < data->electrical_connection_permitted_value_set_data_size; ++j) {
            const ElectricalConnectionPermittedValueSetDataType* const item
                = data->electrical_connection_permitted_value_set_data[j];
            if (ParameterIdMatch(description, item) && (item->permitted_value_set != NULL)) {
                return true;
            }
        }
    }

    return false;
}

const ElectricalConnectionDescriptionDataType* ElectricalConnectionCommonGetDescriptionWithId(
    const ElectricalConnectionCommon* self,
    ElectricalConnectionIdType electrical_connection_id
) {
    const ElectricalConnectionDescriptionDataType filter = {.electrical_connection_id = &electrical_connection_id};
    return ElectricalConnectionCommonGetDescriptionWithFilter(self, &filter);
}

const ElectricalConnectionDescriptionDataType* ElectricalConnectionCommonGetDescriptionWithFilter(
    const ElectricalConnectionCommon* self,
    const ElectricalConnectionDescriptionDataType* filter
) {
    const ElectricalConnectionDescriptionListDataType* const descriptions_list
        = HelperGetFeatureData(self->feature_local, self->feature_remote, description_fcn);

    return HelperGetListUniqueMatch(description_fcn, descriptions_list, filter);
}

bool ElectricalConnectionIdMatch(
    const ElectricalConnectionDescriptionDataType* description,
    const ElectricalConnectionParameterDescriptionDataType* parameter_description
) {
    if ((description == NULL) || (parameter_description == NULL)) {
        return false;
    }

    if (description->electrical_connection_id == NULL) {
        return false;
    }

    return *description->electrical_connection_id == *parameter_description->electrical_connection_id;
}

const ElectricalConnectionDescriptionDataType* ElectricalConnectionCommonGetDescriptionWithParameterDescriptionFilter(
    const ElectricalConnectionCommon* self,
    const ElectricalConnectionParameterDescriptionDataType* filter
) {
    if ((self == NULL) || (filter == NULL)) {
        return NULL;
    }

    const ElectricalConnectionParameterDescriptionListDataType* const parameters_description_list
        = ElectricalConnectionCommonGetParameterDescriptionList(self);

    const ElectricalConnectionParameterDescriptionDataType* const param
        = HelperGetListUniqueMatch(parameter_description_fcn, parameters_description_list, filter);

    if (param == NULL) {
        return NULL;
    }

    const ElectricalConnectionDescriptionListDataType* const descriptions_list
        = HelperGetFeatureData(self->feature_local, self->feature_remote, description_fcn);

    const ElectricalConnectionDescriptionDataType descriptions_filter = {
        .electrical_connection_id = param->electrical_connection_id,
    };

    return HelperGetListUniqueMatch(description_fcn, descriptions_list, &descriptions_filter);
}

const ElectricalConnectionPermittedValueSetDataType* ElectricalConnectionCommonGetPermittedValueSetWithFilter(
    const ElectricalConnectionCommon* self,
    const ElectricalConnectionPermittedValueSetDataType* filter
) {
    const ElectricalConnectionPermittedValueSetListDataType* data
        = HelperGetFeatureData(self->feature_local, self->feature_remote, permitted_value_set_fcn);

    return HelperGetListUniqueMatch(permitted_value_set_fcn, data, filter);
}

// TODO: Port GetPermittedValueDataForFilter(), GetPhaseCurrentLimits(),
// AdjustValueToBeWithinPermittedValuesForParameterId() (not used in LPC/MPC)

const ElectricalConnectionCharacteristicDataType* ElectricalConnectionCommonGetCharacteristicWithFilter(
    const ElectricalConnectionCommon* self,
    const ElectricalConnectionCharacteristicDataType* filter
) {
    if ((self == NULL) || (filter == NULL)) {
        return NULL;
    }

    const ElectricalConnectionCharacteristicListDataType* characteristics_list
        = ElectricalConnectionCommonGetCharacteristicList(self);

    return HelperGetListUniqueMatch(characteristic_fcn, characteristics_list, filter);
}

bool ElectricalConnectionCommonCheckCharacteristicWithFilter(
    const ElectricalConnectionCharacteristicListDataType* data,
    const ElectricalConnectionCharacteristicDataType* filter
) {
    if ((data == NULL) || (filter == NULL)) {
        return false;
    }

    EebusDataListMatchIterator it = {0};
    HelperListMatchFirst(characteristic_fcn, data, filter, &it);
    return !EebusDataListMatchIteratorIsDone(&it);
}
