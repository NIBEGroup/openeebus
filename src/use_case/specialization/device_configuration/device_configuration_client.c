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
 * @brief Device Configuration Client functionality implementation
 */

#include "src/use_case/specialization/device_configuration/device_configuration_client.h"

#include "src/spine/model/device_configuration_types.h"

static const FunctionType key_value_fcn = kFunctionTypeDeviceConfigurationKeyValueListData;

EebusError DeviceConfigurationClientConstruct(
    DeviceConfigurationClient* self,
    EntityLocalObject* local_entity,
    EntityRemoteObject* remote_entity
) {
  const EebusError err = FeatureInfoClientConstruct(
      &self->feature_info_client,
      kFeatureTypeTypeDeviceConfiguration,
      local_entity,
      remote_entity
  );

  if (err != kEebusErrorOk) {
    return err;
  }

  DeviceConfigurationCommonConstruct(&self->device_cfg_common, NULL, self->feature_info_client.remote_feature);
  return kEebusErrorOk;
}

EebusError DeviceConfigurationClientRequestKeyValueDescription(
    DeviceConfigurationClient* self,
    const DeviceConfigurationKeyValueDescriptionListDataSelectorsType* selectors,
    const DeviceConfigurationKeyValueDescriptionDataElementsType* elements
) {
  return FEATURE_LOCAL_READ_FROM_REMOTE(
      self->feature_info_client.local_feature,
      self->feature_info_client.remote_feature,
      kFunctionTypeDeviceConfigurationKeyValueDescriptionListData,
      selectors,
      elements,
      NULL,
      NULL
  );
}

EebusError DeviceConfigurationClientRequestKeyValue(
    DeviceConfigurationClient* self,
    const DeviceConfigurationKeyValueListDataSelectorsType* selectors,
    const DeviceConfigurationKeyValueDataElementsType* elements,
    ReplyMessageCallback cb,
    void* ctx
) {
  return FEATURE_LOCAL_READ_FROM_REMOTE(
      self->feature_info_client.local_feature,
      self->feature_info_client.remote_feature,
      key_value_fcn,
      selectors,
      elements,
      cb,
      ctx
  );
}

EebusError DeviceConfigurationClientWriteKeyValueList(
    DeviceConfigurationClient* self,
    const DeviceConfigurationKeyValueListDataType* key_value_list,
    ResultMessageCallback cb,
    void* ctx
) {
  if (key_value_list == NULL) {
    return kEebusErrorInputArgumentNull;
  }

  if ((key_value_list->device_configuration_key_value_data == NULL)
      || (key_value_list->device_configuration_key_value_data_size == 0)) {
    return kEebusErrorInputArgument;
  }

  return FEATURE_LOCAL_WRITE_TO_REMOTE(
      self->feature_info_client.local_feature,
      self->feature_info_client.remote_feature,
      key_value_fcn,
      key_value_list,
      NULL,
      NULL,
      cb,
      ctx
  );
}
