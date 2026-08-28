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
 * @brief Device Configuration Server functionality implementation
 */

#include "src/use_case/specialization/device_configuration/device_configuration_server.h"

#include "src/common/array_util.h"
#include "src/common/eebus_data/eebus_data_list.h"
#include "src/spine/model/filter.h"
#include "src/spine/model/model.h"
#include "src/use_case/specialization/device_configuration/device_configuration_common.h"
#include "src/use_case/specialization/helper.h"

static const FunctionType key_value_fcn             = kFunctionTypeDeviceConfigurationKeyValueListData;
static const FunctionType key_value_description_fcn = kFunctionTypeDeviceConfigurationKeyValueDescriptionListData;

EebusError DeviceConfigurationServerConstruct(DeviceConfigurationServer* self, EntityLocalObject* local_entity) {
    const EebusError err
        = FeatureInfoServerConstruct(&self->feature_info_server, kFeatureTypeTypeDeviceConfiguration, local_entity);
    if (err != kEebusErrorOk) {
        return err;
    }

    DeviceConfigurationCommonConstruct(&self->device_cfg_common, self->feature_info_server.local_feature, NULL);
    return kEebusErrorOk;
}

DeviceConfigurationKeyIdType GetNextDeviceConfigurationKeyId(const DeviceConfigurationServer* self) {
    FeatureLocalObject* const fl = self->feature_info_server.local_feature;

    const DeviceConfigurationKeyValueDescriptionListDataType* const descriptions_list
        = FEATURE_LOCAL_GET_DATA(fl, key_value_description_fcn);

    if ((descriptions_list == NULL) || (descriptions_list->device_configuration_key_value_description_data == NULL)) {
        return (DeviceConfigurationKeyIdType)0;
    }

    DeviceConfigurationKeyIdType next_id = (DeviceConfigurationKeyIdType)0;

    for (size_t i = 0; i < descriptions_list->device_configuration_key_value_description_data_size; ++i) {
        const DeviceConfigurationKeyValueDescriptionDataType* const description
            = descriptions_list->device_configuration_key_value_description_data[i];

        if ((description->key_id != NULL) && (*description->key_id >= next_id)) {
            next_id = *description->key_id + 1;
        }
    }

    return next_id;
}

EebusError DeviceConfigurationServerAddKeyValueDescription(
    const DeviceConfigurationServer* self,
    const DeviceConfigurationKeyValueDescriptionDataType* description
) {
    DeviceConfigurationKeyValueDescriptionDataType description_tmp = *description;

    description_tmp.key_id = &(DeviceConfigurationKeyIdType){GetNextDeviceConfigurationKeyId(self)};

    const FilterType filter_partial = FILTER_PARTIAL(key_value_description_fcn, NULL, NULL, NULL);

    const DeviceConfigurationKeyValueDescriptionDataType* const description_data[] = {&description_tmp};

    DeviceConfigurationKeyValueDescriptionListDataType description_list = {
        .device_configuration_key_value_description_data      = description_data,
        .device_configuration_key_value_description_data_size = ARRAY_SIZE(description_data),
    };

    FeatureLocalObject* const fl = self->feature_info_server.local_feature;

    return FEATURE_LOCAL_UPDATE_DATA(fl, key_value_description_fcn, &description_list, &filter_partial, NULL);
}

EebusError DeviceConfigurationServerUpdateKeyValueWithKeyId(
    DeviceConfigurationServer* self,
    const DeviceConfigurationKeyValueDataType* key_value,
    const DeviceConfigurationKeyValueDataElementsType* delete_elements,
    DeviceConfigurationKeyIdType key_id
) {
    const DeviceConfigurationKeyValueDescriptionDataType filter = {.key_id = &key_id};
    return DeviceConfigurationServerUpdateKeyValueWithFilter(self, key_value, delete_elements, &filter);
}

EebusError GetKeyIdWithDescription(
    const DeviceConfigurationServer* self,
    const DeviceConfigurationKeyValueDescriptionDataType* description,
    DeviceConfigurationKeyIdType* key_id
) {
    const DeviceConfigurationKeyValueDescriptionListDataType* const key_value_description_list
        = DeviceConfigurationCommonGetKeyValueDescriptionList(&self->device_cfg_common);

    const DeviceConfigurationKeyValueDescriptionDataType* const key_value_description
        = HelperGetListUniqueMatch(key_value_description_fcn, key_value_description_list, description);

    if (key_value_description == NULL) {
        return kEebusErrorOther;
    }

    if (key_value_description->key_id == NULL) {
        return kEebusErrorNoChange;
    }

    *key_id = *key_value_description->key_id;
    return kEebusErrorOk;
}

EebusError DeviceConfigurationServerUpdateKeyValueWithFilter(
    const DeviceConfigurationServer* self,
    const DeviceConfigurationKeyValueDataType* key_value,
    const DeviceConfigurationKeyValueDataElementsType* delete_elements,
    const DeviceConfigurationKeyValueDescriptionDataType* filter
) {
    DeviceConfigurationKeyIdType key_id;
    EebusError key_id_err = GetKeyIdWithDescription(self, filter, &key_id);
    if (key_id_err != kEebusErrorOk) {
        return key_id_err;
    }

    DeviceConfigurationKeyValueDataType data_tmp = *key_value;

    data_tmp.key_id = &key_id;

    const DeviceConfigurationKeyValueDataType* const key_value_data[] = {&data_tmp};

    DeviceConfigurationKeyValueListDataType key_value_list = {
        .device_configuration_key_value_data      = key_value_data,
        .device_configuration_key_value_data_size = ARRAY_SIZE(key_value_data),
    };

    const FilterType filter_partial = FILTER_PARTIAL(key_value_fcn, NULL, NULL, NULL);

    const DeviceConfigurationKeyValueListDataSelectorsType delete_selectors = {
        .key_id = &key_id,
    };

    const FilterType* delete_filter = &FILTER_DELETE(key_value_fcn, NULL, &delete_selectors, delete_elements);
    if (delete_elements == NULL) {
        delete_filter = NULL;
    }

    FeatureLocalObject* const fl = self->feature_info_server.local_feature;

    return FEATURE_LOCAL_UPDATE_DATA(fl, key_value_fcn, &key_value_list, &filter_partial, delete_filter);
}
