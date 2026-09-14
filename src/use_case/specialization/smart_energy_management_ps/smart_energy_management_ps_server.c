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
 * @brief Smart Energy Management PS Server functionality implementation
 */

#include "src/use_case/specialization/smart_energy_management_ps/smart_energy_management_ps_server.h"

#include "src/common/array_util.h"
#include "src/spine/feature/feature_local.h"
#include "src/spine/model/filter.h"
#include "src/spine/model/model.h"

EebusError
SmartEnergyManagementPsServerConstruct(SmartEnergyManagementPsServer* self, EntityLocalObject* local_entity) {
  const EebusError err
      = FeatureInfoServerConstruct(&self->feature_info_server, kFeatureTypeTypeSmartEnergyManagementPs, local_entity);
  if (err != kEebusErrorOk) {
    return err;
  }

  SmartEnergyManagementPsCommonConstruct(
      &self->smart_energy_management_ps_common,
      self->feature_info_server.local_feature,
      NULL
  );

  return kEebusErrorOk;
}

EebusError
SmartEnergyManagementPsServerUpdate(SmartEnergyManagementPsServer* self, const SmartEnergyManagementPsDataType* data) {
  if ((self == NULL) || (data == NULL)) {
    return kEebusErrorInputArgumentNull;
  }

  FeatureLocalObject* const fl = self->feature_info_server.local_feature;

  return FEATURE_LOCAL_UPDATE_DATA(fl, kFunctionTypeSmartEnergyManagementPsData, data, NULL, NULL);
}

EebusError SmartEnergyManagementPsServerUpdatePartial(
    SmartEnergyManagementPsServer* self,
    const AlternativesIdType* alternatives_id,
    const PowerSequenceNodeScheduleInformationDataType* node_schedule,
    const SmartEnergyManagementPsPowerSequenceType* power_sequence
) {
  if (self == NULL) {
    return kEebusErrorInputArgumentNull;
  }

  const SmartEnergyManagementPsPowerSequenceType* power_sequences[1] = {power_sequence};

  SmartEnergyManagementPsAlternativesRelationType relation = {
      .alternatives_id = alternatives_id,
  };

  const SmartEnergyManagementPsAlternativesType alternative = {
      .relation            = (alternatives_id != NULL) ? &relation : NULL,
      .power_sequence      = power_sequences,
      .power_sequence_size = ARRAY_SIZE(power_sequences),
  };

  const SmartEnergyManagementPsAlternativesType* alternatives[1] = {&alternative};

  SmartEnergyManagementPsDataType data = {
      .node_schedule_information = node_schedule,
      .alternatives              = alternatives,
      .alternatives_size         = ARRAY_SIZE(alternatives),
  };

  const FilterType filter_partial = FILTER_PARTIAL(kFunctionTypeSmartEnergyManagementPsData, NULL, NULL, NULL);

  FeatureLocalObject* const fl = self->feature_info_server.local_feature;

  return FEATURE_LOCAL_UPDATE_DATA(fl, kFunctionTypeSmartEnergyManagementPsData, &data, &filter_partial, NULL);
}
