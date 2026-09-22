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
 * @brief Smart Energy Management PS Client functionality implementation
 */

#include "src/use_case/specialization/smart_energy_management_ps/smart_energy_management_ps_client.h"

#include "src/common/array_util.h"
#include "src/spine/model/loadcontrol_types.h"

EebusError SmartEnergyManagementPsClientConstruct(
    SmartEnergyManagementPsClient* self,
    EntityLocalObject* local_entity,
    EntityRemoteObject* remote_entity
) {
  const EebusError err = FeatureInfoClientConstruct(
      &self->feature_info_client,
      kFeatureTypeTypeSmartEnergyManagementPs,
      local_entity,
      remote_entity
  );

  if (err != kEebusErrorOk) {
    return err;
  }

  SmartEnergyManagementPsCommonConstruct(&self->sem_ps_common, NULL, self->feature_info_client.remote_feature);
  return kEebusErrorOk;
}

EebusError
SmartEnergyManagementPsClientRequestData(SmartEnergyManagementPsClient* self, ReplyMessageCallback cb, void* ctx) {
  return FEATURE_LOCAL_READ_FROM_REMOTE(
      self->feature_info_client.local_feature,
      self->feature_info_client.remote_feature,
      kFunctionTypeSmartEnergyManagementPsData,
      NULL,
      NULL,
      cb,
      ctx
  );
}

EebusError SmartEnergyManagementPsClientWritePartial(
    SmartEnergyManagementPsClient* self,
    const AlternativesIdType* alternative_id,
    const SmartEnergyManagementPsPowerSequenceType* power_sequence,
    ResultMessageCallback cb,
    void* ctx
) {
  if (self == NULL) {
    return kEebusErrorInputArgumentNull;
  }

  const OperationsObject* const ops = FEATURE_GET_FUNCTION_OPERATIONS(
      FEATURE_OBJECT(self->feature_info_client.remote_feature),
      kFunctionTypeSmartEnergyManagementPsData
  );

  if ((ops == NULL) || !OPERATIONS_GET_WRITE_PARTIAL(ops)) {
    return kEebusErrorNoChange;
  }

  const SmartEnergyManagementPsPowerSequenceType* power_sequences[1] = {power_sequence};

  const SmartEnergyManagementPsAlternativesRelationType relation = {.alternatives_id = alternative_id};

  const SmartEnergyManagementPsAlternativesType alternative = {
      .relation            = (alternative_id != NULL) ? &relation : NULL,
      .power_sequence      = power_sequences,
      .power_sequence_size = ARRAY_SIZE(power_sequences),
  };

  const SmartEnergyManagementPsAlternativesType* alternatives[1] = {&alternative};

  SmartEnergyManagementPsDataType sem_ps_data = {
      .alternatives      = alternatives,
      .alternatives_size = ARRAY_SIZE(alternatives),
  };

  return FEATURE_LOCAL_WRITE_TO_REMOTE(
      self->feature_info_client.local_feature,
      self->feature_info_client.remote_feature,
      kFunctionTypeSmartEnergyManagementPsData,
      &sem_ps_data,
      NULL,
      NULL,
      cb,
      ctx
  );
}
