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

#include "src/use_case/specialization/load_control/load_control_server.h"

#include "src/common/array_util.h"
#include "src/common/eebus_data/eebus_data_list.h"
#include "src/spine/feature/feature_local.h"
#include "src/spine/model/filter.h"
#include "src/spine/model/loadcontrol_types.h"
#include "src/spine/model/model.h"

static const FunctionType limit_description_fcn = kFunctionTypeLoadControlLimitDescriptionListData;
static const FunctionType limit_fcn             = kFunctionTypeLoadControlLimitListData;

EebusError LoadControlServerConstruct(LoadControlServer* self, EntityLocalObject* local_entity) {
    const EebusError err
        = FeatureInfoServerConstruct(&self->feature_info_server, kFeatureTypeTypeLoadControl, local_entity);
    if (err != kEebusErrorOk) {
        return err;
    }

    LocalLoadControlCommonConstruct(&self->load_control_common, self->feature_info_server.local_feature, NULL);
    return kEebusErrorOk;
}

LoadControlLimitIdType LoadControlServerGetNextLimitId(const LoadControlServer* self) {
    const LoadControlLimitDescriptionListDataType* const descriptions_list
        = LoadControlCommonGetLimitDescriptionList(&self->load_control_common);

    if ((descriptions_list == NULL) || (descriptions_list->load_control_limit_description_data == NULL)) {
        return 0;
    }

    LoadControlLimitIdType next_id = (LoadControlLimitIdType)0;
    for (size_t i = 0; i < descriptions_list->load_control_limit_description_data_size; ++i) {
        const LoadControlLimitDescriptionDataType* const limit_description
            = descriptions_list->load_control_limit_description_data[i];
        if ((limit_description->limit_id != NULL) && (*limit_description->limit_id >= next_id)) {
            next_id = *limit_description->limit_id + 1;
        }
    }

    return next_id;
}

EebusError LoadControlServerAddLimitDescription(
    LoadControlServer* self,
    LoadControlLimitDescriptionDataType* description,
    LoadControlLimitIdType* limit_id
) {
    if (description == NULL) {
        return kEebusErrorInputArgumentNull;
    }

    if (description->limit_id != NULL) {
        return kEebusErrorNoChange;
    }

    *limit_id = LoadControlServerGetNextLimitId(self);

    description->limit_id = limit_id;

    const FilterType filter_partial = FILTER_PARTIAL(limit_description_fcn, NULL, NULL, NULL);

    const LoadControlLimitDescriptionDataType* const description_data[] = {description};

    LoadControlLimitDescriptionListDataType limits_description = {description_data, ARRAY_SIZE(description_data)};

    FeatureLocalObject* const fl = self->feature_info_server.local_feature;

    return FEATURE_LOCAL_UPDATE_DATA(fl, limit_description_fcn, &limits_description, &filter_partial, NULL);
}

EebusError UpdateLimitData(
    const LoadControlServer* self,
    const LoadControlLimitListDataType* limits,
    const LoadControlLimitListDataSelectorsType* delete_selectors,
    const LoadControlLimitDataElementsType* delete_elements
) {
    const FilterType filter_partial = FILTER_PARTIAL(limit_fcn, NULL, NULL, NULL);
    const FilterType* filter_delete = &FILTER_DELETE(limit_fcn, NULL, delete_selectors, delete_elements);
    if ((delete_selectors == NULL) && (delete_elements == NULL)) {
        filter_delete = NULL;
    }

    FeatureLocalObject* const fl = self->feature_info_server.local_feature;

    return FEATURE_LOCAL_UPDATE_DATA(fl, limit_fcn, limits, &filter_partial, filter_delete);
}

EebusError LoadControlServerUpdateLimitWithId(
    LoadControlServer* self,
    LoadControlLimitDataType* limt_data,
    LoadControlLimitIdType limit_id
) {
    if (limt_data == NULL) {
        return kEebusErrorInputArgumentNull;
    }

    const LoadControlLimitDescriptionDataType filter = {.limit_id = &limit_id};

    const LoadControlLimitDescriptionDataType* const description
        = LoadControlCommonGetLimitDescriptionWithFilter(&self->load_control_common, &filter);

    if (description == NULL) {
        return kEebusErrorNoChange;
    }

    LoadControlLimitDataType limit_data_tmp = *limt_data;

    limit_data_tmp.limit_id = description->limit_id;

    const LoadControlLimitDataType* const limit_data[] = {&limit_data_tmp};

    const LoadControlLimitListDataType limits = {limit_data, ARRAY_SIZE(limit_data)};

    return UpdateLimitData(self, &limits, NULL, NULL);
}

EebusError LoadControlServerUpdateLimitWithFilter(
    const LoadControlServer* self,
    const LoadControlLimitDataType* limit_data,
    const LoadControlLimitDescriptionDataType* filter,
    const LoadControlLimitListDataSelectorsType* delete_selectors,
    const LoadControlLimitDataElementsType* delete_elements
) {
    if ((limit_data == NULL) || (filter == NULL)) {
        return kEebusErrorInputArgumentNull;
    }

    const LoadControlLimitDescriptionDataType* const description
        = LoadControlCommonGetLimitDescriptionWithFilter(&self->load_control_common, filter);

    if ((description == NULL) || (description->limit_id == NULL)) {
        return kEebusErrorNoChange;
    }

    LoadControlLimitDataType limit_data_tmp = *limit_data;

    limit_data_tmp.limit_id = description->limit_id;

    const LoadControlLimitDataType* const limits_data[] = {&limit_data_tmp};

    const LoadControlLimitListDataType limits = {limits_data, ARRAY_SIZE(limits_data)};

    return UpdateLimitData(self, &limits, delete_selectors, delete_elements);
}
