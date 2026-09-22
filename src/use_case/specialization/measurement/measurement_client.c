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
 * @brief Measurement Server functionality implementation
 */

#include "src/use_case/specialization/measurement/measurement_client.h"
#include "src/spine/model/measurement_types.h"

EebusError MeasurementClientConstruct(
    MeasurementClient* self,
    EntityLocalObject* local_entity,
    EntityRemoteObject* remote_entity
) {
  const EebusError err = FeatureInfoClientConstruct(
      &self->feature_info_client,
      kFeatureTypeTypeMeasurement,
      local_entity,
      remote_entity
  );

  if (err != kEebusErrorOk) {
    return err;
  }

  MeasurementCommonConstruct(&self->measurement_common, NULL, self->feature_info_client.remote_feature);
  return kEebusErrorOk;
}

EebusError MeasurementClientRequestDescriptions(
    MeasurementClient* self,
    const MeasurementDescriptionListDataSelectorsType* selectors,
    const MeasurementDescriptionDataElementsType* elements
) {
  return FEATURE_LOCAL_READ_FROM_REMOTE(
      self->feature_info_client.local_feature,
      self->feature_info_client.remote_feature,
      kFunctionTypeMeasurementDescriptionListData,
      selectors,
      elements,
      NULL,
      NULL
  );
}

EebusError MeasurementClientRequestConstraints(
    MeasurementClient* self,
    const MeasurementConstraintsListDataSelectorsType* selector,
    const MeasurementConstraintsDataElementsType* elements
) {
  return FEATURE_LOCAL_READ_FROM_REMOTE(
      self->feature_info_client.local_feature,
      self->feature_info_client.remote_feature,
      kFunctionTypeMeasurementConstraintsListData,
      selector,
      elements,
      NULL,
      NULL
  );
}

EebusError MeasurementClientRequestData(
    MeasurementClient* self,
    const MeasurementListDataSelectorsType* selector,
    const MeasurementDataElementsType* elements,
    ReplyMessageCallback cb,
    void* ctx
) {
  return FEATURE_LOCAL_READ_FROM_REMOTE(
      self->feature_info_client.local_feature,
      self->feature_info_client.remote_feature,
      kFunctionTypeMeasurementListData,
      selector,
      elements,
      cb,
      ctx
  );
}
