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
 * @brief Electrical Connection Client functionality implementation
 */

#include "src/use_case/specialization/electrical_connection/electrical_connection_client.h"

EebusError ElectricalConnectionClientConstruct(
    ElectricalConnectionClient* self,
    EntityLocalObject* local_entity,
    EntityRemoteObject* remote_entity
) {
    EebusError err = FeatureInfoClientConstruct(
        &self->feature_info_client,
        kFeatureTypeTypeElectricalConnection,
        local_entity,
        remote_entity
    );

    if (err != kEebusErrorOk) {
        return err;
    }

    ElectricalConnectionCommonConstruct(&self->el_connection_common, NULL, self->feature_info_client.remote_feature);
    return kEebusErrorOk;
}

EebusError ElectricalConnectionClientRequestDescriptions(
    ElectricalConnectionClient* self,
    const ElectricalConnectionDescriptionListDataSelectorsType* selectors,
    const ElectricalConnectionDescriptionDataElementsType* elements
) {
    return FEATURE_LOCAL_READ_FROM_REMOTE(
        self->feature_info_client.local_feature,
        self->feature_info_client.remote_feature,
        kFunctionTypeElectricalConnectionDescriptionListData,
        selectors,
        elements,
        NULL,
        NULL
    );
}

EebusError ElectricalConnectionClientRequestParameterDescriptions(
    ElectricalConnectionClient* self,
    const ElectricalConnectionDescriptionListDataSelectorsType* selectors,
    const ElectricalConnectionDescriptionDataElementsType* elements
) {
    return FEATURE_LOCAL_READ_FROM_REMOTE(
        self->feature_info_client.local_feature,
        self->feature_info_client.remote_feature,
        kFunctionTypeElectricalConnectionParameterDescriptionListData,
        selectors,
        elements,
        NULL,
        NULL
    );
}

EebusError ElectricalConnectionClientRequestPermittedValueSets(
    ElectricalConnectionClient* self,
    const ElectricalConnectionPermittedValueSetListDataSelectorsType* selectors,
    const ElectricalConnectionPermittedValueSetDataElementsType* elements
) {
    return FEATURE_LOCAL_READ_FROM_REMOTE(
        self->feature_info_client.local_feature,
        self->feature_info_client.remote_feature,
        kFunctionTypeElectricalConnectionPermittedValueSetListData,
        selectors,
        elements,
        NULL,
        NULL
    );
}

EebusError ElectricalConnectionClientRequestCharacteristics(
    ElectricalConnectionClient* self,
    const ElectricalConnectionCharacteristicListDataSelectorsType* selectors,
    const ElectricalConnectionCharacteristicDataElementsType* elements,
    ReplyMessageCallback cb,
    void* ctx
) {
    return FEATURE_LOCAL_READ_FROM_REMOTE(
        self->feature_info_client.local_feature,
        self->feature_info_client.remote_feature,
        kFunctionTypeElectricalConnectionCharacteristicListData,
        selectors,
        elements,
        cb,
        ctx
    );
}
