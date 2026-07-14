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
 * @brief Energy Guard LPP use case implementation
 */

#include "src/use_case/actor/eg/lpp/eg_lpp.h"

#include <stddef.h>

#include "src/common/array_util.h"
#include "src/use_case/actor/eg/eg_lp.h"

static const UseCaseActorType valid_actor_types[] = {kUseCaseActorTypeControllableSystem};
static const EntityTypeType valid_entity_types[]  = {
    kEntityTypeTypeCEM,
    kEntityTypeTypeEVSE,
    kEntityTypeTypeInverter,
    kEntityTypeTypeSmartEnergyAppliance,
    kEntityTypeTypeSubMeterElectricity,
};

static const FeatureTypeType use_case_scenario_support_1_features[] = {kFeatureTypeTypeLoadControl};
static const FeatureTypeType use_case_scenario_support_2_features[] = {kFeatureTypeTypeDeviceConfiguration};
static const FeatureTypeType use_case_scenario_support_3_features[] = {kFeatureTypeTypeDeviceDiagnosis};
static const FeatureTypeType use_case_scenario_support_4_features[] = {kFeatureTypeTypeElectricalConnection};

static const UseCaseScenario use_case_scenarios[] = {
    {
     .scenario             = (UseCaseScenarioSupportType)1,
     .mandatory            = true,
     .server_features      = use_case_scenario_support_1_features,
     .server_features_size = ARRAY_SIZE(use_case_scenario_support_1_features),
     },
    {
     .scenario             = (UseCaseScenarioSupportType)2,
     .mandatory            = true,
     .server_features      = use_case_scenario_support_2_features,
     .server_features_size = ARRAY_SIZE(use_case_scenario_support_2_features),
     },
    {
     .scenario             = (UseCaseScenarioSupportType)3,
     .mandatory            = true,
     .server_features      = use_case_scenario_support_3_features,
     .server_features_size = ARRAY_SIZE(use_case_scenario_support_3_features),
     },
    {
     .scenario             = (UseCaseScenarioSupportType)4,
     .mandatory            = false,
     .server_features      = use_case_scenario_support_4_features,
     .server_features_size = ARRAY_SIZE(use_case_scenario_support_4_features),
     },
};

static const UseCaseInfo eg_lpp_use_case_info = {
    .valid_actor_types       = valid_actor_types,
    .valid_actor_types_size  = ARRAY_SIZE(valid_actor_types),
    .valid_entity_types      = valid_entity_types,
    .valid_entity_types_size = ARRAY_SIZE(valid_entity_types),
    .use_case_scenarios      = use_case_scenarios,
    .use_case_scenarios_size = ARRAY_SIZE(use_case_scenarios),
    .actor                   = kUseCaseActorTypeEnergyGuard,
    .use_case_name_id        = kUseCaseNameTypeLimitationOfPowerProduction,
    .version                 = "1.0.0",
    .sub_revision            = "release",
    .available               = true,
};

EgLpUseCaseObject* EgLppUseCaseCreate(EntityLocalObject* local_entity, EgLpListenerObject* eg_lpp_listener) {
  return EgLpUseCaseCreate(kEnergyDirectionTypeProduce, &eg_lpp_use_case_info, local_entity, eg_lpp_listener);
}
