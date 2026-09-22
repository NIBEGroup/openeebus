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
 * @brief Smart Energy Management PS Common functionality implementation
 */

#include "src/use_case/specialization/smart_energy_management_ps/smart_energy_management_ps_common.h"

#include "src/spine/model/loadcontrol_types.h"
#include "src/spine/model/model.h"
#include "src/use_case/specialization/helper.h"

void SmartEnergyManagementPsCommonConstruct(
    SmartEnergyManagementPsCommon* self,
    FeatureLocalObject* feature_local,
    FeatureRemoteObject* feature_remote
) {
  self->feature_local  = feature_local;
  self->feature_remote = feature_remote;
}
