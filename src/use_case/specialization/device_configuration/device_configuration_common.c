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
 * @brief Device Configuration Common functionality implementation
 */

#include "src/use_case/specialization/device_configuration/device_configuration_common.h"

#include "src/spine/model/model.h"
#include "src/use_case/specialization/helper.h"

static bool LimitIdMatch(const DeviceConfigurationKeyIdType* id_a, const DeviceConfigurationKeyIdType* id_b);

void DeviceConfigurationCommonConstruct(
    DeviceConfigurationCommon* self,
    FeatureLocalObject* feature_local,
    FeatureRemoteObject* feature_remote
) {
    self->feature_local  = feature_local;
    self->feature_remote = feature_remote;
}

bool LimitIdMatch(const DeviceConfigurationKeyIdType* id_a, const DeviceConfigurationKeyIdType* id_b) {
    if ((id_a == NULL) || (id_b == NULL)) {
        return false;
    }

    return *id_a == *id_b;
}

bool DeviceConfigurationCommonCheckKeyValueWithFilter(
    const DeviceConfigurationCommon* self,
    const DeviceConfigurationKeyValueListDataType* data,
    const DeviceConfigurationKeyValueDescriptionDataType* description
) {
    if ((data == NULL) || (description == NULL)) {
        return false;
    }

    EebusDataListMatchIterator it = {0};
    DeviceConfigurationCommonKeyValueDescriptionMatchFirst(self, description, &it);
    for (; !EebusDataListMatchIteratorIsDone(&it); EebusDataListMatchIteratorNext(&it)) {
        const DeviceConfigurationKeyValueDescriptionDataType* const cur_description
            = EebusDataListMatchIteratorGet(&it);

        for (size_t j = 0; j < data->device_configuration_key_value_data_size; ++j) {
            const DeviceConfigurationKeyValueDataType* const key_value = data->device_configuration_key_value_data[j];
            if (LimitIdMatch(key_value->key_id, cur_description->key_id) && (key_value->value != NULL)) {
                return true;
            }
        }
    }

    return false;
}

const DeviceConfigurationKeyValueDescriptionDataType* DeviceConfigurationCommonGetKeyValueDescriptionWithKeyId(
    const DeviceConfigurationCommon* self,
    DeviceConfigurationKeyIdType key_id
) {
    const DeviceConfigurationKeyValueDescriptionDataType filter = {.key_id = &key_id};

    const DeviceConfigurationKeyValueDescriptionListDataType* const key_value_descriptions_list
        = DeviceConfigurationCommonGetKeyValueDescriptionList(self);

    return HelperGetListUniqueMatch(
        kFunctionTypeDeviceConfigurationKeyValueDescriptionListData,
        key_value_descriptions_list,
        &filter
    );
}

void DeviceConfigurationCommonKeyValueDescriptionMatchFirst(
    const DeviceConfigurationCommon* self,
    const DeviceConfigurationKeyValueDescriptionDataType* filter,
    EebusDataListMatchIterator* it
) {
    const DeviceConfigurationKeyValueDescriptionListDataType* const key_value_descriptions_list
        = DeviceConfigurationCommonGetKeyValueDescriptionList(self);

    HelperListMatchFirst(
        kFunctionTypeDeviceConfigurationKeyValueDescriptionListData,
        key_value_descriptions_list,
        filter,
        it
    );
}

const DeviceConfigurationKeyValueDescriptionDataType* DeviceConfigurationCommonGetKeyValueDescriptionWithFilter(
    const DeviceConfigurationCommon* self,
    const DeviceConfigurationKeyValueDescriptionDataType* filter
) {
    const DeviceConfigurationKeyValueDescriptionListDataType* const key_value_descriptions_list
        = DeviceConfigurationCommonGetKeyValueDescriptionList(self);

    return HelperGetListUniqueMatch(
        kFunctionTypeDeviceConfigurationKeyValueDescriptionListData,
        key_value_descriptions_list,
        filter
    );
}

const DeviceConfigurationKeyValueDataType* DeviceConfigurationCommonGetKeyValueWithKeyId(
    const DeviceConfigurationCommon* self,
    DeviceConfigurationKeyIdType key_id
) {
    return DeviceConfigurationCommonGetKeyValueWithFilter(
        self,
        &(DeviceConfigurationKeyValueDescriptionDataType){.key_id = &key_id}
    );
}

const DeviceConfigurationKeyValueDataType* DeviceConfigurationCommonGetKeyValueWithFilter(
    const DeviceConfigurationCommon* self,
    const DeviceConfigurationKeyValueDescriptionDataType* filter
) {
    const DeviceConfigurationKeyValueListDataType* const key_value_data = HelperGetFeatureData(
        self->feature_local,
        self->feature_remote,
        kFunctionTypeDeviceConfigurationKeyValueListData
    );

    if ((key_value_data == NULL) || (key_value_data->device_configuration_key_value_data == NULL)) {
        return NULL;
    }

    EebusDataListMatchIterator it = {0};
    DeviceConfigurationCommonKeyValueDescriptionMatchFirst(self, filter, &it);
    if (EebusDataListMatchIteratorIsDone(&it)) {
        return NULL;
    }

    const DeviceConfigurationKeyValueDescriptionDataType* const description = EebusDataListMatchIteratorGet(&it);

    for (size_t i = 0; i < key_value_data->device_configuration_key_value_data_size; ++i) {
        const DeviceConfigurationKeyValueDataType* const key_value
            = key_value_data->device_configuration_key_value_data[i];
        if ((key_value->key_id != NULL) && (*key_value->key_id == *description->key_id)) {
            return key_value;
        }
    }

    return NULL;
}
