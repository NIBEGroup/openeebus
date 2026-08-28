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
 * @brief Load Control Client functionality implementation
 */

#include "src/use_case/specialization/load_control/load_control_common.h"

#include "src/common/eebus_data/eebus_data_list.h"
#include "src/spine/model/loadcontrol_types.h"
#include "src/spine/model/model.h"
#include "src/use_case/specialization/helper.h"

static const FunctionType limit_description_fcn = kFunctionTypeLoadControlLimitDescriptionListData;
static const FunctionType limit_fcn             = kFunctionTypeLoadControlLimitListData;

void LocalLoadControlCommonConstruct(
    LoadControlCommon* self,
    FeatureLocalObject* feature_local,
    FeatureRemoteObject* feature_remote
) {
    self->feature_local  = feature_local;
    self->feature_remote = feature_remote;
}

bool LimitIdMatch(const LoadControlLimitIdType* id_a, const LoadControlLimitIdType* id_b) {
    if ((id_a == NULL) || (id_b == NULL)) {
        return false;
    }

    return *id_a == *id_b;
}

bool LoadControlCommonCheckLimitWithFilter(
    const LoadControlCommon* self,
    const LoadControlLimitListDataType* data,
    const LoadControlLimitDescriptionDataType* filter
) {
    if ((data == NULL) || (filter == NULL)) {
        return false;
    }

    const LoadControlLimitDescriptionListDataType* const descriptions_list
        = HelperGetFeatureData(self->feature_local, self->feature_remote, limit_description_fcn);

    EebusDataListMatchIterator it = {0};
    HelperListMatchFirst(limit_description_fcn, descriptions_list, filter, &it);

    for (; !EebusDataListMatchIteratorIsDone(&it); EebusDataListMatchIteratorNext(&it)) {
        const LoadControlLimitDescriptionDataType* const description = EebusDataListMatchIteratorGet(&it);
        for (size_t j = 0; j < data->load_control_limit_data_size; ++j) {
            const LoadControlLimitDataType* const item = data->load_control_limit_data[j];
            if (LimitIdMatch(item->limit_id, description->limit_id) && (item->value != NULL)) {
                return true;
            }
        }
    }

    return false;
}

const LoadControlLimitDescriptionDataType*
LoadControlCommonGetLimitDescriptionWithId(LoadControlCommon* self, LoadControlLimitIdType limit_id) {
    const LoadControlLimitDescriptionListDataType* const descriptions_list
        = HelperGetFeatureData(self->feature_local, self->feature_remote, limit_description_fcn);

    const LoadControlLimitDescriptionDataType filter = {.limit_id = &limit_id};

    return HelperGetListUniqueMatch(limit_description_fcn, descriptions_list, &filter);
}

const LoadControlLimitDescriptionDataType* LoadControlCommonGetLimitDescriptionWithFilter(
    const LoadControlCommon* self,
    const LoadControlLimitDescriptionDataType* filter
) {
    const LoadControlLimitDescriptionListDataType* const descriptions_list
        = HelperGetFeatureData(self->feature_local, self->feature_remote, limit_description_fcn);

    return HelperGetListUniqueMatch(limit_description_fcn, descriptions_list, filter);
}

const LoadControlLimitDataType*
LoadControlCommonGetLimitWithId(const LoadControlCommon* self, LoadControlLimitIdType limit_id) {
    return LoadControlCommonGetLimitWithFilter(
        self,
        &(const LoadControlLimitDescriptionDataType){.limit_id = &limit_id}
    );
}

const LoadControlLimitDataType*
LoadControlCommonGetLimitWithFilter(const LoadControlCommon* self, const LoadControlLimitDescriptionDataType* filter) {
    const LoadControlLimitDescriptionDataType* const description
        = LoadControlCommonGetLimitDescriptionWithFilter(self, filter);

    if (description == NULL || description->limit_id == NULL) {
        return NULL;
    }

    const LoadControlLimitListDataType* const limits_list
        = HelperGetFeatureData(self->feature_local, self->feature_remote, limit_fcn);

    const LoadControlLimitDataType limits_filter = {.limit_id = description->limit_id};

    return HelperGetListUniqueMatch(limit_fcn, limits_list, &limits_filter);
}
