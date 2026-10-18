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
 * @brief Smart Energy Management Common functionality
 */

#ifndef SRC_USE_CASE_SPECIALIZATION_SMART_ENERGY_MANAGEMENT_PS_SMART_ENERGY_MANAGEMENT_PS_COMMON_H_
#define SRC_USE_CASE_SPECIALIZATION_SMART_ENERGY_MANAGEMENT_PS_SMART_ENERGY_MANAGEMENT_PS_COMMON_H_

#include "src/spine/feature/feature_local.h"
#include "src/spine/feature/feature_remote.h"
#include "src/spine/model/smart_energy_management_ps_types.h"
#include "src/use_case/specialization/helper.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct SmartEnergyManagementPsCommon SmartEnergyManagementPsCommon;

struct SmartEnergyManagementPsCommon {
  FeatureLocalObject* feature_local;
  FeatureRemoteObject* feature_remote;
};

/**
 * @brief Constructs a SmartEnergyManagementPsCommon instance for local and remote features.
 *
 * This function initializes a SmartEnergyManagementPsCommon instance by associating it with a local feature object
 * and a remote feature object. It sets up the necessary structures for managing smart energy management communication
 * between the local and remote features.
 *
 * @param self A pointer to the SmartEnergyManagementPsCommon instance to be initialized.
 * @param feature_local A pointer to the local feature object.
 * @param feature_remote A pointer to the remote feature object.
 */
void SmartEnergyManagementPsCommonConstruct(
    SmartEnergyManagementPsCommon* self,
    FeatureLocalObject* feature_local,
    FeatureRemoteObject* feature_remote
);

/**
 * @brief Retrieves the Smart Energy Management PS data.
 *
 * This function fetches the Smart Energy Management PS data associated with the given SmartEnergyManagementPsCommon
 * instance.
 *
 * @param self A pointer to the SmartEnergyManagementPsCommon instance to retrieve data from.
 * @return A pointer to the SmartEnergyManagementPsDataType structure containing the data.
 */
static inline const SmartEnergyManagementPsDataType* SmartEnergyManagementPsCommonGetData(
    const SmartEnergyManagementPsCommon* self
) {
  return (const SmartEnergyManagementPsDataType*)
      HelperGetFeatureData(self->feature_local, self->feature_remote, kFunctionTypeSmartEnergyManagementPsData);
}

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_USE_CASE_SPECIALIZATION_SMART_ENERGY_MANAGEMENT_PS_SMART_ENERGY_MANAGEMENT_PS_COMMON_H_
