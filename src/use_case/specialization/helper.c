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
 * @brief Helper functions implementation
 */

#include "src/common/api/eebus_data_interface.h"
#include "src/common/eebus_data/eebus_data_container.h"
#include "src/spine/feature/feature_local.h"
#include "src/spine/feature/feature_remote.h"
#include "src/spine/model/model.h"

const void* HelperGetFeatureData(
    const FeatureLocalObject* feature_local,
    const FeatureRemoteObject* feature_remote,
    FunctionType function_type
) {
    if (feature_local != NULL) {
        return FEATURE_LOCAL_GET_DATA(feature_local, function_type);
    } else if (feature_remote != NULL) {
        return FEATURE_REMOTE_GET_DATA(feature_remote, function_type);
    } else {
        return NULL;
    }
}

void HelperListMatchFirst(
    FunctionType function_type,
    const void* data_container,
    const void* filter,
    EebusDataListMatchIterator* it
) {
    EbusDataContainerListMatchFirst(ModelGetDataCfg(function_type), &data_container, it, filter);
}

const void* HelperGetListUniqueMatch(FunctionType function_type, const void* data_container, const void* filter) {
    EebusDataListMatchIterator it;

    EbusDataContainerListMatchFirst(ModelGetDataCfg(function_type), &data_container, &it, filter);
    if (EebusDataListMatchIteratorIsDone(&it)) {
        return NULL;
    }

    const void* const ret = EebusDataListMatchIteratorGet(&it);
    EebusDataListMatchIteratorNext(&it);
    if (!EebusDataListMatchIteratorIsDone(&it)) {
        // More that one match found - match is not unique
        return NULL;
    }

    return ret;
}
