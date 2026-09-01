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

#include "src/use_case/specialization/load_control/load_control_client.h"

#include "src/spine/model/filter.h"
#include "src/spine/model/loadcontrol_types.h"

static const FunctionType limit_fcn = kFunctionTypeLoadControlLimitListData;

EebusError LoadControlClientConstruct(
    LoadControlClient* self,
    EntityLocalObject* local_entity,
    EntityRemoteObject* remote_entity
) {
    const EebusError err = FeatureInfoClientConstruct(
        &self->feature_info_client,
        kFeatureTypeTypeLoadControl,
        local_entity,
        remote_entity
    );

    if (err != kEebusErrorOk) {
        return err;
    }

    LocalLoadControlCommonConstruct(&self->load_control_common, NULL, self->feature_info_client.remote_feature);
    return kEebusErrorOk;
}

EebusError LoadControlClientRequestLimitDescriptions(
    LoadControlClient* self,
    const LoadControlLimitDescriptionListDataSelectorsType* selectors,
    const LoadControlLimitDescriptionDataElementsType* elements
) {
    return FEATURE_LOCAL_READ_FROM_REMOTE(
        self->feature_info_client.local_feature,
        self->feature_info_client.remote_feature,
        kFunctionTypeLoadControlLimitDescriptionListData,
        selectors,
        elements,
        NULL,
        NULL
    );
}

EebusError LoadControlClientRequestLimitConstraints(
    LoadControlClient* self,
    const LoadControlLimitConstraintsListDataSelectorsType* selectors,
    const LoadControlLimitConstraintsDataElementsType* elements
) {
    return FEATURE_LOCAL_READ_FROM_REMOTE(
        self->feature_info_client.local_feature,
        self->feature_info_client.remote_feature,
        kFunctionTypeLoadControlLimitConstraintsListData,
        selectors,
        elements,
        NULL,
        NULL
    );
}

EebusError LoadControlClientReadLimit(
    LoadControlClient* self,
    const LoadControlLimitDescriptionDataType* filter,
    ReplyMessageCallback cb,
    void* ctx
) {
    const LoadControlLimitDescriptionDataType* const description
        = LoadControlCommonGetLimitDescriptionWithFilter(&self->load_control_common, filter);

    if ((description == NULL) || (description->limit_id == NULL)) {
        return kEebusErrorNoChange;
    }

    const LoadControlLimitListDataSelectorsType selectors = {
        .limit_id = description->limit_id,
    };

    return FEATURE_LOCAL_READ_FROM_REMOTE(
        self->feature_info_client.local_feature,
        self->feature_info_client.remote_feature,
        limit_fcn,
        &selectors,
        NULL,
        cb,
        ctx
    );
}

EebusError LoadControlClientWriteLimitList(
    LoadControlClient* self,
    const LoadControlLimitListDataType* limit_list,
    const LoadControlLimitListDataSelectorsType* delete_selectors,
    const LoadControlLimitDataElementsType* delete_elements,
    ResultMessageCallback cb,
    void* ctx
) {
    if (limit_list == NULL) {
        return kEebusErrorInputArgumentNull;
    }

    if (limit_list->load_control_limit_data == NULL || limit_list->load_control_limit_data_size == 0) {
        return kEebusErrorInputArgument;
    }

    const FilterType filter_delete_tmp = FILTER_DELETE(limit_fcn, NULL, delete_selectors, delete_elements);

    const FilterType* const filter_delete
        = ((delete_selectors != NULL) && (delete_elements != NULL)) ? &filter_delete_tmp : NULL;

    return FEATURE_LOCAL_WRITE_TO_REMOTE(
        self->feature_info_client.local_feature,
        self->feature_info_client.remote_feature,
        limit_fcn,
        limit_list,
        NULL,
        filter_delete,
        cb,
        ctx
    );
}
